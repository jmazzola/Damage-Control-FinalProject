/***********************************************
* Filename:  		InputManager.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:
************************************************/

#ifndef _CINPUTMANAGER_H_
#define _CINPUTMANAGER_H_

#include "..\Core\stdafx.h"

#define DIRECTINPUT_VERSION 0x0800

#include "eControls.h"
#include "..\Math\vec3.h"

typedef DirectX::XMFLOAT3 point;

class CInputManager
{
	enum eInputMode
	{
		KEYBOARD,
		GAMEPAD,
	} m_inputMode;

	enum eBitControls
	{
		Bit_Current = 0x80,
		Bit_Previous = 0x40,
		Bit_Toggled = 0x01,
		Mask_State = 0xC0,
		State_Pressed = Bit_Current,
		State_Released = Bit_Previous,
	};

	point m_vPlayerPos;

	// buffer of currently pressed keys
	char m_currBuffer[(unsigned int)eControls::NUM_KEYS];
	// buffer of previously pressed keys
	char m_prevBuffer[(unsigned int)eControls::NUM_KEYS];

	// the x position of the mouse
	int m_iMouse_x = 0;
	// the y position of the mouse
	int m_iMouse_y = 0;

	// current state of the left trigger
	float m_fLeftTrigger = 0.f;
	// current state of the right trigger
	float m_fRightTrigger = 0.f;

	IDirectInputDevice8* m_pMouse = nullptr;
	IDirectInputDevice8* m_pKeyboard = nullptr;
	IDirectInputDevice8* m_pController = nullptr;
	LPDIRECTINPUT8 m_pDirectInput = nullptr;

	// singleton instance
	static CInputManager* g_instance;

	// window handle
	HWND m_hwnd;

	// private ctor and dtor
	CInputManager( );
	~CInputManager( );
public:
	void	Update(void);

	// returns a boolean indicateing 'button' is currently held down by
	// the user
	bool	IsButtonDown(eControls button);

	// returns a boolean indicating 'keyCode' is not currently held down
	// by the user
	bool	IsButtonUp(eControls button);

	// indicates if 'button' has changed from up to down in the current cycle
	bool	IsKeyPressed(eControls button);

	// indicates that 'keycode' was held down in the previous frame
	bool WasKeyDown(eControls button);

	// gets the state of the left trigger
	float GetLeftTrigger(void) const
	{
		return m_fLeftTrigger;
	}

	// gets the state of the right trigger
	float GetRightTrigger(void) const
	{
		return m_fRightTrigger;
	}

	// gets the current location of the mouse
	vec3f GetMousePosition(void) const
	{
		return vec3f(float(m_iMouse_x), float(m_iMouse_y), 0.f);
	}

	// inilitizes the input manager
	static bool Initialize(HWND);
	static void Shutdown(void);

	eControls GetAnyKeyPressed(void) const;

	static CInputManager* GetInstance(void)
	{
		return g_instance;
	}
	/***************
	*  Accessors
	***************/

	// gets the direction the player should be aiming at
	vec3f GetAimVector(void);

	// gets the direction the player should be moving in
	vec3f GetMovementVector(void);

	/***************
	* Mutators
	***************/

	// sets the current position of the mouse
	void SetMousePosition(int x, int y)
	{
		SetCursorPos(x, y);
	}
};

#endif // _CINPUTMANAGER_H_
