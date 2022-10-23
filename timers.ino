/* The colpitts - ISR routine
 * Counter for colpitt oscillator cycles
 * TCNT0 is incremented on every external clock cycle
 * Interrupt is executed whenever Timer0 reaches TOP, reverts
 * and reaches BOTTOM again,
 * i.e. after 0x1FF + 1 = 512 clock cycles (~every 3.66 ms)
 */
SIGNAL(TIMER0_COMPB_vect)
{
  uint8_t dataAvailable = newData;
  unsigned long currentTime = micros();

  TCNT0_OverflowCount++; // Increment overflow counter
  
  if (TCNT0_OverflowCount < SIGNAL_CYCLES)
    return; // End interrupt here
  
  signalTimeDelta =  currentTime - lastSignalTime;
  lastSignalTime = currentTime;
  dataAvailable++;
  newData = dataAvailable;

  // Prepare for next measurement
  TCNT0_OverflowCount = 0;
}

void setup_timer0()
{
  // Warning: This timer 0 override deactivates arduino-native millis() / micros() functionality!
  
  // Used to count cycles of Colpitts oscillator on 8-bit timer (extended to 9 bits = 512)
  // Select external clock on T0-pin (Arduino Micro D6), clock on rising edge
  TCCR0A = (1 << WGM00); // (WGM00, Phase correct PWM mode --> This adds 1 bit resolution)
  TCCR0B = (1 << CS00) | (1 << CS01) | (1 << CS02);
  
  // Reset counter
  TCNT0 = 0;
  
  // Clear interrupt flags by setting TOV0 high
  TIFR0 = (1 << OCF0B); 

  // Enable timer Output Compare Match B Interrup (ie, ISR(TIMER0_COMPB_vect))
  // Overflows every ~3.66 ms with 140 kHz input
  TIMSK0 = (1 << OCIE0B);
  OCR0B = 0;
}
