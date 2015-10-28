/***********************************************
* Filename:			Player.cpp
* Date:      		05/15/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Michael Mozdzierz
* Purpose:			The player represents the user
*					in the game
************************************************/
#include "Player.h"
#include "PickUp.h"
#include "../../Input Manager/InputManager.h"

#include "../../Renderer/DebugRenderer.h"
#include "../../Renderer/Renderer.h"
#include "../../Renderer/Scene.h"

#include "../../Weapon/Pistol.h"
#include "../../Weapon/MachineGun.h"
#include "../../Weapon/ShotGun.h"
#include "../../Weapon/DroneBuddy.h"
#include "../../Radar.h"
#include "../../Audio Manager/AudioSystemWwise.h"

#include "../../State Machine/GameplayState.h"
#include "../../Animation Manager/Animation State Machine/States/IdleAnimState.h"

#include "../../Weapon/ModBank.h"
#include "../StatusEffects.h"

using namespace MathHelper;

//Normal time between steps
//#define STEP_DELAY 0.25f
#define STEP_DELAY 0.15f
#define CARGO_CAPACITY 15

#define STARTING_SCRAP 1

struct Plane
{
	DirectX::XMFLOAT3 n; // normal
	float d; // distance from origin
};

//Test if segment AB intersects plane p. If so, return 1, along with
//the intersection t value and the intersection point Q. If not, return 0
int IntersectSegmentPlane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, Plane p, float&t, DirectX::XMFLOAT3& q)
{
	//Compute t value at which the directed line ab intersects theplane
	DirectX::XMFLOAT3 ab = b - a;
	t = (p.d - DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&p.n), DirectX::XMLoadFloat3(&a)))) / DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&p.n), DirectX::XMLoadFloat3(&ab)));

	//If t in [0..1] compute and return intersection DirectX::XMFLOAT3
	if (t >= 0.0f&&t <= 1.0f)
	{
		q = a + t*ab;
		return 1;
	}

	//Else t is +INF, -INF, NaN, or not in[0..1], so no intersection
	return 0;
}

//Intersect segment ab against plane of triangle def. If intersecting,
//return t value and position q of intersection
int IntersectSegmentPlane(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 d, DirectX::XMFLOAT3 e, DirectX::XMFLOAT3 f,
						  float& t, DirectX::XMFLOAT3& q)
{
	Plane p;
	DirectX::XMStoreFloat3(&p.n, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&(e - d)), DirectX::XMLoadFloat3(&(f - d)))));
	p.d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&p.n), DirectX::XMLoadFloat3(&d)));
	return IntersectSegmentPlane(a, b, p, t, q);
}

/*****************************************************************
* CPlayer(): Inilitizes the player
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/15/2015
* Mod. Initials:
*****************************************************************/
CPlayer::CPlayer(void) : m_fMoveSpeed(500)
{
	// Set the entity type
	m_nEntityType = CEntity::ePLAYER;

	m_pPistol = m_pCurrentWeapon = new CPistol();
	m_pMachineGun = new CMachineGun();
	m_pShotgun = new CShotGun();
	m_pDroneBuddy = new CDroneBuddy(this);

	//m_pCargo = new CModBank( );
	//m_pCargo->SetBankSize(CARGO_CAPACITY);

	m_pStatusEffects = new CStatusBank();

	m_fCurrentHealth = 100.f;
	m_fMaxHealth = 100.0f;

	tCollider = new TSphere();
	(tCollider)->type = TCollider::eSphere;
	(tCollider)->fRadius = 40.0f;

	AvailableMoveDirs[0] = AvailableMoveDirs[1] = AvailableMoveDirs[2] = AvailableMoveDirs[3] = true;
	//AvailableMoves[0] = AvailableMoves[1] = AvailableMoves[2] = AvailableMoves[3] = true;

	m_eRObjectType = RObjectType::ROBJECT_ANIMATED;

	m_fOxygen = 100.f;
	m_bGodMode = false;

	ModelTransform.parent = &MyWorld;
	DirectX::XMStoreFloat4x4(&ModelTransform.Local, DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(180.0f)));

	m_XMBindRadius = DirectX::XMFLOAT2(95, 95);

	m_nScrapAmount = 0;
	m_nTotalScrapAmount = 0;
	m_bTabShow = false;

	m_pGameStats = new CGameStats();

	m_fDeployTimer = 0.0f;

	isStrafing = -1;
	isWalkingForward = -1;
}

