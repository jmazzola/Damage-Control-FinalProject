/***********************************************
* Filename:  		VertexStructures.h
* Date:      		05/13/2015
* Mod. Date: 		05/14/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:			A series of Input Layouts
************************************************/

#ifndef _VERTEXSTRUCTURES_H_
#define _VERTEXSTRUCTURES_H_

#include "..\Core\stdafx.h"

struct VertexLight
{
	DirectX::XMFLOAT3 Position;
	float Range;
	DirectX::XMFLOAT3 Color;
	DirectX::XMFLOAT3 Direction;
	float SpotInnerAngle;
	float SpotOuterAngle;

	static const int InputElementCount = 5;
	static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct VertexPosition
{
	VertexPosition( )
	{ }

	VertexPosition(DirectX::XMFLOAT3 const& position)
		: position(position)
	{ }

	DirectX::XMFLOAT3 position;

	static const int InputElementCount = 1;
	static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct VertexPositionTexture
{
	VertexPositionTexture( )
	{ }

	VertexPositionTexture(DirectX::XMFLOAT3 const& position, DirectX::XMFLOAT2 const& textureCoordinate)
		: position(position),
		textureCoordinate(textureCoordinate)
	{ }

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 textureCoordinate;

	static const int InputElementCount = 2;
	static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct VertexPositionTextureColor
{
	VertexPositionTextureColor( )
	{ }

	VertexPositionTextureColor(DirectX::XMFLOAT3 const& position, DirectX::XMFLOAT2 const& textureCoordinate, DirectX::XMFLOAT4 const& color)
		: position(position),
		textureCoordinate(textureCoordinate),
		color(color)
	{ }

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 textureCoordinate;
	DirectX::XMFLOAT4 color;

	static const int InputElementCount = 3;
	static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct VertexPositionNormalTexture
{
	VertexPositionNormalTexture( )
	{ }

	VertexPositionNormalTexture(DirectX::XMFLOAT3 const& position, DirectX::XMFLOAT3 const& normal, DirectX::XMFLOAT2 const& textureCoordinate)
		: position(position),
		normal(normal),
		textureCoordinate(textureCoordinate)
	{ }

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 textureCoordinate;
	DirectX::XMFLOAT3 normal;
	//int cpIndex;

	static const int InputElementCount = 3;
	static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct VertexPositionNormalTextureSkinned
{
	VertexPositionNormalTextureSkinned( )
	{ }

	VertexPositionNormalTextureSkinned(DirectX::XMFLOAT3 const& position, DirectX::XMFLOAT3 const& normal, DirectX::XMFLOAT2 const& textureCoordinate, DirectX::XMINT4& bone,
									   DirectX::XMFLOAT4& weights)
									   : position(position),
									   normal(normal),
									   textureCoordinate(textureCoordinate),
									   bone(bone), weights(weights)
	{ }

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 textureCoordinate;
	DirectX::XMFLOAT3 normal;
	DirectX::XMINT4   bone;
	DirectX::XMFLOAT4 weights;
	//int cpIndex;

	static const int InputElementCount = 5;
	static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct VertexPositionNormalTextureTangentBiNormal
{
	VertexPositionNormalTextureTangentBiNormal( )
	{ }

	VertexPositionNormalTextureTangentBiNormal(DirectX::XMFLOAT3 const& position, DirectX::XMFLOAT3 const& normal, DirectX::XMFLOAT2 const& textureCoordinate,
											   DirectX::XMFLOAT4 const& tangent, DirectX::XMFLOAT3 const& binormal) :
											   position(position),
											   normal(normal),
											   textureCoordinate(textureCoordinate),
											   tangent(tangent),
											   binormal(binormal)

	{ }

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 textureCoordinate;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 tangent;
	DirectX::XMFLOAT3 binormal;

	static const int InputElementCount = 5;
	static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

struct VertexPositionNormalTextureTangentBiNormalSkinned
{
	VertexPositionNormalTextureTangentBiNormalSkinned( )
	{ }

	VertexPositionNormalTextureTangentBiNormalSkinned(DirectX::XMFLOAT3 const& position, DirectX::XMFLOAT3 const& normal, DirectX::XMFLOAT2 const& textureCoordinate,
													  DirectX::XMFLOAT4 const& tangent, DirectX::XMFLOAT3 const& binormal, int& cp, DirectX::XMINT4& bone,
													  DirectX::XMFLOAT4& weights) :
													  position(position),
													  normal(normal),
													  textureCoordinate(textureCoordinate),
													  tangent(tangent),
													  binormal(binormal),
													  cpIndex(cp),
													  bone(bone), weights(weights)

	{ }

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 textureCoordinate;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 tangent;
	DirectX::XMFLOAT3 binormal;
	int cpIndex;
	DirectX::XMINT4   bone;
	DirectX::XMFLOAT4 weights;

	static const int InputElementCount = 8;
	static const D3D11_INPUT_ELEMENT_DESC InputElements[InputElementCount];
};

class InputLayoutManager
{
public:
	enum VertexFormat
	{
		eVERTEX_LIGHT,
		eVERTEX_POS, eVERTEX_POSTEX, eVERTEX_POSNORMTEX,
		eVERTEX_POSTEXCOLOR,
		eVERTEX_POSNORMTEXSKINNED,
		eVERTEX_POSNORMTEXTANBIN,
		eVERTEX_POSNORMTEXTANBINSKINNED,
		eVERTEX_MAX
	};

	~InputLayoutManager(void);

	static InputLayoutManager &GetReference( );

	static void DeleteInstance( );

	bool ApplyState(VertexFormat format);

	VertexFormat GetCurrentState( )
	{ }

	void ClearState( );

private:
	InputLayoutManager(void);
	InputLayoutManager(const InputLayoutManager &)
	{ }
	InputLayoutManager &operator=(const InputLayoutManager &)
	{ }

	static InputLayoutManager* instancePtr;

	CComPtr<ID3D11InputLayout> inputLayouts[eVERTEX_MAX];

	VertexFormat currentFormat;
	void Initialize(void);

	void CreateLayout(char* filename, const D3D11_INPUT_ELEMENT_DESC* inputElements, UINT inputElementsCount, VertexFormat format);
};

#endif // _VERTEXSTRUCTURES_H_
