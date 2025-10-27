#include "cse7761.h"

#include "esphome/core/log.h"

#include <sstream>
#include <iomanip>
#include <inttypes.h>

namespace esphome {
  namespace cse7761 {

    static const char *const TAG = "cse7761";

    /*********************************************************************************************\
     * CSE7761 - Energy  (Sonoff Dual R3 Pow v1.x)
     *
     * Based on Tasmota source code
     * See https://github.com/arendst/Tasmota/discussions/10793
     * https://github.com/arendst/Tasmota/blob/development/tasmota/xnrg_19_cse7761.ino
     *
     * 2025 mazkagaz : Réimplémentation pour le Sonoff POWT
     * \*********************************************************************************************/

    static const int CSE7761_UREF = 42563;  // RmsUc
    static const int CSE7761_IREF = 52241;  // RmsIAC
    static const int CSE7761_PREF = 44513;  // PowerPAC

    static const uint8_t CSE7761_REG_SYSCON = 0x00;     // (2) System Control Register (0x0A04)
    static const uint8_t CSE7761_REG_EMUCON = 0x01;     // (2) Metering control register (0x0000)
    static const uint8_t CSE7761_REG_EMUCON2 = 0x13;    // (2) Metering control register 2 (0x0001)
    static const uint8_t CSE7761_REG_PULSE1SEL = 0x1D;  // (2) Pin function output select register (0x3210)

    // offsets registers
    static const uint8_t CSE7761_REG_POWER_PA_OFFSET = 0x0A; // (2) PowerPAOS (Active Power Offset)
    static const uint8_t CSE7761_REG_POWER_PB_OFFSET = 0x0B; // (2) PowerPAOS (Active Power Offset)
    static const uint8_t CSE7761_REG_RMS_IA_OFFSET = 0x0E; // (2) RmsIAOS (Current RMS Offset)
    static const uint8_t CSE7761_REG_RMS_IB_OFFSET = 0x0F; // (2) RmsIAOS (Current RMS Offset)

    // Calibration constants
    // TODO: read then from esphome yaml for more adaptability
    static const int CALIBRATION_MEASUREMENTS = 20;
    static const float CALIBRATION_CURRENT_A_B_SCALE_FACTOR = 10.46; // see Doc/CALIBRATION.md
    static const float CALIBRATION_POWER_A_B_SCALE_FACTOR = -8.89; // see Doc/CALIBRATION.md

    static const uint8_t CSE7761_REG_ANGLE = 0x22;      // (2) The phase angle between current and voltage (0x0000)
    static const uint8_t CSE7761_REG_UFREQ = 0x23;      // (2) The effective value of channel A current (0x0000)
    static const uint8_t CSE7761_REG_RMSIA = 0x24;      // (3) The effective value of channel A current (0x000000)
    static const uint8_t CSE7761_REG_RMSIB = 0x25;      // (3) The effective value of channel B current (0x000000)
    static const uint8_t CSE7761_REG_RMSU = 0x26;       // (3) Voltage RMS (0x000000)
    static const uint8_t CSE7761_REG_POWERPA = 0x2C;    // (4) Channel A active power, update rate 27.2Hz (0x00000000)
    static const uint8_t CSE7761_REG_POWERPB = 0x2D;    // (4) Channel B active power, update rate 27.2Hz (0x00000000)
    static const uint8_t CSE7761_REG_SYSSTATUS = 0x43;  // (1) System status register

    static const uint8_t CSE7761_REG_COEFFCHKSUM = 0x6F;  // (2) Coefficient checksum
    static const uint8_t CSE7761_REG_RMSIAC = 0x70;       // (2) Channel A effective current conversion coefficient

    static const uint8_t CSE7761_SPECIAL_COMMAND = 0xEA;   // Start special command
    static const uint8_t CSE7761_CMD_RESET = 0x96;         // Reset command, after receiving the command, the chip resets
    static const uint8_t CSE7761_CMD_CLOSE_WRITE = 0xDC;   // Close write operation
    static const uint8_t CSE7761_CMD_ENABLE_WRITE = 0xE5;  // Enable write operation

    enum CSE7761 { RMS_IAC, RMS_IBC, RMS_UC, POWER_PAC, POWER_PBC, POWER_SC, ENERGY_AC, ENERGY_BC };