/*****************************************************************
* ~CPlayer() Destroyes an instance of player
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/15/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ CPlayer::~CPlayer(void)
{
	if (m_pPistol)
	{
		delete m_pPistol;
	}
	if (m_pMachineGun)
	{
		delete m_pMachineGun;
	}
	if (m_pShotgun)
	{
		delete m_pShotgun;
	}
	if (m_pDroneBuddy)
	{
		delete m_pDroneBuddy;
	}
	m_pCurrentWeapon = nullptr;
	if (tCollider)
	{
		delete tCollider;
		tCollider = nullptr;
	}
	//if (m_pCargo)
	//{
	//	delete m_pCargo;
	//	m_pCargo = nullptr;
	//}

	if (m_pGameStats)
	{
		delete m_pGameStats;
		m_pGameStats = nullptr;
	}
}

/*****************************************************************
* Update(): Inilitizes a rectangle to zero
*
* Ins: dt - the ammount of time elapsed since the last update
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/15/2015
* Mod. Initials:
*****************************************************************/
/*virtual*/ void CPlayer::Update(float dt) /*override*/
{
	if (!dt || m_bIsAlive == false)
	{
		return;
	}

	CLivingEntities::Update(dt);

	/*std::string strPos("X: " + std::to_string(GetPosition().x));
	strPos.append(" Y: " + std::to_string(GetPosition().y));
	strPos.append(" Z: " + std::to_string(GetPosition().z));
	strPos.append("\n");
	DebugPrint(strPos.c_str());*/

	Input(dt);

	//SplitCircle(dt);
	SplitSquare(dt);

	if (m_pCurrentWeapon)
	{
		m_pCurrentWeapon->Update(dt);
	}

	if (m_pCurrentWeapon == m_pPistol)
	{
		m_pGameStats->m_fFavoriteWeapTimers[CGameStats::PISTOL] += dt;
	}
	else if (m_pCurrentWeapon == m_pMachineGun)
	{
		m_pGameStats->m_fFavoriteWeapTimers[CGameStats::MACHINE_GUN] += dt;
	}
	else if (m_pCurrentWeapon == m_pShotgun)
	{
		m_pGameStats->m_fFavoriteWeapTimers[CGameStats::SHOTGUN] += dt;
	}

	m_pDroneBuddy->Update(dt);

	m_fOxygen -= 1.0f * dt;

	if (m_bGodMode)
	{
		m_fCurrentHealth = m_fMaxHealth;
		m_fOxygen = 100.0f;
	}

	if (m_fOxygen <= 0.f)
	{
		m_fCurrentHealth -= 5.f * dt;
		TakeDamage((5.f * dt));
	}

	if (m_pAnimation)
	{
		GetAnimStateMachine().Update(dt);
		GetAnimationBlender().Process(dt);
		m_cDeformer.Process(GetAnimationBlender().GetPose());

		if (m_pAnimation->m_pModel)
		{
			if (m_pAnimation->m_pModel->m_pVertexBuffer == NULL)
			{
				CGameplayState::GetAssetManager()->CreateBuffer(CRenderer::GetReference().GetDevice(), m_cDeformer.GetSkinVerts(),
																D3D11_BIND_VERTEX_BUFFER,
																&m_pAnimation->m_pModel->m_pVertexBuffer);
			}
		}
	}

	// Handle Deploying timers

	MyWorld.Local = MyWorld.GetWorld();

#ifdef _DEBUG
	DirectX::XMFLOAT4 player_color;
	DirectX::XMStoreFloat4(&player_color, DirectX::Colors::White);
	DirectX::XMFLOAT3 player = GetPosition( );

	CDebugRenderer::GetReference( ).SquareRenderer(player, m_XMBindRadius, player_color);
	CDebugRenderer::GetReference( ).CircleRenderer(player, m_XMBindRadius, player_color);

	DirectX::XMFLOAT4 bind_color;
	DirectX::XMStoreFloat4(&bind_color, DirectX::Colors::White);

	CDebugRenderer::GetReference( ).SquareRenderer(m_XMBindPos, DirectX::XMFLOAT2(50, 50), bind_color);
	CDebugRenderer::GetReference( ).CircleRenderer(m_XMBindPos, DirectX::XMFLOAT2(15, 15), bind_color);
	CDebugRenderer::GetReference( ).CircleRenderer(m_XMBindPos, DirectX::XMFLOAT2(5, 5), bind_color);

	DirectX::XMFLOAT4 mouse_color;
	DirectX::XMStoreFloat4(&mouse_color, DirectX::Colors::White);

	CDebugRenderer::GetReference( ).SquareRenderer(m_XMMousePos, DirectX::XMFLOAT2(50, 50), mouse_color);
	CDebugRenderer::GetReference( ).CircleRenderer(m_XMMousePos, DirectX::XMFLOAT2(15, 15), mouse_color);
	CDebugRenderer::GetReference( ).CircleRenderer(m_XMMousePos, DirectX::XMFLOAT2(5, 5), mouse_color);
#endif

}

