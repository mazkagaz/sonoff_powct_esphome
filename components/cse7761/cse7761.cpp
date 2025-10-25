#include "cse7761.h"

#include "esphome/core/log.h"

#include <sstream>
#include <iomanip>
#include <inttypes.h> // Nécessaire pour les macros PRIu64 et PRId64

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

#ifdef CALIBRATION_CODE
    // registres des offsets
    static const uint8_t CSE7761_REG_POWER_PA_OFFSET = 0x0A; // PowerPAOS (Active Power Offset - 32bit)
    static const uint8_t CSE7761_REG_POWER_PB_OFFSET = 0x0B; // PowerPAOS (Active Power Offset - 32bit)
    static const uint8_t CSE7761_REG_RMS_IA_OFFSET = 0x0E; // RmsIAOS (Current RMS Offset - 16bit)
    static const uint8_t CSE7761_REG_RMS_IB_OFFSET = 0x0F; // RmsIAOS (Current RMS Offset - 16bit)

    // Définir le nombre de mesures de calibration
    static const int CALIBRATION_MEASUREMENTS = 10;
#endif

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
    // Fonction setup : mise en route du composant
    //***********************************************************************************************
    void CSE7761Component::setup() {
      // reset du cse7761
      this->write_(CSE7761_SPECIAL_COMMAND, CSE7761_CMD_RESET);
      // vérification du reset et initialisation this->chip_init_()
      uint16_t syscon = this->read_(0x00, 2);  // Default 0x0A04
      if ((0x0A04 == syscon) && this->chip_init_()) {
        // composant trouvé et initialisé, arrêt des écritures sur les registres
        this->write_(CSE7761_SPECIAL_COMMAND, CSE7761_CMD_CLOSE_WRITE);
        ESP_LOGD(TAG, "CSE7761 found");
        // composant prêt à communiquer
        this->data_.ready = true;
      } else {
        // composant non trouvé ou erreur d'initialisation
        this->mark_failed();
      }
    }

