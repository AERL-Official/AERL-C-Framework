/*
AERL C++ Framework, Engineered & Developed By AERL Team.
Copyright  ©  2026 Applied Electronics And Robotics Laboratory LLP
Version : Beta ( 01/07/2026 Released)
Flags---------------------------------------------------------------
Development Board Sleep Can Be A Issue
Wifi & BLE Compatibility Coming Soon On v1 ( about 1 week from beta release ( july 4, 2026 ))
Enjoy. Learn. Advance
Thanks for trying out & contributing.
AERL Team
*/

/* AERL SOFTWARE SEAL
PROJECT NAME : AERL C++ Frame Work
Version : Beta
Release Date : 01/07/2026
Category : Embedded Systems
Type : Open-Sourced
*/

#ifndef AERL_H
#define AERL_H

#include <Arduino.h>
#include <initializer_list>
#include <type_traits>
#include <string.h>
#include <Wire.h>
#include <SPI.h>

#ifndef AERL_MAX_TIMED_PINS
#define AERL_MAX_TIMED_PINS 16
#endif
#ifndef AERL_MAX_RUN_SLOTS
#define AERL_MAX_RUN_SLOTS 32
#endif
#ifndef AERL_FLASH_INTERVAL_MS
#define AERL_FLASH_INTERVAL_MS 100
#endif

#if defined(ESP32) || defined(ESP_PLATFORM)
  #include "esp_timer.h"
  #include "esp_sleep.h"
  static inline uint64_t aerl_now_us() { return (uint64_t)esp_timer_get_time(); }
  inline portMUX_TYPE aerl_mux = portMUX_INITIALIZER_UNLOCKED;
  #define AERL_LOCK()   portENTER_CRITICAL(&aerl_mux)
  #define AERL_UNLOCK() portEXIT_CRITICAL(&aerl_mux)
#else
  static inline uint64_t aerl_now_us() {
    static uint32_t last = 0; static uint64_t hi = 0;
    uint32_t now = micros();
    if (now < last) hi += 0x100000000ULL;
    last = now;
    return hi + now;
  }
  #define AERL_LOCK()   do {} while (0)
  #define AERL_UNLOCK() do {} while (0)
#endif

class AERLClass {
 public:
  void on(uint8_t pin);
  void off(uint8_t pin);
  void glow(uint8_t pin, uint32_t ms);
  void flash(uint8_t pin, uint32_t ms);
  void glctrl(uint8_t pin, uint32_t high_ms, uint32_t low_ms);

  template <class T> bool read(uint8_t pin, T &var) {
    static_assert(std::is_arithmetic<T>::value,
                  "AERL.read(pin, var): var must be a number or bool (int, bool, float...), not String.");
    pinMode(pin, INPUT);
    bool v = (digitalRead(pin) == HIGH);
    var = static_cast<T>(v);
    return v;
  }
  template <class T> int analogread(uint8_t pin, T &var) {
    static_assert(std::is_arithmetic<T>::value,
                  "AERL.analogread(pin, var): var must be a number (int, float...), not String.");
    int v = analogRead(pin);
    var = static_cast<T>(v);
    return v;
  }

  void delay(uint32_t ms);
  void bcdelay(uint32_t ms);

  void sleep(uint32_t ms, std::initializer_list<uint8_t> pinsDown = {});
  void wakeup(uint32_t ms, std::initializer_list<uint8_t> pinsUp = {});

  void activateSerial(uint32_t baud);
  void activate(int assign = 0);

  template <class T> void log(const T &data)     { Serial.print(data); }
  template <class T> void logline(const T &data) { Serial.println(data); }

  template <class T> void format(const T &last) { Serial.println(last); }
  template <class T, class... Rest>
  void format(const T &first, const Rest &...rest) {
    Serial.print(first);
    Serial.print(':');
    format(rest...);
  }

  bool run(uint32_t times = 1,
           const char *file = __builtin_FILE(),
           int line = __builtin_LINE());

  void comm_activate(uint8_t tx, uint8_t rx, uint32_t baud, uint8_t channel);
  template <class T> void send_uart(uint8_t channel, const T &data) {
    if (channel < 3 && _uart[channel]) _uart[channel]->print(data);
  }
  bool receive_uart(uint8_t channel, String &var);