void CPlayer::Input(float dt)
{
	if (!GetAlive())
	{
		return;
	}

	if (CInputManager::GetInstance()->IsButtonDown(eControls::LeftControl))
	{
		if (CInputManager::GetInstance()->IsButtonDown(eControls::Equals))
		{
			m_XMBindRadius.x += (25 * dt);
			m_XMBindRadius.y += (25 * dt);
		}
		else if (CInputManager::GetInstance()->IsButtonDown(eControls::Minus))
		{
			m_XMBindRadius.y -= (25 * dt);
			m_XMBindRadius.x -= (25 * dt);
		}

		if (CInputManager::GetInstance()->IsKeyPressed(eControls::NUMPAD_0))
		{
			m_bGodMode = !m_bGodMode;
		}

		/*if (CInputManager::GetInstance()->IsButtonDown(eControls::NUMPAD_STAR))
		{
			m_bIsAlive = false;
		}*/

#ifndef _DEBUG
	  /*DirectX::XMFLOAT4 player_color;
		DirectX::XMStoreFloat4(&player_color, DirectX::Colors::White);
		DirectX::XMFLOAT3 player = GetPosition();

		CDebugRenderer::GetReference().SquareRenderer(player, m_XMBindRadius, player_color);
		CDebugRenderer::GetReference().CircleRenderer(player, m_XMBindRadius, player_color);

		DirectX::XMFLOAT4 bind_color;
		DirectX::XMStoreFloat4(&bind_color, DirectX::Colors::White);

		CDebugRenderer::GetReference().SquareRenderer(m_XMBindPos, DirectX::XMFLOAT2(50, 50), bind_color);
		CDebugRenderer::GetReference().CircleRenderer(m_XMBindPos, DirectX::XMFLOAT2(15, 15), bind_color);
		CDebugRenderer::GetReference().CircleRenderer(m_XMBindPos, DirectX::XMFLOAT2(5, 5), bind_color);

		DirectX::XMFLOAT4 mouse_color;
		DirectX::XMStoreFloat4(&mouse_color, DirectX::Colors::White);

		CDebugRenderer::GetReference().SquareRenderer(m_XMMousePos, DirectX::XMFLOAT2(50, 50), mouse_color);
		CDebugRenderer::GetReference().CircleRenderer(m_XMMousePos, DirectX::XMFLOAT2(15, 15), mouse_color);
		CDebugRenderer::GetReference().CircleRenderer(m_XMMousePos, DirectX::XMFLOAT2(5, 5), mouse_color);
		*/
#endif
	}

	//subsystem sound
	if (CInputManager::GetInstance()->IsKeyPressed(eControls::Spacebar))
	{
		m_bTabShow = !m_bTabShow;
		if (true == m_bTabShow)
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2DL_PLAYER_SSTAB);
		}
		else
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2DL_PLAYER_SSTABHIDE);
		}
	}

	DirectX::XMFLOAT3 position = GetPosition();
	(tCollider)->vCenter = DirectX::XMFLOAT3(position.x, position.y, position.z);

	vec3f movment3f;
	{
		//Calculate our movement speed
		float fSpeed = m_fMoveSpeed;
		if (m_pStatusEffects)
		{
			fSpeed = m_pStatusEffects->GetEffectedStat(STAT_EFF_SLOW, fSpeed);
		}
		movment3f = CInputManager::GetInstance()->GetMovementVector() * fSpeed;
	}

	// check for idle
	/*if (movment3f.magnitude( ) <= 0.001f)
	{
	TurnOnBit(actionBitfield, IS_IDLE);
	}
	else
	{
	TurnOffBit(actionBitfield, IS_IDLE);
	}*/

	DirectX::XMFLOAT3 player = GetPosition();

	DirectX::XMFLOAT3 to_mouse = m_XMMousePos - player;

	auto dot = DirectX::XMVector3Dot(XMLoadFloat3(&to_mouse), XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 1)));
	auto lengthA = DirectX::XMVector3Length(XMLoadFloat3(&to_mouse));
	auto lengthB = DirectX::XMVector3Length(XMLoadFloat3(&DirectX::XMFLOAT3(0, 0, 1)));
	auto theta = std::acos(DirectX::XMVectorGetX(DirectX::XMVectorDivide(dot, DirectX::XMVectorMultiply(lengthA, lengthB))));

	dot = DirectX::XMVector3Dot(XMLoadFloat3(&to_mouse), XMLoadFloat3(&DirectX::XMFLOAT3(1, 0, 0)));
	theta = DirectX::XMVectorGetX(dot) > 0 ? theta : -theta;

	if (CInputManager::GetInstance()->IsButtonDown(eControls::W) && movment3f.magnitude() >= 500.0f)
	{
		BitTwiddler::TurnOffBit(actionBitfield, IS_IDLE);
		isWalkingForward = 1;
		isStrafing = -1;
	}
	else if (CInputManager::GetInstance()->IsButtonUp(eControls::W) && movment3f.magnitude() <= 0.0f)
	{
		BitTwiddler::TurnOnBit(actionBitfield, IS_IDLE);
		isWalkingForward = -1;
		isStrafing = -1;
	}

	else if (CInputManager::GetInstance()->IsButtonDown(eControls::S) && movment3f.magnitude() >= 500.0f)
	{
		BitTwiddler::TurnOffBit(actionBitfield, IS_IDLE);
		isWalkingForward = 0;
		isStrafing = -1;
	}
	else if (CInputManager::GetInstance()->IsButtonUp(eControls::S) && movment3f.magnitude() <= 0.0f)
	{
		BitTwiddler::TurnOnBit(actionBitfield, IS_IDLE);
		isWalkingForward = -1;
		isStrafing = -1;
	}

	if (CInputManager::GetInstance()->IsButtonDown(eControls::A) && movment3f.magnitude() >= 500.0f)
	{
		BitTwiddler::TurnOffBit(actionBitfield, IS_IDLE);
		isWalkingForward = -1;
		isStrafing = 1;
	}
	else if (CInputManager::GetInstance()->IsButtonUp(eControls::A) && movment3f.magnitude() <= 0.0f)
	{
		BitTwiddler::TurnOnBit(actionBitfield, IS_IDLE);
		isWalkingForward = -1;
		isStrafing = -1;
	}

	else if (CInputManager::GetInstance()->IsButtonDown(eControls::D) && movment3f.magnitude() >= 500.0f)
	{
		BitTwiddler::TurnOffBit(actionBitfield, IS_IDLE);
		isWalkingForward = -1;
		isStrafing = 0;
	}
	else if (CInputManager::GetInstance()->IsButtonUp(eControls::D) && movment3f.magnitude() <= 0.0f)
	{
		BitTwiddler::TurnOnBit(actionBitfield, IS_IDLE);
		isWalkingForward = -1;
		isStrafing = -1;
	}

	#pragma region Strafing Logic
	if (theta > DirectX::XMConvertToRadians(45.0f) && theta < DirectX::XMConvertToRadians(135.0f)) // if we're facing the right
	{
		// D will animate the character forward
		if (CInputManager::GetInstance()->IsButtonDown(eControls::D) && movment3f.magnitude() >= 500.0f)
		{
			BitTwiddler::TurnOffBit(actionBitfield, IS_IDLE);
			isWalkingForward = 1;
			isStrafing = -1;
		}

		// S will animate the character backward
		if (CInputManager::GetInstance()->IsButtonDown(eControls::A) && movment3f.magnitude() >= 500.0f)
		{
			BitTwiddler::TurnOffBit(actionBitfield, IS_IDLE);
			isWalkingForward = 0;
			isStrafing = -1;
		}
	}
	else if (theta < DirectX::XMConvertToRadians(-45.0f) && theta > DirectX::XMConvertToRadians(-135.0f))
	{
		// D will animate the character forward
		if (CInputManager::GetInstance()->IsButtonDown(eControls::A) && movment3f.magnitude() >= 500.0f)
		{
			BitTwiddler::TurnOffBit(actionBitfield, IS_IDLE);
			isWalkingForward = 1;
			isStrafing = -1;
		}

		// S will animate the character backward
		if (CInputManager::GetInstance()->IsButtonDown(eControls::D) && movment3f.magnitude() >= 500.0f)
		{
			BitTwiddler::TurnOffBit(actionBitfield, IS_IDLE);
			isWalkingForward = 0;
			isStrafing = -1;
		}
	}
