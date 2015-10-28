//--------------------------------------------------------------------------------
// Written by Justin Murphy
//--------------------------------------------------------------------------------
#include "AudioSystemWwise.h"
#include "../Entity Manager/Entity.h"
#include "../Core/stdafx.h"
#include "../Renderer/Renderer.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkModule.h>
#include <AK/MusicEngine/Common/AkMusicEngine.h>
#include <AK/Tools/Common/AkMonitorError.h>
#include <AK/Tools/Win32/AkPlatformFuncs.h>
#include <AkDefaultIOHookBlocking.h>
#include <string>
#include <sstream>
//--------------------------------------------------------------------------------
#pragma comment(lib, "AkSoundEngineDLL.lib")
//--------------------------------------------------------------------------------
using namespace AK;
//--------------------------------------------------------------------------------
AudioSystemWwise* AudioSystemWwise::m_spAudioSystem = nullptr;
//--------------------------------------------------------------------------------
AudioSystemWwise::AudioSystemWwise() :
m_fWorldScale(1.f),
m_bInitialized(false)
{
	if (m_spAudioSystem == nullptr)
		m_spAudioSystem = this;
}
//--------------------------------------------------------------------------------
AudioSystemWwise::~AudioSystemWwise()
{

}
//--------------------------------------------------------------------------------
AudioSystemWwise* AudioSystemWwise::Get()
{
	return(m_spAudioSystem);
}
//--------------------------------------------------------------------------------
bool AudioSystemWwise::Initialize()
{
	// Initialize audio engine
	// Memory.
	AkMemSettings memSettings;

	// Streaming.
	AkStreamMgrSettings stmSettings;
	StreamMgr::GetDefaultSettings(stmSettings);

	AkDeviceSettings deviceSettings;
	StreamMgr::GetDefaultDeviceSettings(deviceSettings);
	
	AkInitSettings l_InitSettings;
	AkPlatformInitSettings l_platInitSetings;
	SoundEngine::GetDefaultInitSettings(l_InitSettings);
	SoundEngine::GetDefaultPlatformInitSettings(l_platInitSetings);

	// Setting pool sizes for this game.
	// These sizes are tuned for this game, every game should determine its own optimal values.
	memSettings.uMaxNumPools = 40;

	l_InitSettings.uDefaultPoolSize = 16 * 1024 * 1024;  // 16 MB
	l_InitSettings.uMaxNumPaths = 32;
	l_InitSettings.uMaxNumTransitions = 128;
	l_InitSettings.uCommandQueueSize = 2 * 1024 * 1024; //2 MB
	l_InitSettings.uNumSamplesPerFrame = 2048;
	l_InitSettings.bEnableGameSyncPreparation = true;
	
	l_platInitSetings.uLEngineDefaultPoolSize = 16 * 1024 * 1024;  // 32 MB
	l_platInitSetings.hWnd = CRenderer::GetReference().GetWindow();
	l_platInitSetings.bGlobalFocus = false;

	deviceSettings.uIOMemorySize = 32 * 1024 * 1024;
	deviceSettings.uMaxConcurrentIO = 256;
	deviceSettings.fMaxCacheRatio = 1.0f;

	stmSettings.uMemorySize = 64 * 1024*1024; // 64 MB

	AkMusicSettings musicInit;
	MusicEngine::GetDefaultInitSettings(musicInit);
	musicInit.fStreamingLookAheadRatio = 100;

	AKRESULT eResult = SOUNDENGINE_DLL::Init(&memSettings,
		&stmSettings,
		&deviceSettings,
		&l_InitSettings,
		&l_platInitSetings,
		&musicInit);

	if (eResult != AK_Success)
	{
		// Then, we will run the game without sound
		SOUNDENGINE_DLL::Term();
		return false;
	}

	SOUNDENGINE_DLL::SetBasePath(L"Assets/Sound/");
	//SOUNDENGINE_DLL::SetLangSpecificDirName( L"English(US)/" );

	// register 64 dummy game objects for one-shot positioning. See PostEvent(ID, location) function for details
	for (int i = 256; i < 512; ++i)
	{
		char name[256];
		sprintf_s(name, 256, "Dummy #%i", i - 63);
		SoundEngine::RegisterGameObj(i, name);
	}

	SetWorldScale(180.0f);

	m_bInitialized = true;
	//DebugWPrint(L"AudioSystemWwise has been Initialized\n");
	return true;
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::Shutdown()
{
	SoundEngine::UnregisterAllGameObj();
	// Terminate audio engine
	SOUNDENGINE_DLL::Term();
	//DebugWPrint(L"AudioSystemWwise has been Shutdown\n");
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::Update()
{
	AkListenerPosition listenerPosition;
	AkSoundPosition soundPosition;

	// update listener positions and orientations
	for (unsigned int i = 0; i < m_RegisteredListeners.size(); i++)
	{
		if (m_RegisteredListeners[i])
		{
			DirectX::XMFLOAT3 & pos = ((CRObject*)m_RegisteredListeners[i])->GetPosition();

			listenerPosition.Position.X = pos.x;
			listenerPosition.Position.Y = pos.y;
			listenerPosition.Position.Z = pos.z;

			DirectX::XMFLOAT3 & zAxis = m_RegisteredListeners[i]->GetZAxis();
			listenerPosition.OrientationFront.X = zAxis.x;
			listenerPosition.OrientationFront.Y = zAxis.y;
			listenerPosition.OrientationFront.Z = zAxis.z;

			DirectX::XMFLOAT3 & yAxis = m_RegisteredListeners[i]->GetYAxis();
			listenerPosition.OrientationTop.X = yAxis.x;
			listenerPosition.OrientationTop.Y = yAxis.y;
			listenerPosition.OrientationTop.Z = yAxis.z;

			SoundEngine::SetListenerPosition(listenerPosition, i);
		}
	}

	// set entities positions and orientations
	for (UINT i = 0; i < m_RegisteredEntities.size(); i++)
	{
		if (m_RegisteredEntities[i])
		{
			const DirectX::XMFLOAT3 & pos = ((CRObject*)m_RegisteredEntities[i])->GetPosition();
			soundPosition.Position.X = pos.x;
			soundPosition.Position.Y = pos.y;
			soundPosition.Position.Z = pos.z;

			const DirectX::XMFLOAT3 & zAxis = m_RegisteredEntities[i]->GetZAxis();
			soundPosition.Orientation.X = zAxis.x;
			soundPosition.Orientation.Y = zAxis.y;
			soundPosition.Orientation.Z = zAxis.z;

			SoundEngine::SetPosition((AkGameObjectID)m_RegisteredEntities[i], soundPosition);
		}
	}
	// render the audio
	SOUNDENGINE_DLL::Tick();
}
//--------------------------------------------------------------------------------
bool AudioSystemWwise::RegisterEntity(CEntity* entity, const char* name)
{
	if (entity == nullptr)
	{
		return false;
	}
	if (NULL == name)
	{
		static int entityNum = 0;
		std::stringstream ssNum;
		ssNum << ++entityNum;
		std::string strTemp = "entity" + ssNum.str();

		if (SoundEngine::RegisterGameObj((AkGameObjectID)entity, strTemp.c_str()) != AK_Success)
		{
			wchar_t buff[MAX_PATH];
			wprintf_s(buff, MAX_PATH, L"%s%s%s", L"*Audio Error*\n Registering: \"", strTemp.c_str(), L"\"\n\n");
			DebugWPrint(buff);
			return false;
		}
	}
	else
	{
		if (SoundEngine::RegisterGameObj((AkGameObjectID)entity, name) != AK_Success)
		{
			wchar_t buff[MAX_PATH];
			wprintf_s(buff, MAX_PATH, L"%s%s", L"*Audio Error*\n Registering audio entity", L"\n\nContact the audio programmer and replicate error situation.\n");
			DebugWPrint(buff);
			return false;
		}
	}

	m_RegisteredEntities.push_back(entity);
	return true;
}
//--------------------------------------------------------------------------------
bool AudioSystemWwise::UnRegisterEntity(const CEntity* entity)
{
	for (unsigned int i = 0; i < m_RegisteredEntities.size(); i++)
	{
		if (m_RegisteredEntities[i] == entity)
		{
			m_RegisteredEntities.erase(m_RegisteredEntities.begin() + i);
			if (SoundEngine::UnregisterGameObj((AkGameObjectID)entity) != AK_Success)
			{
				wchar_t buff[MAX_PATH];
				wprintf_s(buff, MAX_PATH, L"%s%s", L"*Audio Error*\n Unegistering audio entity", L"\n\nContact the audio programmer and replicate error situation.\n");
				DebugWPrint(buff);
				return false;
			}
			break;
		}
	}
	return true;
}
//--------------------------------------------------------------------------------
bool AudioSystemWwise::RegisterListener(CEntity* entity, const char* name)
{
	if (entity == nullptr)
	{
		return false;
	}
	if (NULL == name)
	{
		std::stringstream ssNum;
		ssNum << (m_RegisteredListeners.size() + 1);
		std::string strTemp = "listener" + ssNum.str();

		if (SoundEngine::RegisterGameObj((AkGameObjectID)entity, strTemp.c_str()) != AK_Success)
		{
			wchar_t buff[MAX_PATH];
			wprintf_s(buff, MAX_PATH, L"%s%s%s", L"*Audio Error*\n Registering: \"", strTemp.c_str(), L"\"\n\nContact the audio programmer and replicate error situation.\n");
			DebugWPrint(buff);
			return false;
		}
	}
	else
	{
		if (SoundEngine::RegisterGameObj((AkGameObjectID)entity, name) != AK_Success)
		{
			wchar_t buff[MAX_PATH];
			wprintf_s(buff, MAX_PATH, L"%s%s", L"*Audio Error*\n Registering audio entity", L"\n\nContact the audio programmer and replicate error situation.\n");
			DebugWPrint(buff);
			return false;
		}
	}

	// ensure to update listeners scaling factor to the worlds
	AK::SoundEngine::SetListenerScalingFactor(m_RegisteredListeners.size(), m_fWorldScale);
	m_RegisteredListeners.push_back(entity);
	m_RegisteredEntities.push_back(entity);
	return true;
}
//--------------------------------------------------------------------------------
bool AudioSystemWwise::UnRegisterListener(const CEntity* entity)
{
	for (unsigned int i = 0; i < m_RegisteredListeners.size(); i++)
	{
		if (m_RegisteredListeners[i] == entity)
		{
			m_RegisteredListeners.erase(m_RegisteredListeners.begin() + i);
			if (SoundEngine::UnregisterGameObj((AkGameObjectID)entity) != AK_Success)
			{
				wchar_t buff[MAX_PATH];
				wprintf_s(buff, MAX_PATH, L"%s%s", L"*Audio Error*\n Unegistering audio entity", L"\n\nContact the audio programmer and replicate error situation.\n");
				DebugWPrint(buff);
				return false;
			}
			break;
		}
	}
	return UnRegisterEntity(entity);
}
//--------------------------------------------------------------------------------
bool AudioSystemWwise::PostEvent(AudioEvent eventId, const CEntity* eventCaller)
{
	if (eventCaller == nullptr)
	{
		return false;
	}
#ifdef _DEBUG
	bool found = false;
	for each(auto entity in m_RegisteredEntities)
	{
		if (entity == eventCaller)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		DebugWPrint(L"*Audio Error* PostEvent failed Entity not registered.\n");
		return false;
	}
#endif

	if (SoundEngine::PostEvent(eventId, (AkGameObjectID)eventCaller) == AK_INVALID_PLAYING_ID)
	{
		DebugWPrint(L"*Audio Error* PostEvent failed EventID might be incorrect.\n");
		DebugWPrint(L"Make sure you're loading the soundbanks before attempting to play a sound.\n");

		return false;
	}
	return true;
}
//--------------------------------------------------------------------------------
bool AudioSystemWwise::PostEvent(AudioEvent eventId, const DirectX::XMFLOAT3& pos)
{
	// cycle through the dummy game objects
	// number of dummies needs to be big enough to avoid setting a different position
	// on the same object twice in the same game frame
	static int iGameObj = 255;
	if (++iGameObj > 511)
		iGameObj = 256;

	AkSoundPosition soundPosition;
	soundPosition.Orientation.X = 0;
	soundPosition.Orientation.Y = 0;
	soundPosition.Orientation.Z = 1.0f;

	soundPosition.Position.X = pos.x;
	soundPosition.Position.Y = pos.y;
	soundPosition.Position.Z = pos.z;

	SoundEngine::SetPosition(iGameObj, soundPosition);
	if (SoundEngine::PostEvent(eventId, iGameObj) == AK_INVALID_PLAYING_ID)
	{
		DebugWPrint(L"*Audio Error* PostEvent failed EventID might be incorrect.\n");
		DebugWPrint(L"Make sure you're loading the soundbanks before attempting to play a sound.\n");
		return false;
	}
	return true;
}
//--------------------------------------------------------------------------------
bool AudioSystemWwise::PostEvent(AudioEvent eventId, unsigned int listenerId)
{
	if ((m_RegisteredListeners.size() - listenerId) < 0)
	{
		DebugWPrint(L"*Audio Error* The listener is not registered.\n");
		DebugWPrint(L"You cannot play an event on the listener if there is no listener.\n");
		return false;
	}
	else if (SoundEngine::PostEvent(eventId, (AkGameObjectID)m_RegisteredListeners[listenerId]) == AK_INVALID_PLAYING_ID)
	{
		DebugWPrint(L"*Audio Error* PostEvent failed EventID might be incorrect.\n");
		DebugWPrint(L"Make sure you're loading the soundbanks before attempting to play a sound.\n");
	}
	return true;
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::SetMaterialID(const CEntity* pEntity, AkUniqueID akMaterialSwitchDefine, const unsigned int nMaterialID)
{

}
//--------------------------------------------------------------------------------
void AudioSystemWwise::SetRTCPValue(const char* szRTCPName, const AkRtpcValue akRtcpVal)
{
	if (SoundEngine::SetRTPCValue(szRTCPName, akRtcpVal) != AK_Success)
	{
		wchar_t buff[MAX_PATH];
		wprintf_s(buff, MAX_PATH, L"%s%s%s", L"*Audio Error*\n RTCP Value fail on: \"", szRTCPName, L"\"\n\nRTCP name might be incorrect.\n");
		DebugWPrint(buff);
	}
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::SetRTCPValue(const char* szRTCPName, const AkRtpcValue akRtcpVal, const CEntity* pEntity)
{
	if (SoundEngine::SetRTPCValue(szRTCPName, akRtcpVal, (AkGameObjectID)pEntity) != AK_Success)
	{
		wchar_t buff[MAX_PATH];
		wprintf_s(buff, MAX_PATH, L"%s%s%s", L"*Audio Error*\n RTCP Value fail on: \"", szRTCPName, L"\"\n\nRTCP name might be incorrect or the associated entity might not be registered.\n\nNOTE: Make sure you're loading the soundbanks before attempting to play a sound.\n");
		DebugWPrint(buff);
	}
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::SetRTCPValue(const AkRtpcID akRtcpID, const AkRtpcValue akRtcpVal)
{
	if (SoundEngine::SetRTPCValue(akRtcpID, akRtcpVal) != AK_Success)
	{
		wchar_t buff[MAX_PATH];
		wprintf_s(buff, MAX_PATH, L"%s%u%s", L"*Audio Error*\n RTCP Value fail on: \"", akRtcpID, L"\"\n\nRTCP ID might be incorrect.\n");
		DebugWPrint(buff);
	}
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::SetRTCPValue(const AkRtpcID akRtcpID, const AkRtpcValue akRtcpVal, const CEntity* pEntity)
{
	if (SoundEngine::SetRTPCValue(akRtcpID, akRtcpVal, (AkGameObjectID)pEntity))
	{
		wchar_t buff[MAX_PATH];
		wprintf_s(buff, MAX_PATH, L"%s%u%s", L"*Audio Error*\n RTCP Value fail on: \"", akRtcpID, L"\"\n\nRTCP ID might be incorrect or the associated entity might not be registered.\n\nNOTE: Make sure you're loading the soundbanks before attempting to play a sound.\n");
		DebugWPrint(buff);
	}
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::SetState(const char* szStateGroupName, const char* szStateName)
{
	if (AK::SoundEngine::SetState(szStateGroupName, szStateName) != AK_Success)
	{
		wchar_t buff[MAX_PATH];
		wprintf_s(buff, MAX_PATH, L"%s%s%s%s%s", L"*Audio Error*\n Setting State Failure.\nState Group Name: \"", szStateGroupName,
			L"\"\nState Name: ", szStateName, L"\n\nState Group or State Name might not exist.\n");
		DebugWPrint(buff);
	}
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::SetState(const AkStateGroupID akStateGroupID, const AkStateID akStateID)
{
	if (AK::SoundEngine::SetState(akStateGroupID, akStateID) != AK_Success)
	{
		wchar_t buff[MAX_PATH];
		wprintf_s(buff, MAX_PATH, L"%s%u%s%u%s", L"*Audio Error*\n Setting State Failure.\nState Group ID: \"", akStateGroupID,
			L"\"\nState ID: \"", akStateID, L"\"\n\nState Group or State Name might not exist.\n");
		DebugWPrint(buff);
	}
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::SetSwitch(const AkSwitchGroupID akSwitchGroupID, const AkSwitchStateID akStateID, const CEntity* pEntity)
{
	if (AK::SoundEngine::SetSwitch(akSwitchGroupID, akStateID, (AkGameObjectID)pEntity) != AK_Success)
	{
		wchar_t buff[MAX_PATH];
		wprintf_s(buff, MAX_PATH, L"%s%u%s%u%s", L"*Audio Error*\n Setting Switch Failure.\nState Group ID: \"", akSwitchGroupID,
			L"\"\nState ID: \"", akStateID, L"\"\nSwitch Group or Switch Name might not exist.\n");
		DebugWPrint(buff);
	}
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::PostTrigger(const char* szTriggerName)
{
	AK::SoundEngine::PostTrigger(szTriggerName, AK_INVALID_GAME_OBJECT);
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::PostTrigger(const AkTriggerID akTriggerID)
{
	AK::SoundEngine::PostTrigger(akTriggerID, AK_INVALID_GAME_OBJECT);
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::PostTrigger(const char* szTriggerName, const CEntity* pEntity)
{
	AK::SoundEngine::PostTrigger(szTriggerName, (AkGameObjectID)pEntity);
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::PostTrigger(const AkTriggerID akTriggerID, const CEntity* pEntity)
{
	AK::SoundEngine::PostTrigger(akTriggerID, (AkGameObjectID)pEntity);
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::SetBasePath(const wchar_t* strPath)
{
	SOUNDENGINE_DLL::SetBasePath(strPath);
}
//--------------------------------------------------------------------------------
bool AudioSystemWwise::LoadSoundBank(const wchar_t* szBankName)
{
	AkBankID bankID;
	if (SoundEngine::LoadBank(szBankName, AK_DEFAULT_POOL_ID, bankID) == AK_Success)
		m_RegisteredSoundBanks[szBankName] = bankID;
	else
	{
		DebugWPrint(L"*Audio Error*\n Loading sound bank.\nTry checking your soundbank names.\n");
		return false;
	}
	std::wstring buff = L"Loaded SoundBank: ";
	buff += szBankName;
	buff += L"\n";
	//DebugWPrint(buff.c_str());
	return true;
}
//--------------------------------------------------------------------------------
bool AudioSystemWwise::UnloadSoundBank(const wchar_t* szBankName)
{
	if (m_RegisteredSoundBanks.find(szBankName) != m_RegisteredSoundBanks.end())
	{
		if (SoundEngine::UnloadBank(m_RegisteredSoundBanks[szBankName], nullptr, nullptr) != AK_Success)
		{
			DebugWPrint(L"*Audio Error*\n Unloading sound bank.\nTry checking your soundbank names.\n");
			return false;
		}
		else
		{
			m_RegisteredSoundBanks.erase(szBankName);
		}
	}
	else
	{
		DebugWPrint(L"*Audio Error*\n Unloading sound bank.\nSoundbank not in loaded banks.\n");
		return false;
	}

	return true;
}
//--------------------------------------------------------------------------------
void AudioSystemWwise::SetWorldScale(float fScale)
{
	m_fWorldScale = fScale;
	for (unsigned int i = 0; i < m_RegisteredListeners.size(); i++)
	{
		AK::SoundEngine::SetListenerScalingFactor(i, m_fWorldScale);
	}
}
//--------------------------------------------------------------------------------
#ifdef _DEBUG
void AudioSystemWwise::SendMessage(const char* message)
{
	Monitor::PostString(message, Monitor::ErrorLevel_Message);
}
#endif
//--------------------------------------------------------------------------------