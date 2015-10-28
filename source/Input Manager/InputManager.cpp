/***********************************************
* Filename:  		InputManager.cpp
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:			Define the input manager using direct
*					input to access information about the
*					input state
************************************************/
#include "InputManager.h"

#include "KeyBindings.h"
#include "..\Math\vec3.h"

#include <Xinput.h>

#pragma comment(lib, "XInput9_1_0.lib")
#pragma comment(lib, "dinput8.lib")

#define checkBitfield(bitfield, index) (bitfield & (1 << index))



/*static*/ CInputManager* CInputManager::g_instance;
/*****************************************************************
* CInputManager() Default constructor for the input manager.
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/13/2015
* Mod. Initials:
*****************************************************************/
CInputManager::CInputManager( )
{
}

/*****************************************************************
* ~CInputManager(): Destroys the entity manager
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/13/2015
* Mod. Initials:
*****************************************************************/
CInputManager::~CInputManager( )
{
	m_pMouse->Release( );
	m_pKeyboard->Release( );
	m_pDirectInput->Release( );
}

/*****************************************************************
* IsButtonDown(): Returs true if 'button' is held down by the user
*
* Ins: Button - The button to check
*
* Outs:
*
* Returns: bool indicating button state
*
* Mod. Date:		      05/13/2015
* Mod. Initials:
*****************************************************************/
bool CInputManager::IsButtonDown(eControls button)
{
	//return m_currBuffer[(int)button] != 0;
	return ((m_currBuffer[(int)button] & Bit_Current) == Bit_Current);
}

/*****************************************************************
* IsButtonUp(): Returs true if the button is not held down by the user
*
* Ins: Button - The button to check
*
* Outs:
*
* Returns: bool indicating button state
*
* Mod. Date:		      05/13/2015
* Mod. Initials:
*****************************************************************/
bool CInputManager::IsButtonUp(eControls button)
{
	//return m_currBuffer[(int)button] == 0;
	return (m_currBuffer[(int)button] & Bit_Current) != Bit_Current;
}

/*****************************************************************
* IsKeyPressed(): Returns true if 'button' is not held down in
*				  the current frame, but was held down in the
*				  previous frame
*
* Ins: Button - the button to check
*
* Outs:
*
* Returns: bool indicating button state
*
* Mod. Date:		      05/13/2015
* Mod. Initials:
*****************************************************************/
bool CInputManager::IsKeyPressed(eControls button)
{
	//return IsButtonDown(button) && !WasKeyDown(button);
	return ((m_currBuffer[(int)button] & Mask_State) == State_Pressed);
}

/*****************************************************************
* Update(): Gathers Xinput information Updates the current
*			and previous input buffers
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/13/2015
* Mod. Initials:
*****************************************************************/
void CInputManager::Update(void)
{
	if (GetActiveWindow( ) != m_hwnd)
		return;
	// copy the current input buffer into the previous buffer
	memcpy(m_prevBuffer, m_currBuffer, sizeof(m_currBuffer));
	// get mouse data
	DIMOUSESTATE mouseState;
	m_pMouse->Acquire( );
	HRESULT hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseState);
	while (FAILED(hr))
		hr = m_pMouse->Acquire( );

	// get keyboard data
	BYTE keyboard[256] = {};
	GetKeyState(0);

#define UPDATE_PREV_KEY( key )			(m_prevBuffer[ key ] = m_currBuffer[ key ])
#define UPDATE_PREV_KEYS( start, stop )	for( int i = start; i <= stop; i++ )	UPDATE_PREV_KEY(i);

	UPDATE_PREV_KEYS(1, 6);
	UPDATE_PREV_KEYS(8, 9);
	UPDATE_PREV_KEYS(12, 13);
	UPDATE_PREV_KEYS(16, 20);
	UPDATE_PREV_KEY(27);
	UPDATE_PREV_KEYS(32, 57);
	UPDATE_PREV_KEYS(65, 93);
	UPDATE_PREV_KEYS(95, 135);
	UPDATE_PREV_KEYS(144, 145);
	UPDATE_PREV_KEYS(160, 183);
	UPDATE_PREV_KEYS(186, 192);
	UPDATE_PREV_KEYS(219, 223);
	UPDATE_PREV_KEY(226);
	UPDATE_PREV_KEYS(246, 251);
	UPDATE_PREV_KEYS(253, 254);

