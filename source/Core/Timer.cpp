#include "Timer.h"

CTimer::CTimer(unsigned char samples, double smoothFactor)
{
	// clear the structure and init basic values
	ZeroMemory(&localStack, sizeof(THREAD_DATA));
	localStack.numSamples = max(1, samples);// one sample is minimum
	localStack.blendWeight = smoothFactor;
	localStack.threadID = GetCurrentThreadId();
	localStack.threadDelay = 0; // no delay by default
	Restart();
}
void CTimer::Restart()
{
	// get processor frequency (length of each tick on this core)
	QueryPerformanceFrequency(&localStack.frequency);
	// reset counters
	localStack.deltaTime = localStack.totalTime = localStack.smoothDelta = 0.0;
	localStack.signalCount = 0;
	// Track the start time
	QueryPerformanceCounter(&localStack.start);
	localStack.signals[localStack.signalCount++] = localStack.start;
}
double CTimer::TotalTime()
{
	return localStack.totalTime;
}
double CTimer::TotalTimeExact()
{
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now); // what is the time right now?
	LONGLONG elapsed = now.QuadPart - localStack.start.QuadPart; // determine time elapsed since the start.
	return double(elapsed) / double(localStack.frequency.QuadPart); // return in seconds
}
// Append to the signal buffer and compute resulting times
void CTimer::Signal()
{
	// make room for the new signal
	memmove_s(localStack.signals + 1u, sizeof(LARGE_INTEGER)* 255, localStack.signals, sizeof(LARGE_INTEGER)* localStack.numSamples);
	// append to the front of signals and up the count (no more than the last index tho)
	QueryPerformanceCounter(localStack.signals);
	localStack.signalCount = min(localStack.signalCount + 1, 255);
	// with our signal buffer updated, we can now compute our timing values
	localStack.totalTime = double((*localStack.signals).QuadPart - localStack.start.QuadPart) / double(localStack.frequency.QuadPart);
	localStack.deltaTime = double(localStack.signals[0].QuadPart - localStack.signals[1].QuadPart) / double(localStack.frequency.QuadPart);
	// with our signal buffer updated we can compute our weighted average for a smoother delta curve.
	double totalWeight = 0, runningWeight = 1;
	LONGLONG totalValue = 0, sampleDelta;
	// loop up to num samples or as many as we have available
	for (unsigned char i = 0; i < min(localStack.numSamples, localStack.signalCount - 1); ++i)
	{
		// determine each delta as we go
		sampleDelta = localStack.signals[i].QuadPart - localStack.signals[i + 1].QuadPart;
		totalValue += LONGLONG(sampleDelta * runningWeight); // this cast is expensive, need to look into optimizing
		totalWeight += runningWeight; // tally all the weights used
		runningWeight *= localStack.blendWeight; // adjust the weight of next delta
	}
	// with our totals calculated, determine the weighted average.
	localStack.smoothDelta = (totalValue / totalWeight) / double(localStack.frequency.QuadPart);
	// done calculating deltas
}
double CTimer::Delta()
{
	return localStack.deltaTime;
}
double CTimer::SmoothDelta()
{
	return localStack.smoothDelta;
}
// Slow down or speed up a thread to match a desired cycle rate(Hz)
void CTimer::Throttle(double targetHz, bool smoothing)
{
	// If thread throttling is requested, attempt to match desired Hz
	if (targetHz)
	{
		// what delta is desired for this thread of execution?
		double desiredDelta = 1.0 / targetHz;
		double currentDelta = (smoothing) ? localStack.smoothDelta : localStack.deltaTime;
		// how long did we previously sleep to acheive the currentDelta? (seconds)
		double prevDelay = localStack.threadDelay * 0.001;
		// how much time exists between desiredDelta and currentDelta? (seconds)
		double adjustDelta = desiredDelta - currentDelta;
		// tweak the previous delay by the adjustment factor
		double currDelay = prevDelay + adjustDelta;
		// update the thread delay to match the current delay as closely as possible.
		localStack.threadDelay = DWORD(max(0, currDelay * 1000));
		// only sleep if we need to slow down this thread
		if (currDelay > 0)
			Sleep(localStack.threadDelay);
	}
}
