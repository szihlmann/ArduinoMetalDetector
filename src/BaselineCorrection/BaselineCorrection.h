/*
	BaselineCorrection.h  - Library to create a signal background / baseline
	Created by S. Zihlmann, 28.8.2022
*/

#ifndef BaselineCorrection_h
#define BaselineCorrection_h

#include <inttypes.h>

class BaselineCorrection
{

  public:
    BaselineCorrection(); //ctor
	void initialize(uint32_t, unsigned long);
	void update(uint32_t, unsigned long);
	uint32_t getValue(void);
	
  private:
	void _updateBaseline(void);
	int Abs(int);
	
	uint32_t _currentInputValue;
	uint32_t _baseline;
	unsigned long _lastUpdated;
	unsigned long _currentTime;
	
};

#endif