    //***********************************************************************************************
    // setup: starting routine
    //***********************************************************************************************
    void CSE7761Component::setup() {
      // cse7761 reset
      this->write_(CSE7761_SPECIAL_COMMAND, CSE7761_CMD_RESET);
      uint16_t syscon = this->read_(0x00, 2);  // Default 0x0A04
      // cse7761 init with specific register configuration
      if ((0x0A04 == syscon) && this->chip_init_()) {
        // cse7761 is present and working
        this->write_(CSE7761_SPECIAL_COMMAND, CSE7761_CMD_CLOSE_WRITE);
        ESP_LOGD(TAG, "CSE7761 found");
        this->data_.ready = true;
        // Static numeric identifier (random) 0x1F2B4A7D
        this->pref_ = global_preferences->make_preference<EnergyDataStruct>(0x1F2B4A7D, true);
        EnergyDataStruct saved_values;

        if (this->pref_.load(&saved_values)) {
          this->accumulated_energy_received_ = saved_values.received;
          this->accumulated_energy_exported_ = saved_values.exported;
          ESP_LOGCONFIG(TAG, "Loaded accumulated energy: %.3f Wh (Received), %.3f Wh (Exported)", this->accumulated_energy_received_, this->accumulated_energy_exported_);
        } else {
          ESP_LOGCONFIG(TAG, "No accumulated energy found, starting from 0.0 Wh.");
          this->accumulated_energy_received_ = 0.0f;
          this->accumulated_energy_exported_ = 0.0f;
        }
      } else {
        this->mark_failed();
      }
    }

    //***********************************************************************************************
    // set_calibration_mode : start/stop calibration process
    //***********************************************************************************************
    void CSE7761Component::set_calibration_mode(bool state) {
      if (this->calibration_enabled_ != state) {
        this->calibration_enabled_ = state;
        ESP_LOGI(TAG, "Calibration mode %s", state ? "ENABLED" : "DISABLED");
        if (!state) {
          this->calibration_count_ = 0;
          this->sum_current_B_ = 0;
          this->sum_power_B_ = 0;
        }
      }
    }

    //***********************************************************************************************
    // perform_calibration_write_ : perform calibration as soon as CALIBRATION_MEASUREMENTS
    // measurements have been collected
    //***********************************************************************************************
    void CSE7761Component::perform_calibration_write_() {

      float calibration_count_F = (float)this->calibration_count_;
      // sofware calibration unless I find a way to use hardware calibration
      this->software_current_offset_B_ -= this->sum_current_B_ / calibration_count_F;
      this->software_power_offset_B_ -= this->sum_power_B_ / calibration_count_F;
      this->software_current_offset_A_ = CALIBRATION_CURRENT_A_B_SCALE_FACTOR * this->software_current_offset_B_;
      this->software_power_offset_A_ = CALIBRATION_POWER_A_B_SCALE_FACTOR * this->software_power_offset_B_;
      /* // offsets registers  : works fine with channel B but channel A bias is to large on Sonoff POWCT
      * // (> max uint16_t data) and can't be corrected with the cse7761 offset registers. Or I did not find
      * // the way at this moment.
      * this->write_(CSE7761_SPECIAL_COMMAND, CSE7761_CMD_ENABLE_WRITE);
      *
      *       uint8_t sys_status = this->read_(CSE7761_REG_SYSSTATUS, 1);
      *       if (sys_status & 0x10) {
      *         this->write_(CSE7761_REG_RMS_IB_OFFSET | 0x80, offset_I_reg_B);
      *         this->write_(CSE7761_REG_RMS_IA_OFFSET | 0x80, offset_I_reg_A);
      *         this->write_(CSE7761_REG_POWER_PB_OFFSET | 0x80, offset_P_reg_B);
      *         this->write_(CSE7761_REG_POWER_PA_OFFSET | 0x80, offset_P_reg_A);
      *         this->write_(CSE7761_SPECIAL_COMMAND, CSE7761_CMD_CLOSE_WRITE);
      *         this->calibration_done_ = true;
      *         ESP_LOGI(TAG, "New calibration done: BIAS_IA=%d BIAS_IB=%d BIAS_PA=%d BIAS_PB=%d",(int16_t) offset_I_reg_A,(int16_t) offset_I_reg_B,(int16_t) offset_P_reg_A,(int16_t) offset_P_reg_B);
      *       } else {
      *         ESP_LOGD(TAG, "Write failed at perform_calibration_write_t");
      *       }*/
      
      // Restart a new calibration cycle: to prevent time shift or external conditions dependences
      // (as temperature, humidity...etc...)
      this->calibration_count_ = 0;
      this->sum_current_B_ = 0;
      this->sum_power_B_ = 0;
    }

