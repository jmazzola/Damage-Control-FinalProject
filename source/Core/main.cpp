/***********************************************
* Filename:			main.cpp
* Date:      		05/13/2015
* Mod. Date:		05/13/2015
* Mod. Initials:	MJG
* Author:    		Michael Gergar
* Purpose:			This is the entry point
*				for the application, everything
*				starts here
************************************************/

#include "../Core/stdafx.h"

#include <ctime>
#include "..\State Machine\StateMachine.h"
#include "DebugLogger.h"
#include "..\Input Manager\InputManager.h"

#include "..\Asset Manager\AssetManager.h"

#include "..\Renderer\Renderer.h"
#include "..\Renderer\HUDRenderer.h"

#include "..\Particle Manager\ParticleManager.h"
#include "../AchieveTracker.h"

#include <crtdbg.h>
//#include <vld.h>
//#define _CRTDBG_MAP_ALLOC

#ifndef _BUILD_TURNIN_
//#define _BUILD_TURNIN_
#endif

HWND window;
HCURSOR cursor;
RECT rcOldClip;

/*****************************************************************
* main() - Entry point for the application, currently just loops
*			StateMachine's update
*
* Ins:			  HINSTANCE, HINSTANCE, LPWSTR, int
*
* Outs:		      void
*
* Returns:		  int
*
* Mod. Date:		      05/13/2015
* Mod. Initials:	      MJG
*****************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
					_In_opt_ HINSTANCE hPrevInstance,
					_In_ LPWSTR lpCmdLine,
					_In_ int nShowCmd)
{
	////ENABLE_LEAK_DETECTION();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	_CrtSetBreakAlloc(-1);

	HINSTANCE instance = hInstance;
	//Seeds rand
	srand(unsigned int(time(0)));

#ifdef DEBUG
	//Enable debug logger
	SetupDebugConsole( );
#endif

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = (WNDPROC)WndProc;
	wndClass.lpszClassName = "DamageControl";
	wndClass.hInstance = instance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	wndClass.hIcon = (HICON)LoadImage( // returns a HANDLE so we have to cast to HICON
									  NULL,             // hInstance must be NULL when loading from a file
									  "DamageControl.ico",   // the icon file name
									  IMAGE_ICON,       // specifies that the file is an icon
									  0,                // width of the image (we'll specify default later on)
									  0,                // height of the image
									  LR_LOADFROMFILE |  // we want to load a file (as opposed to a resource)
									  LR_DEFAULTSIZE |   // default metrics based on the type (IMAGE_ICON, 32x32)
									  LR_SHARED         // let the system release the handle when it's no longer used
									  );

	int err = GetLastError( );
	RegisterClassEx(&wndClass);

	//1024 x 768 -- original
	//1440 x 810 -- reduced 1920 x 1080
	UINT width = 1440, height = 810;

#ifdef _BUILD_TURNIN_
	width = 1024;
	height = 768;
#endif

	DWORD dwStyle = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

	RECT wndRect = { 0, 0, (LONG)width, (LONG)height };
	//AdjustWindowRect(&wndRect, dwStyle, FALSE);

	int nWindowWidth = wndRect.right - wndRect.left;
	int nWindowHeight = wndRect.bottom - wndRect.top;

	/*HWND*/ window = CreateWindow("DamageControl",
								   "Corrupted Works | Damage Control",
								   dwStyle,
								   (GetSystemMetrics(SM_CXSCREEN) - nWindowWidth) / 2,
								   (GetSystemMetrics(SM_CYSCREEN) - nWindowHeight) / 2,
								   nWindowWidth,
								   nWindowHeight,
								   NULL,
								   NULL,
								   instance,
								   NULL);

	// set up cursor
	std::string loc = MISCPATH;
	loc += "cursor.cur";
	cursor = LoadCursorFromFileA(loc.c_str( ));

	GetClipCursor(&rcOldClip);
	SetCapture(window);

	// set up input manager
	CInputManager::GetInstance( )->Initialize(window);

	CParticleManager::GetInstance( );

	//initialze the renderer before the window shows so the Device Context
	//	exists when the WM_SIZE message pings.
	CRenderer::GetReference( ).Initialize(window, width, height);
	CHUDRenderer::Initialize( );

	// testing particle system ////////////////////////////////////////////////////////////////////
	//CParticleSystem* particleSystem = CParticleSystem::LoadFrom("Assets/Particles/particles.xml");
	//delete particleSystem;
	///////////////////////////////////////////////////////////////////////////////////////////////

	ShowWindow(window, SW_SHOW);

	//Enable error function
	SetUnhandledExceptionFilter(BasicErrorFunc);

	CStateMachine cApplication;
	cApplication.Start( );

	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && cApplication.GetPlaying( ))
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			cApplication.Update( );
		}
	}

	//Clean up the application
	cApplication.Stop( );

	CInputManager::Shutdown( );

	CHUDRenderer::Shutdown( );
	CRenderer::DeleteInstance( );
	CParticleManager::DeleteInstance( );
	CAchieveTracker::GetInstance( )->Save( );
	CAchieveTracker::Terminate( );

	ClipCursor(&rcOldClip);
	ReleaseCapture( );

	return (int)msg.wParam;
}


void ClipHandler(LPARAM lParam)
{
	RECT win;
	GetClientRect(window, &win);

	POINT point1, point2;
	point1.x = win.left, point1.y = win.top;
	point2.x = win.right, point2.y = win.bottom;

	ClientToScreen(window, &point1);
	ClientToScreen(window, &point2);

	RECT rcClip;
	rcClip.left = point1.x; rcClip.top = point1.y;
	rcClip.right = point2.x; rcClip.bottom = point2.y;

	ClipCursor(&rcClip);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//HWND hwnd_active = GetForegroundWindow( );
	//if (hwnd_active != window)
	//{
	//	ClipCursor(&rcOldClip);
	//	ReleaseCapture( );
	//	BlockInput(TRUE);
	//}

	switch (message)
	{
		case WM_SYSCOMMAND:
		{
			if (wParam == SC_KEYMENU && (lParam >> 16) <= 0)
			{
				return 0;
			}
			//return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}

		//case WM_KILLFOCUS:
		//	{
		//		ClipCursor(&rcOldClip);
		//		ReleaseCapture( );
		//		//BlockInput(TRUE);
		//	}
		//	break;

		case WM_ACTIVATE:
		{
			switch (LOWORD(wParam))
			{
				case WA_CLICKACTIVE:
				case WA_ACTIVE:
				{
					ClipHandler(lParam);
					SetCapture(window);
					//BlockInput(FALSE);
				}
				break;
				case WA_INACTIVE:
				{
					ClipCursor(&rcOldClip);
					ReleaseCapture( );
					//BlockInput(TRUE);
				}
				break;
			}

		}
		break;

		case WM_MOVE:
		{
			ClipHandler(lParam);
			break;
		}
		case WM_SIZE:
		{
			ClipHandler(lParam);

			UINT nWidth = LOWORD(lParam);
			UINT nHeight = HIWORD(lParam);

			CRenderer::GetReference( ).ResizeBuffers(nWidth, nHeight);

			break;
		}
		case WM_SETCURSOR:
		{
			int hitTest = LOWORD(lParam);

			if (hitTest == HTCLIENT)
				SetCursor(cursor);
			else
			{
				//ShowCursor(TRUE);
				DefWindowProc(hWnd, WM_SETCURSOR, wParam, lParam);
			}
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
