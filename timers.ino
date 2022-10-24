/* The colpitts - ISR routine
 * Counter for colpitt oscillator cycles
 * TCNT0 is incremented on every external clock cycle
 * Interrupt is executed whenever Timer0 reaches TOP, reverts
 * and reaches BOTTOM again,
 * i.e. after 0x1FF + 1 = 512 clock cycles (~every 3.66 ms)
 */
SIGNAL(TIMER0_COMPB_vect)
{
  static uint16_t previousInputCapture = 0;
  static boolean skipCycle = false;
  
  uint16_t inputCapture = ICR1; // Read most recent Input Capture of timer 1
  uint8_t TCNT0_buf = TCNT0; // Read current timer0 counter
  uint8_t timer_overflows = TCNT1_overflowCount; // Read timer 1 overflows
  
  uint32_t elapsedTime;
  uint8_t dataAvailable = newData;
  
  TCNT0_overflowCount++; // Increment clock counter

  if (TCNT0_overflowCount < SIGNAL_CYCLES)
    return; // End interrupt here, number of cycles not yet reached

  // Calculate total elapsed time using input capture
  // Ensure we didn't miss a colpitts-tick before we had the chance to read ICR1. 
  if (TCNT0_buf == 1){
    if (!skipCycle) // If last reading was erroneous, we need to skip one cycle.
    {
      elapsedTime = ((unsigned long) timer_overflows << 16) + inputCapture;
      elapsedTime -= previousInputCapture;
      signalTimeDelta = elapsedTime;
      dataAvailable++;
      newData = dataAvailable;
    }
    skipCycle = false;   
  }
  else
  {
    skipCycle = true; // Skip also next result because it's probably erroneous
  }

  // Prepare for next measurement
  previousInputCapture = inputCapture;
  TCNT0_overflowCount = 0;
  TCNT1_overflowCount = 0;  
}

/* The CLOCK - ISR routine
 * Executed whenever Timer1 reaches TOP and is reset to BOTTOM,
 * i.e. after 0xFFFF + 1 = 65'536 clock cycles (every 4.096 ms)
 */
ISR(TIMER1_OVF_vect)
{
  // For Micros()
  unsigned long a = timer1_micros;
  a += 4096; // 4096 microseconds per timer1 overflow (2^16 / 16 MHz)
  timer1_micros = a;

  // For high precision readout using input capture
  uint8_t b = TCNT1_overflowCount;
  b++; // Increment clock counter
  TCNT1_overflowCount = b;
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
  
  // Clear interrupt flags by setting OCF0B high
  // OCF0B: Timer/Counter 0 Output Compare B Match Flag
  TIFR0 = (1 << OCF0B); 

  // Enable timer Output Compare Match B Interrupt (i.e., ISR(TIMER0_COMPB_vect))
  // Overflows every ~3.66 ms with 140 kHz input
  TIMSK0 = (1 << OCIE0B);
  OCR0B = 0;
}

void setup_timer1()
{
  /* Timer to generate a precise clock with 16 MHz on 16-bit counter
   * Setup timer 1 for 16 MHz clock source (prescaler = 1, CS10)
   * Activate input noise canceller (ICNC1)
   * Input capture on rising edge (ICES1) on Pin ICP1 (Arduino Micro D4)
   * Set WGM(Waveform Generation Mode) to 0 (Normal)
   */
  TCCR1A = 0x0;
  TCCR1B  = (1 << CS10) | (1 << ICNC1) | (1 << ICES1);
  TCCR1C = 0x0;

  // Reset counter and start measuring time
  TCNT1 = 0;
  
  // Clear interrupt flags by setting TOV1 high
  TIFR1 = (1 << TOV1); 

  // Enable timer overflow interrupt (i.e., ISR(TIMER1_OVF_VECT))
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
