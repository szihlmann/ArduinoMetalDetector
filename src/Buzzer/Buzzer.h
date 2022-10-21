/*
	Buzzer.h  - Library to output buzzer sound and flashing led
	Uses tone-library
	Created by S. Zihlmann, 28.8.2022
*/

#ifndef Buzzer_h
#define Buzzer_h

class Buzzer
{

  public:
    Buzzer(byte, byte); //ctor
    void update(long);
	void tick(unsigned long);
	void setLowTone(int);
	void setHighTone(int);
	
  private:
	void updateBuzzPeriod(void);
	void ledOutput(boolean);
	boolean stateEngine(void);
	void stopTone(void);
	void playLow(void);
	void playHigh(void);
	long _timeDelta;	
	unsigned long _previousTime = 0;
	unsigned long _currentTime = 0;
	byte _buzzerState = 0;
	byte _buzzDuration = 10; // Duration of a buzz,  milliseconds
	int _buzzerCutOff = 1000; // Buzzer is switched off for longer buzz-periods, milliseconds
	byte _speakerPin;
	byte _LedPin;
	int _lowTone = 500; // Tone frequency Speaker, Hz
	int _highTone = 2000;
	long _buzzPeriod = 0;
	int _tone = 10000;
	enum toneStates {Stopped, Playing_Low, Playing_High};
	toneStates _toneState = Stopped;
	

};

#endif
