# ArduinoMetalDetector
Arduino code for a colpitts oscillator based metal detector on Arduino Micro.
- This is a fork from evankale/ArduinoMetalDetector who did the initial work
- Author: Serge Zihlmann

## Purpose
- This metal detector uses a colpitts oscillator and detects frequency changes when the coil of the oscillator approaches any metal piece.
- The original commit from evenkale used the Arduino native micros() function to measure the time between 5000 oscillation cycles. Anyhow, the micros() function has a resolution of 4 microseconds only on 16 MHz Arduino.
- My approach intents to use the full resolution based on the 16 MHz clock which equals to 62.5 ns (64x better). This is done by feeding the output of the colpitts oscillator not only to a dedicated counter to keep track of signal cycles, but also to the Input Capture Unit of timer 1. Timer 1 is configured as a high resolution clock to simply keep track of time.
- The enhanced time resolution allows shorter observation periods for the signal while keeping the original resolution. Thus, the metal detector will react faster.
- Otherwise, when keeping the observation period, the resolution will be enhanced and smaller metal parts can be detected.

## Baseline correction
- The oscillation frequency tends to drift over time (coil heatup?).
- The implemented baseline correction will counteract this effect.