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

// Number of cycles from external counter needed to generate a signal event
// Larger number: Better resolution and longer aquisition time. Max value 65535 (16 bit)
#define CYCLES_PER_SIGNAL 5000

// Show that TIMER1_COMPA_vect has recently fired and new data is available
// newData will increment at every interrupt to allow a delay at startup 
volatile uint8_t newData = 0;

// Pin definitions
#define SPEAKER_PIN 10
#define LED_PIN 11

unsigned long lastSignalTime = 0;
long signalTimeDelta = 0;
long storedTimeDelta = 0;

// Create buzzer instance
Buzzer buzzer = Buzzer(SPEAKER_PIN, LED_PIN);

void setup()
{
  setup_timer1();
  while (newData < 10); // Wait to assign a stable value
  storedTimeDelta = signalTimeDelta;
}

void loop()
{
  unsigned long cTime = millis();
  
  if (newData){
    newData = 0;
    // Calculate difference in signal time
    long TimeDelta = (long)(storedTimeDelta) - (long)(signalTimeDelta);
    buzzer.update(TimeDelta); 
  }
  buzzer.tick(cTime);
}