#pragma endregion

	if (theta > DirectX::XMConvertToRadians(90.0f) && theta < DirectX::XMConvertToRadians(180.0f))
	{
		// S will animate the character backward
		if (CInputManager::GetInstance()->IsButtonDown(eControls::S) && movment3f.magnitude() >= 500.0f)
		{
			BitTwiddler::TurnOffBit(actionBitfield, IS_IDLE);
			isWalkingForward = 1;
			isStrafing = -1;
		}

		// S will animate the character backward
		if (CInputManager::GetInstance()->IsButtonDown(eControls::W) && movment3f.magnitude() >= 500.0f)
		{
			BitTwiddler::TurnOffBit(actionBitfield, IS_IDLE);
			isWalkingForward = 0;
			isStrafing = -1;
		}
	}
	else if (theta < DirectX::XMConvertToRadians(-90.0f) && theta > DirectX::XMConvertToRadians(-180.0f))
	{
		// D will animate the character forward
		if (CInputManager::GetInstance()->IsButtonDown(eControls::S) && movment3f.magnitude() >= 500.0f)
		{
			BitTwiddler::TurnOffBit(actionBitfield, IS_IDLE);
			isWalkingForward = 1;
			isStrafing = -1;
		}

		// S will animate the character backward
		if (CInputManager::GetInstance()->IsButtonDown(eControls::W) && movment3f.magnitude() >= 500.0f)
		{
			BitTwiddler::TurnOffBit(actionBitfield, IS_IDLE);
			isWalkingForward = 0;
			isStrafing = -1;
		}
	}

	//Sprint functionality
	m_bSprinting = false;
	if (m_fOxygen > 15.0f)
	{
		if (CInputManager::GetInstance()->IsButtonDown(eControls::LeftShift))
		{
			m_bSprinting = true;
			movment3f *= 1.6f;
		}
	}

	CDroneBuddy* pDrone = static_cast<CDroneBuddy*>(m_pDroneBuddy);

	// Drone deployment
	if (CInputManager::GetInstance()->IsKeyPressed(eControls::MouseRight) && m_pDroneBuddy->GetUnlocked())
	{
		if (pDrone->IsReady())
		{
			BitTwiddler::TurnOnBit(actionBitfield, IS_DEPLOYING);
		}
	}

	// Handle drone deployment animation
	if (IsDeploying())
	{
		m_fDeployTimer += dt;

		if (IsIdle())
		{
			if (m_pAnimation == CGameplayState::GetAssetManager()->GetAnimByTag("Player_Drone"))
			{
				if (/*GetAnimationBlender().GetInterpolator(1).GetTime()*/m_fDeployTimer >= 1.0f)
				{
					pDrone->Enable();
					BitTwiddler::TurnOffBit(actionBitfield, IS_DEPLOYING);
					m_fDeployTimer = 0.0f;
				}
			}
		}
		else
		{
			if (m_fDeployTimer >= 1.0f)
			{
				m_fDeployTimer = 0.0f;
				pDrone->Enable();
				BitTwiddler::TurnOffBit(actionBitfield, IS_DEPLOYING);
			}
		}
	}

	// Reload functionality
	if (CInputManager::GetInstance()->IsKeyPressed(eControls::R) && m_pCurrentWeapon && m_pCurrentWeapon->GetCurrentMagazine() < m_pCurrentWeapon->GetMaxAmmo())
	{
		m_pCurrentWeapon->Reload();
	}

	// Movement functionality
	DirectX::XMFLOAT3 movement = DirectX::XMFLOAT3(movment3f.x, movment3f.y, movment3f.z);
	if ((AvailableMoveDirs[0] == false /*|| AvailableMoves[0] == false*/) && movment3f.z > 0) // up
	{
		movement.z = 0;
	}
	else if ((AvailableMoveDirs[1] == false /*|| AvailableMoves[1] == false*/) && movment3f.z < 0) // down
	{
		movement.z = 0;
	}
	if ((AvailableMoveDirs[2] == false /*|| AvailableMoves[2] == false*/) && movment3f.x > 0) // left
	{
		movement.x = 0;
	}
	else if ((AvailableMoveDirs[3] == false /*|| AvailableMoves[3] == false*/) && movment3f.x < 0) // right
	{
		movement.x = 0;
	}
	//Sprinting
	if ((movement.x != 0 || movement.z) != 0 && m_bSprinting && m_fOxygen > 15.0f)
	{
		m_fOxygen -= 10.f * dt;
	}

	// hindering the players movement while colliding with an enemy
	if (isSaboteurColliding && m_fHinderedTimer > 0)
	{
		movement = movement * 0.4f;
		m_fHinderedTimer -= dt;
	}
	else if (isHunterColliding && m_fHinderedTimer > 0)
	{
		movement = movement * 0.6f;
		m_fHinderedTimer -= dt;
	}
	else if (isCreepColliding && m_fHinderedTimer > 0)
	{
		movement = movement * 0.8f;
		m_fHinderedTimer -= dt;
	}
	if (m_fHinderedTimer <= 0)
	{
		m_fHinderedTimer = 0;
		isSaboteurColliding = false;
		isHunterColliding = false;
		isCreepColliding = false;
	}

	// apply movement
	position = position + movement * dt;
	MyWorld.Local._41 = position.x;
	MyWorld.Local._42 = position.y;
	MyWorld.Local._43 = position.z;

	{
		float moveLength = MathHelper::Length(movement);
		//Count down to the next step
		if (m_fStepTimer > 0)
		{
			m_fStepTimer -= moveLength * dt * 0.001f;
			if (m_fStepTimer < 0)
			{
				m_fStepTimer = 0;
			}
		}
		//Play step sounds
		else if (moveLength > 0)
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_PLAYER_STEPS);
			m_fStepTimer = STEP_DELAY;
		}
	}
	//Switch Weapons
	if (CInputManager::GetInstance()->IsKeyPressed(eControls::OEM_1))
	{
		SwitchWeapon(GUN_SEMI_AUTO);
	}
	else if (CInputManager::GetInstance()->IsKeyPressed(eControls::OEM_2))
	{
		SwitchWeapon(GUN_FULL_AUTO);
	}
	else if (CInputManager::GetInstance()->IsKeyPressed(eControls::OEM_3))
	{
		SwitchWeapon(GUN_SCATTER);
	}

	// rotate towards mouse
	POINT mouse;
	if (GetCursorPos(&mouse))
	{
		Turn(mouse);
	}

