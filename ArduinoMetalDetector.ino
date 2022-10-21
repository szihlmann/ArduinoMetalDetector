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

// Number of cycles from external counter needed to generate a signal event
// Larger number: Better resolution and longer aquisition time. Max value 65535 (16 bit)
#define CYCLES_PER_SIGNAL 5000

// Base tone frequency (speaker)
#define BASE_TONE_FREQUENCY 280

// Pin definitions
#define SPEAKER_PIN 10

unsigned long lastSignalTime = 0;
long signalTimeDelta = 0;
long storedTimeDelta = 0;

void setup()
{
  setup_timer1();
  pinMode(SPEAKER_PIN, OUTPUT);
}

void loop()
{
  float sensitivity = mapFloat(100, 0, 1023, 0.5, 10.0);
  int storedTimeDeltaDifference = storedTimeDelta - signalTimeDelta;
  unsigned int toneFrequency = BASE_TONE_FREQUENCY + abs(storedTimeDeltaDifference) * sensitivity;
  tone(SPEAKER_PIN, toneFrequency);
}

float mapFloat(int input, int inMin, int inMax, float outMin, float outMax)
{
  float scale = (float)(input - inMin) / (inMax - inMin);
  return ((outMax - outMin) * scale) + outMin;
}