#ifdef CALIBRATION_CODE
    //***********************************************************************************************
    // Fonction perform_calibration_write_ : vise à calibrer les offset de mesure en utilisant le
    // bruit mesuré sur le channel B qui est à vide.
    // Pour l'instant ça ne marche pas car je n'arrive pas à écrire dans les registres d'offset
    // et je ne sais pas pourquoi ça ne fonctionne pas.
    // Lorsque ça fonctionnera, au lieu d'écrire dans les offset du channel B, je laisserai le 
    // channel B avec son bruit et je corrigerai périodiquement l'offset du channel A.
    // Pourquoi périodiquement :  parce que l'offset est probablement sensible aux conditions
    // d'utilisation de la puce (température, etc...). Donc recalibrer périodiquement peut permettre
    // d'éliminer les fluctuations d'offset.
    // Pourquoi calibrer alors que ce sont des valeurs qui peuvent être jugées négligeables pour
    // la mesure : parce que je voudrais mesurer la fréquence du signal qui est très sensible au
    // bruit et que pour l'instant je n'arrive pas à obtenir cette mesure. J'émets l'hypothèse,
    // peut-être fausse, que le signal de tension est trop bruité pour l'instant.
    // De plus l'offset n'est pas si négligeable : retourner le tore de mesure du courant sur de 
    // faibles puissances permet de constater que ce décalage est tout à fait mesurable.
    // Cet offset pourrait être corrigé de façon logicielle mais cela ne gèglerait pas les erreurs
    // internes à tous les calculs faits directement dans la puce.
    //***********************************************************************************************
    void CSE7761Component::perform_calibration_write_() {
      if (this->calibration_done_){return;}
      
      ESP_LOGI(TAG, "Calibration de l'offset terminée. Calcul et écriture des offsets du CANAL B.");

      ESP_LOGI(TAG, "DEBUG: Sums (Count=%u): Current_B_Sum=%" PRIu64 ", Power_B_Sum=%" PRId64,
               this->calibration_count_, this->sum_current_B_, this->sum_power_B_);

      // --- Calcul des Moyennes Flottantes ---
      float calibration_count_F = (float)this->calibration_count_; // 50.0f
      double avg_current = (double)this->sum_current_B_ / calibration_count_F;
      double avg_power = (double)this->sum_power_B_ / calibration_count_F;

      // 1. COURANT B (RmsIBOS)
      // L'offset est l'opposé de la moyenne, divisé par le facteur 256.0f (CSE7761 datasheet pour 24 bits -> 16 bits)
      // Le résultat doit être casté en int16_t avant l'écriture.
      // avg_current est 2152
      int16_t rmsibos = (int16_t)round(-(avg_current / 256.0f));

      // 2. PUISSANCE B (PBOS)
      // L'offset est l'opposé de la moyenne, divisé par le facteur 16.0f (CSE7761 datasheet pour 32 bits -> 16 bits)
      // avg_power est -36239
      int16_t pbos = (int16_t)round(-(avg_power / 16.0f));

      // --- Préparation pour l'écriture (l'écriture UART utilise des uint16_t) ---
      uint16_t offset_I_reg = (uint16_t)rmsibos;
      uint16_t offset_P_reg = (uint16_t)pbos;

      ESP_LOGI(TAG, "DEBUG: Averages: I_RAW=0x%08X (Dec %.0f), P_RAW=%d (Dec %.0f)",
               (uint32_t)avg_current, avg_current, (int32_t)avg_power, avg_power);
      ESP_LOGI(TAG, "Valeurs de correction 16-bit écrites: RmsIBOS(0x0F)=0x%04X, PBOS(0x0B)=0x%04X", offset_I_reg, offset_P_reg);

      this->write_(CSE7761_SPECIAL_COMMAND, CSE7761_CMD_ENABLE_WRITE);

      uint8_t sys_status = this->read_(CSE7761_REG_SYSSTATUS, 1);
      if (sys_status & 0x10) {  // Write enable to protected registers (WREN)
        ESP_LOGD(TAG, "Valeurs de correction avant modification RmsIBOS(0x0F)=0x%04X", this->read_(CSE7761_REG_RMS_IB_OFFSET,2));
        ESP_LOGD(TAG, "Valeurs de correction avant modification RmsIBOS(0x0F)=0x%04X", this->read_(CSE7761_REG_POWER_PB_OFFSET,2));
        // 1. Courant : RmsIBOS (0x0F) - 16-bit
        this->write_(CSE7761_REG_RMS_IB_OFFSET, offset_I_reg);
        // 2. Puissance : PBOS (0x0B) - 16-bit
        this->write_(CSE7761_REG_POWER_PB_OFFSET, offset_P_reg);
        delay(100);
        
        ESP_LOGD(TAG, "Valeurs de correction après modification RmsIBOS(0x0F)=0x%04X", this->read_(CSE7761_REG_RMS_IB_OFFSET,2));
        ESP_LOGD(TAG, "Valeurs de correction après modification RmsIBOS(0x0F)=0x%04X", this->read_(CSE7761_REG_POWER_PB_OFFSET,2));
        this->write_(CSE7761_SPECIAL_COMMAND, CSE7761_CMD_CLOSE_WRITE);
        
        // Marquer la calibration comme effectuée
        this->calibration_done_ = true;
        ESP_LOGI(TAG, "TEST de correction du Canal B terminé. I2 et P2 devraient être proches de zéro.");
      } else {
        ESP_LOGD(TAG, "Write failed at perform_calibration_write_t");
      }
      
      // Relancer le processus de calibration adaptatif
      this->calibration_count_ = 0;
    }
