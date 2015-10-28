/***********************************************
* Filename:  		GameplayState.cpp
* Date:      		05/12/2015
* Mod. Date: 		05/12/2015
* Mod. Initials:	MJG
* Author:    		Michael J. Gergar
* Purpose:   		This is the state used for
*					primary gameplay
************************************************/

#define DIRECTINPUT_VERSION 0x0800

#include "GameplayState.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/RObject.h"
#include "../Renderer/ICamera.h"
#include "../Renderer/ThirdPersonCamera.h"
#include "../Renderer/FirstPersonCamera.h"
#include "../Renderer/Skybox.h"
#include "../Renderer/Scene.h"

#include "../Asset Manager/AssetManager.h"

#include "../Entity Manager/EntityManager.h"
#include "../Entity Manager/Entities/Player.h"
#include "../Entity Manager/Entities/Subsystems/Engines.h"
#include "../Entity Manager/Entities/Subsystems/LifeSupport.h"
#include "../Entity Manager/Entities/Subsystems/Reactor.h"
#include "../Entity Manager/AI/MonsterSpawner.h"
#include "../Weapon/Pistol.h"

#include "../Core/EnumerationHolder.h"

#include "../Input Manager/InputManager.h"

#include "../Audio Manager/AudioSystemWwise.h"

#include "../GUI Manager/GUIManager.h"
#include "../GUI Manager/HealthBar.h"
#include "../GUI Manager/UISubsystemIndicator.h"
#include "../GUI Manager/UIProgressMap.h"
#include "../GUI Manager/MeltdownCounter.h"

#include "../Particle Manager/ParticleManager.h"
#include "../Particle Manager/ParticleSystem.h"
#include "../Particle Manager/ParticleEmitter.h"
#include "../Entity Manager/AI/AIDirector.h"

#include "../Weapon/Bullet.h"
#include "..\Entity Manager\Entity.h"

#include "../Renderer/structs.h"

#include "../Cutscene Manager/Cutscene.h"
#include "../Weapon/ModBank.h"
#include "../Weapon/ModSystem.h"

#include "../Entity Manager/Level.h"

#include "MainMenuState.h"

#define CHEATS_ACTIVE 1

extern HWND window;

DirectX::XMFLOAT3 gAMBIENCE = DirectX::XMFLOAT3(5.0f, 5.0f, 5.0f);

void CenterCursor()
{
	//alright so i'm attempting to set the cursor to the center
	//	of the screen when we start the game. [for the camera, mostly]
	//
	//so first, we need the client rectangle [window upper-left/upper-right],
	//	except the rect returned isn't in screen-space.
	RECT win;
	GetClientRect(window, &win);

	//that's what this is for. we catch the left/top, then transform from
	//	client to screen
	POINT point1 = { win.left, win.top };
	ClientToScreen(window, &point1);

	//so now what we *should* be able to do is use the transformed point
	//	added with half the height/width
	auto x = point1.x + (CRenderer::GetReference().GetWidth() / 2);
	auto y = point1.y + (CRenderer::GetReference().GetHeight() / 2);

	//set the cursor from the transformed points.
	SetCursorPos(x, y);

	//that's the idea at least.
	//it seems to work. so like score and stuff.
	//--David


}

/*static*/ CAssetManager* CGameplayState::m_pAssetManager = nullptr;
/*****************************************************************
* Default Constructor
*****************************************************************/
CGameplayState::CGameplayState(void) : CBaseState(STATE_GAMEPLAY)
{
	m_bShowCursor = false;
	m_xmStartPos = DirectX::XMFLOAT3(0, 0, 10);
	m_nControl = -1;

	{

		//m_pGUI->AddElement(m_pScreen);

		m_bInfinite = false;
	}
}

/*****************************************************************
* Default Destructor
*****************************************************************/
CGameplayState::~CGameplayState(void)
{
	Exit();
	UnLoad();
}