#undef UPDATE_PREV_KEYS
#undef UPDATE_PREV_KEY

	if (GetForegroundWindow() == m_hwnd)
		GetKeyboardState(keyboard);

	// Ignore: 0, 7, 10-11, 14-15, 21-26, 28-31, 58-64, 94, 136-143, 146-159, 184-185, 193-218, 224-225, 227-245, 252, 255
#define UPDATE_KEY( key )			m_currBuffer[ key ] = ((m_currBuffer[ key ] & Bit_Current) >> 1) | (keyboard[ key ])
#define UPDATE_KEYS( start, stop )	for( int i = start; i <= stop; i++ )	UPDATE_KEY(i);

	UPDATE_KEYS(1, 6);
	UPDATE_KEYS(8, 9);
	UPDATE_KEYS(12, 13);
	UPDATE_KEYS(16, 20);
	UPDATE_KEY(27);
	UPDATE_KEYS(32, 57);
	UPDATE_KEYS(65, 93);
	UPDATE_KEYS(95, 135);
	UPDATE_KEYS(144, 145);
	UPDATE_KEYS(160, 183);
	UPDATE_KEYS(186, 192);
	UPDATE_KEYS(219, 223);
	UPDATE_KEY(226);
	UPDATE_KEYS(246, 251);
	UPDATE_KEYS(253, 254);

#undef UPDATE_KEYS
#undef UPDATE_KEY

	// gather gamepad data
	//XINPUT_STATE gamepadState;
	//hr = XInputGetState(0, &gamepadState);

	//for (unsigned int i = 0; i < 10; i++)
	//{
	//	char* currentKey = &m_currBuffer[(int)eControls::Dpad_Up + i];
	//	*currentKey = 0x0;
	//	if (checkBitfield(gamepadState.Gamepad.wButtons, i) != 0)
	//		*currentKey = char(0xff);
	//}

	//m_currBuffer[(int)eControls::Controller_A] = checkBitfield(gamepadState.Gamepad.wButtons, 12) != 0 ? 0xff : 0;
	//m_currBuffer[(int)eControls::Controller_B] = checkBitfield(gamepadState.Gamepad.wButtons, 13) != 0 ? 0xff : 0;
	//m_currBuffer[(int)eControls::Controller_X] = checkBitfield(gamepadState.Gamepad.wButtons, 14) != 0 ? 0xff : 0;
	//m_currBuffer[(int)eControls::Controller_Y] = checkBitfield(gamepadState.Gamepad.wButtons, 15) != 0 ? 0xff : 0;
	/*
		m_fLeftTrigger = (float)gamepadState.Gamepad.bLeftTrigger / 255.f;
		m_fRightTrigger = (float)gamepadState.Gamepad.bRightTrigger / 255.f;*/

	// update the current mouse position
	//m_iMouse_x = mouseState.lX;
	//m_iMouse_y = mouseState.lY;

	POINT mousePos;
	GetCursorPos(&mousePos);

	m_iMouse_x = mousePos.x;
	m_iMouse_y = mousePos.y;

	m_currBuffer[(int)eControls::ScrollWheelUp] = mouseState.lZ > 0 ? 0xff : 0;
	m_currBuffer[(int)eControls::ScrollWheelDown] = mouseState.lZ < 0 ? 0xff : 0;

	// update current mouse buttons
	//m_currBuffer[(int)eControls::MouseLeft] = mouseState.rgbButtons[0] ? 0xff : 0;
	//m_currBuffer[(int)eControls::MouseMiddle] = mouseState.rgbButtons[2] ? 0xff : 0;
	//m_currBuffer[(int)eControls::MouseRight] = mouseState.rgbButtons[1] ? 0xff : 0;

	// keep mouse position relitive to the current window
	RECT windowRect;
	GetWindowRect(m_hwnd, &windowRect);
	m_iMouse_x -= windowRect.left;
	m_iMouse_y -= windowRect.top;
}

