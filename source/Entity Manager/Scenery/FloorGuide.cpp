#include "stdafx.h"
#include "FloorGuide.h"
#include "../../Renderer/Scene.h"
#include "../Entities/Subsystems/Subsystem.h"

//How high above the display should we draw the arrow?
#define ARROW_Y -0.5f
#define LIGHT_RANGE 150.0f
#define COLLIDER_RADIUS 60.0f
#define COLLIDER_HEIGHT 1.0f

//COLORSw
#define REACTOR_BRIGHT (DirectX::XMFLOAT3{5.0f, 20.0f, 6.0f})
#define REACTOR_DIM (DirectX::XMFLOAT3{3.32f, 13.33f, 4.0f})

#define LIFESUPPORT_BRIGHT (DirectX::XMFLOAT3{5.0f, 6.0f, 20.0f})
#define LIFESUPPORT_DIM (DirectX::XMFLOAT3{3.32f, 4.0f, 13.33f})

#define ENGINE_BRIGHT (DirectX::XMFLOAT3{19.0f, 8.0f, 2.5f})
#define ENGINE_DIM (DirectX::XMFLOAT3{12.668f, 5.33f, 3.32f})

#define NORMAL_DURATION 1.2f
#define NORMAL_COUNT 0

#define DAMAGE_DURATION 0.8f
#define DAMAGE_COUNT 1

#define DOWN_DURATION 0.6f
#define DOWN_COUNT 2

namespace FloorGuides
{

	CFloorGuide::CFloorGuide(void)
	{
		//Get the names in there
		m_szFrameName = "FloorGuideFrame";
		m_szReactorName = "FloorGuideReactor";
		m_szLifeSupportName = "FloorGuideLifeSupport";
		m_szEngineName = "FloorGuideEngine";

		//Add a test Floor Guide
		//AddGuide(DirectX::XMFLOAT3(0, 0.6f, -100), 0.0f, 0.05f, 0.2f);

		Reset();
	}

	CFloorGuide::~CFloorGuide()
	{
		//Clear out all the guides
		while (m_lMarkers.begin() != m_lMarkers.end())
		{
			//delete m_lMarkers.front()->m_cDisplay.GetLight();
			delete m_lMarkers.front();
			m_lMarkers.pop_front();
		}
	}

	void CFloorGuide::Reset(void)
	{
		m_eSystemStatus = 0;
		m_uCurrentArrow = 1;
		m_uPrevArrow = 0;
		m_fBlinkProgress = 0;
		m_fBlinkElapsed = 0;
		m_fBlinkDuration = 1.5f;
		m_nBlinkCount = 0;

		CalculateNextBlink();
		ApplyBlink(true);
	}

	void CFloorGuide::Update(float fDt)
	{
		ApplyBlink(ProcessBlink(fDt));
	}

	void CFloorGuide::AddToScene(CScene* pScene)
	{
		if (pScene)
		{
			for (auto iter = m_lMarkers.begin(); iter != m_lMarkers.end(); iter++)
			{
				pScene->AddObject(&((*iter)->m_cFrame));
				pScene->AddObject(&((*iter)->m_cDisplay));
				pScene->AddLight(*((*iter)->m_cDisplay.GetLight()));

			}
		}
	}


	//Adds a guide marker to the floor guide
	void CFloorGuide::AddGuide(TGuideMarker* pMarker)
	{
		if (pMarker)
		{
			m_lMarkers.push_back(pMarker);
		}
	}
	//Creates a guide marker with this information and adds it to the floor guide
	void CFloorGuide::AddGuide(DirectX::XMFLOAT3 xmPosition, float fReactorRotation, float fLifeSupportRotation, float fEngineRotation)
	{
		TGuideMarker* pMarker = new TGuideMarker();
		//Set the positions
		pMarker->m_cDisplay.GetLocalMatrix()._41 = xmPosition.x;
		pMarker->m_cDisplay.GetLocalMatrix()._42 = xmPosition.y + 0.5f;
		pMarker->m_cDisplay.GetLocalMatrix()._43 = xmPosition.z;

		//Set the rotation dudes
		pMarker->m_fReactorRotation = fReactorRotation;
		pMarker->m_fLifeSupportRotation = fLifeSupportRotation;
		pMarker->m_fEngineRotation = fEngineRotation;

		//Set the render setting stuff
		//pMarker->m_cDisplay.SetTag(m_szDisplayName);
		pMarker->m_cDisplay.SetRObjectType(CRObject::ROBJECT_STATIC);
		pMarker->m_cDisplay.SetTag(m_szEngineName);

		//Add a light for good measure
		pMarker->m_cDisplay.GetLocalMatrix()._42 -= 1.0f;
		pMarker->m_cDisplay.SetLightParams(new LightParams(pMarker->m_cDisplay.GetPosition(), DirectX::XMFLOAT3{ 0, 0, 0 }, 0));
		pMarker->m_cDisplay.GetLocalMatrix()._42 += 1.0f;

		//Make a collider
		//For the display
		TAABB* pCollider = new TAABB;
		pCollider->type = TAABB::eAABB;
		pCollider->vMin = pCollider->vMax = pMarker->m_cDisplay.GetPosition();
		pCollider->vMax.x += COLLIDER_RADIUS;
		pCollider->vMax.z += COLLIDER_RADIUS;
		pCollider->vMax.y += COLLIDER_HEIGHT;
		pCollider->vMin.x -= COLLIDER_RADIUS;
		pCollider->vMin.z -= COLLIDER_RADIUS;
		pMarker->m_cDisplay.SetCollider(pCollider);

		//Make the frame
		pMarker->m_cFrame.SetTag(m_szFrameName);
		pMarker->m_cFrame.GetLocalMatrix() = pMarker->m_cDisplay.GetLocalMatrix();
		pMarker->m_cFrame.SetRObjectType(CRObject::ROBJECT_STATIC);
		pMarker->m_cFrame.SetCollider(new TAABB(*pCollider));

		AddGuide(pMarker);
	}