/*****************************************************************
* Input(): Called every frame, processes any state specific input
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  void
*
* Mod. Date:		      05/12/2015
* Mod. Initials:	      MJG
*****************************************************************/
void CGameplayState::Input(void)
{
	//Pause the game
	if (CInputManager::GetInstance()->IsKeyPressed(eControls::ESCAPE) ||
		CInputManager::GetInstance()->IsKeyPressed(eControls::P) ||
		CInputManager::GetInstance()->IsKeyPressed(eControls::F11))
	{
		QueueStateChange(eStateChange::STATE_CH_PAUSE);
	}

	//Show controls
	// Having a separate state for this is a bit overkill, but was done
	// to have a better idea of how different states will work later
	else if (CInputManager::GetInstance()->IsKeyPressed(eControls::F1))
	{
		QueueStateChange(eStateChange::STATE_CH_OPTIONS);
	}
	//Open the shop from anywhere
	else if (CInputManager::GetInstance()->IsKeyPressed(eControls::Tab))
	{
		QueueStateChange(eStateChange::STATE_CH_SHOP);
	}

#if CHEATS_ACTIVE
	//CHEATS
	else if (CInputManager::GetInstance()->IsButtonDown(eControls::LeftControl))
	{
		//KILL PLAYER HP
		/*if (CInputManager::GetInstance()->IsKeyPressed(eControls::NUMPAD_9))
		{
		m_pEntityManager->GetPlayer()->SetCurrentHealth(-1);
		}*/

		//KILL ALL ENEMIES
		if (CInputManager::GetInstance()->IsKeyPressed(eControls::NUMPAD_8))
		{
			m_pEntityManager->KillAllEnemies();
		}

		//Refill scrap
		if (CInputManager::GetInstance()->IsKeyPressed(eControls::NUMPAD_2))
		{
			m_pEntityManager->GetPlayer()->SetScrapCount(9999);
			DebugPrint("[CHEAT]: Scrap refilled!\n");
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_PURCHASE);
		}
		//Max out weapons levels
		else if (CInputManager::GetInstance()->IsKeyPressed(eControls::NUMPAD_3))
		{
			m_pEntityManager->GetPlayer()->MaxUpgrades();
			DebugPrint("[CHEAT]: Weapons pimped!\n");
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_PURCHASE);
		}

		//Open the upgrade menu
		else if (CInputManager::GetInstance()->IsKeyPressed(eControls::F2))
		{
			QueueStateChange(eStateChange::STATE_CH_SHOP);
		}

		/*else if (CInputManager::GetInstance( )->IsKeyPressed(eControls::L))
		{
		static bool bLightSwitch;
		m_pLevelManager->SetLightSwitch(bLightSwitch);
		m_pLevelManager->SetLightShade({ 1, 1, 1 });
		bLightSwitch = !bLightSwitch;
		}
		else if (CInputManager::GetInstance( )->IsKeyPressed(eControls::R))
		{
		m_pLevelManager->SetLightShade({ 1, 0.1f, 0.1f });
		}
		else if (CInputManager::GetInstance( )->IsKeyPressed(eControls::G))
		{
		m_pLevelManager->SetLightShade({ 0.1f, 1.0f, 0.1f });
		}
		else if (CInputManager::GetInstance( )->IsKeyPressed(eControls::B))
		{
		m_pLevelManager->SetLightShade({ 0.1f, 0.1f, 1.0f });
		}
		else if (CInputManager::GetInstance( )->IsKeyPressed(eControls::F))
		{
		m_pLevelManager->Flicker( );
		}*/
		/*else if (CInputManager::GetInstance( )->IsKeyPressed(eControls::K))
		{
		QueueStateChange(eStateChange::STATE_CH_CUTSCENE);
		}
		else if (CInputManager::GetInstance( )->IsKeyPressed(eControls::W))
		{
		QueueStateChange(eStateChange::STATE_CH_WIN);
		}
		else if (CInputManager::GetInstance()->IsKeyPressed(eControls::C))
		{
		QueueStateChange(eStateChange::STATE_CH_CUTSCENE);
		}*/
	}
#endif
}

#undef max
#undef min

