/*
  AERL C++ Framework — Blink example

  Shows off four things at once:
    - run()     : do something ONCE, even though it lives inside loop()
    - glctrl()  : blink a pin on a custom ON/OFF pattern, non-blocking
    - format()  : print several values on one line, colon-separated
    - delay()   : pace the loop without freezing the background blink

  Wire an LED (or use the onboard one) to pin 2.
*/

#include <Arduino.h>
#include <AERL.h>

void setup() {
  AERL.activateSerial(115200);
  AERL.logline("AERL blink demo starting...");
}

void loop() {
  // Start the blink pattern exactly once: 200 ms ON, 800 ms OFF, forever.
  // Because glctrl() is non-blocking, the LED keeps blinking on its own
  // while the rest of loop() runs freely.
  if (AERL.run()) {
    AERL.glctrl(2, 200, 800);
  }

  // Heartbeat once a second, without stopping the blink above.
  static int beat = 0;
  AERL.format("beat", beat++);
  AERL.delay(1000);
}
