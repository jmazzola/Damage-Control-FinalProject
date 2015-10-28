#ifndef _CUTSCENE_H_
#define _CUTSCENE_H_

#include "../Core/stdafx.h"

class CFirstPersonCamera;
class CScene;

#include <vector>

struct TFrame
{
	DirectX::XMFLOAT3 m_xmPos;
	DirectX::XMFLOAT2 m_xmAngle;

	TFrame(DirectX::XMFLOAT3 xmf3, DirectX::XMFLOAT2 xmf2)
	{
		m_xmPos = xmf3;
		m_xmAngle = xmf2;
	}
};

class CCutscene
{
public:

	CCutscene() = default;
	CCutscene(CFirstPersonCamera* pFPS, std::vector<TFrame> vFrames);
	~CCutscene();

	void Play();
	void Update(float fDT);

private:

	std::vector<TFrame> m_listNodes;
	CFirstPersonCamera* m_pFPSCam;
	
	int m_iPrevNode;
	int m_iCurrentNode;

};
#endif // _CUTSCENE_H_
