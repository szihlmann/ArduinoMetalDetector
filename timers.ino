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
  unsigned long currentTime = Micros();

  TCNT0_overflowCount++; // Increment overflow counter
  
  if (TCNT0_overflowCount < SIGNAL_CYCLES)
    return; // End interrupt here
  
  signalTimeDelta =  currentTime - lastSignalTime;
  lastSignalTime = currentTime;
  dataAvailable++;
  newData = dataAvailable;

  // Prepare for next measurement
  TCNT0_overflowCount = 0;
}

/* The CLOCK - ISR routine
 * Executed whenever Timer1 reaches TOP and is reset to BOTTOM,
 * i.e. after 0xFFFF + 1 = 65'536 clock cycles (every 4.096 ms)
 */
ISR(TIMER1_OVF_vect)
{
  unsigned long a = timer1_micros;
  a += 4096; // 4096 microseconds per timer1 overflow (2^16 / 16 MHz)
  timer1_micros = a; 
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

void setup_timer1()
{
  /* Timer to generate a precise clock with 16 MHz on 16-bit counter
   * Setup timer 1 for 16 MHz clock source (prescaler = 1, CS10)
   * Activate input noise canceller (ICNC1)
   * Set WGM(Waveform Generation Mode) to 0 (Normal)
   */
  TCCR1A = 0x0;
  TCCR1B  = (1 << CS10) | (1 << ICES1);
  TCCR1C = 0x0;

  // Reset counter and start measuring time
  TCNT1 = 0;
  
  // Clear interrupt flags by setting TOV1 high
  TIFR1 = (1 << TOV1); 

  // Enable timer overflow interrupt (ie, ISR(TIMER1_OVF_VECT))
  // Overflows every 4.096 ms
  TIMSK1 |= (1 << TOIE1);
}

// new millis() function based on timer 1
// Overflows simultaneously to micros
unsigned long Millis()
{
  unsigned long m = Micros();
  
  m = (m + 500) / 1000; // Calculate milliseconds (round)

  return m;
}

// new micros() function based on timer 1
// unsigned long timer1_micros: 32 bit = 4'294 seconds = 71 minutes to overflow
unsigned long Micros() {
  unsigned long m;
  uint8_t oldSREG = SREG;
  uint16_t t;
  
  // Disable interrupts while we read timer1_micros or we might get an
  // inconsistent value (e.g. in the middle of a write to timer1_micros)
  cli();
  m = timer1_micros;

  t = TCNT1;

  if ((TIFR1 & (1<<TOV1)) && (t < 32768U))
    m++;

  SREG = oldSREG;
  
  m += (t+8)>>4; // (divide by 16 to get microseconds, add 8 to round properly)  
  
  return m;
}