	bool CFloorGuide::ProcessBlink(float fDt)
	{
		m_fBlinkElapsed += fDt;
		m_fBlinkProgress = m_fBlinkElapsed / m_fBlinkDuration;
		bool bReturn = false;

		//We finished this blink
		if (m_fBlinkProgress >= 1)
		{
			//Make sure we get the progress back down
			m_fBlinkProgress -= ((int)m_fBlinkProgress);
			//Restart that same blink
			if (m_nBlinkCount > 0)
			{
				m_nBlinkCount--;
			}
			//There's gonna be a brand new blink!!!
			else
			{
				CalculateNextBlink();
				bReturn = true;
			}
			//Reset the elapsed time
			m_fBlinkElapsed = m_fBlinkDuration * m_fBlinkProgress;
		}
		
		//Calculate the alpha, I mean the light range - m_fBlinkProgress: 0 & 1 are transparent, 0.5 is opaque
		m_fBrightness = (1 - fabsf(m_fBlinkProgress * 2 - 1));
		m_xmCurColor = m_xmTarColor * m_fBrightness;

		return bReturn;
	}

	void CFloorGuide::CalculateNextBlink(void)
	{
		m_uPrevArrow = m_uCurrentArrow;
		//Determine the sequence based on what's damaged
		CalculateSystemStatus();
		bool bDone = false;
		while (!bDone)
		{
			m_uCurrentArrow++;
			if (m_uCurrentArrow > 2)
			{
				m_uCurrentArrow = 0;
			}
			//Reactor
			if (m_uCurrentArrow == 0)
			{
				if (m_eSystemStatus == 0)
				{
					m_nBlinkCount = NORMAL_COUNT;
					m_fBlinkDuration = NORMAL_DURATION;
					m_xmTarColor = REACTOR_DIM;
					bDone = true;
				}
				//If the engine is under attack
				else if (m_eSystemStatus & REACTOR_DAMAGED)
				{
					if (m_eSystemStatus & REACTOR_DOWN)
					{
						m_nBlinkCount = DOWN_COUNT;
						m_fBlinkDuration = DOWN_DURATION;
					}
					else
					{
						m_nBlinkCount = DAMAGE_COUNT;
						m_fBlinkDuration = DAMAGE_DURATION;
					}
					m_xmTarColor = REACTOR_BRIGHT;
					bDone = true;
				}
			}
			//Life Support
			else if (m_uCurrentArrow == 1)
			{
				if (m_eSystemStatus == 0)
				{
					m_nBlinkCount = NORMAL_COUNT;
					m_fBlinkDuration = NORMAL_DURATION;
					m_xmTarColor = LIFESUPPORT_DIM;
					bDone = true;
				}
				//If the engine is under attack
				else if (m_eSystemStatus & LIFESUPPORT_DAMAGED)
				{
					if (m_eSystemStatus & LIFESUPPORT_DOWN)
					{
						m_nBlinkCount = DOWN_COUNT;
						m_fBlinkDuration = DOWN_DURATION;
					}
					else
					{
						m_nBlinkCount = DAMAGE_COUNT;
						m_fBlinkDuration = DAMAGE_DURATION;
					}
					m_xmTarColor = LIFESUPPORT_BRIGHT;
					bDone = true;
				}
			}
			//Engine
			else
			{
				//If nothing is happening
				if (m_eSystemStatus == 0)
				{
					m_nBlinkCount = NORMAL_COUNT;
					m_fBlinkDuration = NORMAL_DURATION;
					m_xmTarColor = ENGINE_DIM;
					bDone = true;
				}
				//If the engine is under attack
				else if (m_eSystemStatus & ENGINE_DAMAGED)
				{
					if (m_eSystemStatus & ENGINE_DOWN)
					{
						m_nBlinkCount = DOWN_COUNT;
						m_fBlinkDuration = DOWN_DURATION;
					}
					else
					{
						m_nBlinkCount = DAMAGE_COUNT;
						m_fBlinkDuration = DAMAGE_DURATION;
					}
					m_xmTarColor = ENGINE_BRIGHT;
					bDone = true;
				}
			}
		}
	}

