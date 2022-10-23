// This signal is called when the timer value TCNT1 reaches OCR1A
// (Note: TCNT1 is incremented on every external clock cycle)
SIGNAL(TIMER1_COMPA_vect)
{
  unsigned long currentTime = micros();
  signalTimeDelta =  currentTime - lastSignalTime;
  lastSignalTime = currentTime;
  uint8_t dataAvailable = newData;

  dataAvailable++;
  newData = dataAvailable;

  // Move OCR1A value ahead
  OCR1A += CYCLES_PER_SIGNAL;
}

void setup_timer1()
{
  // Set WGM(Waveform Generation Mode) to 0 (Normal)
  // Timer runs from 0x00 to 0xFFFF and overruns
  TCCR1A = 0b00000000;
  
  // Set CSS(Clock Speed Selection) to 0b111 (External clock source on T1 pin
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
}