#ifdef _DEBUG
	//DirectX::XMFLOAT4 mouse_color;
	//DirectX::XMStoreFloat4(&mouse_color, DirectX::Colors::White);

	//CDebugRenderer::GetReference( ).SquareRenderer(m_XMMousePos, DirectX::XMFLOAT2(50, 50), mouse_color);
	//CDebugRenderer::GetReference( ).CircleRenderer(m_XMMousePos, DirectX::XMFLOAT2(15, 15), mouse_color);
	//CDebugRenderer::GetReference( ).CircleRenderer(m_XMMousePos, DirectX::XMFLOAT2(5, 5), mouse_color);
#endif
}

#undef far

void CPlayer::Turn(POINT _mouse)
{
	static DirectX::XMFLOAT3 lastPos;

	CD3D11_VIEWPORT viewport = CRenderer::GetReference().GetViewport();
	CICamera* camera = CScene::GetCamera();

	ScreenToClient(CRenderer::GetReference().GetWindow(), &_mouse);

	float pointX = (float)((2.0 * ((float)_mouse.x) / (float)viewport.Width) - 1.0f);
	float pointY = (float)((2.0 * ((float)_mouse.y) / (float)viewport.Height) - 1.0f) * -1.0f;

	DirectX::XMFLOAT2 mouse = DirectX::XMFLOAT2(pointX, pointY);
	DirectX::XMFLOAT3 orig = DirectX::XMFLOAT3(mouse.x, mouse.y, 0.0f);
	DirectX::XMFLOAT3 far = DirectX::XMFLOAT3(mouse.x, mouse.y, 1.0f);

	DirectX::XMFLOAT3 origin; DirectX::XMStoreFloat3(&origin,
													 DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&orig),
													 DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&camera->ViewProjection()))));

	DirectX::XMFLOAT3 posfar; DirectX::XMStoreFloat3(&posfar,
													 DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&far),
													 DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&camera->ViewProjection()))));

	DirectX::XMFLOAT3 player = XMMatrixPosition(MyWorld.GetWorld());
	DirectX::XMFLOAT3 d, e, f, q;
	float t;

	d = DirectX::XMFLOAT3(player.x - 100, player.y, player.z + 100);
	e = DirectX::XMFLOAT3(player.x + 100, player.y, player.z + 100);
	f = DirectX::XMFLOAT3(player.x - 100, player.y, player.z - 100);

	//m_XMBindPos = player;
	if (IntersectSegmentPlane(origin, posfar, d, e, f, t, q))
	{
		DirectX::XMFLOAT3 eye = player;
		DirectX::XMFLOAT3 at = lastPos;

		DirectX::XMFLOAT3 x, y, z;
		DirectX::XMStoreFloat3(&z, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&(at - eye))));

		DirectX::XMStoreFloat3(&x, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 1, 0)), DirectX::XMLoadFloat3(&z)));
		DirectX::XMStoreFloat3(&x, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&x)));

		DirectX::XMStoreFloat3(&y, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&z), DirectX::XMLoadFloat3(&x)));
		DirectX::XMStoreFloat3(&y, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&y)));

		DirectX::XMMATRIX matrix(
			DirectX::XMLoadFloat3(&x),
			DirectX::XMLoadFloat3(&y),
			DirectX::XMLoadFloat3(&z),
			DirectX::XMLoadFloat3(&eye));

		DirectX::XMFLOAT4X4 worldMatrix;
		DirectX::XMStoreFloat4x4(&worldMatrix, matrix);
		worldMatrix._41 = MyWorld.GetWorld()._41;
		worldMatrix._42 = MyWorld.GetWorld()._42;
		worldMatrix._43 = MyWorld.GetWorld()._43;
		worldMatrix._44 = MyWorld.GetWorld()._44;

		MyWorld.Local = worldMatrix;

		float epsilon = .1f;

		if (abs(q.x - lastPos.x) > epsilon ||
			abs(q.z - lastPos.z) > epsilon)
		{
			lastPos = q;
		}

		m_XMMousePos = lastPos;
	}
}

