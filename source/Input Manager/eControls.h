#ifndef _KEYS_H
#define _KEYS_H
#define _WIN32_LEAN_AND_MEAN

#include "..\Core\stdafx.h"

#include <dinput.h>

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//		Author:Michael Mozdzierz															//
//		Date: 10/06/2014																	//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////



// Enumerates key values from the keyboard
typedef enum class eControls
{
	//None = 0,
	//ESCAPE = DIK_ESCAPE,
	//OEM_1,
	//OEM_2,
	//OEM_3,
	//OEM_4,
	//OEM_5,
	//OEM_6,
	//OEM_7,
	//OEM_8,
	//OEM_9,
	//OEM_0,
	//Minus,
	//Equals,
	//Backspace,
	//Tab,

	//Q, W, E, R, T, Y, U, I, O, P,

	//LeftBracket,
	//RightBracket,
	//Enter,
	//LeftControl,

	//A, S, D, F, G, H, J, K, L,

	////LeftArrow = VK_LEFT,
	////UpArrow = VK_UP,
	////RightArrow = VK_RIGHT,
	////DownArrow = VK_DOWN,

	//SemiColon,
	//Apostrophe,
	//LeftShift = DIK_LSHIFT,
	//BackSlash,
	// 
	//Z, X, C, V, B, N, M,

	//Comma,
	//Period,
	//Slash,
	//RightShift,
	//NUMPAD_STAR,
	//LeftAlt,
	//Spacebar = DIK_SPACE,
	//CapsLock,

	//F1, 
	//F2, 
	//F3, 
	//F4, 
	//F5, 
	//F6, 
	//F7, 
	//F8, 
	//F9, 
	//F10,

	//Numlock,
	//ScrollLock,

	//NUMPAD_7,
	//NUMPAD_8, 
	//NUMPAD_9, 

	//NUMPAD_Subtract,

	//NUMPAD_4, 
	//NUMPAD_5, 
	//NUMPAD_6,

	//NUMPAD_Add,

	//NUMPAD_1, 
	//NUMPAD_2, 
	//NUMPAD_3,
	//NUMPAD_0,

	//NUMLOCK_Decimal,

	//F11 = DIK_F11,
	//F12, 
	//F13,

	//RightControl = DIK_RCONTROL,

	//MouseLeft = 256,
	//MouseRight,
	//MouseMiddle,

	//ScrollWheelUp,
	//ScrollWheelDown,

	//Dpad_Up,
	//Dpad_Down,
	//Dpad_Left,
	//Dpad_Right,

	//Controller_Start,
	//Controller_Back,
	//Controller_LeftStick,
	//Controller_RightStick,
	//Controller_LeftShoulder,
	//Controller_RightShoulder,
	//Controller_A,
	//Controller_B,
	//Controller_X,
	//Controller_Y,

None = 0,	// NO KEY

MouseLeft = 1, LButton = 1,	// Left Mouse Button
MouseRight = 2, RButton = 2,	// Right Mouse Button
Break = 3, Cancel = 3,
MouseMiddle = 4, MButton = 4,	// Middle Mouse Button
MouseX1 = 5, XButton1 = 5,	// X1 Mouse Button (back?)
MouseX2 = 6, XButton2 = 6,	// X2 Mouse Button (forward?)
Backspace = 8, Back = 8,
Tab = 9,
Clear = 12,
Enter = 13, Return = 13,
Shift = 16,								// Either Shift Key
Control = 17,								// Either Ctrl Key
Alt = 18, Menu = 18,	// Either Alt / Menu Key
Pause = 19,
CapsLock = 20, Capital = 20,
Escape = 27, ESCAPE = 27,
Spacebar = 32, Space = 32,
PageUp = 33, Prior = 33,
PageDown = 34, Next = 34,
End = 35,
Home = 36,
LeftArrow = 37, Left = 37,
UpArrow = 38, Up = 38,
RightArrow = 39, Right = 39,
DownArrow = 40, Down = 40,
Select = 41,
Print = 42,
Execute = 43,
PrintScreen = 44, Snapshot = 44,
Insert = 45,
Delete = 46,
Help = 47,
N0 = 48, Zero = 48, OEM_0 = 48,	// Number 0
N1 = 49, One = 49, OEM_1 = 49,// Number 1
N2 = 50, Two = 50, OEM_2 = 50,// Number 2
N3 = 51, Three = 51, OEM_3 = 51,// Number 3
N4 = 52, Four = 52, OEM_4 = 52,// Number 4
N5 = 53, Five = 53, OEM_5 = 53,// Number 5
N6 = 54, Six = 54, OEM_6 = 54,// Number 6
N7 = 55, Seven = 55, OEM_7 = 55,// Number 7
N8 = 56, Eight = 56, OEM_8 = 56,// Number 8
N9 = 57, Nine = 57, OEM_9 = 57,// Number 9
A = 65,
B = 66,
C = 67,
D = 68,
E = 69,
F = 70,
G = 71,
H = 72,
I = 73,
J = 74,
K = 75,
L = 76,
M = 77,
N = 78,
O = 79,
P = 80,
Q = 81,
R = 82,
S = 83,
T = 84,
U = 85,
V = 86,
W = 87,
X = 88,
Y = 89,
Z = 90,
LeftWindows = 91,
RightWindows = 92,
Apps = 93,	// Applications
Sleep = 95,
NUMPAD_0 = 96,
NUMPAD_1 = 97,
NUMPAD_2 = 98,
NUMPAD_3 = 99,
NUMPAD_4 = 100,
NUMPAD_5 = 101,
NUMPAD_6 = 102,
NUMPAD_7 = 103,
NUMPAD_8 = 104,
NUMPAD_9 = 105,
Multiply = 106, NUMPAD_STAR = 106,
Add = 107,
Separator = 108,
Subtract = 109,
Decimal = 110,
Divide = 111,
F1 = 112,
F2 = 113,
F3 = 114,
F4 = 115,
F5 = 116,
F6 = 117,
F7 = 118,
F8 = 119,
F9 = 120,
F10 = 121,
F11 = 122,
F12 = 123,
F13 = 124,
F14 = 125,
F15 = 126,
F16 = 127,
F17 = 128,
F18 = 129,
F19 = 130,
F20 = 131,
F21 = 132,
F22 = 133,
F23 = 134,
F24 = 135,
NumLock = 144,
ScrollLock = 145, Scroll = 145,
ScrollWheelUp = 158,
ScrollWheelDown = 159,
LeftShift = 160,
RightShift = 161,
LeftControl = 162,
RightControl = 163,
LeftAlt = 164, LeftMenu = 164,
RightAlt = 165, RightMenu = 165,
BrowserBack = 166,
BrowserForward = 167,
BrowserRefresh = 168,
BrowserStop = 169,
BrowserSearch = 170,
BrowserFavorites = 171,
BrowserHome = 172,
VolumeMute = 173,
VolumeDown = 174,
VolumeUp = 175,
MediaNextTrack = 176,
MediaPreviousTrack = 177,
MediaStop = 178,
MediaPlayPause = 179,
LaunchMail = 180,
SelectMedia = 181, LaunchMediaSelect = 181,
LaunchApplication1 = 182,
LaunchApplication2 = 183,
Semicolon = 186, OemSemicolon = 186,
Plus = 187, OemPlus = 187, Equals = 187,
Comma = 188, OemComma = 188,
Minus = 189, OemMinus = 189,
Period = 190, OemPeriod = 190,
Question = 191, OemQuestion = 191,
Tilde = 192, OemTilde = 192,
OpenBracket = 219, OemOpenBracket = 219,
Pipe = 220, OemPipe = 220,
CloseBracket = 221, OemCloseBracket = 221,
Quote = 222, OemQuote = 222,
Oem8 = 223,	// Miscellaneous
Backslash = 226, OemBackslash = 226,
Attn = 246,
CrSel = 247,
ExSel = 248,
EraseEof = 249,
Play = 250,
Zoom = 251,
Pa1 = 253,
OemClear = 254,


	NUM_KEYS,


} Keys;

enum class MouseButtons
{
};

#endif