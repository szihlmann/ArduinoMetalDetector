# Timer usages

## Timer 0
- 8-bit timer/counter
- Used natively for millis()/micros() on Arduino --> **Deactivated!!**
- Used to count colpitts oscillator cycles on  T1
- Clock source: External clock source on T1 (ArduinoMicro Pin D12)

## Timer 1
- 16-bit timer/counter
- Used to generate custom clock with high resolution
- new functions Millis()/Micros() are based on this clock to replace native millis()/micros()
- Clock source: 16 MHz clock source

## Timer 3
- 16-bit timer/counter
- Used in conjunction with tone() library

## Timer 4
- 10-bit timer/counter
- Unused