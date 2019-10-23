#pragma once

#include <list>
#include "esphome/core/component.h"
#include "esphome/core/automation.h"

#ifdef ARDUINO_ARCH_ESP32
#include <HTTPClient.h>
#endif
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#endif

namespace esphome {
namespace http_request {

struct Header {
  const char *name;
  const char *value;
};

class HttpRequestComponent : public Component {
 public:
  void setup() override {
#ifdef ARDUINO_ARCH_ESP8266
    this->wifi_client_ = new BearSSL::WiFiClientSecure();
    this->wifi_client_->setInsecure();
#endif
  }
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

  void set_url(const char *url) { this->url_ = url; }
  void set_method(const char *method) { this->method_ = method; }
  void set_useragent(const char *useragent) { this->useragent_ = useragent; }
  void set_timeout(uint16_t timeout) { this->timeout_ = timeout; }
  void set_body(std::string body) { this->body_ = body; }
  void set_headers(std::list<Header> headers) { this->headers_ = headers; }
  void send();

 protected:
  HTTPClient client_{};
  const char *url_;
  const char *method_;
  const char *useragent_{nullptr};
  uint16_t timeout_{5000};
  std::string body_;
  std::list<Header> headers_;
#ifdef ARDUINO_ARCH_ESP8266
  BearSSL::WiFiClientSecure *wifi_client_;
#endif
};

template<typename... Ts> class HttpRequestSendAction : public Action<Ts...> {
 public:
  HttpRequestSendAction(HttpRequestComponent *parent) : parent_(parent) {}
  TEMPLATABLE_VALUE(const char *, url)
  TEMPLATABLE_VALUE(const char *, method)
  TEMPLATABLE_VALUE(std::string, body)
  TEMPLATABLE_VALUE(const char *, useragent)
  TEMPLATABLE_VALUE(uint16_t, timeout)

  void add_header(const char *name, const char *value) {
    Header header;
    header.name = name;
    header.value = value;
    this->headers_.push_back(header);
  }

  void play(Ts... x) override {
    this->parent_->set_url(this->url_.value(x...));
    this->parent_->set_method(this->method_.value(x...));
    if (this->body_.has_value()) {
      this->parent_->set_body(this->body_.value(x...));
    }
    if (this->useragent_.has_value()) {
      this->parent_->set_useragent(this->useragent_.value(x...));
    }
    if (this->timeout_.has_value()) {
      this->parent_->set_timeout(this->timeout_.value(x...));
    }
    if (!this->headers_.empty()) {
      this->parent_->set_headers(this->headers_);
    }
    this->parent_->send();
  }

 protected:
  HttpRequestComponent *parent_;
  std::list<Header> headers_{};
};

}  // namespace http_request
}  // namespace esphome