    //***********************************************************************************************
    // dump_config
    //***********************************************************************************************
    void CSE7761Component::dump_config() {
      ESP_LOGCONFIG(TAG, "CSE7761:");
      if (this->is_failed()) {
        ESP_LOGE(TAG, ESP_LOG_MSG_COMM_FAIL);
      }
      LOG_UPDATE_INTERVAL(this);
      this->check_uart_settings(38400, 1, uart::UART_CONFIG_PARITY_EVEN, 8);
    }

    //***********************************************************************************************
    // get_setup_priority
    //***********************************************************************************************
    float CSE7761Component::get_setup_priority() const { return setup_priority::DATA; }

    //***********************************************************************************************
    // update : measurements update
    //***********************************************************************************************
    void CSE7761Component::update() {
      if (this->data_.ready) {this->get_data_();}
    }

    //***********************************************************************************************
    // write_ : write data "data" to rgister "reg"
    // - uint8_t reg : register address
    // - uint16_t data : data to write
    //***********************************************************************************************
    void CSE7761Component::write_(uint8_t reg, uint16_t data) {
      uint8_t buffer[5];

      buffer[0] = 0xA5;
      buffer[1] = reg;
      uint32_t len = 2;
      if (data) {
        if (data < 0xFF) {
          buffer[2] = data & 0xFF;
          len = 3;
        } else {
          buffer[2] = (data >> 8) & 0xFF;
          buffer[3] = data & 0xFF;
          len = 4;
        }
        uint8_t crc = 0;
        for (uint32_t i = 0; i < len; i++) {
          crc += buffer[i];
        }
        buffer[len] = ~crc;
        len++;
      }

      this->write_array(buffer, len);
    }

    //***********************************************************************************************
    // read_once_ : try one register read
    // - uint8_t reg : register address
    // - uint8_t size : register size
    // - uint32_t *value : pointer to read data returned
    // TODO: add an enum with register sizes to simplify this function and all reading and writing
    //       functions.
    //***********************************************************************************************
    bool CSE7761Component::read_once_(uint8_t reg, uint8_t size, uint32_t *value) {
      while (this->available()) {
        this->read();
      }

      this->write_(reg, 0);

      uint8_t buffer[8] = {0};
      uint32_t rcvd = 0;

      for (uint32_t i = 0; i <= size; i++) {
        int value = this->read();
        if (value > -1 && rcvd < sizeof(buffer) - 1) {
          buffer[rcvd++] = value;
        }
      }

      if (!rcvd) {
        ESP_LOGD(TAG, "Received 0 bytes for register %hhu", reg);
        return false;
      }

      rcvd--;
      uint32_t result = 0;
      // CRC check
      uint8_t crc = 0xA5 + reg;
      for (uint32_t i = 0; i < rcvd; i++) {
        result = (result << 8) | buffer[i];
        crc += buffer[i];
      }
      crc = ~crc;
      if (crc != buffer[rcvd]) {
        return false;
      }

      *value = result;
      return true;
    }

    //***********************************************************************************************
    // read_ : read register data by trying 3 times max to call read_once_
    // - uint8_t reg : register address
    // - uint8_t size : register size
    // return uint32_t value : data read in register
    // TODO: add an enum with register sizes to simplify this function and all reading and writing
    //       functions.
    //***********************************************************************************************
    uint32_t CSE7761Component::read_(uint8_t reg, uint8_t size) {
      bool result = false;  // Start loop
      uint8_t retry = 3;    // Retry up to three times
      uint32_t value = 0;   // Default no value
      while (!result && retry > 0) {
        retry--;
        if (this->read_once_(reg, size, &value))
          return value;
      }
      ESP_LOGE(TAG, "Reading register %hhu failed!", reg);
      return value;
    }