/*****************************************************************
* Update(): Called every frame, updates anything inside that
*                needs it
*
* Ins:			  float fDT
*
* Outs:		      void
*
* Returns:		  void
*
* Mod. Date:		      05/12/2015
* Mod. Initials:	      MJG
*****************************************************************/
#include "../AchieveTracker.h"
void CGameplayState::Update(float fDT)
{
	m_pLevelManager->Update(fDT);
	m_pGUI->Update(fDT);
	m_pEntityManager->Update(fDT, m_pScene);

	DirectX::XMFLOAT3 _XMBindPos = XMFLOAT3{ 0, 0, 0 };
	if (nullptr != m_pEntityManager->GetPlayer())
	{
		_XMBindPos = m_pEntityManager->GetPlayer()->GetBindPosition();
	}
	m_pScene->Update(_XMBindPos, fDT);

	//screen fade stuff
	if (m_bFadeIn)
	{
		m_fFadeTimer += fDT;
		m_pScreen->SetColor({ 1, 1, 1, blend(amount_to_fade_in, 0.0f, m_fFadeTimer * (1 / TRANSITION_TIME)) });
	}
	else if (m_bFadeOut)
	{
		m_fFadeTimer += fDT;
		m_pScreen->SetColor({ 1, 1, 1, blend(0.0f, 1.0f, m_fFadeTimer * (1 / TRANSITION_TIME)) });
	}

	if (m_pScreen->GetColor().w <= amount_to_fade_out)
	{
		m_fFadeTimer = 0.0f;
		m_bFadeIn = FALSE;
	}
	else if (m_pScreen->GetColor().w >= amount_to_fade_in)
	{
		m_fFadeTimer = 0.0f;
		m_bFadeOut = FALSE;
	}
	m_pScreen->SetVisible(m_bFadeIn || m_bFadeOut);

	if (nullptr != m_pEntityManager && nullptr != m_pEntityManager->GetDirector())
	{
		if (0 == fDT)
		{
			m_pEntityManager->GetDirector()->SetIsPaused(true);
		}
		else
		{
			m_pEntityManager->GetDirector()->SetIsPaused(false);
		}
	}

	if (!fDT)
	{
		return;
	}
	// check win/lose state
	CEngines* pEngines = nullptr;
	CReactor* pReactor = nullptr;
	std::list<CSubSystem*> listOfSubsystems = m_pEntityManager->GetSubSystems();
	for (auto i = listOfSubsystems.begin(); i != listOfSubsystems.end(); i++)
	{
		if (dynamic_cast<CEngines*>(*i))
		{
			pEngines = (CEngines*)*i;
		}
		if (dynamic_cast<CReactor*>(*i))
		{
			pReactor = (CReactor*)*i;
		}
	}

	if (m_pEntityManager->GetPlayer()->GetCurrentHealth() <= 0.f)
	{
		QueueStateChange(eStateChange::STATE_CH_CUTSCENE);
		m_pEntityManager->GetPlayer()->SetAlive(false);
	}

	if (m_pEntityManager->GetPlayer()->GetOxygen() <= 0.f)
	{
		QueueStateChange(eStateChange::STATE_CH_CUTSCENE);
		m_pEntityManager->GetPlayer()->SetAlive(false);
	}

	if (pReactor->GetMeltdownTimer() <= 0)
	{
		QueueStateChange(eStateChange::STATE_CH_CUTSCENE);
		m_pEntityManager->GetPlayer()->SetAlive(false);
	}
	//Change lights based on the state of the reactor
	if (pReactor->GetAlive())
	{
		m_pLevelManager->SetEmergencyLightSwitch(false);
		m_pLevelManager->SetLightSwitch(true);
		m_pLevelManager->SetLightShade(fmax(pReactor->PercentHealth(), 0.15f));
	}
	else
	{
		m_pLevelManager->SetLightSwitch(false);
		m_pLevelManager->SetEmergencyLightSwitch(true);
		//if ((rand() % 500) == 0)
		//{
		//	m_pLevelManager->Flicker();
		//}
	}

	if (true == m_bInfinite)
	{
		pEngines->SetGoalDistance(pEngines->GetGoalDistance() + 100);
		CAchieveTracker::GetInstance()->UnlockAchievement(CAchieveTracker::eSurvivor, fDT);
	}
	else
	{
		if (pEngines->IsComplete())
		{
			m_bStateChanges[STATE_CH_WIN] = true;
		}
	}

	auto clamp = [&](float _fMin, float _fMax, float& _fChannel) -> void
	{
		_fChannel = std::min(_fMax, std::max(_fMin, _fChannel));
	};

	float speed = 1.0f;
	static float l = 0.0f;
	if (pEngines->GetAlive() && false == pEngines->GetStopped())
	{
		clamp(l += fDT, speed, (CScene::GetSkybox()->m_fSpeedMod));
		clamp(0, speed, l);
	}
	else
	{
		memset(&l, 0, sizeof(float));
		CScene::GetSkybox()->m_fSpeedMod -= speed * fDT;
		clamp(0, speed, CScene::GetSkybox()->m_fSpeedMod);
	}

	// update particle systems
	CParticleManager::GetInstance()->Update(fDT);

	//Add the ship's general lights to the scene
	m_pLevelManager->AddLightsToScene(m_pScene);

}