void CPlayer::TakeDamage(float fAmount)
{
	if (m_bIsAlive)
	{
		CLivingEntities::TakeDamage(fAmount);

		m_pGameStats->m_nTotalDamageTaken += (int)fAmount;

		if (m_fCurrentHealth <= 0 && m_fCurrentHealth > -100000.0f)
		{
			//Rbitrary limit to only scream once
			m_fCurrentHealth = -200000.0f;
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_PLAYER_DIE);
		}
		else
		{
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_PLAYER_HIT);
		}
	}
}

void CPlayer::Reset(void)
{
	m_fCurrentHealth = m_fMaxHealth;
	m_bIsAlive = true;
	m_bGodMode = false;
	DirectX::XMStoreFloat4x4(&MyWorld.Local, DirectX::XMMatrixIdentity());
	MyWorld.Local._43 += 1500;
	//Reset everything
	if (m_pStatusEffects)
	{
		m_pStatusEffects->Dispel();
	}
	//if (m_pCargo)
	//{
	//	m_pCargo->Reset( );
	//}
	if (m_pPistol)
	{
		m_pPistol->Reset();
	}
	if (m_pShotgun)
	{
		m_pShotgun->Reset();
	}
	if (m_pMachineGun)
	{
		m_pMachineGun->Reset();
	}
	m_pCurrentWeapon = nullptr;
	//Lock the drone, but reduce it's cooldown for us
	if (m_pDroneBuddy)
	{
		m_pDroneBuddy->SetUnlocked(false);
		CDroneBuddy* pDrone = static_cast<CDroneBuddy*>(m_pDroneBuddy);
		pDrone->Disable();
		pDrone->SetCooldown(0.0f);
		pDrone->Reset();
	}
	//TODO: Reset any other player stats for when the game starts
	m_nTotalScrapAmount = m_nScrapAmount = STARTING_SCRAP;
	//(*m_pDroneBuddy) = CDroneBuddy(this);
	m_fOxygen = 100.0f;

}