    //***********************************************************************************************
    // coefficient_by_unit_ : coef to convert row measurements
    // - uint32_t unit : index of measurements, see enum CSE7761
    //***********************************************************************************************
    uint32_t CSE7761Component::coefficient_by_unit_(uint32_t unit) {
      switch (unit) {
        case RMS_IAC:
          return (0x800000 * 100 / this->data_.coefficient[RMS_IAC]) * 10;  // Stay within 32 bits
        case RMS_IBC:
          return (0x800000 * 100 / this->data_.coefficient[RMS_IBC]) * 10;  // Stay within 32 bits
        case RMS_UC:
          return 0x400000 * 100 / this->data_.coefficient[RMS_UC];
        case POWER_PAC:
          return 0x80000000 / this->data_.coefficient[POWER_PAC];
        case POWER_PBC:
          return 0x80000000 / this->data_.coefficient[POWER_PBC];
          // TODO: to verify the folowing formulas are OK with CSE7761 datasheet
        case POWER_SC:
          return 0x80000000 / this->data_.coefficient[POWER_SC];
        case ENERGY_AC:
          return 0x80000000 / this->data_.coefficient[ENERGY_AC];
        case ENERGY_BC:
          return 0x80000000 / this->data_.coefficient[ENERGY_BC];
      }
      return 0;
    }

    //***********************************************************************************************
    // chip_init_ : init all the sys_status registers of the chip to make
    // return TRUE if OK, else return FALSE
    // TODO: link the configuration choices with external trigers and be able to change then at any
    //       time
    //***********************************************************************************************
    bool CSE7761Component::chip_init_() {
      uint16_t calc_chksum = 0xFFFF;
      for (uint32_t i = 0; i < 8; i++) {
        // les adresses des 8 registres se suivent -> adressse du premier + i
        this->data_.coefficient[i] = this->read_(CSE7761_REG_RMSIAC + i, 2);
        calc_chksum += this->data_.coefficient[i];
      }
      calc_chksum = ~calc_chksum;
      uint16_t coeff_chksum = this->read_(CSE7761_REG_COEFFCHKSUM, 2);
      if ((calc_chksum != coeff_chksum) || (!calc_chksum)) {
        ESP_LOGD(TAG, "Default calibration");
        this->data_.coefficient[RMS_IAC] = CSE7761_IREF;
        this->data_.coefficient[RMS_UC] = CSE7761_UREF;
        this->data_.coefficient[POWER_PAC] = CSE7761_PREF;
      }

      this->write_(CSE7761_SPECIAL_COMMAND, CSE7761_CMD_ENABLE_WRITE);

      uint8_t sys_status = this->read_(CSE7761_REG_SYSSTATUS, 1);
      if (sys_status & 0x10) {  // Write enable to protected registers (WREN)
        this->write_(CSE7761_REG_SYSCON | 0x80, 0xFF04);
        // old values: unsigned power, no frequency
        //this->write_(CSE7761_REG_EMUCON | 0x80, 0x1183);
        //this->write_(CSE7761_REG_EMUCON2 | 0x80, 0x0FC1);
        // signed power, no frequency
        this->write_(CSE7761_REG_EMUCON | 0x80, 0x1583);
        this->write_(CSE7761_REG_EMUCON2 | 0x80, 0x0FC1);
        // signed power + frequency (does not work, tension signal must be too durty)
        //this->write_(CSE7761_REG_EMUCON | 0x80, 0x1D83);
        //this->write_(CSE7761_REG_EMUCON2 | 0x80, 0x8FC1);

        this->write_(CSE7761_REG_PULSE1SEL | 0x80, 0x3290);
      } else {
        ESP_LOGD(TAG, "Write failed at chip_init");
        return false;
      }
      return true;
    }

