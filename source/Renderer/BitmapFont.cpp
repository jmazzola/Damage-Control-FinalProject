#include "stdafx.h"
#include "BitmapFont.h"

#include "../Asset Manager/AssetManager.h"

#include "VertexStructures.h"

#include "RenderStateManager.h"
#include "../TinyXML2/tinyxml2.h"

#include "../GUI Manager/GUIManager.h"

using namespace std;
using namespace tinyxml2;

CBitmapFont *CBitmapFont::instancePtr = 0;

void LoadChar(XMLElement* element, map<char, TCharInfo>& rects)
{
	// pull rect values out of file
	int x = atoi(element->Attribute("x"));
	int y = atoi(element->Attribute("y"));
	int width = atoi(element->Attribute("width"));
	int height = atoi(element->Attribute("height"));

	// create the rectangle for this char
	CRectangle r = { (float)x, (float)x + width, (float)y, (float)y + height };

	// find which char this rectangle is for
	int char_index = atoi(element->Attribute("id"));

	// find the y offset for this charecter
	double offset_y = atof(element->Attribute("yoffset"));

	// push the char and its info into the map
	rects[char(char_index)].m_offsetY = (float)offset_y;
	rects[char(char_index)].m_rect = r;
	rects[char(char_index)].m_cValue = char(char_index);
}

CBitmapFont::CBitmapFont(void)
{
	//ID3D11Resource* pT;
	//DirectX::CreateWICTextureFromFile(CRenderer::GetReference().GetDevice( ), L"Assets/Images/OCRFont_0.png", &pT, &fontAtlas);
	//SafeRelease(pT);
	CAssetManager::LoadTexture(L"Assets/Images/OCRFont_0.png", &fontAtlas);

	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.ByteWidth = sizeof(VertexPositionTextureColor) * 6 * numLetters;
	HRESULT hr = CRenderer::GetReference( ).GetDevice( )->CreateBuffer(&vertexBufferDesc, NULL, &fontVertexBuffer);

	tinyxml2::XMLDocument doc;
	doc.LoadFile("Assets/Images/OCRFont.fnt");

	XMLElement* root = doc.FirstChildElement( );
	root->NextSiblingElement( );

	// get the 'chars' element out of the file
	XMLElement* char_root = root->FirstChildElement("chars");

	// allocate an iterator-pointer for each char defined in this document
	XMLElement* char_iter = (XMLElement*)char_root->FirstChild( );

	// while there is still a charecter left to be read...
	while (char_iter)
	{
		// load-in this char
		LoadChar(char_iter, m_charMap);

		// go to the next node in the document
		char_iter = (XMLElement*)char_iter->NextSibling( );
	}

	// find the 'pages' node, the node that stores the filename of this font
	XMLElement* pages_node = (XMLElement*)root->FirstChildElement("pages")->FirstChild( );

	// get the font size from the 'info' node
	XMLElement* info = root->FirstChildElement("info");
	m_nWidthHeight = atoi(info->Attribute("size"));
}

CBitmapFont::~CBitmapFont(void)
{
	SafeRelease(fontAtlas);
	SafeRelease(fontVertexBuffer);
}

CBitmapFont* CBitmapFont::GetInstance( )
{
	if (!instancePtr)
	{
		instancePtr = new CBitmapFont;
	}
	return instancePtr;
}

void CBitmapFont::DeleteInstance( )
{
	delete instancePtr;
	instancePtr = nullptr;
}

