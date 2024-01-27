#include "esphome/core/log.h"
#include "dew_point.h"

namespace esphome {
namespace dew_point {

static const char *const TAG = "dew_point.sensor";

void DewPointComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Dew Point '%s'...", this->get_name().c_str())

  ESP_LOGD(TAG, "  Added callback for temperature '%s'", this->temperature_sensor_->get_name().c_str());
  this->temperature_sensor_->add_on_state_callback([this](float state) { this->temperature_callback_(state); });
  if (this->temperature_sensor_->has_state()) {
    this->temperature_callback_(this->temperature_sensor_->get_state());
  }

  ESP_LOGD(TAG, "  Added callback for relative humidity '%s'", this->humidity_sensor_->get_name().c_str());
  this->humidity_sensor_->add_on_state_callback([this](float state) { this->humidity_callback_(state); });
  if (this->humidity_sensor_->has_state()) {
    this->humidity_callback_(this->humidity_sensor_->get_state());
  }
}

void DewPointComponent::dump_config() {
  LOG_SENSOR("", "Dew Point", this);

  ESP_LOGCONFIG(TAG, "Sources");
  ESP_LOGCONFIG(TAG, "  Temperature: '%s'", this->temperature_sensor_->get_name().c_str());
  ESP_LOGCONFIG(TAG, "  Relative Humidity: '%s'", this->humidity_sensor_->get_name().c_str());
}

float DewPointComponent::get_setup_priority() const { return setup_priority::DATA; }

void DewPointComponent::loop() {
  if (!this->next_update_) {
    return;
  }
  this->next_update_ = false;

  // Ensure we have source data
  const bool no_temperature = std::isnan(this->temperature_);
  const bool no_humidity = std::isnan(this->humidity_);
  if (no_temperature || no_humidity) {
    if (no_temperature) {
      ESP_LOGW(TAG, "No valid state from temperature sensor!");
    }
    if (no_humidity) {
      ESP_LOGW(TAG, "No valid state from temperature sensor!");
    }
    ESP_LOGW(TAG, "Unable to calculate dew point.");
    this->publish_state(NAN);
    this->status_set_warning();
    return;
  }
  if (this->temperature_ > 60.0 || this->temperature_ < -45.0) {
    ESP_LOGW(TAG, "No valid temperature! Range from -45°C to 60°C");
    ESP_LOGW(TAG, "Unable to calculate dew point.");
    this->publish_state(NAN);
    this->status_set_warning();
    return;
  }

  // Calculate dew point
  const float dew_point = get_dew_point(this->temperature_, this->humidity_);

  // Publish dew point
  ESP_LOGD(TAG, "Publishing dew point %f °C", dew_point);
  this->status_clear_warning();
  this->publish_state(dew_point);
}

// float DewPointComponent::get_dew_point(float temperature, float humidity) {
//   const float a = 6.112;
//   const float b = 17.62;
//   const float c = 243.12;

//   float gamma = log(humidity / 100.0) + b * temperature / (c + temperature);
//   //double dewPoint = c * gamma / (b - gamma);

//   return c * gamma / (b - gamma);
// }

// Sonntag90 formula better Magnus formula
float DewPointComponent::get_dew_point(float temperature, float humidity) {
  const float a = 6.112;
  const float b = 17.62;
  const float c = 243.12;
  const float d = 2.51e-3;

  float gamma = log(humidity / 100.0) + b * temperature / (c + temperature);
  float dewPoint = c * gamma / (b - gamma);

  // Correction for high humidity
  if (humidity >= 50) {
      dewPoint += d * (humidity - 50) * sqrt(humidity + 10);
  }

  return dewPoint;  
}

}  // namespace dew_point
}  // namespace esphome