void CPlayer::HandlePickUp(CPickUp* pPickUp)
{
	//Error check
	if (pPickUp == nullptr)
	{
		return;
	}

	if (pPickUp->GetAlive())
	{
		//Get any bonuses
		m_nScrapAmount += pPickUp->GetScrapValue();
		m_nTotalScrapAmount += pPickUp->GetScrapValue();
		std::string szTemp = "[PLAYER] Pick-Up received, TotalScrap:" + std::to_string(m_nScrapAmount) + "\n";
		//DebugPrint(szTemp.c_str( ));
	}
	//Kill the messenger
	pPickUp->Disable();
}

void CPlayer::MaxUpgrades(void)
{
	//m_pPistol->UpgradeWeaponTier(MAX_WEAPON_LEVEL);
	//m_pMachineGun->UpgradeWeaponTier(MAX_WEAPON_LEVEL);
	//m_pShotgun->UpgradeWeaponTier(MAX_WEAPON_LEVEL);
}

bool CPlayer::RegisterAnimation(void)
{
	GetAnimStateMachine().SetEntity(this);
	GetAnimStateMachine().ChangeState(CIdleAnimState::GetInstance());

	m_pAnimation = CGameplayState::GetAssetManager()->GetAnimByTag(m_szTag);
	if (m_pAnimation)
	{
		//m_cInterpolator.SetAnimation(&m_pAnimation->m_pAnim);

		m_cDeformer.SetMesh(&m_pAnimation->m_pModel->m_cMesh);

		if (m_pAnimation->m_pModel->m_pVertexBuffer == NULL)
		{
			CGameplayState::GetAssetManager()->CreateBuffer(CRenderer::GetReference().GetDevice(), m_cDeformer.GetSkinVerts(),
															D3D11_BIND_VERTEX_BUFFER,
															&m_pAnimation->m_pModel->m_pVertexBuffer);
		}
		return true;
	}
	return false;
}

