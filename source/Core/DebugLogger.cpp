/***********************************************
* Filename:  		DebugLogger.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/13/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		These functions document crashes
*			 and errors for the dev team
************************************************/

//=============================================================================
//=============================================================================
// DebugConsole.cpp
//
// Utility to popup & utilize console window in GUI application.
//
// taken from:
// http://dslweb.nwnexus.com/~ast/dload/guicon.htm
//=============================================================================
//=============================================================================

// includes
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <fstream>
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")
// local
#include "DebugLogger.h"

///////////////////////////////////////////////////////////////////////////////
// SetupDebugConsole
//
// Function to allocate and display conosle window & to re-route all standard
// input, ouput & error streams to it.
///////////////////////////////////////////////////////////////////////////////
void SetupDebugConsole(short bufferWidth, short bufferHeight, short windowWidth, short windowHeight)
{
	// locals
	CONSOLE_SCREEN_BUFFER_INFO  coninfo;
	FILE                       *pFile;
	int                         conHandle;
	HANDLE                      stdHandle;
	SMALL_RECT                  window = { 0, };

	// allocate console
	AllocConsole();

	// reset console properties
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = bufferHeight;
	coninfo.dwSize.X = bufferWidth;
	window.Left = 0;
	window.Top = 0;
	window.Right = windowWidth - 1;
	window.Bottom = windowHeight - 1;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), true, &window);

	// redirect STDOUT to console
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	conHandle = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
	pFile = _fdopen(conHandle, "w");
	*stdout = *pFile;
	setvbuf(stdout, NULL, _IONBF, 0); // unbuffered

	// redirect STDIN to console
	stdHandle = GetStdHandle(STD_INPUT_HANDLE);
	conHandle = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
	pFile = _fdopen(conHandle, "r");
	*stdin = *pFile;
	setvbuf(stdin, NULL, _IONBF, 0); // unbuffered

	// redirect STDERR to console
	stdHandle = GetStdHandle(STD_ERROR_HANDLE);
	conHandle = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
	pFile = _fdopen(conHandle, "w");
	*stderr = *pFile;
	setvbuf(stderr, NULL, _IONBF, 0); // unbuffered

	// route cout, wcout, cin, wcin, wcerr, cerr, wclog & clog as well
	std::ios::sync_with_stdio();
}


LONG WINAPI BasicErrorFunc(_EXCEPTION_POINTERS *pExceptionInfo)
{
	//Name the dump file based on the date/time
	std::string filename = "DC_CrashReport_";
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	filename += std::to_string(sysTime.wMonth);
	filename += '-';
	filename += std::to_string(sysTime.wDay);
	filename += '-';
	filename += std::to_string(sysTime.wYear);
	filename += '_';
	filename += std::to_string(sysTime.wHour);
	filename += '-';
	filename += std::to_string(sysTime.wMinute);
	filename += '-';
	filename += std::to_string(sysTime.wSecond);
	filename += ".mdmp";
	//Make the file
	HANDLE hFile = ::CreateFileA(filename.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		//TODO:Enable more complex dump reports
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = NULL;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
		filename = "Dump File Saved - " + filename;
		MessageBox(NULL, filename.c_str(), NULL, MB_OK);
		::CloseHandle(hFile);


		//TODO:Tell main to enable this errorFunc
		/*
		SetUnhandledExceptionFilter(BasicErrorFunc);
		*/
	}

	return 0;
}