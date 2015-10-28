/***********************************************
* Filename:  		BurnsideTimer.h
* Date:      		05/22/2015
* Mod. Date: 		05/22/2015
* Mod. Initials:	JPM
* Author:    		Johnathon Burnside, Justin Mazzola
* Purpose:   		This class will take care of doing 
*					simple timers.
************************************************/

#ifndef _BURNSIDETIMER_H_
#define _BURNSIDETIMER_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class BurnsideTimer
{
public:

	BurnsideTimer(void)
	{
		QueryPerformanceFrequency(&m_nFrequency);
	}

	void Reset(void)
	{
		QueryPerformanceCounter(&m_nLastTime);
	}

	float GetElapsedTime(void)
	{
		LARGE_INTEGER curTime;
		QueryPerformanceCounter(&curTime);
		return (float)(curTime.QuadPart - m_nLastTime.QuadPart) / m_nFrequency.QuadPart;
	}

private:

	LARGE_INTEGER m_nLastTime;
	LARGE_INTEGER m_nFrequency;
};
#endif

