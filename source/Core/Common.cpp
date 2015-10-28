/***********************************************
* Filename:  		Common.cpp
* Date:      		05/23/2015
* Mod. Date: 		05/23/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		File used for functions
*				potentially used by everyone
************************************************/
#include "stdafx.h"
#include "Common.h"
#include <mutex>
#include <stdlib.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
/*****************************************************************
* DebugPrint(): Prints to the debug logger in a threadsafe manner
*				does nothing if not in Debug Configuration
*
* Ins:			  sDebugLog
*
* Outs:		      void
*
* Returns:		  void
*
* Mod. Date:		      05/23/2015
* Mod. Initials:	      MJG
*****************************************************************/
void DebugPrint(const char* sDebugLog, ConsoleColor eColor)
{
#ifdef _DEBUG
	static std::mutex s_mPrinterMutex;
	//Keeps log ledgible across any possible threads
	s_mPrinterMutex.lock( );
	ChangeConsoleColor(eColor);
	printf(sDebugLog);
	ChangeConsoleColor(ConsoleColor::Default);
	s_mPrinterMutex.unlock( );
	outputFile << sDebugLog;

	OutputDebugString(sDebugLog);
#endif
}

/*****************************************************************
* DebugPrint(): Prints to the debug logger in a threadsafe manner
*				does nothing if not in Debug Configuration
*
* Ins:			  sDebugLog
*
* Outs:		      void
*
* Returns:		  void
*
* Mod. Date:		      05/23/2015
* Mod. Initials:	      MJG
*****************************************************************/
void DebugWPrint(const wchar_t* sDebugLog, ConsoleColor eColor)
{
#ifdef _DEBUG
	static std::mutex s_mPrinterMutex;
	//Keeps log ledgible across any possible threads
	s_mPrinterMutex.lock( );
	ChangeConsoleColor(eColor);
	wprintf(sDebugLog);
	ChangeConsoleColor(ConsoleColor::Default);
	s_mPrinterMutex.unlock( );
#endif

	outputFile << sDebugLog;
	OutputDebugStringW(sDebugLog);
}

/*****************************************************************
* DebugPrint(): Changes the current color of the ouput loger
*
* Ins:			  color - the color to change the text
*
* Outs:		      void
*
* Returns:		  void
*
* Mod. Date:		      05/23/2015
* Mod. Initials:	      MJG
*****************************************************************/
void ChangeConsoleColor(ConsoleColor color)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, (WORD)color);
}