/*****************************************************************
* Render(): Called every frame, renders everything in the state
*                that needs it to the back buffer
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  void
*
* Mod. Date:		      05/12/2015
* Mod. Initials:	      MJG
*****************************************************************/
void CGameplayState::Render(CAssetManager* pAssetManager)
{
	CRenderer::GetReference().Render(m_pScene, pAssetManager);
	m_pScene->ClearScene();
	m_pScene->ClearLights(true, true);

	m_pGUI->Render();

	m_pEntityManager->GetDirector()->Render();
}

/*****************************************************************
* Enter(): Called when switching to a state, initializes members
*                and allocates memory
*
* Ins:			  CGameplayState* cPrevState
*
* Outs:		      void
*
* Returns:		  bool, true if successfully entered
*
* Mod. Date:		      05/12/2015
* Mod. Initials:	      MJG
*****************************************************************/
bool CGameplayState::FirstEnter(void)
{
	m_pScene->AddCamera(new CThirdPersonCamera());
	m_pScene->AddSkybox(new CSkybox());

	m_bFadeIn = true, m_bFadeOut = false;
	m_fFadeTimer = 0.0f;

	amount_to_fade_in = 1.0f;
	amount_to_fade_out = 0.0f;
	TRANSITION_TIME = .5f;

	if (nullptr != m_pEntityManager && nullptr != m_pEntityManager->GetDirector())
	{
		m_pEntityManager->GetDirector()->SetIsPaused(false);
	}

	CAchieveTracker::GetInstance()->SetIsExit(false);

	CenterCursor();

	//DebugPrint("[GameplayState] Entering GameplayState...\n");
	if (GetEntered())
	{
		return true;
	}

	CBaseState::FirstEnter();

	CParticleManager::GetInstance()->DeactivateAll();

	GetGUI()->SetVisible(true);
	m_pGUI->SetRadarRender(true);

	// player information
	{
		CPlayer* pPlayer = m_pEntityManager->GetPlayer();
		pPlayer->Reset();
		m_pEntityManager->GetDirector()->SetInfiniteMode(m_bInfinite);
		pPlayer->SetPosition(m_xmStartPos);
	}

	//Reset the sub-systems
	{
		for (auto& i : m_pEntityManager->GetSubSystems())
		{
			if ((*i).GetSystemType() == eSubsystemtype::eSystemEngine)
			{
				CEngines* eng = (CEngines*)(i);
				eng->Reset();
			}
			else
			{
				i->Reset();
			}
		}
	}
	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_ALL);
	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_AMBIENCE_LOOP);
	//DebugPrint("[GamePlayState] Press [BackSpace] to return to Main Menu\n");

	m_pEntityManager->GetDirector()->SetInfiniteMode(m_bInfinite);
	if (m_pEntityManager->GetFloorGuide())
	{
		m_pEntityManager->GetFloorGuide()->Reset();
	}
	if (true == m_bInfinite)
	{
		if (nullptr != m_pGUI->GetProgressMap())
		{
			m_pGUI->GetProgressMap()->SetVisible(false);
		}

		m_pEntityManager->GetPlayer()->SetScrapCount(500);
		m_pEntityManager->GetPlayer()->SetTotalScrapCount(500);
		m_pEntityManager->GetPlayer()->GetPistol()->SetUnlocked(true);
		m_pEntityManager->GetPlayer()->SwitchWeapon(EGunType::GUN_SEMI_AUTO);

	}
	else
	{
		if (nullptr != m_pGUI->GetProgressMap())
		{
			m_pGUI->GetProgressMap()->SetVisible(true);
		}
	}

	m_pEntityManager->GetDirector()->Reset();

	return true;
}