  void comm_activatei2c(uint8_t sda, uint8_t scl, uint8_t channel);
  void send_i2c(uint8_t channel, uint8_t address, uint8_t data);
  bool receive_i2c(uint8_t channel, uint8_t address, String &var);

  void comm_activatespi(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t csn, uint8_t channel);
  void send_spi(uint8_t channel, uint8_t data);

  void run_engine();
  void _tick();

 private:
  struct TimedPin {
    bool     active = false;
    uint8_t  pin    = 0;
    uint8_t  kind   = 0;
    bool     level  = false;
    uint64_t highUs = 0, lowUs = 0;
    uint64_t nextUs = 0;
    uint64_t endUs  = 0;
    bool     hasEnd = false;
  };
  struct RunSlot { const char *file = nullptr; int line = 0; uint32_t count = 0; };

  TimedPin _timed[AERL_MAX_TIMED_PINS];
  RunSlot  _runs[AERL_MAX_RUN_SLOTS];
  HardwareSerial *_uart[3] = { nullptr, nullptr, nullptr };

  TimedPin *_slotFor(uint8_t pin);
  void _scheduleNext();
};

inline AERLClass AERL;

#if defined(ESP32) || defined(ESP_PLATFORM)
  inline esp_timer_handle_t aerl_timer = nullptr;
  inline void aerl_timer_cb(void *) { AERL._tick(); }
  inline void aerl_arm(uint64_t us) {
    if (!aerl_timer) {
      esp_timer_create_args_t args = {};
      args.callback = &aerl_timer_cb;
      args.dispatch_method = ESP_TIMER_TASK;
      args.name = "aerl";
      esp_timer_create(&args, &aerl_timer);
    }
    if (us < 50) us = 50;
    esp_timer_stop(aerl_timer);
    if (esp_timer_start_once(aerl_timer, us) != ESP_OK) esp_timer_restart(aerl_timer, us);
  }
#endif

inline TwoWire *aerl_i2cBus(uint8_t channel) {
#if defined(ESP32) || defined(ESP_PLATFORM)
  return (channel == 1) ? &Wire1 : &Wire;
#else
  (void)channel; return &Wire;
#endif
}

inline uint8_t     aerl_spi_cs[2]  = { 255, 255 };
inline SPISettings aerl_spi_cfg[2] = { SPISettings(1000000, MSBFIRST, SPI_MODE0),
                                       SPISettings(1000000, MSBFIRST, SPI_MODE0) };

inline void AERLClass::on(uint8_t pin) {
  pinMode(pin, OUTPUT);
  AERL_LOCK();
  for (auto &s : _timed) if (s.active && s.pin == pin) s.active = false;
  digitalWrite(pin, HIGH);
  AERL_UNLOCK();
}

inline void AERLClass::off(uint8_t pin) {
  pinMode(pin, OUTPUT);
  AERL_LOCK();
  for (auto &s : _timed) if (s.active && s.pin == pin) s.active = false;
  digitalWrite(pin, LOW);
  AERL_UNLOCK();
}

inline AERLClass::TimedPin *AERLClass::_slotFor(uint8_t pin) {
  for (auto &s : _timed) if (s.active && s.pin == pin) return &s;
  for (auto &s : _timed) if (!s.active) return &s;
  return nullptr;
}

inline void AERLClass::glow(uint8_t pin, uint32_t ms) {
  pinMode(pin, OUTPUT);
  uint64_t now = aerl_now_us(), endUs = now + (uint64_t)ms * 1000ULL;
  bool ok = false;
  AERL_LOCK();
  TimedPin *s = _slotFor(pin);
  if (s) {
    s->pin = pin; s->kind = 0; s->level = true; s->highUs = 0; s->lowUs = 0;
    s->hasEnd = true; s->endUs = endUs; s->nextUs = endUs;
    s->active = true;
    digitalWrite(pin, HIGH);
    ok = true;
  }
  AERL_UNLOCK();
  if (ok) _scheduleNext();
}