    //***********************************************************************************************
    // get_data_ : get measurements from chip and convert them to USI units
    // TODO: get datas according to chip configuration and user choices (ex only channel A, frequency)
    //       to reduce unnecessaries register reads
    //***********************************************************************************************
    void CSE7761Component::get_data_() {
      uint32_t uvalue;
      int32_t svalue;

      // The effective value of current and voltage Rms is a 24-bit signed number,
      // the highest bit is 0 for valid data, <-- NO : it is the sign bit on 24 bits for current
      //   and when the highest bit is 1, the reading will be processed as zero
      //   <-- and NO : it will be value|0xff00000000 to have the corresponding signed int32_t
      // The active power parameter PowerA/B is in two’s complement format, 32-bit
      // data, the highest bit is Sign bit.

      // TODO: add a new class member this->voltage, open the sonoff, connect it to serial
      // without ac power and measure the noise to calibrate the tension
      uvalue = this->read_(CSE7761_REG_RMSU, 3);
      this->data_.voltage_rms = (uvalue >= 0x800000) ? 0 : uvalue;
      float voltage = (float) this->data_.voltage_rms / this->coefficient_by_unit_(RMS_UC);
      if (this->voltage_sensor_ != nullptr) {
        this->voltage_sensor_->publish_state(voltage);
      }

      svalue = this->read_(CSE7761_REG_RMSIA, 3);
      this->data_.current_rms[0] = (svalue&0x800000)?svalue|0xFF000000:svalue;
      this->active_current_A_ = (((float) this->data_.current_rms[0]) / this->coefficient_by_unit_(RMS_IAC))/std::numbers::pi+this->software_current_offset_A_;
      if (this->current_sensor_1_ != nullptr) {
        this->current_sensor_1_->publish_state(this->active_current_A_);
      }

      svalue = this->read_(CSE7761_REG_RMSIB, 3);
      this->data_.current_rms[1] = (svalue&0x800000)?svalue|0xFF000000:svalue;
      this->active_current_B_ = (((float) this->data_.current_rms[1]) / this->coefficient_by_unit_(RMS_IBC))/std::numbers::pi+this->software_current_offset_B_;
      if (this->current_sensor_2_ != nullptr) {
        this->current_sensor_2_->publish_state(this->active_current_B_);
      }
/* TODO: make a debug function to print bytes in hex or binary
 *      ESP_LOGD(TAG, "Différence des intensités brutes à vide %d", this->data_.current_rms[0]-this->data_.current_rms[1]);
 *       ESP_LOGD(TAG, "Rapport des intensités brutes à vide %f", (float) this->data_.current_rms[0] / (float) this->data_.current_rms[1]);
 *       std::stringstream ss_bin;
 *       uint32_t u32_value = (uint32_t)svalue;
 *       for (int j=3; j>=0; j--){
 *         uint8_t byte_value = uint8_t ((u32_value >> j*8) & 0xFF);
 *         for (int i = 7; i >= 0; --i) {
 *           ss_bin << ((byte_value >> i) & 1);
 *         }
 *         ss_bin << " ";
 *       }
 *       ESP_LOGD(TAG, "Channel 2 I RAW VALUE: %s", ss_bin.str().c_str());*/

//      uvalue = this->read_(CSE7761_REG_UFREQ, 2);
//      this->data_.frequency = (uvalue >= 0x8000) ? 0 : uvalue;
//      svalue = this->read_(CSE7761_REG_ANGLE, 2);
//      this->data_.angle = (svalue >= 0x8000) ? 0 : svalue;
//      float frequency = 3579545/8/((float) this->data_.frequency);
//      float angle = (float) (frequency-50 < frequency-60) ? (0.0805*(float) this->data_.angle)  : (0.0965*(float) this->data_.angle);

      uint32_t now = esphome::millis();
      svalue = this->read_(CSE7761_REG_POWERPA, 4);
      this->data_.active_power[0] = (int32_t) svalue;
      this->active_power_A_ = (((float) this->data_.active_power[0]) / this->coefficient_by_unit_(POWER_PAC))/std::numbers::pi+this->software_power_offset_A_;
      if (this->power_sensor_1_ != nullptr) {
        this->power_sensor_1_->publish_state(this->active_power_A_);
      }
      if (!this->ok_energy_){
        this->last_active_power_A_ = this->active_power_A_;
        this->last_update_time_ = (double) now;
        this->ok_energy_ = true;
      }
      else{
        double time_delta_s = ((double)now - this->last_update_time_) / 1000.0f;
        this->last_update_time_ = (double) now;
        double mean_power = (this->last_active_power_A_ + this->active_power_A_) / 2.0f;
        this->last_active_power_A_ = this->active_power_A_;
        // Energy = Power (W) * Delta Time (s) / 3600 (s/h) = Wh
        if (mean_power > 0.0f){
          this->accumulated_energy_received_ += (mean_power * time_delta_s) / 3600.0f;
          if (this->energy_received_ != nullptr) {
            this->energy_received_->publish_state(this->accumulated_energy_received_ / 1000.0f); // Publish in kWh
          }
        }
        else{ //mean_power <= 0.0f
          this->accumulated_energy_exported_ += (mean_power * time_delta_s) / 3600.0f;
          if (this->energy_exported_ != nullptr) {
            this->energy_exported_->publish_state(this->accumulated_energy_exported_ / 1000.0f); // Publish in kWh
          }
        }
      }


      svalue = this->read_(CSE7761_REG_POWERPB, 4);
      this->data_.active_power[1] = (int32_t) svalue; // mesure du bruit
      this->active_power_B_ = (((float) this->data_.active_power[1]) / this->coefficient_by_unit_(POWER_PBC))/std::numbers::pi+this->software_power_offset_B_;
      if (this->power_sensor_2_ != nullptr) {
        this->power_sensor_2_->publish_state(this->active_power_B_);
      }

/* TODO: make a debug function to print bytes in hex or binary
 *       ss_bin.str("");
 *       u32_value = (uint32_t)svalue;
 *       for (int j=3; j>=0; j--){
 *         uint8_t byte_value = uint8_t ((u32_value >> j*8) & 0xFF);
 *         for (int i = 7; i >= 0; --i) {
 *           ss_bin << ((byte_value >> i) & 1);
 *         }
 *         ss_bin << " ";
 *       }
 *       ESP_LOGD(TAG, "Channel 2 P RAW VALUE: %s", ss_bin.str().c_str());
 */
/* TODO: make a function to collect datas and calculate new personnal coef values
 *       // logs to make calibration study
 *       ESP_LOGD(TAG, "Différence des puissances brutes à vide %d", this->data_.active_power[0]-this->data_.active_power[1]);
 *       ESP_LOGD(TAG, "Rapport des puissances brutes à vide %f", (float) this->data_.active_power[0] / (float) this->data_.active_power[1]);*/

      if (this->calibration_enabled_) {
        // calibrating
        if (this->calibration_count_ == 0) {
          this->sum_current_B_ = 0;
          this->sum_power_B_ = 0;
        }
        // channel B always idle -> used for calibration
        this->sum_current_B_ += this->active_current_B_;
        this->sum_power_B_ += this->active_power_B_;
        this->calibration_count_++;

        if (this->calibration_count_ >= CALIBRATION_MEASUREMENTS) {
          this->perform_calibration_write_();
        }
      }


      // save every 12 hours
      if (this->last_save_time_ == 0 || (esphome::millis() - this->last_save_time_) >= 3600000*12) {
        this->last_save_time_ = esphome::millis();

        // Utilisation de la structure pour la sauvegarde
        EnergyDataStruct current_values = {
          .received = this->accumulated_energy_received_,
          .exported = this->accumulated_energy_exported_
        };
        this->pref_.save(&current_values);
        ESP_LOGV(TAG, "Saving accumulated energy: %.3f Wh (R), %.3f Wh (E)", this->accumulated_energy_received_, this->accumulated_energy_exported_);
      }

    }