/*****************************************************************
* getAimVector(): Returns the direction that the player should
*				  be faceing based on the current input mode.
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/13/2015
* Mod. Initials:
*****************************************************************/
vec3f CInputManager::GetAimVector(void)
{
	/*if (m_inputMode == GAMEPAD)
	{
		XINPUT_STATE state;
		vec3f outval;

		XInputGetState(0, &state);

		outval.x = state.Gamepad.sThumbRX;
		outval.y = state.Gamepad.sThumbRY;
		outval.z = 0.f;

		return outval;
	}
	else*/
		return
		vec3f {
		m_vPlayerPos.x - GetMousePosition( ).x,
		m_vPlayerPos.y - GetMousePosition( ).y,
		0.f
	};
}

/*****************************************************************
* getMovementVector(): Gets the velocity for the player, based
*					   on the current input mode
*
* Ins:
*
* Outs:
*
* Returns: Movement vector for the player
*
* Mod. Date:		      05/13/2015
* Mod. Initials:
*****************************************************************/
vec3f CInputManager::GetMovementVector(void)
{
	vec3f outval;
	ZeroMemory(&outval, sizeof(point));
	//if (m_inputMode == GAMEPAD)
	//{
	//	XINPUT_STATE state;

	//	XInputGetState(0, &state);

	//	outval.x = state.Gamepad.sThumbLX;
	//	outval.y = state.Gamepad.sThumbLY;

	//	//return outval;
	//}
	//else
	{
		if (IsButtonDown(eControls::W))
			outval.z += 1.f;
		else if (IsButtonDown(eControls::S))
			outval.z -= 1.f;
		if (IsButtonDown(eControls::A))
			outval.x -= 1.f;
		else if (IsButtonDown(eControls::D))
			outval.x += 1.f;

		if (IsButtonDown(eControls::W) && IsButtonDown(eControls::D))
		{
			outval.z -= .25f;
			outval.x -= .25f;
		}

		else if (IsButtonDown(eControls::W) && IsButtonDown(eControls::A))
		{
			outval.z -= .25f;
			outval.x += .25f;
		}


		if (IsButtonDown(eControls::S) && IsButtonDown(eControls::D))
		{
			outval.z += .25f;
			outval.x -= .25f;
		}

		else if (IsButtonDown(eControls::S) && IsButtonDown(eControls::A))
		{
			outval.z += .25f;
			outval.x += .25f;
		}



		//return outval;
	}

	return outval;
}

/*****************************************************************
* WasKeyDown() Returns true if the key was held down in the
*			   previous frame
*
* Ins: Button - the button the check
*
* Outs:
*
* Returns: Movement vector for the player
*
* Mod. Date:		      05/13/2015
* Mod. Initials:
*****************************************************************/
bool CInputManager::WasKeyDown(eControls button)
{
	//return m_prevBuffer[(int)button] != 0;
	return ((m_prevBuffer[(int)button] & Bit_Current) == Bit_Current);
}

/*****************************************************************
* Initialize() Creates the input devices and sets up input buffers
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/13/2015
* Mod. Initials:
*****************************************************************/
bool CInputManager::Initialize(HWND hwnd)
{
	g_instance = new CInputManager;
	g_instance->m_hwnd = hwnd;
	HRESULT hr = DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8A, (void**)&g_instance->m_pDirectInput, nullptr);
	// assert inilitization of direct input
	assert(SUCCEEDED(hr) && "Failed to inilitze input manager");

	g_instance->m_pDirectInput->CreateDevice(GUID_SysMouse, &g_instance->m_pMouse, nullptr);
	assert(g_instance->m_pMouse && "Failed to inilitize mouse input!");
	g_instance->m_pMouse->SetDataFormat(&c_dfDIMouse);
	g_instance->m_pMouse->Acquire( );

	g_instance->m_pDirectInput->CreateDevice(GUID_SysKeyboard, &g_instance->m_pKeyboard, nullptr);
	assert(g_instance->m_pKeyboard && "Failed to inilitize keyboard input!");
	g_instance->m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	g_instance->m_pKeyboard->Acquire( );

	// set the current input mode
	g_instance->m_inputMode = KEYBOARD;
	XINPUT_STATE state;
	DWORD result = XInputGetState(0, &state);
	if (result == ERROR_SUCCESS)
		g_instance->m_inputMode = GAMEPAD;

	return true;
}

