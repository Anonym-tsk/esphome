#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace mhz19 {

enum MHZ19ABCLogic {
  MHZ19_ABC_NONE = 0,
  MHZ19_ABC_ENABLED,
  MHZ19_ABC_DISABLED
};

class MHZ19Component : public PollingComponent, public uart::UARTDevice {
 public:
  float get_setup_priority() const override;

  void setup() override;
  void update() override;
  void dump_config() override;

  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_co2_sensor(sensor::Sensor *co2_sensor) { co2_sensor_ = co2_sensor; }
  void set_abc_enabled(bool abc_enabled) { abc_logic_ = abc_enabled ? MHZ19_ABC_ENABLED : MHZ19_ABC_DISABLED; }

 protected:
  bool mhz19_write_command_(const uint8_t *command, uint8_t *response);

  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *co2_sensor_{nullptr};
  MHZ19ABCLogic abc_logic_{MHZ19_ABC_NONE};
};

}  // namespace mhz19
}  // namespace esphome
