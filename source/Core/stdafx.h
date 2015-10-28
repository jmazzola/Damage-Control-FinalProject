/***********************************************
* Filename:			stdafx.h
* Date:      		05/14/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:			header that includes all the headers that will be
precompiled
************************************************/

#ifndef _STDAFX_H_
#define _STDAFX_H_

#define SHADER_FILEPATH "Assets/Shader Output/"

#define TEXTURESPATH "Assets\\Textures\\"
#define TEXTURESPATH_W L"Assets\\Textures\\"

#define ANIMATIONPATH "Assets\\Animations\\"
#define ANIMATIONPATH_W L"Assets\\Animations\\"

#define MISCPATH "Assets\\Misc\\"

#define IMAGESPATH "Assets\\Images\\"

#define MODELSPATH "Assets\\Models\\"
#define MODELSPATH_W L"Assets\\Models\\"

#define PARTICLESPATH "Assets\\Particles\\"

#define SFXPATH "Assets\\SFX\\"

#define SOUNDPATH "Assets\\Sound\\"

#define XMLPATH "Assets\\XML\\"

#define MODULEIMAGEPATH "Assets\\Images\\Mods\\"

#define TEXTFILESPATH "Assets\\Text files\\"

//Whether or not you should check the fbx file when loading fbxbin files
//	Will check it if the fbxbin isn't found
#ifdef _DEBUG
#define CHECK_FBX
#endif

//includes
#include <Windows.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <Shlwapi.h>

#include <atlbase.h>

#pragma comment(lib, "Shlwapi.lib")

//Namespaces

#include "Common.h"

#include <fstream>
extern std::fstream outputFile;
#define OutputToLog(text)\

#define SafeRelease(x)		{ if(x) { x->Release(); x = 0; } }
#define SafeDelete(x)		{ if(x) { delete x;		x = 0; } }
#define SafeDeleteArr(x)	{ if(x) { delete[] x;	x = 0; } }

#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)				\
{							\
	HRESULT macroHR = (x);  \
	if(FAILED(macroHR))     \
		assert(false);		\
}
#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif

namespace MathHelper
{
	inline DirectX::XMFLOAT3 XMMatrixUp(DirectX::XMFLOAT4X4& m)
	{
		return DirectX::XMFLOAT3(m._12, m._22, m._32);
	}
	inline DirectX::XMFLOAT3 XMMatrixForward(DirectX::XMFLOAT4X4& m)
	{
		return DirectX::XMFLOAT3(m._13, m._23, -m._33);
	}
	inline DirectX::XMFLOAT3 XMMatrixPosition(DirectX::XMFLOAT4X4& m)
	{
		return DirectX::XMFLOAT3(m._41, m._42, m._43);
	}

	inline float Dot(DirectX::XMFLOAT3& a, DirectX::XMFLOAT3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline float Length(DirectX::XMFLOAT3& a)
	{
		return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	}

	inline float SquareLength(DirectX::XMFLOAT3& a)
	{
		return (a.x * a.x + a.y * a.y + a.z * a.z);
	}

	inline DirectX::XMFLOAT3 Normalize(DirectX::XMFLOAT3& v)
	{
		auto len = Length(v);
		return DirectX::XMFLOAT3(v.x / len, v.y / len, v.z / len);
	}

	inline DirectX::XMFLOAT3 Negate(DirectX::XMFLOAT3& v)
	{
		return DirectX::XMFLOAT3(-v.x, -v.y, -v.z);
	}

	inline DirectX::XMFLOAT3 Cross(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
	{
		return DirectX::XMFLOAT3((a.y * b.z - a.z * b.y), (a.z * b.x - a.x * b.z), (a.x * b.y - a.y * b.x));
	}

	inline DirectX::XMFLOAT3 operator + (DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
	{
		return DirectX::XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
	}

	inline DirectX::XMFLOAT3 operator - (DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
	{
		return DirectX::XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
	}

	inline DirectX::XMFLOAT3 operator * (DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
	{
		return DirectX::XMFLOAT3(a.x * b.x, a.y * b.y, a.z * b.z);
	}

	inline DirectX::XMFLOAT3 operator * (float a, DirectX::XMFLOAT3 b)
	{
		return DirectX::XMFLOAT3(a * b.x, a * b.y, a * b.z);
	}

	inline DirectX::XMFLOAT3 operator * (DirectX::XMFLOAT3 a, float b)
	{
		return DirectX::XMFLOAT3(a.x * b, a.y * b, a.z * b);
	}

	inline DirectX::XMFLOAT4 operator + (DirectX::XMFLOAT4 a, DirectX::XMFLOAT4 b)
	{
		return DirectX::XMFLOAT4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
	}

	inline DirectX::XMFLOAT4 operator - (DirectX::XMFLOAT4 a, DirectX::XMFLOAT4 b)
	{
		return DirectX::XMFLOAT4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
	}

	inline DirectX::XMFLOAT4 operator * (DirectX::XMFLOAT4 a, DirectX::XMFLOAT4 b)
	{
		return DirectX::XMFLOAT4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
	}

	inline DirectX::XMFLOAT4 operator * (float a, DirectX::XMFLOAT4 b)
	{
		return DirectX::XMFLOAT4(a * b.x, a * b.y, a * b.z, a * b.w);
	}

	inline DirectX::XMFLOAT4 operator * (DirectX::XMFLOAT4 a, float b)
	{
		return DirectX::XMFLOAT4(a.x * b, a.y * b, a.z * b, a.w * b);
	}

	inline DirectX::XMFLOAT3 operator / (DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
	{
		try
		{
			return DirectX::XMFLOAT3(a.x / b.x, a.y / b.y, a.z / b.z);
		}
		catch (...)
		{
			throw "Logical error : Divide by zero";
		}
	}

	inline DirectX::XMFLOAT3 operator / (DirectX::XMFLOAT3 a, float b)
	{
		try
		{
			return DirectX::XMFLOAT3(a.x / b, a.y / b, a.z / b);
		}
		catch (...)
		{
			throw "Logical error : Divide by zero";
		}
	}
}

namespace CustomPathFuncs
{
	inline void CustomPathRemoveExtension(std::string& szFileName)
	{
		size_t dotIndex = szFileName.find_last_of(".");
		szFileName = (dotIndex == std::string::npos) ? szFileName : szFileName.substr(0, dotIndex);
	}

	inline void CustomPathAddExtension(std::string& szFileName, std::string& szExt)
	{
		size_t nullterm = szFileName.find_first_of('\0');
		if (nullterm != std::string::npos)
		{
			szFileName.erase(nullterm, szFileName.length());
		}

		szFileName += szExt;
	}

	inline void RemoveSubstringInString(std::string& dest, std::string toDelete)
	{
		std::string::size_type n = toDelete.length();

		for (std::string::size_type i = dest.find(toDelete); i != std::string::npos; i = dest.find(toDelete))
		{
			dest.erase(i, n);
		}
	}

}

namespace BitTwiddler
{
	inline void TurnOnBit(int& nField, int nBit) { nField |= (1 << nBit); }

	inline void TurnOffBit(int& nField, int nBit) { nField &= ~(1 << nBit); }

	inline void ToggleBit(int& nField, int nBit) { nField ^= (1 << nBit); }

	inline bool CheckBit(int& nField, int nBit) { return (nField & (1 << nBit)) ? true : false; }

}

#endif //_STDAFX_H_

