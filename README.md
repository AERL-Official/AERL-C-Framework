# AERL C++ Framework

A HAL framework built for embedded systems and robotics — designed to make writing and debugging embedded code fast, simple, and reliable, especially for beginners.

**Status:** Beta — this is the first version, there will be bugs. Please co-operate.

## ✨ Features

- Easy to write and debug code
- Written in C
- Optimized for performance and development speed, without sacrificing reliability

**A note on time:** every duration you pass is in **milliseconds**. Internally the engine works in microseconds — so you keep it simple while the timing stays precise.

---

## ✨ Usage Guide

### How To Apply

Download `AERL.h` into your project (for example a `src` folder), then in your sketch add:

```cpp
#include <Arduino.h>
#include <AERL.h>
```

There is no object to create — just use `AERL.` directly. Your normal `void setup()` and `void loop()` stay exactly as they are.

## ✨ Functions

### Turning pins ON and OFF

`AERL.on(pin)` turns the pin ON, sending power through it — like flipping a switch to ON.

`AERL.off(pin)` turns the pin OFF, cutting the power — like flipping the switch back OFF.

`AERL.glow(pin, ms)` turns the pin ON for a set time (in milliseconds), then automatically turns it back OFF. This is non-blocking, meaning the rest of your code keeps running normally while this happens — your program doesn't freeze waiting for it.

`AERL.flash(pin, ms)` rapidly switches the pin ON and OFF, like a blinking LED, for the given duration.

`AERL.glctrl(pin, high_ms, low_ms)` gives you full manual control — you decide exactly how long the pin stays ON and exactly how long it stays OFF, repeating on that pattern.

```cpp
AERL.on(pin);
AERL.off(pin);
AERL.glow(pin, ms);
AERL.flash(pin, ms);
AERL.glctrl(pin, high_ms, low_ms);
```

### Running something once, or a fixed number of times

`AERL.run()` returns true only the **first** time your code reaches it, and false every time after — so you can do something once, even inside `loop()`.

`AERL.run(n)` returns true the **first n times**, then false — for running something a set number of times. Each spot in your code counts on its own.

```cpp
if (AERL.run())  { AERL.logline("this runs once"); }
if (AERL.run(5)) { AERL.logline("this runs five times"); }
```

### Reading inputs

`AERL.read(pin, variable)` checks whether a pin is receiving power or not — useful for things like checking if a button is pressed. The result gets stored in `variable`, becoming true if ON and false if OFF.

`AERL.analogread(pin, variable)` works the same way, but for components that give a range of values instead of a simple ON/OFF — like a potentiometer or a light sensor. The reading gets stored in `variable`.

```cpp
AERL.read(pin, variable);
AERL.analogread(pin, variable);
```

### Delays

`AERL.delay(ms)` pauses only that specific task, while the rest of your program keeps running normally in the background.

`AERL.bcdelay(ms)` pauses your entire program until the delay finishes. Use this only when you actually want everything to stop and wait.

```cpp
AERL.delay(ms);
AERL.bcdelay(ms);
```

### Sleep & wake

`AERL.sleep(ms, pins)` puts your board into a low-power state for the given duration. The pins you list get switched off to save power, while only essential background functions keep running. // Bug Prone Function On Beta, Please be careful.

`AERL.wakeup(ms, pins)` brings everything back online after sleeping, turning the listed pins back on. // Bug Prone Function On Beta, Please be careful.

```cpp
AERL.sleep(ms, {pin1, pin2});
AERL.wakeup(ms, {pin1, pin2});
```

### Activation

`AERL.activateSerial(baudrate)` starts communication between your board and your computer over USB — the equivalent of `Serial.begin()`.

`AERL.activate(assign)` is a general-purpose starter function — used for initializing any component that normally needs its own `.begin()` call.

```cpp
AERL.activateSerial(baudrate);
AERL.activate(assign);
```

### Logging

`AERL.log(data)` prints data to your Serial Monitor without moving to a new line — same as `Serial.print()`.

`AERL.logline(data)` prints data and then moves to a new line — same as `Serial.println()`.

`AERL.format(data1, data2, data3)` prints multiple variables on a single line, separated by colons (`data1:data2:data3`). You can include as many variables as you want.

```cpp
AERL.log(data);
AERL.logline(data);
AERL.format(data1, data2, data3);
```

### Communication

**UART** lets two devices talk to each other over two wires (TX and RX).

`AERL.comm_activate(TX, RX, baud, channel)` sets up the connection. `AERL.send_uart(channel, data)` sends data out. `AERL.receive_uart(channel, variable)` receives incoming data and stores it in `variable`.

```cpp
AERL.comm_activate(TX, RX, baud, channel); // set the TX and RX pins, the baud rate, then a channel (int)
AERL.send_uart(channel, data);             // which channel, and what to send
AERL.receive_uart(channel, variable);      // the channel, and where to store what is received
```

**I2C** lets multiple devices share just two wires (SDA and SCL) to communicate.

`AERL.comm_activatei2c(SDA, SCL, channel)` sets up the connection. `AERL.send_i2c(channel, address, data)` sends data to the device at `address`. `AERL.receive_i2c(channel, address, variable)` receives data from it into `variable`.

```cpp
AERL.comm_activatei2c(SDA, SCL, channel);
AERL.send_i2c(channel, address, data);      // address = the target device, e.g. 0x3C
AERL.receive_i2c(channel, address, variable);
```

**SPI** is a faster communication method using four wires (MOSI, MISO, SCK, CSN), typically used for displays and high-speed sensors.

`AERL.comm_activatespi(MOSI, MISO, SCK, CSN, channel)` sets up the connection. `AERL.send_spi(channel, data)` sends data over it (the CSN line is handled for you).

```cpp
AERL.comm_activatespi(MOSI, MISO, SCK, CSN, channel);
AERL.send_spi(channel, data);
```

---

Developed by AERL Team - APPLIED ELECTRONICS AND ROBOTICS LABORATORY LLP,
Feel Free to contribute. ✨
