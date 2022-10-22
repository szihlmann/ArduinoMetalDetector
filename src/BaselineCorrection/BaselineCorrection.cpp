#include "BaselineCorrection.h"
#include <inttypes.h>

//Unsigned long variables are extended size variables for number storage, and store 32 bits (4 bytes).

BaselineCorrection::BaselineCorrection() //ctor
{
}

void BaselineCorrection::initialize(uint32_t initValue, unsigned long currentTime)
{
	_baseline = initValue;
	_currentTime = currentTime;
}

void BaselineCorrection::update(uint32_t currentInputValue, unsigned long currentTime)
{
	_currentInputValue = currentInputValue;
	_currentTime = currentTime;
	_updateBaseline();
}

void BaselineCorrection::_updateBaseline(void)
{
	// Do the background correction
	long difference = (long)(_baseline) - (long)(_currentInputValue);
    int absDifference = Abs((int)difference);
    if (absDifference < 3){
      _baseline = _currentInputValue; // accept small changes
	  _lastUpdated = _currentTime; // Reset time reference
    }
    
    if (_currentTime > (_lastUpdated + 3000)){ // Accept new value if off for 3 seconds
      _baseline = _currentInputValue; // accept large change
      _lastUpdated = _currentTime;
    }
}

uint32_t BaselineCorrection::getValue(void)
{
	return _baseline;
}

int BaselineCorrection::Abs(int value)
{
	if (value  < 0)
		return -value;
	return value;
}