void CPlayer::SplitCircle(float _fDT)
{
	DirectX::XMFLOAT3 player = GetPosition();

	DirectX::XMFLOAT3 to_mouse = m_XMMousePos - player;
	DirectX::XMFLOAT3 point = (to_mouse) / 2;

	m_XMBindPos = player + point;

	auto dot = DirectX::XMVector3Dot(XMLoadFloat3(&to_mouse), XMLoadFloat3(&XMFLOAT3(0, 0, 1)));
	auto lengthA = DirectX::XMVector3Length(XMLoadFloat3(&to_mouse));
	auto lengthB = DirectX::XMVector3Length(XMLoadFloat3(&XMFLOAT3(0, 0, 1)));
	auto theta = std::acos(DirectX::XMVectorGetX(DirectX::XMVectorDivide(dot, DirectX::XMVectorMultiply(lengthA, lengthB))));

	dot = DirectX::XMVector3Dot(XMLoadFloat3(&to_mouse), XMLoadFloat3(&XMFLOAT3(1, 0, 0)));
	theta = DirectX::XMVectorGetX(dot) > 0 ? theta : -theta;

	m_XMBindPos.x = player.x + m_XMBindRadius.x * sin(theta);
	m_XMBindPos.z = player.z + m_XMBindRadius.y * cos(theta);

#ifdef _DEBUG
	//DirectX::XMFLOAT4 split_color;
	//DirectX::XMStoreFloat4(&split_color, DirectX::Colors::Cyan);
	//
	//CDebugRenderer::GetReference( ).SquareRenderer(m_XMBindPos, DirectX::XMFLOAT2(50, 50), split_color);
	//CDebugRenderer::GetReference( ).CircleRenderer(m_XMBindPos, DirectX::XMFLOAT2(15, 15), split_color);
	//CDebugRenderer::GetReference( ).CircleRenderer(m_XMBindPos, DirectX::XMFLOAT2(5, 5), split_color);

#endif
}

void CPlayer::SplitSquare(float _fDT)
{
	DirectX::XMFLOAT3 player = GetPosition( );

	DirectX::XMFLOAT3 to_mouse = m_XMMousePos - player;
	DirectX::XMFLOAT3 point = (to_mouse) / 2;

	if (abs(point.x) > m_XMBindRadius.x)
	{
		point.x > 0
			? point.x = m_XMBindRadius.x
			: point.x = -m_XMBindRadius.x;
	}

	if (abs(point.z) > m_XMBindRadius.y)
	{
		point.z > 0
			? point.z = m_XMBindRadius.y
			: point.z = -m_XMBindRadius.y;
	}

	m_XMBindPos = player + point;

#if _DEBUG
	DirectX::XMFLOAT4 split_color;
	DirectX::XMStoreFloat4(&split_color, DirectX::Colors::White);

	//CDebugRenderer::GetReference( ).SquareRenderer(m_XMBindPos, DirectX::XMFLOAT2(50, 50), split_color);
	//CDebugRenderer::GetReference( ).CircleRenderer(m_XMBindPos, DirectX::XMFLOAT2(15, 15), split_color);
	//CDebugRenderer::GetReference( ).CircleRenderer(m_XMBindPos, DirectX::XMFLOAT2(5, 5), split_color);
#endif
}

bool CPlayer::SwitchWeapon(EGunType eWhichWeapon)
{
	if (eWhichWeapon == EGunType::GUN_SEMI_AUTO && m_pPistol && m_pCurrentWeapon != m_pPistol && m_pPistol->GetUnlocked( ))
	{
		m_pCurrentWeapon = m_pPistol;
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_PISTOL_DRAW);
		return true;
}
	else if (eWhichWeapon == EGunType::GUN_FULL_AUTO && m_pMachineGun && m_pCurrentWeapon != m_pMachineGun && m_pMachineGun->GetUnlocked())
	{
		m_pCurrentWeapon = m_pMachineGun;
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MACHGUN_DRAW);
		return true;
	}
	else if (eWhichWeapon == EGunType::GUN_SCATTER && m_pShotgun && m_pCurrentWeapon != m_pShotgun && m_pShotgun->GetUnlocked())
	{
		m_pCurrentWeapon = m_pShotgun;
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_SHOTGUN_DRAW);
		return true;
	}

	return false;
}