bool CGameplayState::ReEnter(void)
{
	CBaseState::ReEnter();
	CenterCursor();
	m_bFadeIn = true, m_bFadeOut = false;
	m_fFadeTimer = 0.0f;

	amount_to_fade_in = .5f;
	amount_to_fade_out = 0.0f;
	TRANSITION_TIME = .5f;

	m_pEntityManager->GetDirector()->SetInfiniteMode(m_bInfinite);

	if (true == m_bInfinite)
	{
		if (nullptr != m_pGUI->GetProgressMap())
		{
			m_pGUI->GetProgressMap()->SetVisible(false);
		}
	}
	else
	{
		if (nullptr != m_pGUI->GetProgressMap())
		{
			m_pGUI->GetProgressMap()->SetVisible(true);
		}
	}

	CParticleManager::GetInstance()->DeactivateAll();



	return true;
}

void CGameplayState::HandlePrevState(CBaseState* pPrevState)
{
	CBaseState::HandlePrevState(pPrevState);
	//Find which level to load?
	if (pPrevState->GetType() == STATE_MAINMENU)
	{
		m_bInfinite = static_cast<CMainMenuState*>(pPrevState)->GetEndlessMode();
	}
}

/*****************************************************************
* Load(): Called before switching to a state to ensure all
*				assets are loaded
*
* Ins:			  CAssetManager* pAssets
*
* Outs:		      void
*
* Returns:		  bool, true if all assets are loaded
*
* Mod. Date:		      05/13/2015
* Mod. Initials:	      MJG
*****************************************************************/
bool CGameplayState::Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands)
{

	if (pAssets == nullptr)
	{
		return false;
	}
	if (GetLoaded())
	{
		return true;
	}

	//Tell the state machine to load some other dudes
	if (pLoadCommands)
	{
		pLoadCommands[STATE_PAUSE] = STATE_COMMAND_LOAD;
		pLoadCommands[STATE_CUTSCENE] = STATE_COMMAND_LOAD;
		pLoadCommands[STATE_UPGRADE] = STATE_COMMAND_LOAD;
		pLoadCommands[STATE_LOSE] = STATE_COMMAND_LOAD;
		pLoadCommands[STATE_WIN] = STATE_COMMAND_LOAD;
	}
	m_pScene = new CScene();

	//Allocate a level manager
	m_pLevelManager = new CLevel();

	CBaseState::Load(pAssets);

	m_pAssetManager = pAssets;

	//Load in the module system and module list
	CModSystem::GetInstance()->LoadModules(std::string(XMLPATH + std::string("ModuleList.xml")).c_str());

	//Create the entity manager
	m_pEntityManager = new CEntityManager();
	CAIDirector* pD = m_pEntityManager->GetDirector();
	pD->SetGameplay(this);
	pD->SetInfiniteMode(m_bInfinite);
	pD->Reset();

	CMainMenuState* pM = dynamic_cast<CMainMenuState*>(m_pPrevState);
	if (pM)
	{
		pD->SetWaifuOn(pM->GetWaifuBander());
	}

	CSubSystem::g_pGamePlayState = this;

	//Load models
	{
#pragma region Load Cool Models

		pAssets->LoadFBX(std::string(MODELSPATH + std::string("DankAssSpaceGanja.fbx")), "Weeds");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("WoodTable.fbx")), "WoodTables");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("BatterySingle.fbx")), "SingleBattery");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("ControlPanel.fbx")), "ControlPanel");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("FlattubeShort.fbx")), "FlattubeShort");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("Railings.fbx")), "Railings");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("PipeCorner.fbx")), "CornerPipe");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("PipeCorner2.fbx")), "CornerPipe2");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("BatterySmall.fbx")), "SmallBattery");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("StraightPipe.fbx")), "StraightPipe");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("FlattubeBase.fbx")), "FlattubeBase");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("BentPipe.fbx")), "BentPipe");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("SchoolProjector.fbx")), "SchoolProjector");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("BioScanner.fbx")), "BioScanner");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("SpaceLift.fbx")), "SpaceLift");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("EngineMonitorStand.fbx")), "EngineMonitor");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("Vent.fbx")), "Vent");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("StarMap.fbx")), "StarMap");

