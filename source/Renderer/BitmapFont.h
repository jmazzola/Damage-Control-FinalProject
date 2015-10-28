#ifndef _CBITMAPFONT_H_
#define _CBITMAPFONT_H_

#include "Renderer.h"
#include "../GUI Manager/Rectangle.h"
#include <map>

struct TCharInfo
{
	CRectangle m_rect;

	float m_offsetY;

	char m_cValue;
};

class CBitmapFont
{
public:
	~CBitmapFont(void);

	static CBitmapFont* GetInstance( );

	static void DeleteInstance( );

	const float m_fTexWidth = 1200.f;
	const float m_fTexHeight = 256.f;

	bool DrawString(const char* text, float posX, float posY, DirectX::XMFLOAT4 color = (DirectX::XMFLOAT4)DirectX::Colors::White, float size = 1.f);

	/*****************************************************************
	* ClearBuffer(): Function to clear the vertex buffer.
	*
	* Ins: N/A
	*
	* Outs: N/A
	*
	* Returns: N/A
	*
	* Mod. Date:		08/17/2015
	* Mod. Initials:	JM
	*****************************************************************/
	void ClearBuffer( );

	std::map<char, TCharInfo> m_charMap;

	// the max width and height of any given charecter in the font
	int m_nWidthHeight;

private:
	CBitmapFont(void);
	CBitmapFont(const CBitmapFont &)
	{ }
	CBitmapFont &operator=(const CBitmapFont &)
	{
		return *this;
	}

	static CBitmapFont *instancePtr;

	ID3D11ShaderResourceView * fontAtlas;
	ID3D11Buffer             * fontVertexBuffer;

	int numLetters = 2048;
	float textureWidth = 1024.0f;
};

#endif //_CBITMAPFONT_H_
