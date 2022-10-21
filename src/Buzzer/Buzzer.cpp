#include <Arduino.h>
#include "Buzzer.h"

Buzzer::Buzzer(byte SPEAKER_PIN, byte LedPin) //ctor
{
  _speakerPin = SPEAKER_PIN;
  _LedPin = LedPin;
  pinMode(SPEAKER_PIN, OUTPUT);
  pinMode(LedPin, OUTPUT);
  noTone(SPEAKER_PIN);
}

void Buzzer::update(long timeDelta)
{
  if (_timeDelta != timeDelta)
  {
    _timeDelta = timeDelta;
    updateBuzzPeriod();
  }
}

void Buzzer::updateBuzzPeriod(void)
{
  float fTimeDelta = (float) _timeDelta / 1000.0; // milliseconds
  fTimeDelta = abs(fTimeDelta);
  
  if (_timeDelta == 0) 
    _buzzPeriod = _buzzerCutOff; // Avoid division by zero
  else
  {
    _buzzPeriod = 1001.0 / ( 120.0*fTimeDelta + 1.0);
	//_buzzPeriod = refTimeDelta / (10*abs(_timeDelta * sensitivity)); 
    //_buzzPeriod = (float) 1001 / ( 3.0*(float)abs(_timeDelta) + 1.0);
    //_buzzPeriod = (float) 1001 / (100.0*(float)(1000*abs(_timeDelta))/((float)(refTimeDelta/1000))/5.0 + 1.0);  // 5 Promille Abweichung --> Vollgas
  }
}

void Buzzer::tick(unsigned long currentTime)
{
  _currentTime = currentTime;
  while ( stateEngine() );
}

boolean Buzzer::stateEngine(void)
{
	boolean progress = true;
	
  // Buzzer state machine. Set buzzer and LED
  switch (_buzzerState) {
    case 0: // Check if buzzer is active
	  if (_buzzPeriod < _buzzerCutOff) // Buzzer is active
		_buzzerState++;
	  else
	    progress = false;
      break;
	  
	case 1: // Initiate buzz
	  _previousTime = _currentTime;
	  _buzzerState++;
	  break;

	case 2: // Set tone frequency
	  if (_timeDelta > 0)
	    playHigh(); 
      else
	    playLow();
	  
	  _buzzerState++;
	  break;
	  
    case 3: // Keep playing until buzzDuration ended
	  if (_currentTime > _previousTime + _buzzDuration)
		_buzzerState++;
	  else
	    progress = false;
      break; 
	  
	case 4: // Prepare for stopping
	  if (_buzzPeriod > _buzzDuration) // Continue to stop the buzz (no continuous buzzing)
		stopTone();	// Stop tone & LED
	  _buzzerState++;  
      break; 	 
	  
	case 5: // Wait until duration is over
      if(_currentTime > _previousTime + _buzzPeriod)
	    _buzzerState++;
	  else
	    progress = false;
	  break; 
	
	case 6: // Restart
	  _buzzerState = 0;
	  break;
	  
    default:
      break;
  }
  
  return progress;
}

void Buzzer::stopTone(void)
{
  if (_toneState != Stopped)	
  {
    noTone(_speakerPin);
    ledOutput(false);
    _toneState = Stopped;  
  }
}
void Buzzer::playLow(void)
{
  if (_toneState != Playing_Low)	
  {
    tone(_speakerPin, _lowTone); 
	ledOutput(true);
	_toneState = Playing_Low;
  }
}
void Buzzer::playHigh(void)
{
  if (_toneState != Playing_High)	
  {
    tone(_speakerPin, _highTone); 
	ledOutput(true);
	_toneState = Playing_High;
  }
}
  
void Buzzer::ledOutput(boolean ledStatus)
  {
    if (ledStatus)
	{
	  digitalWrite(_LedPin, HIGH);
	}
	else
	{
	  digitalWrite(_LedPin, LOW);
	}
  }
  
  void Buzzer::setLowTone(int frequency)
  {
	  _lowTone = frequency;
  }
  
  void Buzzer::setHighTone(int frequency)
  {
	  _highTone = frequency;
  }