	void CFloorGuide::CalculateSystemStatus(void)
	{
		m_eSystemStatus = 0;
		//Reactor stuff
		if (m_pReactor)
		{
			if (!m_pReactor->GetAlive())
			{
				m_eSystemStatus |= REACTOR_DOWN;
			}
			if (m_pReactor->GetCurrentHealth() < m_pReactor->GetMaxHealth())
			{
				m_eSystemStatus |= REACTOR_DAMAGED;
			}
		}
		//Reactor stuff
		if (m_pLifeSupport)
		{
			if (!m_pLifeSupport->GetAlive())
			{
				m_eSystemStatus |= LIFESUPPORT_DOWN;
			}
			if (m_pLifeSupport->GetCurrentHealth() < m_pLifeSupport->GetMaxHealth())
			{
				m_eSystemStatus |= LIFESUPPORT_DAMAGED;
			}
		}
		//Reactor stuff
		if (m_pEngine)
		{
			if (!m_pEngine->GetAlive())
			{
				m_eSystemStatus |= ENGINE_DOWN;
			}
			if (m_pEngine->GetCurrentHealth() < m_pEngine->GetMaxHealth())
			{
				m_eSystemStatus |= ENGINE_DAMAGED;
			}
		}

	}

	void CFloorGuide::ApplyBlink(bool bChangeTexture)
	{
		if (m_lMarkers.begin() != m_lMarkers.end())
		{
			//Changes per frame
			for (auto iter = m_lMarkers.begin(); iter != m_lMarkers.end(); iter++)
			{
				(*iter)->m_cDisplay.SetColor(DirectX::XMFLOAT4{ m_xmCurColor.x, m_xmCurColor.y, m_xmCurColor.z, 1 });
				(*iter)->m_cDisplay.GetLight()->Range = m_fBrightness * LIGHT_RANGE;
			}
			//Changes per blink
			if (bChangeTexture)
			{
				//Why a separate foor loop?
				//	Because I'd rather have an extra for loop every few seconds than 20 if checks every frame :)
				for (auto iter = m_lMarkers.begin(); iter != m_lMarkers.end(); iter++)
				{
					//Textures
					(*iter)->m_cDisplay.SetTag(GetCurrTag());
					//Calculate the new Local Matrix
					DirectX::XMFLOAT4X4 xmMat;
					DirectX::XMStoreFloat4x4(&xmMat, DirectX::XMMatrixIdentity());
					xmMat._41 = (*iter)->m_cDisplay.GetLocalMatrix()._41;
					xmMat._42 = (*iter)->m_cDisplay.GetLocalMatrix()._42;
					xmMat._43 = (*iter)->m_cDisplay.GetLocalMatrix()._43;
					DirectX::XMStoreFloat4x4(&xmMat, DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY((*iter)->m_fSystemRotation[m_uCurrentArrow]), DirectX::XMLoadFloat4x4(&xmMat)));
					(*iter)->m_cDisplay.GetLocalMatrix() = xmMat;
					//Colors
					//(*iter)->m_cDisplay.SetColor(DirectX::XMFLOAT4{ m_xmCurColor.x, m_xmCurColor.y, m_xmCurColor.z, 1 });
					(*iter)->m_cDisplay.GetLight()->Color = m_xmTarColor;
				}
			}
		}
	}

	void CFloorGuide::AddSubSystem(CSubSystem* pSystem, unsigned char uWhich)
	{
		if (uWhich < 3)
		{
			m_pSubSystems[uWhich] = pSystem;
		}
	}

	TGuideMarker* CFloorGuide::GetMarker(unsigned int index) const
	{
		if (m_lMarkers.size() <= index)
		{
			return nullptr;
		}
		auto iter = m_lMarkers.begin();
		for (size_t i = 0; i < index && iter != m_lMarkers.end(); i++)
		{
			iter++;
		}
		return (*iter);
	}

}