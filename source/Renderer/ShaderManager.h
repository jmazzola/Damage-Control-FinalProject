/***********************************************
* Filename:  		ShaderManager.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/28/2015
* Mod. Initials:	DG
* Author:    		David Goad
* Purpose:   		Management class for shaders
************************************************/

#ifndef _SHADERMANAGER_H_
#define _SHADERMANAGER_H_

#include "../Core/stdafx.h"

class CShaderManager
{
public:
	enum PixelShader
	{
		PS_TEXTURE,
		PS_SKYBOX,
		PS_FONT,
		PS_DEBUG_LINE,
		PS_PARTICLES,

		PS_GBUFFER,

		//PS_MASK,

		PS_POINT_LIGHTING,
		PS_SPOT_LIGHTING,
		PS_DIRECTIONAL_LIGHTING,

		//PS_POINT_LIGHTING_MSAA,
		//PS_SPOT_LIGHTING_MSAA,
		//PS_DIRECTIONAL_LIGHTING_MSAA,

		PS_FINAL,

		PS_COUNT
	};
	enum VertexShader
	{
		VS_TEST,
		VS_HUD,
		VS_SKYBOX,
		VS_FONT,
		VS_SKINNING,
		VS_DEBUG_LINE,
		VS_PARTICLES,

		VS_Screen,

		VS_GBUFFER,
		VS_GBUFFER_SKINNING,

		//VS_MASK,

		VS_POINT_LIGHTING,
		VS_SPOT_LIGHTING,
		VS_DIRECTIONAL_LIGHTING,

		VS_FINAL,
		VS_FINAL_SKINNING,

		VS_COUNT,
	};
	enum GeometryShader
	{
		GS_NONE,
		GS_PARTICLES,

		GS_POINT_LIGHTING,
		GS_SPOT_LIGHTING,
		GS_DIRECTIONAL_LIGHTING,

		GS_COUNT,
	};

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
	~CShaderManager(void);

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
	static bool LoadShader(char **_ppByteCode, size_t& _unSize, const char* _szFile);

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
	static void DeleteInstance( );

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
	bool ApplyPixelShader(PixelShader shader);

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
	bool ApplyVertexShader(VertexShader shader);

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
	bool ApplyGeometryShader(GeometryShader shader);

	PixelShader GetCurrentPixelShader( )
	{
		return currentPixelShader;
	}
	VertexShader GetCurrentVertexShader( )
	{
		return currentVertexShader;
	}
	GeometryShader GetCurrentGeometryShader( )
	{
		return currentGeometryShader;
	}

	void ClearPixelShader( );
	void ClearVertexShader( );
	void ClearGeometryShader( );

	void ClearPipeline( );

	static CShaderManager &GetReference( );

private:
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
	CShaderManager(void);

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
	CShaderManager(const CShaderManager &)
	{
	}

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
	CShaderManager &operator=(const CShaderManager &)
	{
		return *this;
	}

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
	void CreateShaders( );

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
	ID3D11PixelShader* CShaderManager::CreatePixelShader(std::string _szFileName);

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
	ID3D11VertexShader* CShaderManager::CreateVertexShader(std::string _szFileName);

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
	ID3D11GeometryShader* CShaderManager::CreateGeometryShader(std::string _szFileName);

	static CShaderManager *instancePtr;

	ID3D11VertexShader* m_d3dVertexShader[VS_COUNT];
	ID3D11PixelShader* m_d3dPixelShader[PS_COUNT];
	ID3D11GeometryShader* m_d3dGeometryShader[GS_COUNT];

	PixelShader currentPixelShader;
	VertexShader currentVertexShader;
	GeometryShader currentGeometryShader;
};

#endif //_SHADERMANAGER_H_