#pragma endregion

		//Load in sub-system models
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("LifeSupport.fbx")), "LifeSupport");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("LSTankFrame.fbx")), "LSFrame");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("LSTankGlass.fbx")), "LSGlass");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("LSTankBody.fbx")), "LSCadaver");

		pAssets->LoadFBX(std::string(MODELSPATH + std::string("EngineCell.fbx")), "Engine");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("HoloDisplays.fbx")), "HoloDisplay");

		//pAssets->LoadFBX(std::string(MODELSPATH + std::string("Reactor.fbx")), "Reactor");
		//Load in the pieces of the reactor
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("ReactorCap.fbx")), "ReactorCap");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("ReactorServos.fbx")), "ReactorServos");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("ReactorCore.fbx")), "ReactorCore");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("ReactorBase.fbx")), "ReactorBase");

		//bullet
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("bullet.fbx")), "Bullet");

		//spammer
		//pAssets->LoadFBX(std::string(MODELSPATH + std::string("PickUp.fbx")), "Spammer");

		//Represents the Scrap PickUp
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("PickUp.fbx")), "PickUp");

		//misc crap
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("VariantPla.fbx")), "VariantPlane");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("flash.fbx")), "Muzzle Flash");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("saboteur_effect.fbx")), "Saboteur Attack Effect");

		//guns
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("BluePistol.fbx")), "Pistol");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("BlueRifle.fbx")), "MachineGun");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("BlueShotty.fbx")), "ShotGun");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("FakeDrone.fbx")), "DroneBuddy");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("supergun.fbx")), "Supergun");

		//	pistol crosshairs
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("Pistol Crosshair.fbx")), "Pistol Crosshair");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("Pistol Reloading Crosshair.fbx")), "Pistol Reloading Crosshair");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("Pistol Repairing Crosshair.fbx")), "Pistol Repairing Crosshair");

		//	rifle crosshairs
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("MachineGun Crosshair.fbx")), "MachineGun Crosshair");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("MachineGun Reloading Crosshair.fbx")), "MachineGun Reloading Crosshair");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("MachineGun Repairing Crosshair.fbx")), "MachineGun Repairing Crosshair");

		//	shotgun crosshairs
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("ShotGun Crosshair.fbx")), "ShotGun Crosshair");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("ShotGun Reloading Crosshair.fbx")), "ShotGun Reloading Crosshair");
		pAssets->LoadFBX(std::string(MODELSPATH + std::string("ShotGun Repairing Crosshair.fbx")), "ShotGun Repairing Crosshair");
	}

	pAssets->LoadFBX(std::string(MODELSPATH + std::string("DisplayFrame.fbx")), "FloorGuideFrame");
	pAssets->LoadFBX(std::string(MODELSPATH + std::string("ReactorDisplay.fbx")), "FloorGuideReactor");
	pAssets->LoadFBX(std::string(MODELSPATH + std::string("LifeSupportDisplay.fbx")), "FloorGuideLifeSupport");
	pAssets->LoadFBX(std::string(MODELSPATH + std::string("EngineDisplay.fbx")), "FloorGuideEngine");

	//Load animations
	{
		// Player animations
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("playerIdle.fbx")), "Player_Idle");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("playerRunFW.fbx")), "Player_Run");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("playerRunBW.fbx")), "Player_Run_BW");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("playerStrafeL.fbx")), "Player_Strafe_L");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("playerStrafeR.fbx")), "Player_Strafe_R");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("playerDeath.fbx")), "Player_Die");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("playerHit.fbx")), "Player_Hit");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("playerDeployDrone.fbx")), "Player_Drone");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("playerShootMG.fbx")), "Player_Shoot_MG");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("playerShootP.fbx")), "Player_Shoot_P");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("playerShootS.fbx")), "Player_Shoot_S");

		// Enemy Animations - Creep
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("newCreepRun.fbx")), "Creep_Run");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("newCreepAttack.fbx")), "Creep_Attack");

		// Enemy Animations - Hunter
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("hunterIdle.fbx")), "Hunter_Idle");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("hunterRun.fbx")), "Hunter_Run");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("hunterDie.fbx")), "Hunter_Die");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("hunterHit.fbx")), "Hunter_Hit");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("hunterAttack.fbx")), "Hunter_Attack");

		// Enemy Animations - Saboteur
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("sabRun_2.fbx")), "Sab_Run");
		pAssets->LoadFBX(std::string(ANIMATIONPATH + std::string("sabHit.fbx")), "Sab_Hit");
	}

	//full color ambient: m_pScene->AddLight(LightParams({ 30.0f, 30.0f, 30.0f }, { 0, -1, 0 }));
	m_pScene->AddLight(LightParams(gAMBIENCE, { 0, -1, 0 }));


	//the level
	if (m_bInfinite)
	{
		pAssets->LoadMapFromFBX(std::string(MODELSPATH + std::string("TeriyakiMap.fbx")), this, EMAP_ALL);
	}
	else
	{
		pAssets->LoadMapFromFBX(std::string(MODELSPATH + std::string("KungPaoMap.fbx")), this, EMAP_ALL);
	}

	//Register the player, who's created in the entity manager
	{
		m_pEntityManager->GetPlayer()->RegisterAnimation();
		AudioSystemWwise::Get()->RegisterListener(m_pEntityManager->GetPlayer(), "Player");
	}

	// load gui stuff
	m_pGUI = new CGUIManager();
	m_pGUI->LoadMe(std::string("Assets/Xml/Hud.xml"), this);

	m_pScreen = new CUIElement();

	m_pScreen->SetWidth(1000);
	m_pScreen->SetHeight(800);
	m_pScreen->SetPosition(vec3f(500, 400, 1));
	m_pScreen->SetVisible(true);

	m_pScreen->LoadTexture(L"Assets/Images/Black.png");

	m_pGUI->AddElement(m_pScreen);

	for (int i = 0; i < 30; i++)
	{
		//Allocates space for bullets
		CRObject* pBullet = new CBullet();
		m_pEntityManager->AddToManager(static_cast<CBullet*>(pBullet), false);

		//Load in some pick-ups
		CPickUp* pTemp = new CPickUp(m_pEntityManager->GetPlayer());
		m_pEntityManager->AddToManager(pTemp, false);
	}

	//create X amount of enemies
	for (int i = 0; i < 100; i++)
	{
		CCreepEnemy* creep = new CCreepEnemy();
		creep->SetTag("Creep");
		creep->RegisterAnimation();
		m_pEntityManager->AddToManager(creep, false);

		CHunterEnemy* hunter = new CHunterEnemy();
		hunter->SetTag("Hunter");
		hunter->RegisterAnimation();
		m_pEntityManager->AddToManager(hunter, false);

		CSaboteurEnemy* Sabo = new CSaboteurEnemy();
		Sabo->SetTag("Saboteur");
		Sabo->RegisterAnimation();
		m_pEntityManager->AddToManager(Sabo, false);
	}

	CPathSearch::GetInstance();

	int a = 0;
	a++;

	return true;
}