/*****************************************************************
* Shuts the input manager down
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/13/2015
* Mod. Initials:
*****************************************************************/
void CInputManager::Shutdown(void)
{
	delete g_instance;
}

//const wchar_t* CInputManager::GetKeyName(eControls key) const
//{
//	// Sanity-check the key index
//	assert((int)key >= 0 && (int)key <= 255 && "InputManager::GetKeyName - invalid key (range is 0-255)");
//	if ((int)key < 0 || (int)key > 255)
//		return L"Invalid";
//
//
//	// Access the contiguous block of names (wchar_t[256][32])
//	const wchar_t* names = GetAllKeyNames();
//
//	// Manually index into the array (skip 32 characters per key)
//	return (names + ((int)key * 32));
//}
//
//const wchar_t* CInputManager::GetAllKeyNames(void)
//{
//	// Return all the mouse / keyboard key names in a
//	// contiguous block of wchar_t[256][32]
//	// Literal constant string has static storage
//	return
//		L"Unknown 0x00\0 000000000000000000"
//		L"Left Mouse Button\0 0000000000000"
//		L"Right Mouse Button\0 000000000000"
//		L"Unknown 0x03\0 000000000000000000"
//		L"Middle Mouse Button\0 00000000000"
//		L"X1 Mouse Button\0 000000000000000"
//		L"X2 Mouse Button\0 000000000000000"
//		L"Unknown 0x07\0 000000000000000000"
//		L"Backspace\0 000000000000000000000"
//		L"Tab\0 000000000000000000000000000"
//		L"Unknown 0x0A\0 000000000000000000"
//		L"Unknown 0x0B\0 000000000000000000"
//		L"Clear\0 0000000000000000000000000"
//		L"Enter\0 0000000000000000000000000"
//		L"Unknown 0x0E\0 000000000000000000"
//		L"Unknown 0x0F\0 000000000000000000"
//		L"Shift\0 0000000000000000000000000"
//		L"Control\0 00000000000000000000000"
//		L"Alt\0 000000000000000000000000000"
//		L"Pause\0 0000000000000000000000000"
//		L"Caps Lock\0 000000000000000000000"
//		L"Unknown 0x15\0 000000000000000000"
//		L"Unknown 0x16\0 000000000000000000"
//		L"Unknown 0x17\0 000000000000000000"
//		L"Unknown 0x18\0 000000000000000000"
//		L"Unknown 0x19\0 000000000000000000"
//		L"Unknown 0x1A\0 000000000000000000"
//		L"Escape\0 000000000000000000000000"
//		L"Unknown 0x1C\0 000000000000000000"
//		L"Unknown 0x1D\0 000000000000000000"
//		L"Unknown 0x1E\0 000000000000000000"
//		L"Unknown 0x1F\0 000000000000000000"
//		L"Space\0 0000000000000000000000000"
//		L"Page Up\0 00000000000000000000000"
//		L"Page Down\0 000000000000000000000"
//		L"End\0 000000000000000000000000000"
//		L"Home\0 00000000000000000000000000"
//		L"Left Arrow\0 00000000000000000000"
//		L"Up Arrow\0 0000000000000000000000"
//		L"Right Arrow\0 0000000000000000000"
//		L"Down Arrow\0 00000000000000000000"
//		L"Select\0 000000000000000000000000"
//		L"Print\0 0000000000000000000000000"
//		L"Execute\0 00000000000000000000000"
//		L"Print Screen\0 000000000000000000"
//		L"Insert\0 000000000000000000000000"
//		L"Delete\0 000000000000000000000000"
//		L"Help\0 00000000000000000000000000"
//		L"0\0 00000000000000000000000000000"
//		L"1\0 00000000000000000000000000000"
//		L"2\0 00000000000000000000000000000"
//		L"3\0 00000000000000000000000000000"
//		L"4\0 00000000000000000000000000000"
//		L"5\0 00000000000000000000000000000"
//		L"6\0 00000000000000000000000000000"
//		L"7\0 00000000000000000000000000000"
//		L"8\0 00000000000000000000000000000"
//		L"9\0 00000000000000000000000000000"
//		L"Unknown 0x3A\0 000000000000000000"
//		L"Unknown 0x3B\0 000000000000000000"
//		L"Unknown 0x3C\0 000000000000000000"
//		L"Unknown 0x3D\0 000000000000000000"
//		L"Unknown 0x3E\0 000000000000000000"
//		L"Unknown 0x3F\0 000000000000000000"
//		L"Unknown 0x3A\0 000000000000000000"
//		L"A\0 00000000000000000000000000000"
//		L"B\0 00000000000000000000000000000"
//		L"C\0 00000000000000000000000000000"
//		L"D\0 00000000000000000000000000000"
//		L"E\0 00000000000000000000000000000"
//		L"F\0 00000000000000000000000000000"
//		L"G\0 00000000000000000000000000000"
//		L"H\0 00000000000000000000000000000"
//		L"I\0 00000000000000000000000000000"
//		L"J\0 00000000000000000000000000000"
//		L"K\0 00000000000000000000000000000"
//		L"L\0 00000000000000000000000000000"
//		L"M\0 00000000000000000000000000000"
//		L"N\0 00000000000000000000000000000"
//		L"O\0 00000000000000000000000000000"
//		L"P\0 00000000000000000000000000000"
//		L"Q\0 00000000000000000000000000000"
//		L"R\0 00000000000000000000000000000"
//		L"S\0 00000000000000000000000000000"
//		L"T\0 00000000000000000000000000000"
//		L"U\0 00000000000000000000000000000"
//		L"V\0 00000000000000000000000000000"
//		L"W\0 00000000000000000000000000000"
//		L"X\0 00000000000000000000000000000"
//		L"Y\0 00000000000000000000000000000"
//		L"Z\0 00000000000000000000000000000"
//		L"Left Windows\0 000000000000000000"
//		L"Right Windows\0 00000000000000000"
//		L"Applications\0 000000000000000000"
//		L"Unknown 0x5E\0 000000000000000000"
//		L"Sleep\0 0000000000000000000000000"
//		L"NumPad 0\0 0000000000000000000000"
//		L"NumPad 1\0 0000000000000000000000"
//		L"NumPad 2\0 0000000000000000000000"
//		L"NumPad 3\0 0000000000000000000000"
//		L"NumPad 4\0 0000000000000000000000"
//		L"NumPad 5\0 0000000000000000000000"
//		L"NumPad 6\0 0000000000000000000000"
//		L"NumPad 7\0 0000000000000000000000"
//		L"NumPad 8\0 0000000000000000000000"
//		L"NumPad 9\0 0000000000000000000000"
//		L"Multiply\0 0000000000000000000000"
//		L"Add\0 000000000000000000000000000"
//		L"Separator\0 000000000000000000000"
//		L"Subtract\0 0000000000000000000000"
//		L"Decimal\0 00000000000000000000000"
//		L"Divide\0 000000000000000000000000"
//		L"F1\0 0000000000000000000000000000"
//		L"F2\0 0000000000000000000000000000"
//		L"F3\0 0000000000000000000000000000"
//		L"F4\0 0000000000000000000000000000"
//		L"F5\0 0000000000000000000000000000"
//		L"F6\0 0000000000000000000000000000"
//		L"F7\0 0000000000000000000000000000"
//		L"F8\0 0000000000000000000000000000"
//		L"F9\0 0000000000000000000000000000"
//		L"F10\0 000000000000000000000000000"
//		L"F11\0 000000000000000000000000000"
//		L"F12\0 000000000000000000000000000"
//		L"F13\0 000000000000000000000000000"
//		L"F14\0 000000000000000000000000000"
//		L"F15\0 000000000000000000000000000"
//		L"F16\0 000000000000000000000000000"
//		L"F17\0 000000000000000000000000000"
//		L"F18\0 000000000000000000000000000"
//		L"F19\0 000000000000000000000000000"
//		L"F20\0 000000000000000000000000000"
//		L"F21\0 000000000000000000000000000"
//		L"F22\0 000000000000000000000000000"
//		L"F23\0 000000000000000000000000000"
//		L"F24\0 000000000000000000000000000"
//		L"Unknown 0x88\0 000000000000000000"
//		L"Unknown 0x89\0 000000000000000000"
//		L"Unknown 0x8A\0 000000000000000000"
//		L"Unknown 0x8B\0 000000000000000000"
//		L"Unknown 0x8C\0 000000000000000000"
//		L"Unknown 0x8D\0 000000000000000000"
//		L"Unknown 0x8E\0 000000000000000000"
//		L"Unknown 0x8F\0 000000000000000000"
//		L"Num Lock\0 0000000000000000000000"
//		L"Scroll Lock\0 0000000000000000000"
//		L"Unknown 0x92\0 000000000000000000"
//		L"Unknown 0x93\0 000000000000000000"
//		L"Unknown 0x94\0 000000000000000000"
//		L"Unknown 0x95\0 000000000000000000"
//		L"Unknown 0x96\0 000000000000000000"
//		L"Unknown 0x97\0 000000000000000000"
//		L"Unknown 0x98\0 000000000000000000"
//		L"Unknown 0x99\0 000000000000000000"
//		L"Unknown 0x9A\0 000000000000000000"
//		L"Unknown 0x9B\0 000000000000000000"
//		L"Unknown 0x9C\0 000000000000000000"
//		L"Unknown 0x9D\0 000000000000000000"
//		L"Unknown 0x9E\0 000000000000000000"
//		L"Unknown 0x9F\0 000000000000000000"
//		L"Left Shift\0 00000000000000000000"
//		L"Right Shift\0 0000000000000000000"
//		L"Left Control\0 000000000000000000"
//		L"Right Control\0 00000000000000000"
//		L"Left Alt\0 0000000000000000000000"
//		L"Right Alt\0 000000000000000000000"
//		L"Browser Back\0 000000000000000000"
//		L"Browser Forward\0 000000000000000"
//		L"Browser Refresh\0 000000000000000"
//		L"Browser Stop\0 000000000000000000"
//		L"Browser Search\0 0000000000000000"
//		L"Browser Favorites\0 0000000000000"
//		L"Browser Home\0 000000000000000000"
//		L"Volume Mute\0 0000000000000000000"
//		L"Volume Down\0 0000000000000000000"
//		L"Volume Up\0 000000000000000000000"
//		L"Media Next Track\0 00000000000000"
//		L"Media Previous Track\0 0000000000"
//		L"Media Stop\0 00000000000000000000"
//		L"Media Play / Pause\0 000000000000"
//		L"Launch Mail\0 0000000000000000000"
//		L"Select Media\0 000000000000000000"
//		L"Launch Application 1\0 0000000000"
//		L"Launch Application 2\0 0000000000"
//		L"Unknown 0xB8\0 000000000000000000"
//		L"Unknown 0xB9\0 000000000000000000"
//		L"Semicolon\0 000000000000000000000"
//		L"Plus\0 00000000000000000000000000"
//		L"Comma\0 0000000000000000000000000"
//		L"Minus\0 0000000000000000000000000"
//		L"Period\0 000000000000000000000000"
//		L"Question\0 0000000000000000000000"
//		L"Tilde\0 0000000000000000000000000"
//		L"Unknown 0xC1\0 000000000000000000"
//		L"Unknown 0xC2\0 000000000000000000"
//		L"Unknown 0xC3\0 000000000000000000"
//		L"Unknown 0xC4\0 000000000000000000"
//		L"Unknown 0xC5\0 000000000000000000"
//		L"Unknown 0xC6\0 000000000000000000"
//		L"Unknown 0xC7\0 000000000000000000"
//		L"Unknown 0xC8\0 000000000000000000"
//		L"Unknown 0xC9\0 000000000000000000"
//		L"Unknown 0xCA\0 000000000000000000"
//		L"Unknown 0xCB\0 000000000000000000"
//		L"Unknown 0xCC\0 000000000000000000"
//		L"Unknown 0xCD\0 000000000000000000"
//		L"Unknown 0xCE\0 000000000000000000"
//		L"Unknown 0xCF\0 000000000000000000"
//		L"Unknown 0xD0\0 000000000000000000"
//		L"Unknown 0xD1\0 000000000000000000"
//		L"Unknown 0xD2\0 000000000000000000"
//		L"Unknown 0xD3\0 000000000000000000"
//		L"Unknown 0xD4\0 000000000000000000"
//		L"Unknown 0xD5\0 000000000000000000"
//		L"Unknown 0xD6\0 000000000000000000"
//		L"Unknown 0xD7\0 000000000000000000"
//		L"Unknown 0xD8\0 000000000000000000"
//		L"Unknown 0xD9\0 000000000000000000"
//		L"Unknown 0xDA\0 000000000000000000"
//		L"Open Bracket\0 000000000000000000"
//		L"Pipe\0 00000000000000000000000000"
//		L"Close Bracket\0 00000000000000000"
//		L"Quote\0 0000000000000000000000000"
//		L"OEM 8\0 0000000000000000000000000"
//		L"Unknown 0xE0\0 000000000000000000"
//		L"Unknown 0xE1\0 000000000000000000"
//		L"Backslash\0 000000000000000000000"
//		L"Unknown 0xE3\0 000000000000000000"
//		L"Unknown 0xE4\0 000000000000000000"
//		L"Unknown 0xE5\0 000000000000000000"
//		L"Unknown 0xE6\0 000000000000000000"
//		L"Unknown 0xE7\0 000000000000000000"
//		L"Unknown 0xE8\0 000000000000000000"
//		L"Unknown 0xE9\0 000000000000000000"
//		L"Unknown 0xEA\0 000000000000000000"
//		L"Unknown 0xEB\0 000000000000000000"
//		L"Unknown 0xEC\0 000000000000000000"
//		L"Unknown 0xED\0 000000000000000000"
//		L"Unknown 0xEE\0 000000000000000000"
//		L"Unknown 0xEF\0 000000000000000000"
//		L"Unknown 0xF0\0 000000000000000000"
//		L"Unknown 0xF1\0 000000000000000000"
//		L"Unknown 0xF2\0 000000000000000000"
//		L"Unknown 0xF3\0 000000000000000000"
//		L"Unknown 0xF4\0 000000000000000000"
//		L"Unknown 0xF5\0 000000000000000000"
//		L"Attn\0 00000000000000000000000000"
//		L"CrSel\0 0000000000000000000000000"
//		L"ExSel\0 0000000000000000000000000"
//		L"Erase EOF\0 000000000000000000000"
//		L"Play\0 00000000000000000000000000"
//		L"Zoom\0 00000000000000000000000000"
//		L"Unknown 0xFC\0 000000000000000000"
//		L"PA1\0 000000000000000000000000000"
//		L"OEM Clear\0 000000000000000000000"
//		L"Unknown 0xFC\0 000000000000000000"
//		;
//}

eControls CInputManager::GetAnyKeyPressed(void) const
{


	// Check if any character-key is down first (to match GetAnyCharDown)
	// otherwise, use any key
	eControls any = eControls::None;


	// Check if any character-key is pressed
	for (int key = 255; key >= 0; key--)
	{
		if ((m_currBuffer[key] & Mask_State ) == Bit_Current )
		{
			// Attempt to translate the key into a character
			// (some keys do not convert: Shift, Ctrl, Alt, etc.)
			unsigned int scan = MapVirtualKeyW((unsigned int)key, MAPVK_VK_TO_VSC);

			WCHAR ch[32];
			int result = ToUnicode((unsigned int)key, scan, (const BYTE*)m_currBuffer, ch, 32, 0);
			if (result == 1)				// it is a character-key
			{
				return (eControls)key;
			}
			else if (any == eControls::None)		// it is the first 'any' key
			{
				any = (eControls)key;
			}
		}
	}


	// No character-keys are down
	return any;
}