    //***********************************************************************************************
    // read_register_service : advanced debug function to read registers and push datas in
    // home assistant entities. Make debug easier without recompile the code several times.
    // - std::string register_number_str: register number come as a string from home assistant
    // - int size : register size
    // TODO: add an enum with register sizes to simplify this function and all reading and writing
    //       functions.
    //***********************************************************************************************
    void CSE7761Component::read_register_service(std::string register_number_str, int size) {
      // Optionnel : Loguer l'appel
      ESP_LOGD(TAG, "Service appelé: Lecture du registre %s sur %d octets.", register_number_str, size);

      uint32_t register_number;
      char *end_ptr;
      unsigned long val = std::strtoul(register_number_str.c_str(), &end_ptr, 0);
      if (end_ptr == register_number_str.c_str() || *end_ptr != '\0') {
        ESP_LOGE(TAG, "Erreur: Entrée de registre invalide ou non-numérique: '%s'", register_number_str.c_str());
        this->debug_sensor_hex_->publish_state("Erreur: Format d'entrée invalide.");
        this->debug_sensor_bin_->publish_state("Erreur: Format d'entrée invalide.");
        return;
      }
      if (val > 0xFFFFFFFF) {
        ESP_LOGE(TAG, "Erreur: Le registre est trop grand (hors de la plage 32-bit)");
        this->debug_sensor_hex_->publish_state("Erreur: Registre hors plage.");
        this->debug_sensor_bin_->publish_state("Erreur: Registre hors plage.");
        return;
      }
      register_number = (uint32_t)val;

      std::vector<uint8_t> raw_data = read_register(register_number, size);

      std::stringstream ss_hex,ss_bin;
      for (uint8_t byte_value : raw_data){
        // hex
        ss_hex << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
        << (int)byte_value << " ";
        // bin
        for (int i = 7; i >= 0; --i) {
          ss_bin << ((byte_value >> i) & 1);
        }
        ss_bin << " ";
      }

      std::string hex_data = ss_hex.str();
      std::string bin_data = ss_bin.str();
      ESP_LOGI(TAG, "Contenu du registre 0x%X: %s=%s", register_number, hex_data.c_str(), bin_data.c_str());
      if (debug_sensor_hex_) {
        debug_sensor_hex_->publish_state(hex_data);
      }
      if (debug_sensor_bin_) {
        debug_sensor_bin_->publish_state(bin_data);
      }
    }