/*****************************************************s************
* UnLoad(): Called when you no longer need this state's assets
*
* Ins:			  CAssetManager* pAssets
*
* Outs:		      void
*
* Returns:		  bool, true if all assets are unloaded
*
* Mod. Date:		      05/13/2015
* Mod. Initials:	      MJG
*****************************************************************/
void CGameplayState::UnLoad(eStateLoadCommands* pLoadCommands)
{
	if (!GetLoaded())
	{
		return;
	}
	CBaseState::UnLoad();

	//Tell the state machine to load some other dudes
	if (pLoadCommands)
	{
		pLoadCommands[STATE_PAUSE] = STATE_COMMAND_UNLOAD;
		pLoadCommands[STATE_CUTSCENE] = STATE_COMMAND_UNLOAD;
		pLoadCommands[STATE_UPGRADE] = STATE_COMMAND_UNLOAD;
		pLoadCommands[STATE_LOSE] = STATE_COMMAND_UNLOAD;
		pLoadCommands[STATE_WIN] = STATE_COMMAND_UNLOAD;
	}

	if (m_pLevelManager)
	{
		m_pLevelManager->UnloadLevelGeometry(m_pAssetManager);
		delete m_pLevelManager;
		m_pLevelManager = nullptr;
	}

	SafeDelete(m_pScene);
	if (m_pEntityManager)
	{
		//Unregister the player while we have a chance
		AudioSystemWwise::Get()->UnRegisterListener(m_pEntityManager->GetPlayer());
		delete m_pEntityManager;
		m_pEntityManager = nullptr;
	}

	CPathSearch::DeleteInstance();
	SafeDelete(m_pEntityManager);
}

