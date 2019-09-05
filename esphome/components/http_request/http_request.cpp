#include "http_request.h"
#include "esphome/core/log.h"

namespace esphome {
namespace http_request {

static const char *TAG = "http_request";

void HttpRequestComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "HTTP Request:");
  ESP_LOGCONFIG(TAG, "  URI: %s", this->uri_);
  ESP_LOGCONFIG(TAG, "  Method: %s", this->method_);
  ESP_LOGCONFIG(TAG, "  Timeout: %dms", this->timeout_);
  if (this->fingerprint_ != nullptr) {
    ESP_LOGCONFIG(TAG, "  Using SSL Fingerprint");
  }
  if (this->useragent_ != nullptr) {
    ESP_LOGCONFIG(TAG, "  User-Agent: %s", this->useragent_);
  }
  if (this->headers_.size() > 0) {
    ESP_LOGCONFIG(TAG, "  Headers:");
    for (const auto &header : this->headers_) {
      ESP_LOGCONFIG(TAG, "    %s: %s", header.name, header.value);
    }
  }
}

void HttpRequestComponent::send() {
  bool beginStatus;
  #ifdef ARDUINO_ARCH_ESP32
  if (this->ca_certificate_ != nullptr)
    beginStatus = this->client_.begin(this->uri_, this->ca_certificate_);
  #endif
  #ifdef ARDUINO_ARCH_ESP8266
  if (this->fingerprint_ != nullptr)
    beginStatus = this->client_.begin(this->uri_, this->fingerprint_);
  #endif
  else
    beginStatus = this->client_.begin(this->uri_);

  if (!beginStatus) {
    this->client_.end();
    this->status_set_warning();
    ESP_LOGW(TAG, "HTTP Request failed at the begin phase. Please check the configuration");
    return;
  }

  this->client_.setTimeout(this->timeout_);
  if (this->useragent_ != nullptr) {
    this->client_.setUserAgent(this->useragent_);
  }
  for (const auto &header : this->headers_) {
    this->client_.addHeader(header.name, header.value, false, true);
  }

  int httpCode = this->client_.sendRequest(this->method_, this->payload_.c_str());
  this->client_.end();

  if (httpCode < 0) {
    ESP_LOGW(TAG, "HTTP Request failed; URI: %s; Error: %s", this->uri_, HTTPClient::errorToString(httpCode).c_str());
    this->status_set_warning();
    return;
  }

  if (httpCode < 200 || httpCode >= 300) {
    ESP_LOGW(TAG, "HTTP Request failed; URI: %s; Code: %d", this->uri_, httpCode);
    this->status_set_warning();
    return;
  }

  this->status_clear_warning();
  ESP_LOGD(TAG, "HTTP Request completed; URI: %s; Code: %d", this->uri_, httpCode);
}

}  // namespace http_request
}  // namespace esphome
