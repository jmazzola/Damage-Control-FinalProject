/***********************************************
* Filename:  		DebugLogger.h
* Date:      		05/12/2015
* Mod. Date: 		05/13/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		These functions document crashes
*			 and errors for the dev team
************************************************/

//=============================================================================
//=============================================================================
// DebugConsole.h
//
// Utility to popup & utilize console window in GUI application.
//
// taken from:
// http://dslweb.nwnexus.com/~ast/dload/guicon.htms
//=============================================================================
//=============================================================================
#ifndef _DEBUG_CONSOLE_H_
#define _DEBUG_CONSOLE_H_

#include "..\Core\stdafx.h"

// defines
#define MAX_CONSOLE_BUFFER_LINES    300     // console line count
#define MAX_CONSOLE_BUFFER_COLUMNS  80      // console line count
#define MAX_CONSOLE_WINDOW_LINES    24      // console line count
#define MAX_CONSOLE_WINDOW_COLUMNS  80      // console line count

///////////////////////////////////////////////////////////////////////////////
// SetupDebugConsole
//
// Function to allocate and display conosle window & to re-route all standard
// input, ouput & error streams to it.
//
// Note buffer and window Width/Height is in characters NOT pixels.
///////////////////////////////////////////////////////////////////////////////
void SetupDebugConsole(
	short bufferWidth = MAX_CONSOLE_BUFFER_COLUMNS,
	short bufferHeight = MAX_CONSOLE_BUFFER_LINES,
	short windowWidth = MAX_CONSOLE_WINDOW_COLUMNS,
	short windowHeight = MAX_CONSOLE_WINDOW_LINES);

/*****************************************************************
* BasicErrorFunc
*
* Ins:		_EXCEPTION_POINTERS pExceptionInfo
*
* Outs:		void
*
* Returns:	LONG
*
* Mod. Date:			05/13/2015
* Mod. Initials:		MJG
* Author:    		Anner J. Bonilla 
* Purpose:   		Creates a minidump file if
*			 the game crashes without a debugger
************************************************/
LONG WINAPI BasicErrorFunc(_EXCEPTION_POINTERS *pExceptionInfo);

#endif
