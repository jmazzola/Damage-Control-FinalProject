#ifndef _CSKYBOX_H_
#define _CSKYBOX_H_

#include "RObject.h"

#define PLANE_SIZE_X 2.0f
#define PLANE_SIZE_Y 2.0f
#define TILES 1.0f

class CSkybox : public CRObject
{
public:

	float m_fSpeedMod;

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
	CSkybox( );

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
	~CSkybox( );

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
	void Update(float _fDT);

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
	void Render( );

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
	//void CreateCube( );

	void CreatePlane( );

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
	//template<typename T>
	//void CreateBuffer(_In_ ID3D11Device* pDevice, T const& data, D3D11_BIND_FLAG bindFlags, _Outptr_ ID3D11Buffer** ppBuffer);

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
	void CreateSkybox( );

	ID3D11ShaderResourceView* m_d3dTexture;

	ID3D11Buffer*	m_d3dVertexBuffer;
	ID3D11Buffer*	m_d3dIndexBuffer;
};

#endif //_CSKYBOX_H_
