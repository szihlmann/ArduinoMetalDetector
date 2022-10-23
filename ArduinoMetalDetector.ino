/*
 * Copyright (c) 2016 Evan Kale
 * Media: @EvanKale91
 * Email: EvanKale91@gmail.com
 * Website: www.ISeeDeadPixel.com
 *          www.evankale.blogspot.ca
 *          www.youtube.com/EvanKale91
 *
 * This file is part of ArduinoMetalDetector.
 *
 * ArduinoMetalDetector is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "src/Buzzer/Buzzer.h"
#include "src/BaselineCorrection/BaselineCorrection.h"

// Number of cycles from external counter needed to generate a signal event
// Larger number: Better resolution and longer aquisition time. Max value 255 (8 bit)
#define SIGNAL_CYCLES 5 // Multiplies by 512 (=2'560 cycles)

// Pin definitions
#define SPEAKER_PIN 10
#define LED_PIN 11

// Timer 0 - Colpitts counter
volatile uint8_t TCNT0_overflowCount = 0; // Incremented at every overflow of TIMER0_COMPB
volatile uint8_t newData = 0; // Incremented whenever aquisition cycle finished

// Timer 1 - The Clock
volatile unsigned long timer1_micros = 0;

// Buffer for total elapsed time when measuring colpitts frequency
unsigned long lastSignalTime = 0;
long signalTimeDelta = 0;

// Create buzzer instance
Buzzer buzzer = Buzzer(SPEAKER_PIN, LED_PIN);

// Create baseline correction engine
BaselineCorrection baseline = BaselineCorrection();

void setup()
{
  setup_timer0();
  setup_timer1();
  while (newData < 10); // Wait to assign a stable value
  baseline.initialize(signalTimeDelta, Millis());
}

void loop()
{
  unsigned long cTime = Millis();
  
  if (newData){
    newData = 0;
    
    // Calculate difference in signal time
    long TimeDelta = (long)(baseline.getValue()) - (long)(signalTimeDelta);
    buzzer.update(TimeDelta); 

    // Update baseline
    baseline.update(signalTimeDelta, cTime); 
  }
  buzzer.tick(cTime);
}