inline void AERLClass::flash(uint8_t pin, uint32_t ms) {
  pinMode(pin, OUTPUT);
  uint64_t now = aerl_now_us(), iv = (uint64_t)AERL_FLASH_INTERVAL_MS * 1000ULL;
  bool ok = false;
  AERL_LOCK();
  TimedPin *s = _slotFor(pin);
  if (s) {
    s->pin = pin; s->kind = 1; s->level = true; s->highUs = iv; s->lowUs = iv;
    s->hasEnd = true; s->endUs = now + (uint64_t)ms * 1000ULL; s->nextUs = now + iv;
    s->active = true;
    digitalWrite(pin, HIGH);
    ok = true;
  }
  AERL_UNLOCK();
  if (ok) _scheduleNext();
}

inline void AERLClass::glctrl(uint8_t pin, uint32_t high_ms, uint32_t low_ms) {
  if (high_ms < 1) high_ms = 1;
  if (low_ms  < 1) low_ms  = 1;
  pinMode(pin, OUTPUT);
  uint64_t now = aerl_now_us();
  bool ok = false;
  AERL_LOCK();
  TimedPin *s = _slotFor(pin);
  if (s) {
    s->pin = pin; s->kind = 2; s->level = true;
    s->highUs = (uint64_t)high_ms * 1000ULL; s->lowUs = (uint64_t)low_ms * 1000ULL;
    s->hasEnd = false; s->nextUs = now + s->highUs;
    s->active = true;
    digitalWrite(pin, HIGH);
    ok = true;
  }
  AERL_UNLOCK();
  if (ok) _scheduleNext();
}

inline void AERLClass::_tick() {
  uint64_t now = aerl_now_us();
  AERL_LOCK();
  for (auto &s : _timed) {
    if (!s.active) continue;
    if (s.hasEnd && now >= s.endUs) { digitalWrite(s.pin, LOW); s.active = false; continue; }
    if (s.kind != 0 && now >= s.nextUs) {
      s.level = !s.level;
      digitalWrite(s.pin, s.level ? HIGH : LOW);
      uint64_t iv = s.level ? s.highUs : s.lowUs;
      s.nextUs += iv;
      if (now >= s.nextUs) s.nextUs = now + iv;
    }
  }
  AERL_UNLOCK();
  _scheduleNext();
}

inline void AERLClass::_scheduleNext() {
  uint64_t now = aerl_now_us();
  bool any = false; uint64_t soonest = 0;
  AERL_LOCK();
  for (auto &s : _timed) {
    if (!s.active) continue;
    uint64_t evt = s.nextUs;
    if (s.hasEnd && s.endUs < evt) evt = s.endUs;
    uint64_t d = (evt > now) ? (evt - now) : 0;
    if (!any || d < soonest) { soonest = d; any = true; }
  }
  AERL_UNLOCK();
#if defined(ESP32) || defined(ESP_PLATFORM)
  if (any) aerl_arm(soonest);
#else
  (void)any; (void)soonest;
#endif
}

inline void AERLClass::run_engine() { _tick(); }

inline void AERLClass::delay(uint32_t ms) {
#if defined(ESP32) || defined(ESP_PLATFORM)
  vTaskDelay(pdMS_TO_TICKS(ms));
#else
  uint64_t start = aerl_now_us(), target = (uint64_t)ms * 1000ULL;
  while (aerl_now_us() - start < target) { _tick(); yield(); }
#endif
}

inline void AERLClass::bcdelay(uint32_t ms) {
  uint64_t start = aerl_now_us(), target = (uint64_t)ms * 1000ULL;
#if defined(ESP32) || defined(ESP_PLATFORM)
  uint64_t lastYield = start;
#endif
  while (aerl_now_us() - start < target) {
#if defined(ESP32) || defined(ESP_PLATFORM)
    if (aerl_now_us() - lastYield >= 50000ULL) { vTaskDelay(1); lastYield = aerl_now_us(); }
#elif defined(ARDUINO)
    yield();
#endif
  }
}