/*****************************************************************
* Exit(): Called when you switch states and don't need to preserve
*				this one
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  void
*
* Mod. Date:		      05/13/2015
* Mod. Initials:	      MJG
*****************************************************************/
bool CGameplayState::Exit(void)
{
	if (!GetEntered())
	{
		return false;
	}
	CBaseState::Exit();

	ClearStateChanges();

	CParticleManager::GetInstance()->DeleteInstance();

	CParticleManager::DeleteInstance();

	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_ALL);

	return true;
}

/*****************************************************************
* CheckState(): Called at the end of each StateMachine's Update
*				to see if the state should switch to another state
*				returns the state that it switches to, returns
*				nullptr if application is closing, this if
*				no change
*
* Ins:			  void
*
* Outs:		      void
*
* Returns:		  CBaseState*
*
* Mod. Date:		      05/13/2015
* Mod. Initials:	      MJG
*****************************************************************/
eStateType CGameplayState::CheckState(void)
{
	//If it stays this, we continue running
	eStateType eNextState = GetType();

	//if (m_bStateChanges[STATE_CH_QUIT] || m_bStateChanges[STATE_CH_BACK])
	//{
	//	if (m_pScreen->GetColor( ).w < 1.0f || m_pScreen->GetColor( ).w == 0.0f)
	//	{
	//		m_bFadeOut = TRUE;
	//		return eNextState;
	//	}
	//
	//	AudioSystemWwise::Get( )->PostEvent(AK::EVENTS::PLAY_2D_MENU_QUIT);
	//	eNextState = STATE_MAINMENU;
	//}
	if (m_bStateChanges[STATE_CH_PAUSE])
	{
		TRANSITION_TIME = .5f;
		amount_to_fade_in = .5f;
		if (m_pScreen->GetColor().w < .5f || m_pScreen->GetColor().w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}

		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PAUSE_ALL);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_QUIT);
		eNextState = STATE_PAUSE;
	}
	else if (m_bStateChanges[STATE_CH_OPTIONS])
	{
		TRANSITION_TIME = .5f;
		amount_to_fade_in = .5f;
		if (m_pScreen->GetColor().w < .5f || m_pScreen->GetColor().w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}

		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENU_SELECT);
		eNextState = STATE_CONTROLS;
	}
	else if (m_bStateChanges[STATE_CH_SHOP])
	{
		TRANSITION_TIME = .5f;
		amount_to_fade_in = .5f;
		if (m_pScreen->GetColor().w < .5f || m_pScreen->GetColor().w == 0.0f)
		{
			m_bFadeOut = TRUE;
			return eNextState;
		}

		eNextState = STATE_UPGRADE;
	}
	else if (m_bStateChanges[STATE_CH_WIN])
	{
		eNextState = STATE_WIN;
	}
	/*else if (m_bStateChanges[STATE_CH_LOSE])
	{
	eNextState = STATE_LOSE;
	}*/
	else if (m_bStateChanges[STATE_CH_CUTSCENE])
	{
		eNextState = STATE_CUTSCENE;
	}

	if (CBaseState::CheckState() == STATE_NULL)
	{
		eNextState = STATE_NULL;
		CAchieveTracker::GetInstance()->SetIsExit(true);
	}

	//Return the next state
	return eNextState;
}

void CGameplayState::NotifyGuiForSubtitle(bool value)
{
	if (nullptr != m_pGUI)
	{
		m_pGUI->SetSubtuitleBoxVisible(value);
	}
}

CEntityManager* CGameplayState::m_pEntityManager;
