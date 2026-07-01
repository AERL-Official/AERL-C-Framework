# AERL C++ Framework

A HAL framework built for embedded systems and robotics — designed to make writing and debugging embedded code fast, simple, and reliable, especially for beginners.

**Status:** Beta — this is the first version, there will be bugs. Please co-operate.

## ✨ Features

- Easy to write and debug code
- Written in C
- Optimized for performance and development speed, without sacrificing reliability

---

## ✨ Usage Guide

### How To Apply

Downalod the AERL.h, and include in the folder of the project, then in main.cpp or wherver you call 
put '#Inlclude <AERL.H>'.

## ✨ Functions

### Turning pins ON and OFF

`AERL.on(pin)` turns the pin ON, sending power through it — like flipping a switch to ON.

`AERL.off(pin)` turns the pin OFF, cutting the power — like flipping the switch back OFF.

`AERL.glow(pin, duration)` turns the pin ON for a set amount of time, then automatically turns it back OFF. This is non-blocking, meaning the rest of your code keeps running normally while this happens — your program doesn't freeze waiting for it.

`AERL.flash(pin, duration)` rapidly switches the pin ON and OFF, like a blinking LED, for the given duration.

`AERL.glctrl(pin, high_duration, low_duration)` gives you full manual control — you decide exactly how long the pin stays ON and exactly how long it stays OFF, repeating on that pattern.

```cpp
AERL.on(pin);
AERL.off(pin);
AERL.glow(pin, duration);
AERL.flash(pin, duration);
AERL.glctrl(pin, high_duration, low_duration);
```

### Reading inputs

`AERL.read(pin, variable)` checks whether a pin is receiving power or not — useful for things like checking if a button is pressed. The result gets stored in `variable`, becoming true if ON and false if OFF.

`AERL.analogread(pin, variable)` works the same way, but for components that give a range of values instead of a simple ON/OFF — like a potentiometer or a light sensor. The reading gets stored in `variable`.

```cpp
AERL.read(pin, variable);
AERL.analogread(pin, variable);
```

### Delays

`AERL.delay(microseconds)` pauses only that specific task, while the rest of your program keeps running normally in the background.

`AERL.bcdelay(microseconds)` pauses your entire program until the delay finishes. Use this only when you actually want everything to stop and wait.

```cpp
AERL.delay(microseconds);
AERL.bcdelay(microseconds);
```

### Sleep & wake

`AERL.sleep(microseconds, pins)` puts your board into a low-power state for the given duration. The pins you list get switched off to save power, while only essential background functions keep running.// Bug Prone Function On Beta, Please be careful.

`AERL.wakeup(microseconds, pins)` brings everything back online after sleeping, turning the listed pins back on. // Bug Prone Function On Beta, Please be careful.

```cpp
AERL.sleep(microseconds, pins);
AERL.wakeup(microseconds, pins);
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

`AERL.format(data1 : data2 : data3)` lets you print multiple variables in a single line, separated by colons. You can include as many variables as you want.

```cpp
AERL.log(data);
AERL.logline(data);
AERL.format(data1 : data2 : data3);
```

### Communication

**UART** lets two devices talk to each other over two wires (TX and RX).

`AERL.comm_activate(TX, RX, channel)` sets up the connection. `AERL.send_uart(channel, data)` sends data out. `AERL.recieve_uart(channel, variable)` receives incoming data and stores it in `variable`.

```cpp
AERL.comm_activate(TX, RX, Baude rate, channel); // Set the TX and RX Pins, Then the Baud Rate then a channel ( int ) 
AERL.send_uart(channel, data); // set wich channel, and what to send
AERL.recieve_uart(channel, variable); // set the channel and what to recive 
```

**I2C** lets multiple devices share just two wires (SDA and SCL) to communicate.

`AERL.comm_activatei2c(SDA, SCL, channel)` sets up the connection. `AERL.send_i2c(channel, data)` sends data. `AERL.recive_i2c(channel, variable)` receives data into `variable`.

```cpp
AERL.comm_activatei2c(SDA, SCL, channel);
AERL.send_i2c(channel, data);
AERL.recive_i2c(channel, variable);
```

**SPI** is a faster communication method using four wires (MOSI, MISO, SCK, CSN), typically used for displays and high-speed sensors.

`AERL.comm_activatespi(MOSI, MISO, SCK, CSN, channel)` sets up the connection. `AERL.send_spi(channel, data)` sends data over it.

```cpp
AERL.comm_activatespi(MOSI, MISO, SCK, CSN, channel);
AERL.send_spi(channel, data);
```

---

Developed by AERL Team - APPLIED ELECTRONICS AND ROBOTICS LABORATORY LLP,
Feel Free to contribute. ✨
