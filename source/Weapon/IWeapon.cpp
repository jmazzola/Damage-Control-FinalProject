#include "stdafx.h"

#include "IWeapon.h"
#include "../Renderer/DebugRenderer.h"

#include "../State Machine/GameplayState.h"
#include "../Entity Manager/EntityManager.h"
#include "../Entity Manager/Entities/Player.h"
#include "ModBank.h"

using namespace MathHelper;

static float AbsPulse(float time)
{
	const int frequency = 2; // Frequency in Hz
	const int resolution = 25; // How many steps are there between 0 and 1
	return abs(resolution - ((int)(time * frequency * 2 * resolution) % (resolution * 2))) / (float)resolution;
}

CIWeapon::CIWeapon(EGunType eType)
{
	m_eType = eType;

	m_XMBarrel = { 16.314f, 147.025f, 96.879f - 50 };

	m_pLightParams = new LightParams({ 0, 0, 0 }, { 7.5f, 7.5f, (194.0f / 255.0f) * 7.5f }, 0);

	m_pFlash = new CRObject();
	m_pFlash->SetTag("Muzzle Flash");
	m_pFlash->SetColor({ 10, 10, 10, .99f });

	m_pCrosshair = new CRObject;
	m_pCrosshair->SetColor({ 10, 10, 10, .99f });
}

CIWeapon::~CIWeapon()
{
	SafeDelete(m_pModules);
	SafeDelete(m_pLightParams);
	SafeDelete(m_pFlash);
	SafeDelete(m_pCrosshair);
}

void CIWeapon::Update(float dt)
{
	static float fTime = 0.0f; fTime += dt;

	CPlayer* pPlayer = CGameplayState::GetEntityManager()->GetPlayer();

	if (!pPlayer->GetAlive())
		return;

	bool repairing = false;
	auto sys = CGameplayState::GetEntityManager()->GetSubSystems();
	for (auto& s : sys)
	{
		if (s->GetIsRepairing())
			repairing = true;
	}

	m_pCrosshair->SetTag(std::string(pPlayer->GetCurrentWeapon()->GetTag() + " " + "Crosshair"));
	m_pCrosshair->SetColor({ 10, 10, 10, .99f });

	if (m_bReloading)
	{
		m_pCrosshair->SetTag(
			std::string(pPlayer->GetCurrentWeapon()->GetTag() + " " + "Reloading" + " " + "Crosshair"));

		float a = AbsPulse(fTime);
		m_pCrosshair->SetColor({ 10, 10, 10, a <= .99f ? a : .99f });
	}
	else if (repairing)
	{
		m_pCrosshair->SetTag(
			std::string(pPlayer->GetCurrentWeapon()->GetTag() + " " + "Repairing" + " " + "Crosshair"));

		float a = AbsPulse(fTime);
		m_pCrosshair->SetColor({ 10, 10, 10, a <= .99f ? a : .99f });
	}
	else
	{
		fTime = 0;
	}
	MyWorld.Local = pPlayer->GetWeaponBone();

	MyWorld.Local._41 = ((GetZAxis() * 50) + GetPosition()).x;
	MyWorld.Local._42 = ((GetZAxis() * 50) + GetPosition()).y;// +90;
	MyWorld.Local._43 = ((GetZAxis() * 50) + GetPosition()).z;

	float rotation[] =
	{
		0, 15, 30, 45, 60, 75, 90,
	};

	XMStoreFloat4x4(&m_pFlash->GetLocalMatrix(),
					(DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(rotation[rand() % _countof(rotation)])) * DirectX::XMMatrixTranslation(m_XMBarrel.x, m_XMBarrel.y, m_XMBarrel.z)) *
					XMLoadFloat4x4(&MyWorld.Local));

	MyWorld.Local._41 = ((GetZAxis().x * -25) + m_pFlash->GetWorldMatrix()._41);
	MyWorld.Local._42 = ((GetZAxis().y * -25) + m_pFlash->GetWorldMatrix()._42);
	MyWorld.Local._43 = ((GetZAxis().z * -25) + m_pFlash->GetWorldMatrix()._43);

	DirectX::XMFLOAT3 mouse = pPlayer->GetMousePosition();
	DirectX::XMFLOAT3 player = pPlayer->GetPosition();

	DirectX::XMFLOAT3 StartPoint = GetPosition();
	DirectX::XMFLOAT3 EndPoint;

	EndPoint.x = ((GetZAxis() * 2000) + GetPosition()).x;
	EndPoint.y = (GetZAxis() + GetPosition()).y;
	EndPoint.z = ((GetZAxis() * 2000) + GetPosition()).z;

	EndPoint.y = StartPoint.y -= 10;

	m_pLightParams->Position = GetPosition();

	DirectX::XMFLOAT4 Color = { 1, 1, 1, 1 };
	CDebugRenderer::GetReference().LineRenderer(StartPoint, EndPoint, Color);

	DirectX::XMVECTOR r;
	//DirectX::XMMatrixDecompose(&s, &r, &t, XMLoadFloat4x4(&MyWorld.Local));
	r = DirectX::XMQuaternionRotationMatrix(XMLoadFloat4x4(&pPlayer->GetWorldMatrix()));
	XMStoreFloat4x4(&m_pCrosshair->GetLocalMatrix(),
					(DirectX::XMMatrixRotationQuaternion(r) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&mouse))));

	XMStoreFloat4x4(&m_pCrosshair->GetLocalMatrix(),
					DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&m_XMBarrel)) *
					XMLoadFloat4x4(&m_pCrosshair->GetWorldMatrix()));

	//Makes sure our stats are up to date
	if (m_pModules && m_pModules->GetIsChanged())
	{
		m_pModules->ApplyMods(this);
	}
	//Remove the reload order
	m_bToReload = false;
}

bool CIWeapon::Shoot()
{
	if (m_fTotalTime > m_fLastFireTime + 1 / m_fFireRate)
	{
		m_pLightParams->Range = 225;

		DirectX::XMFLOAT4 c = m_pFlash->GetColor();
		m_pFlash->SetColor({ c.x, c.y, c.z, .99f });

		if (CGameplayState::GetEntityManager()->CycleIntoLiveList(CEntity::eBULLET, { 0, 0, 0 }, this))
		{
			CGameplayState::GetEntityManager()->GetPlayer()->GetGameStats()->m_nShotsFired++;
			m_nCurrentMagazine--;
		}

		m_fLastFireTime = m_fTotalTime;
		return true;
	}

	DirectX::XMFLOAT4 c = m_pFlash->GetColor();
	m_pFlash->SetColor({ c.x, c.y, c.z, 0.0f });
	m_pLightParams->Range = 0;

	return false;
}


void CIWeapon::Reset(void)
{
	if (m_pModules)
	{
		m_pModules->Reset();
		m_pModules->ApplyMods(this);
	}
	m_nCurrentMagazine = m_nMaxMagazine;
}

bool CIWeapon::Reload(void)
{
	CPlayer* pPlayer = CGameplayState::GetEntityManager()->GetPlayer();

	if (pPlayer->GetScrapCount() >= GetBulletCost())
	{
		//If we do this the weapons' Update will reload automatically
		m_bToReload = true;

		return true;
	}
	//We have don't enough scrap to reload
	return false;
}
