# AERL C++ Framework — Beta

A HAL (Hardware Abstraction Layer) for absolute beginners in embedded systems and robotics. Include one header, use one ready-made object, and write clean, readable, non-blocking device code — right alongside normal Arduino code.

Developed by **AERL — Applied Electronics and Robotics Laboratory LLP**. MIT License.
**Beta — expect rough edges; read the notes.**

---

## 1. Install

Copy `src/AERL.h` into your project (for example a `src/` folder), then in your sketch:

```cpp
#include <Arduino.h>
#include <AERL.h>
```

There is **no object to create** — `AERL` already exists (like `Serial`). Your normal `void setup()` and `void loop()` stay exactly as they are.

Supported: **ESP32** (primary) and Arduino-compatible boards.

---

## 2. Core ideas

- **One global object.** Everything is `AERL.something(...)`.
- **Milliseconds in.** Every duration you pass is in **ms**. Internally the engine keeps 64-bit microsecond precision.
- **Non-blocking by default.** `glow`, `flash`, `glctrl` run in the background. On **ESP32** this is automatic (nothing to call). On boards without a spare hardware timer, call `AERL.run_engine();` once at the top of `loop()`.

---

## 3. Full reference

### Digital output

| Call | What it does |
|---|---|
| `AERL.on(pin)` | Drive pin HIGH. Cancels any timed action on that pin. |
| `AERL.off(pin)` | Drive pin LOW. Cancels any timed action on that pin. |
| `AERL.glow(pin, ms)` | HIGH for `ms`, then auto-OFF. Non-blocking. |
| `AERL.flash(pin, ms)` | Blink for `ms`, then OFF. Non-blocking. Blink rate ≈ 100 ms; a flash shorter than that is a single pulse. |
| `AERL.glctrl(pin, high_ms, low_ms)` | Repeat HIGH `high_ms` / LOW `low_ms` forever. Non-blocking. Minimum 1 ms per phase. |

```cpp
AERL.glow(2, 500);           // pin 2 lights for half a second, on its own
AERL.glctrl(2, 200, 800);    // blinks 200ms on / 800ms off, forever
```

### Reading (result stored INTO your variable)

| Call | What it does |
|---|---|
| `AERL.read(pin, var)` | Digital read; stores `true`/`false` into `var`. |
| `AERL.analogread(pin, var)` | Analog read; stores the value into `var`. |

`var` must be a number or bool (not a `String`). On ESP32 the analog range is **0–4095** (12-bit); storing into a `byte` truncates.

```cpp
bool pressed; int knob;
AERL.read(4, pressed);
AERL.analogread(34, knob);
```

### Run once / run N times

| Call | What it does |
|---|---|
| `AERL.run()` | True the FIRST time this line runs, false after. |
| `AERL.run(n)` | True the first `n` times, false after. |

Put **each `AERL.run()` on its own line** (call-sites are told apart by file + line).

```cpp
if (AERL.run())  AERL.logline("boot once");
if (AERL.run(5)) AERL.logline("first five loops");
```

### Delays

| Call | What it does |
|---|---|
| `AERL.delay(ms)` | Pauses this task only; background timing keeps running. |
| `AERL.bcdelay(ms)` | Blocking wait. Watchdog-safe, millisecond-granular. |

### Logging

| Call | What it does |
|---|---|
| `AERL.log(data)` | `Serial.print`. |
| `AERL.logline(data)` | `Serial.println`. |
| `AERL.format(a, b, c, ...)` | Prints `a:b:c` on one line, colon-separated, any number of values. |

### Communication

**UART** — channel 0 is the USB Serial console (fixed pins); 1/2 are Serial1/Serial2 with your pins.

```cpp
AERL.comm_activate(TX, RX, baud, channel);
AERL.send_uart(channel, data);
AERL.receive_uart(channel, variable);      // variable is a String
```

**I2C** — channel 0/1 = Wire/Wire1; `address` is the target device (e.g. 0x3C).

```cpp
AERL.comm_activatei2c(SDA, SCL, channel);
AERL.send_i2c(channel, address, data);
AERL.receive_i2c(channel, address, variable);
```

**SPI** — CSN is toggled for you.

```cpp
AERL.comm_activatespi(MOSI, MISO, SCK, CSN, channel);
AERL.send_spi(channel, data);
```

### Activation

```cpp
AERL.activateSerial(baud);   // = Serial.begin(baud)
AERL.activate(assign);       // reserved (beta, no-op)
```

### Sleep & wake (BETA — be careful)

```cpp
AERL.sleep(ms, {pin1, pin2});    // pull pins low, low-power sleep for ms
AERL.wakeup(ms, {pin1, pin2});   // pull pins high, wait ms
```

### Engine (non-ESP32 only)

On ESP32, timing runs in the background automatically. On other boards, call once per loop:

```cpp
void loop() {
  AERL.run_engine();   // makes glow/flash/glctrl advance
  // ...
}
```

---

## 4. Notes & limits (Beta)

- Every duration is **milliseconds**.
- `flash` blinks at ≈ 100 ms; shorter durations are one pulse.
- On ESP32, `analogRead` is 0–4095; ADC2 pins misbehave while WiFi is active.
- UART channel 0 = the USB console; use 1/2 for external UART.
- Put one `AERL.run()` per line.
- `sleep`/`wakeup` use ESP32 light-sleep and are the roughest part of the Beta.
- Up to 16 pins can run a timed action at once (`AERL_MAX_TIMED_PINS`); up to 32 distinct `run()` sites (`AERL_MAX_RUN_SLOTS`). Override with `-D` before including.

## 5. Quality

This Beta was put through an internal adversarial audit and hardened: a concurrency lock for the background timer, 64-bit time (no long-interval overflow), fail-safe `run()`, watchdog-safe `bcdelay`, safe pin handling when the timer table is full, and clearer compile errors.

---

Developed by AERL — Applied Electronics and Robotics Laboratory LLP. Feel free to contribute.