inline void AERLClass::sleep(uint32_t ms, std::initializer_list<uint8_t> pinsDown) {
  for (uint8_t p : pinsDown) { pinMode(p, OUTPUT); digitalWrite(p, LOW); }
#if defined(ESP32) || defined(ESP_PLATFORM)
  esp_sleep_enable_timer_wakeup((uint64_t)ms * 1000ULL);
  esp_light_sleep_start();
  _tick();
#else
  bcdelay(ms);
#endif
}

inline void AERLClass::wakeup(uint32_t ms, std::initializer_list<uint8_t> pinsUp) {
  for (uint8_t p : pinsUp) { pinMode(p, OUTPUT); digitalWrite(p, HIGH); }
  delay(ms);
}

inline void AERLClass::activateSerial(uint32_t baud) { Serial.begin(baud); }
inline void AERLClass::activate(int assign) { (void)assign; }

inline bool AERLClass::run(uint32_t times, const char *file, int line) {
  RunSlot *freeSlot = nullptr;
  for (auto &r : _runs) {
    if (r.file && r.line == line && (r.file == file || strcmp(r.file, file) == 0)) {
      if (r.count < times) { r.count++; return true; }
      return false;
    }
    if (!freeSlot && r.file == nullptr) freeSlot = &r;
  }
  if (freeSlot) { freeSlot->file = file; freeSlot->line = line; freeSlot->count = 1; return times >= 1; }
  return false;
}

inline void AERLClass::comm_activate(uint8_t tx, uint8_t rx, uint32_t baud, uint8_t channel) {
  if (channel > 2) return;
#if defined(ESP32) || defined(ESP_PLATFORM)
  if (channel == 0) {
    Serial.begin(baud); _uart[0] = &Serial;
  } else {
    HardwareSerial *ports[3] = { &Serial, &Serial1, &Serial2 };
    ports[channel]->begin(baud, SERIAL_8N1, rx, tx);
    _uart[channel] = ports[channel];
  }
#else
  (void)tx; (void)rx;
  if (channel == 0) { Serial.begin(baud); _uart[0] = &Serial; }
#endif
}

inline bool AERLClass::receive_uart(uint8_t channel, String &var) {
  if (channel > 2 || !_uart[channel]) return false;
  bool got = false;
  while (_uart[channel]->available()) { var += (char)_uart[channel]->read(); got = true; }
  return got;
}

inline void AERLClass::comm_activatei2c(uint8_t sda, uint8_t scl, uint8_t channel) {
#if defined(ESP32) || defined(ESP_PLATFORM)
  if (channel == 1) Wire1.begin(sda, scl);
  else              Wire.begin(sda, scl);
#else
  (void)sda; (void)scl; (void)channel; Wire.begin();
#endif
}

inline void AERLClass::send_i2c(uint8_t channel, uint8_t address, uint8_t data) {
  TwoWire *w = aerl_i2cBus(channel);
  w->beginTransmission(address);
  w->write(data);
  w->endTransmission();
}

inline bool AERLClass::receive_i2c(uint8_t channel, uint8_t address, String &var) {
  TwoWire *w = aerl_i2cBus(channel);
  bool got = false;
  w->requestFrom((int)address, 1);
  while (w->available()) { var += (char)w->read(); got = true; }
  return got;
}

inline void AERLClass::comm_activatespi(uint8_t mosi, uint8_t miso, uint8_t sck, uint8_t csn, uint8_t channel) {
#if defined(ESP32) || defined(ESP_PLATFORM)
  SPI.begin(sck, miso, mosi, csn);
#else
  (void)mosi; (void)miso; (void)sck; SPI.begin();
#endif
  if (channel < 2) aerl_spi_cs[channel] = csn;
  pinMode(csn, OUTPUT); digitalWrite(csn, HIGH);
}

inline void AERLClass::send_spi(uint8_t channel, uint8_t data) {
  if (channel >= 2) return;
  uint8_t cs = aerl_spi_cs[channel];
  SPI.beginTransaction(aerl_spi_cfg[channel]);
  if (cs != 255) digitalWrite(cs, LOW);
  SPI.transfer(data);
  if (cs != 255) digitalWrite(cs, HIGH);
  SPI.endTransaction();
}

#endif
