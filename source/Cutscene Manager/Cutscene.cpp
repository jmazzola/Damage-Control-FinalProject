#include "Cutscene.h"
#include "../Renderer/FirstPersonCamera.h"


CCutscene::CCutscene(CFirstPersonCamera* pFPS, std::vector<TFrame> inList)
{
	m_listNodes = inList;
	m_pFPSCam = pFPS;
}

CCutscene::~CCutscene()
{
	m_listNodes.clear();
}

void CCutscene::Play()
{
	if (m_listNodes.size() <= 0 || !m_pFPSCam || m_iCurrentNode == m_listNodes.size())
		return;

	//m_pFPSCam->TurnTo(m_listNodes[m_iCurrentNode].m_xmPos);
}

DirectX::XMVECTOR Transform(DirectX::XMVECTOR vector, DirectX::XMVECTOR rotation)
{
	DirectX::XMVECTOR result;
	float x = DirectX::XMVectorGetX(rotation);
	double num = (double)x;
	double expr_16 = num;
	float num2 = (float)(expr_16 + expr_16);
	float y = DirectX::XMVectorGetY(rotation);
	double num3 = (double)y;
	double expr_2B = num3;
	float num4 = (float)(expr_2B + expr_2B);
	float z = DirectX::XMVectorGetZ(rotation);
	double num5 = (double)z;
	double expr_40 = num5;
	float num6 = (float)(expr_40 + expr_40);
	float w = DirectX::XMVectorGetW(rotation);
	float num7 = (float)((double)w * (double)num2);
	float num8 = (float)((double)w * (double)num4);
	float num9 = (float)((double)w * (double)num6);
	float num10 = (float)((double)x * (double)num2);
	float num11 = (float)((double)x * (double)num4);
	float num12 = (float)((double)x * (double)num6);
	float num13 = (float)((double)y * (double)num4);
	float num14 = (float)((double)y * (double)num6);
	float num15 = (float)((double)z * (double)num6);
	float x2 = DirectX::XMVectorGetX(vector);
	float y2 = DirectX::XMVectorGetY(vector);
	float z2 = DirectX::XMVectorGetZ(vector);
	float resX = (float)((1.0 - (double)num13 - (double)num15) * (double)x2 + ((double)num11 - (double)num9) * (double)y2 + ((double)num12 + (double)num8) * (double)z2);
	double num16 = 1.0 - (double)num10;
	float resY = (float)(((double)num11 + (double)num9) * (double)x2 + (num16 - (double)num15) * (double)y2 + ((double)num14 - (double)num7) * (double)z2);
	float resZ = (float)(((double)num14 + (double)num7) * (double)y2 + ((double)num12 - (double)num8) * (double)x2 + (num16 - (double)num13) * (double)z2);
	float resW = 1.0f;

	result = DirectX::XMVectorSet(resX, resY, resZ, resW);
	return result;
}


void CCutscene::Update(float fDT)
{
	//DirectX::XMFLOAT3 destVec = (m_listNodes[m_iCurrentNode].m_xmPos - m_pFPSCam->Position()) * fDT;

	//if (abs(Length(destVec)) < FLT_EPSILON + 50.0f)
	//{
	//	// Update next node
	//	m_iCurrentNode++;

	//	/*// Rotate towards current node
	//	DirectX::XMMATRIX m = DirectX::XMMatrixLookToLH(DirectX::XMLoadFloat3(&m_pFPSCam->Position()), DirectX::XMLoadFloat3(&destVec), DirectX::g_XMIdentityR1);

	//	// Translate camera
	//	m *= DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&destVec));

	//	DirectX::XMVECTOR scale, rot, trans;
	//	DirectX::XMMatrixDecompose(&scale, &rot, &trans, m);

	//	DirectX::XMVECTOR tempQuat = DirectX::XMQuaternionConjugate(rot);

	//	DirectX::XMFLOAT3 forward;
	//	DirectX::XMStoreFloat3(&forward, Transform(DirectX::g_XMIdentityR2, tempQuat));

	//	if (Dot(destVec, forward) < 1.0f)
	//	{

	//	}
	//	else
	//	{

	//	}*/

	//}


	if (m_listNodes.size() <= 0 || !m_pFPSCam)
		return;

	for (size_t i = 0; i < m_listNodes.size(); i++)
	{
		// Grab the vector from the camera to the next node
		DirectX::XMFLOAT3 toVec = m_listNodes[i].m_xmPos - m_pFPSCam->Position();

		m_pFPSCam->UpdateWorld(toVec * fDT);
	}
}