#endif

    //***********************************************************************************************
    // Fonction dump_config : ???
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
    // Fonction get_setup_priority : ???
    //***********************************************************************************************
    float CSE7761Component::get_setup_priority() const { return setup_priority::DATA; }
    // TODO: vérifier si le changement de priorité est nécessaire ou non pour le service debug
    // float CSE7761Component::get_setup_priority() const {
    //   //return setup_priority::DATA;
    //   return 999.0f;
    // }

    //***********************************************************************************************
    // Fonction update : mise à jour des données de mesure du composant
    //***********************************************************************************************
    void CSE7761Component::update() {
      if (this->data_.ready) {this->get_data_();}
    }

    //***********************************************************************************************
    // Fonction write_ : écriture de data dans le registre reg
    // - uint8_t reg : adresse du registre
    // - uint16_t data : donnée à écrire
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
    // Fonction read_once_ : lecture de value de taille size dans le registre reg
    // - uint8_t reg : adresse du registre à lire
    // - uint8_t size : taille du registre à lire
    // - uint32_t *value : pointeur vers la valeur lue
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
    // Fonction read_ : lecture de value de taille size dans le registre reg
    // Appelle read_once_ au plus 3 fois jusqu'à ce que la lecture soit correcte'
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
    // Fonction coefficient_by_unit_ : retour le coefficient pour convertir la donnée brute
    // - uint32_t unit : type de donnée à convertir
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
          // TODO: vérifier les calculs des coefs suivants dans la doc
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
    // Fonstion chip_init_ : initialisation de la puce
    //***********************************************************************************************
    bool CSE7761Component::chip_init_() {
      uint16_t calc_chksum = 0xFFFF;
      // lecture et stockage des 8 coefs de conversion IAC IBC UC PAC PBC PSC EAC EBC
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
        // ancienne valeur : puissance non signée, pas de frequence
        //this->write_(CSE7761_REG_EMUCON | 0x80, 0x1183);
        //this->write_(CSE7761_REG_EMUCON2 | 0x80, 0x0FC1);
        // puissance signée, pas de frequence
        this->write_(CSE7761_REG_EMUCON | 0x80, 0x1583);
        this->write_(CSE7761_REG_EMUCON2 | 0x80, 0x0FC1);
        // puissance signée + fréquence (ne fonctionne que si le signal de tension est "propre")
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
    // Fonstion get_data_ : lecture des mesures
    //***********************************************************************************************
    void CSE7761Component::get_data_() {
      uint32_t uvalue;
      int32_t svalue;
#ifdef CALIBRATION_CODE
      // calibrating
      if (this->calibration_count_ == 0) {
        this->sum_current_B_ = 0;
        this->sum_power_B_ = 0;
      }
      // channel B not used for mesurements -> used for calibrating
      uvalue = this->read_(CSE7761_REG_RMSIB, 3);
      this->sum_current_B_ += uvalue;
      svalue = (int32_t)this->read_(CSE7761_REG_POWERPB, 4);
      this->sum_power_B_ += svalue;
      this->calibration_count_++;
      ESP_LOGD(TAG, "CAL_ACCUM: Mesure %u. Sommes I=%" PRIu64 ", P=%" PRId64,
               this->calibration_count_,
               this->sum_current_B_,
               this->sum_power_B_);

      if (this->calibration_count_ >= CALIBRATION_MEASUREMENTS) {
        this->perform_calibration_write_();
      }
#endif

      // The effective value of current and voltage Rms is a 24-bit signed number,
      // the highest bit is 0 for valid data,
      //   and when the highest bit is 1, the reading will be processed as zero
      // The active power parameter PowerA/B is in two’s complement format, 32-bit
      // data, the highest bit is Sign bit.
      uvalue = this->read_(CSE7761_REG_RMSU, 3);
      this->data_.voltage_rms = (uvalue >= 0x800000) ? 0 : uvalue;

      svalue = this->read_(CSE7761_REG_RMSIA, 3);
      this->data_.current_rms[0] = ((svalue >= 0x800000) || (svalue < 1600)) ? 0 : svalue;  // No load threshold of 10mA
//      uvalue = this->read_(CSE7761_REG_UFREQ, 2);
//      ESP_LOGD(TAG, "frequency data %x", uvalue);
//      this->data_.frequency = (uvalue >= 0x8000) ? 0 : uvalue;
//      svalue = this->read_(CSE7761_REG_ANGLE, 2);
//      ESP_LOGD(TAG, "angle data %x", svalue);
//      this->data_.angle = (svalue >= 0x8000) ? 0 : svalue;

      svalue = this->read_(CSE7761_REG_POWERPA, 4);
      this->data_.active_power[0] = (0 == this->data_.current_rms[0]) ? 0 : ((int32_t) svalue);

      // convert values and publish to sensors

      float voltage = (float) this->data_.voltage_rms / this->coefficient_by_unit_(RMS_UC);
      if (this->voltage_sensor_ != nullptr) {
        this->voltage_sensor_->publish_state(voltage);
      }

//      float frequency = 3579545/8/((float) this->data_.frequency);
//      float angle = (float) (frequency-50 < frequency-60) ? (0.0805*(float) this->data_.angle)  : (0.0965*(float) this->data_.angle);
//      ESP_LOGD(TAG, "frequency %f Hz, angle %f °", frequency, angle);

      for (uint8_t channel = 0; channel < 2; channel++) {
        // Active power = PowerPA * PowerPAC * 1000 / 0x80000000
        float active_power = (float) this->data_.active_power[channel] / this->coefficient_by_unit_(POWER_PAC);  // W
        float amps = (float) this->data_.current_rms[channel] / this->coefficient_by_unit_(RMS_IAC);             // A
        ESP_LOGD(TAG, "Channel %d power %f W, current %f A", channel + 1, active_power, amps);
        if (channel == 0) {
          if (this->power_sensor_1_ != nullptr) {
            this->power_sensor_1_->publish_state(active_power);
          }
          if (this->current_sensor_1_ != nullptr) {
            this->current_sensor_1_->publish_state(amps);
          }
        } else if (channel == 1) {
          if (this->power_sensor_2_ != nullptr) {
            this->power_sensor_2_->publish_state(active_power);
          }
          if (this->current_sensor_2_ != nullptr) {
            this->current_sensor_2_->publish_state(amps);
          }
        }
      }
    }

    //***********************************************************************************************
    // Fonstion read_register_service : fonction de débogage avancée permettant de lire des
    // registres directement depuis homeassistant sans avoir besoin de rajouter des print
    // et de recompiler
    //***********************************************************************************************
    void CSE7761Component::read_register_service(std::string register_number_str, int size) {
      // Optionnel : Loguer l'appel
      ESP_LOGD(TAG, "Service appelé: Lecture du registre %s sur %d octets.", register_number_str, size);

      uint32_t register_number;
      char *end_ptr;
      unsigned long val = std::strtoul(register_number_str.c_str(), &end_ptr, 0);
      if (end_ptr == register_number_str.c_str() || *end_ptr != '\0') {
        ESP_LOGE(TAG, "Erreur: Entrée de registre invalide ou non-numérique: '%s'", register_number_str.c_str());
        this->debug_sensor_->publish_state("Erreur: Format d'entrée invalide.");
        return;
      }
      if (val > 0xFFFFFFFF) {
        ESP_LOGE(TAG, "Erreur: Le registre est trop grand (hors de la plage 32-bit)");
        this->debug_sensor_->publish_state("Erreur: Registre hors plage.");
        return;
      }
      register_number = (uint32_t)val;

      std::vector<uint8_t> raw_data = read_register(register_number, size);

      std::stringstream ss;
      for (uint8_t byte_value : raw_data){
//        // en hexa
//        ss << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
//        << (int)byte_value << " ";
        // en binaire
        for (int i = 7; i >= 0; --i) {
          ss << ((byte_value >> i) & 1);
        }
        ss << " ";
      }

      std::string hex_data = ss.str();
      ESP_LOGI(TAG, "Contenu du registre 0x%X: %s", register_number, hex_data.c_str());
      if (debug_sensor_) {
        debug_sensor_->publish_state(hex_data);
      }
    }

    //***********************************************************************************************
    // Fonstion read_register : réimplémentation de la lecture des registres à ma sauce
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

  }  // namespace cse7761
}  // namespace esphome
