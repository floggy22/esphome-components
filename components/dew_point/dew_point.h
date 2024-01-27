#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace dew_point {

/// This class implements calculation of dew point from temperature and relative humidity.
class DewPointComponent : public sensor::Sensor, public Component {
 public:
  DewPointComponent() = default;

  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { this->temperature_sensor_ = temperature_sensor; }
  void set_humidity_sensor(sensor::Sensor *humidity_sensor) { this->humidity_sensor_ = humidity_sensor; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void loop() override;

 protected:
  void temperature_callback_(float state) {
    this->next_update_ = true;
    this->temperature_ = state;
  }
  void humidity_callback_(float state) {
    this->next_update_ = true;
    this->humidity_ = state;
  }

  /** Calculate dew point in °C
   *
   * @param temperature in -45 to 60 °C 
   * @param humidity humidity in 0-100%   
   */
  static float get_dew_point(float temperature, float humidity);

  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};

  bool next_update_{false};

  float temperature_{NAN};
  float humidity_{NAN};  
};

}  // namespace dew_point
}  // namespace esphome
