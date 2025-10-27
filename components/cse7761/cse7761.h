#pragma once

#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/api/api_server.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"
#include <vector>


namespace esphome {
  namespace cse7761 {

    struct CSE7761DataStruct {
      uint16_t angle = 0;
      uint16_t frequency = 0;
      uint32_t voltage_rms = 0;
      int32_t current_rms[2] = {0};
      uint32_t energy[2] = {0};
      int32_t active_power[2] = {0};
      uint16_t coefficient[8] = {0};
      uint8_t energy_update = 0;
      bool ready = false;
    };

    /// This class implements support for the CSE7761 UART power sensor.
    class CSE7761Component : public PollingComponent, public uart::UARTDevice, public api::CustomAPIDevice {
    public:
      void set_voltage_sensor(sensor::Sensor *voltage_sensor) { voltage_sensor_ = voltage_sensor; }
      void set_active_power_1_sensor(sensor::Sensor *power_sensor_1) { power_sensor_1_ = power_sensor_1; }
      void set_current_1_sensor(sensor::Sensor *current_sensor_1) { current_sensor_1_ = current_sensor_1; }
      void set_active_power_2_sensor(sensor::Sensor *power_sensor_2) { power_sensor_2_ = power_sensor_2; }
      void set_current_2_sensor(sensor::Sensor *current_sensor_2) { current_sensor_2_ = current_sensor_2; }
      void setup() override;
      void dump_config() override;
      float get_setup_priority() const override;
      void update() override;
      // Setter pour le text_sensor qui affichera le résultat
      void set_debug_text_sensor_hex(text_sensor::TextSensor *debug_sensor_hex) { debug_sensor_hex_ = debug_sensor_hex; }
      void set_debug_text_sensor_bin(text_sensor::TextSensor *debug_sensor_bin) { debug_sensor_bin_ = debug_sensor_bin; }
      void read_register_service(std::string register_number_str, int size);
      void write_register_service(std::string register_number_str, std::string value_str);
      void set_calibration_mode(bool state);

    protected:
      // Sensors
      sensor::Sensor *voltage_sensor_{nullptr};
      sensor::Sensor *power_sensor_1_{nullptr};
      sensor::Sensor *current_sensor_1_{nullptr};
      sensor::Sensor *power_sensor_2_{nullptr};
      sensor::Sensor *current_sensor_2_{nullptr};
      CSE7761DataStruct data_;
      // Variables d'état pour la calibration
      bool calibration_enabled_{false};
      //bool calibration_done_{false};
      uint8_t calibration_count_{0};
      double sum_current_B_{0}; // Somme brute des valeurs de Courant B
      double sum_power_B_{0};    // Somme brute des valeurs de Puissance B
      double active_current_A_{0};
      double active_current_B_{0};
      double active_power_A_{0};
      double active_power_B_{0};
      double software_current_offset_A_{0};
      double software_current_offset_B_{0};
      double software_power_offset_A_{0};
      double software_power_offset_B_{0};

      void write_(uint8_t reg, uint16_t data);
      bool read_once_(uint8_t reg, uint8_t size, uint32_t *value);
      uint32_t read_(uint8_t reg, uint8_t size);
      uint32_t coefficient_by_unit_(uint32_t unit);
      bool chip_init_();
      void get_data_();
      // Capteurs pour afficher le résultat
      text_sensor::TextSensor *debug_sensor_hex_{nullptr};
      text_sensor::TextSensor *debug_sensor_bin_{nullptr};

      // Méthode lecture registre
      std::vector<uint8_t> read_register(int reg, int size);

      // calibration des offsets
      void perform_calibration_write_();
    };

  }  // namespace cse7761

}  // namespace esphome