    //***********************************************************************************************
    // read_register : yet another read register function espacialy used to debug
    // TODO: add an enum with register sizes to simplify this function and all reading and writing
    //       functions.
    //***********************************************************************************************
    std::vector<uint8_t> CSE7761Component::read_register(int register_number, int size) {

      // ÉTAPE PRÉPARATOIRE : Vider le tampon UART (comme dans read_once_)
      while (this->available()) {
        this->read();
      }

      // ÉTAPE 1: ENVOI DE LA COMMANDE DE LECTURE
      // Le CSE7761 utilise 0xA5 + reg + data + CRC.
      this->write_(register_number, 0); // Utilise la méthode d'écriture existante

      // ÉTAPE 2: LECTURE BRUTE
      std::vector<uint8_t> data;
      data.reserve(size + 1); // Réserve pour les octets de données + 1 octet de CRC

      // Le CSE7761 lit (taille + 1) octets : 'size' octets de données + 1 octet de CRC
      for (int i = 0; i <= size; i++) {
        // Lire immédiatement, sans temporisation, car la lecture est bloquante dans la boucle.
        int read_result = this->read();

        if (read_result == -1) {
          // Lecture incomplète ou tampon vide
          ESP_LOGW(TAG, "CSE7761: Lecture incomplète. %u octets lus sur %d demandés pour 0x%X.", data.size(), size + 1, register_number);
          return {}; // Retourne un vecteur vide
        }

        data.push_back((uint8_t)read_result);
      }

      // Si la lecture s'est bien passée, data.size() doit être égal à (size + 1)
      if (data.empty() || data.size() != (size + 1)) {
        ESP_LOGE(TAG, "CSE7761: Erreur de lecture de la trame. Taille: %u (attendue %d)", data.size(), size + 1);
        return {};
      }

      // ÉTAPE 3: VÉRIFICATION DU CRC (similaire à read_once_)
      // Le dernier octet est le CRC. Il est stocké à data[size].
      uint8_t received_crc = data[size];
      uint8_t calculated_crc = 0xA5 + register_number;

      // Calcul du CRC sur tous les octets SAUF le dernier (l'octet de CRC reçu)
      for (int i = 0; i < size; i++) {
        calculated_crc += data[i];
      }

      calculated_crc = ~calculated_crc; // Inversion du résultat

      if (calculated_crc != received_crc) {
        ESP_LOGE(TAG, "CSE7761: Erreur CRC pour 0x%X. Calculé: 0x%02X, Reçu: 0x%02X",
                 register_number, calculated_crc, received_crc);
        return {}; // Retourne un vecteur vide en cas d'échec
      }

      // ÉTAPE 4: RETOURNER LES DONNÉES UTILES
      // Le CRC est valide. On retire l'octet de CRC du vecteur avant de le retourner.
      data.pop_back(); // Supprime le dernier élément (le CRC)

      ESP_LOGI(TAG, "Lecture réussie du registre 0x%X. %u octets de données.", register_number, data.size());
      return data;
    }
    
