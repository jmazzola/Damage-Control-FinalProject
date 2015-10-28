/***********************************************
* Filename:			Timer.h
* Date:      		05/12/2015
* Mod. Date:		12/21/2012
* Mod. Initials:	LN
* Author:    		L.Norri CD DRX FullSail University
* Purpose:			CTimer is a timer class designed
*		to be used by D3D11 graphics applications.
*		(use one per thread)
*		Use it for tracking time intervals in 
*		seconds with double percision.
*		It also supports weighted time smoothing 
*		for time based movement. 
*		(should not be used for tracking time)
*		Future versions may support uploading 
*		time data across multiple threads. 
*		(data sent to seperate thread profiler)
************************************************/
 
#ifndef _CTIMER_H_
#define _CTIMER_H_

#include "..\Core\stdafx.h"
#include "Windows.h" // needed for timer ops

 
class CTimer
{
	// per thread timing data
	struct THREAD_DATA
	{
		LARGE_INTEGER signals[256], frequency, start;
		double totalTime, deltaTime, smoothDelta, blendWeight;
		unsigned int threadID, threadDelay;
		unsigned char signalCount, numSamples;
	}localStack; // instance of timing data on this thread

public:
	// Initialize the timer, 
	// samples tracks how many previous signals are used for weighted delta smoothing.
	// smoothFactor is used to assist computing a weighted average.
	// It should be a value less than one. it indicates the rate of falling importance of following samples.
	// Ex: 0.5 would mean that the second sample would be 50% less important the the first and the third would be 25% and so on. 
	CTimer(unsigned char samples = 10, double smoothFactor = 0.75);
	// Restarts the timer on the current thread (will clear all stored signals and total time)
	void Restart();
	// How Much time since the timer was restarted and the last call to signal?(fast)
	double TotalTime(); // will be the same per frame
	// How Much time has passed exactly since the timer was restarted?(slower)
	double TotalTimeExact(); // will be different between calls
	// This marks the cycling time period, typically called only once per frame per thread.
	void Signal();
	// what is the change in time? (between last 2 signals on this thread)
	double Delta();
	// Because the delta is one frame behind and not a linear change, visual motion can appear rough compared to fixed time steps.  
	// This method can be used to reterive a more granular version of the delta.(Better for motion)
	// This is acheived by using previous Delta values to compute a weighted running average.
	double SmoothDelta();
	// Use the "targetHz" parameter to enable thread throttling.
	// By default, thread throttling is not enabled "0". However by specifying a non-zero targetHz,
	// the Throttle function will attempt to gradually adjust the threads speed to match the target Hz by calling "Sleep".
	// this function is best called once per frame per thread just like "Signal"
	// the "smoothing" parameter determines if "SmoothDelta" or "Delta" will be used. 
	void Throttle(double targetHz, bool smoothing = false);
};

#endif //_CTIMER_H_
