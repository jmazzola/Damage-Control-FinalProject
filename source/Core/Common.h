/***********************************************
* Filename:  		Common.h
* Date:      		05/23/2015
* Mod. Date: 		05/23/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		File used for functions
*				potentially used by everyone
************************************************/
#ifndef _COMMON_H_
#define _COMMON_H_

// Dark red = 4
// Purple = 5
// Piss = 6
// Gray = 8
// Blue = 9
// Green = 10
// Cyan = 11
enum class ConsoleColor
{
	Default = 7,
	DarkBlue = FOREGROUND_BLUE,
	DarkRed = FOREGROUND_RED,
	DarkGreen = FOREGROUND_GREEN,
	Piss = 6,
	Red = FOREGROUND_RED | FOREGROUND_INTENSITY,
	Blue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
	Green = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
	Yellow = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
	Cyan = 11,
	Magenta = FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY,
};

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
void DebugPrint(const char* sDebugLog, ConsoleColor eColor = ConsoleColor::Default);

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
void DebugWPrint(const wchar_t* sDebugLog, ConsoleColor eColor = ConsoleColor::Default);

void ChangeConsoleColor(ConsoleColor color);
#endif