    //***********************************************************************************************
    // write_register_service : home assistant service to write data to register
    // - std::string register_number_str
    // - std::string value_str
    // TODO: add an enum with register sizes to simplify this function and all reading and writing
    //       functions.
    //***********************************************************************************************
    void CSE7761Component::write_register_service(std::string register_number_str, std::string value_str) {
      ESP_LOGD(TAG, "Service appelé: Écriture du registre %s avec la valeur %s.", register_number_str, value_str);

      uint8_t register_number;
      uint16_t value;

      // --- 1. Traitement de l'adresse du registre ---
      char *end_ptr_reg;
      unsigned long reg_val = std::strtoul(register_number_str.c_str(), &end_ptr_reg, 0); // La base 0 permet auto-détection (0x pour hex)
      if (end_ptr_reg == register_number_str.c_str() || *end_ptr_reg != '\0' || reg_val > 0xFF) {
        ESP_LOGE(TAG, "Erreur: Adresse de registre invalide ou hors plage (0-0xFF): '%s'", register_number_str.c_str());
        if (this->debug_sensor_hex_) {
            this->debug_sensor_hex_->publish_state("Erreur: Adresse de registre invalide.");
        }
         if (this->debug_sensor_bin_) {
            this->debug_sensor_bin_->publish_state("Erreur: Adresse de registre invalide.");
        }
       return;
      }
      register_number = (uint8_t)reg_val;

      // --- 2. Traitement de la valeur à écrire (16-bit) ---
      char *end_ptr_val;
      unsigned long val_to_write = std::strtoul(value_str.c_str(), &end_ptr_val, 0);
      if (end_ptr_val == value_str.c_str() || *end_ptr_val != '\0' || val_to_write > 0xFFFF) {
        ESP_LOGE(TAG, "Erreur: Valeur d'écriture invalide ou hors plage (0-0xFFFF): '%s'", value_str.c_str());
        if (this->debug_sensor_hex_) {
            this->debug_sensor_hex_->publish_state("Erreur: Valeur d'écriture invalide.");
        }
        if (this->debug_sensor_bin_) {
            this->debug_sensor_bin_->publish_state("Erreur: Valeur d'écriture invalide.");
        }
        return;
      }
      value = (uint16_t)val_to_write;

      // --- 3. Exécution de l'écriture ---
     // Activer l'écriture sur les registres protégés (si nécessaire)
      this->write_(CSE7761_SPECIAL_COMMAND, CSE7761_CMD_ENABLE_WRITE);
      
      // Effectuer l'écriture. On suppose que l'utilisateur inclura 0x80 si c'est un registre protégé.
      // Si l'utilisateur entre "0" pour le registre, il ne se passe rien (data est 0).
      this->write_(register_number | 0x80, value);
      
      // Désactiver l'écriture
      this->write_(CSE7761_SPECIAL_COMMAND, CSE7761_CMD_CLOSE_WRITE);

      // --- 4. Log et publication du résultat ---
      std::stringstream ss;
      ss << std::uppercase << std::hex << "OK: Écrit 0x" << std::setw(4) << std::setfill('0')
         << value << " dans le registre 0x" << std::setw(2) << std::setfill('0')
         << (int)register_number;

      std::string result_msg = ss.str();
      ESP_LOGI(TAG, "%s", result_msg.c_str());
      if (this->debug_sensor_hex_) {
        this->debug_sensor_hex_->publish_state(result_msg);
      }
      if (this->debug_sensor_bin_) {
        this->debug_sensor_bin_->publish_state(result_msg);
      }
    }    

  }  // namespace cse7761
}  // namespace esphome
