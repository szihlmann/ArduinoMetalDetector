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
#define SENSITIVITY_POT_APIN 1
#define SPEAKER_PIN 2
#define RESET_BTN_PIN 12

unsigned long lastSignalTime = 0;
long signalTimeDelta = 0;
long storedTimeDelta = 0;

// This signal is called when the timer value TCNT1 reaches OCR1A
// (Note: TCNT1 is incremented on every external clock cycle)
SIGNAL(TIMER1_COMPA_vect)
{
  unsigned long currentTime = micros();
  signalTimeDelta =  currentTime - lastSignalTime;
  lastSignalTime = currentTime;

  if (storedTimeDelta == 0)
  {
    storedTimeDelta = signalTimeDelta;
  }

  // Move OCR1A value ahead
  OCR1A += CYCLES_PER_SIGNAL;
}

void setup()
{
  // Set WGM(Waveform Generation Mode) to 0 (Normal)
  // Timer runs from 0x00 to 0xFFFF and overruns
  TCCR1A = 0b00000000;
  
  // Set CSS(Clock Speed Selection) to 0b111 (External clock source on T0 pin
  // (ie, pin 5 on UNO). Clock on rising edge.)
  TCCR1B = 0b00000111;
  
  // Reset counter and start measuring time
  TCNT1 = 0;
  OCR1A = CYCLES_PER_SIGNAL;
  lastSignalTime = micros();
  
  // Clear interrupt flag by setting OCF1A high
  TIFR1 |= (1 << OCF1A);
  
  // Enable timer compare interrupt A (ie, SIGNAL(TIMER1_COMPA_VECT))
  TIMSK1 |= (1 << OCIE1A);

  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(SPINNER_PIN, OUTPUT);
  pinMode(TRIGGER_BTN_PIN, INPUT_PULLUP);
  pinMode(RESET_BTN_PIN, INPUT_PULLUP);
}

void loop()
{
  float sensitivity = mapFloat(analogRead(SENSITIVITY_POT_APIN), 0, 1023, 0.5, 10.0);
  int storedTimeDeltaDifference = storedTimeDelta - signalTimeDelta;
  unsigned int toneFrequency = BASE_TONE_FREQUENCY + abs(storedTimeDeltaDifference) * sensitivity;
  tone(SPEAKER_PIN, toneFrequency);
  if (digitalRead(RESET_BTN_PIN) == LOW)
  {
    storedTimeDelta = 0;
  }
}

float mapFloat(int input, int inMin, int inMax, float outMin, float outMax)
{
  float scale = (float)(input - inMin) / (inMax - inMin);
  return ((outMax - outMin) * scale) + outMin;
}
