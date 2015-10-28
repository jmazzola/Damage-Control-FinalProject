/***********************************************
* Filename:  		Texture2D.h
* Date:      		05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		This stores information related
*						to 2D images
************************************************/

#ifndef _TEXTURE2D_H_
#define _TEXTURE2D_H_

#include "../Core/stdafx.h"
#include "VertexStructures.h"

#include "RObject.h"

struct TTexture2D
{
	ID3D11ShaderResourceView* m_d3dSRV = NULL;
	//ID3D11Buffer* m_d3dVertexBuffer = NULL;

	//float m_fPosX = 0, m_fPosY = 0;
	//float m_fTextureHeight = 0, m_fTextureWidth = 0;

	//float m_fScreenWidth = 1024, m_fScreenHeight = 768;

	InputLayoutManager::VertexFormat m_eFormat = InputLayoutManager::VertexFormat::eVERTEX_POSTEX;
};

class CTexture2D : public CRObject
{
public:

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	CTexture2D(float _fX,
			   float _fY,
			   float _fWidth,
			   float _fHeight,
			   std::wstring _szFile);

	/*****************************************************************
	* Desc:	Creates a bare bones CTexture3D, only use if you have
	*			a preexisting TTexture2D that several objects are sharing
	*
	*
	* Mod. Date:		      07/09/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	CTexture2D(void);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	~CTexture2D( );

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void CreateBuffers( );

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void UpdateBuffers( );

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void LoadTexture(std::wstring _szFile);

	TTexture2D* RetrieveThyTexture( ) const
	{
		return m_tpTexture;
	}

	void SetScreenPosition(float _fX,
						   float _fY);

	void SetWidth(float _fWidth)
	{
		m_XMSize.x = _fWidth * .5f;
	}
	void SetHeight(float _fHeight)
	{
		m_XMSize.y = _fHeight *.5f;
	}
	//Only use if textures being used are managed elsewhere (like with the modules)
	void SetTexture(TTexture2D* pTexture) { m_tpTexture = pTexture; }
	void SetColor(DirectX::XMFLOAT4 _XMColor)
	{
		m_XMColor = _XMColor;
	}

	void Translate(float _fX, float _fY)
	{
		MyWorld.GetWorld()._41 += _fX;
		MyWorld.GetWorld()._42 += -_fY;
	}
	void Scale(float _fWidth, float _fHeight)
	{
		m_XMScaling.x = _fWidth * .5f;
		m_XMScaling.y = _fHeight *.5f;
	}
	void Rotate(float _fDegrees)
	{
		//DirectX::XMMATRIX local = DirectX::XMLoadFloat4x4(&m_XMWorldMatrix);
		//DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(-_fDegrees));
		//
		//DirectX::XMStoreFloat4x4(&m_XMWorldMatrix, rotation * local);
		m_fDegrees = _fDegrees;
	}

	void SetUVs(DirectX::XMFLOAT4 _pTexCoords[4]);

	//DirectX::XMFLOAT2 GetDimensions( ) const
	//{
	//	return DirectX::XMFLOAT2(m_XMWorldMatrix._11, m_XMWorldMatrix._22);
	//}
	DirectX::XMFLOAT2 GetScale( ) const
	{
		return m_XMScaling;
	}
	float GetRotation( ) const
	{
		return m_fDegrees;
	}
	DirectX::XMFLOAT2 GetSize(void) const
	{
		return m_XMSize;
	}

private:

	TTexture2D* m_tpTexture = NULL;

	float m_fDegrees;
	DirectX::XMFLOAT2 m_XMScaling;
	DirectX::XMFLOAT2 m_XMSize;
};

#endif //_TEXTURE2D_H_