bool CBitmapFont::DrawString(const char*text, float posX, float posY, DirectX::XMFLOAT4 color, float size)
{
	BOOL fs = CRenderer::GetReference( ).m_bFullScreen;

	if (!fs)
	{
		//size -= 0.1f;
	}

	CRenderStateManager::GetReference( ).ApplyRenderState(
		CRenderStateManager::eRenderState::FONT);

	InputLayoutManager::GetReference( ).ApplyState(
		InputLayoutManager::VertexFormat::eVERTEX_POSTEXCOLOR);

	UINT stride = sizeof(VertexPositionTextureColor);
	UINT offset = 0;

	CRenderer::GetReference( ).GetContext( )->IASetVertexBuffers(0, 1, &fontVertexBuffer, &stride, &offset);
	CRenderer::GetReference( ).GetContext( )->PSSetShaderResources(0, 1, &fontAtlas);

	// size of one letter and std::string size
	int letterSize = sizeof(VertexPositionTextureColor) * 6;
	int textSize = strlen(text);

	// cut std::string if its to long
	if (textSize > numLetters)
		textSize = numLetters;

	// size of one char on screen
	float cScreenWidth = 32.0f / GUI_WIDTH;// CRenderer::GetReference( ).GetWidth( );
	float cScreenHeight = 32.0f / GUI_HEIGHT;// CRenderer::GetReference( ).GetHeight( );
	// texel Size
	float texelWidth = 32.0f / textureWidth;

	D3D11_MAPPED_SUBRESOURCE mappedSub;
	HRESULT hr = CRenderer::GetReference( ).GetContext( )->Map(fontVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSub);

	// Pointer to buffer subresource
	VertexPositionTextureColor * sprite = (VertexPositionTextureColor*)mappedSub.pData;
	memset(sprite, 0, sizeof(VertexPositionTextureColor) * 6 * numLetters);

	const int indexA = static_cast<int>('A');
	const int indexZ = static_cast<int>('Z');

	float xAt = posX;
	float yAt = posY;

	//loop to build the std::string
	for (int i = 0; i < textSize; i++)
	{
		if (text[i] == '\n')
		{
			yAt -= cScreenHeight;
			xAt = posX;
			continue;
		}
		/*Get starting position of the quad. First Quad is just the posX value ,
		then characterwidth is added.*/

		float thisStartX = xAt; // posX + (cScreenWidth * static_cast<char>(i));
		float thisEndX = xAt + cScreenWidth * size;// thisStartX + cScreenWidth;
		float thisStartY = yAt;// posY;
		float thisEndY = yAt + cScreenHeight * size; // thisStartY + cScreenHeight;

		// Write the position of each 6 verticess to subresource
		sprite[0].position = DirectX::XMFLOAT3(thisEndX, thisEndY, 1.0f);
		sprite[1].position = DirectX::XMFLOAT3(thisEndX, thisStartY, 1.0f);
		sprite[2].position = DirectX::XMFLOAT3(thisStartX, thisStartY, 1.0f);

		sprite[3].position = DirectX::XMFLOAT3(thisStartX, thisStartY, 1.0f);
		sprite[4].position = DirectX::XMFLOAT3(thisStartX, thisEndY, 1.0f);
		sprite[5].position = DirectX::XMFLOAT3(thisEndX, thisEndY, 1.0f);

		// calculate texture coordinates from ASCII values.
		int texLookup = 0;
		int letter = static_cast<int>(text[i]);

		// Blank Space ( As the Blank space is just added after Z,
		//it needs some extra treatment
		if (letter < indexA || letter > indexZ)
		{
			texLookup = (indexZ - indexA) + 1;
		}
		else
			// Characters
		{
			texLookup = (letter - indexA);
		}

		//float texStart = 0.0f + (texelWidth * static_cast<float>(texLookup));
		//float texEnd = texStart + texelWidth;
		TCharInfo& charInfo = m_charMap[text[i]];
		CRectangle& rect = charInfo.m_rect;

		// Apply texture coordinates to subresource
		sprite[0].textureCoordinate = DirectX::XMFLOAT2(rect.GetRight( ) / m_fTexWidth, rect.GetTop( ) / m_fTexHeight);
		sprite[1].textureCoordinate = DirectX::XMFLOAT2(rect.GetRight( ) / m_fTexWidth, rect.GetBottom( ) / m_fTexHeight);
		sprite[2].textureCoordinate = DirectX::XMFLOAT2(rect.GetLeft( ) / m_fTexWidth, rect.GetBottom( ) / m_fTexHeight);

		sprite[3].textureCoordinate = DirectX::XMFLOAT2(rect.GetLeft( ) / m_fTexWidth, rect.GetBottom( ) / m_fTexHeight);
		sprite[4].textureCoordinate = DirectX::XMFLOAT2(rect.GetLeft( ) / m_fTexWidth, rect.GetTop( ) / m_fTexHeight);
		sprite[5].textureCoordinate = DirectX::XMFLOAT2(rect.GetRight( ) / m_fTexWidth, rect.GetTop( ) / m_fTexHeight);

		// apply color
		sprite[0].color = color;
		sprite[1].color = color;
		sprite[2].color = color;
		sprite[3].color = color;
		sprite[4].color = color;
		sprite[5].color = color;

		xAt += cScreenWidth * size;

		//set sprite pointer for next sprite
		sprite = sprite + 6;
	}

	CRenderer::GetReference( ).GetContext( )->Unmap(fontVertexBuffer, 0);
	CRenderer::GetReference( ).GetContext( )->Draw(6 * textSize, 0);
	ClearBuffer( );
	return true;
}

void CBitmapFont::ClearBuffer( )
{
	//D3D11_MAPPED_SUBRESOURCE mappedSub;
	//HRESULT hr = CRenderer::GetReference( ).GetContext( )->Map(fontVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSub);
	//
	//memset(mappedSub.pData, 0, sizeof(VertexPositionTextureColor) * 6 * numLetters);
	//
	//CRenderer::GetReference( ).GetContext( )->Unmap(fontVertexBuffer, 0);

}
