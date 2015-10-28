//--------------------------------------------------------------------------------
// AudioSystemWwise
// Written by Justin Murphy
//
// Wwise audio wrapper keeps all audio engine classes abstracted from the games classes,
// allows entities to register so their positions/orientations will be updated internally,
// provides useful interfaces for commonly used features of Wwise
//--------------------------------------------------------------------------------
#ifndef AudioSystemWwise_h
#define AudioSystemWwise_h
//--------------------------------------------------------------------------------
#include "AkSoundEngineDLL.h"
#include <DirectXMath.h>
#include <vector>
#include <map>
#include "Wwise_IDs.h"
//--------------------------------------------------------------------------------
using DirectX::XMFLOAT3;
//--------------------------------------------------------------------------------
#if defined(_WIN64)
typedef unsigned __int64 AudioEvent;				///< Integer (unsigned) type for pointers
#else
typedef __w64 unsigned int AudioEvent;			///< Integer (unsigned) type for pointers
#endif
//--------------------------------------------------------------------------------
class CEntity;
//--------------------------------------------------------------------------------
class AudioSystemWwise
{
public:
	AudioSystemWwise();
	virtual ~AudioSystemWwise();

	// Access to the audiosystem.  There should only be a single instance
	// of the audiosystem at any given time.
	static AudioSystemWwise* Get();

	virtual bool Initialize();
	virtual void Shutdown();

	//Updates the positions and orientations of all registered entities and renders the audio
	virtual void Update();

	// listener and entity registration
	virtual bool RegisterEntity(CEntity* entity, const char* name);
	virtual bool UnRegisterEntity(const CEntity* entity);
	virtual bool RegisterListener(CEntity* entity, const char* name);
	virtual bool UnRegisterListener(const CEntity* entity);

	//Posts an audio event at the caller's position
	virtual bool PostEvent(AudioEvent eventId, const CEntity* eventCaller);
	//Posts an audio event at the specified position
	virtual bool PostEvent(AudioEvent eventId, const DirectX::XMFLOAT3& pos);
	//Posts an audio event at the listeners position (no attenuation/panning)
	virtual bool PostEvent(AudioEvent eventId, unsigned int listenerId = 0);

	// Materials
	void SetMaterialID(const CEntity* pEntity, AkUniqueID akMaterialSwitchDefine, const unsigned int nMaterialID);

	// RTCP handling (dynamic sound adjustment based on a value)
	void SetRTCPValue(const char* szRTCPName, const AkRtpcValue akRtcpVal);
	void SetRTCPValue(const char* szRTCPName, const AkRtpcValue akRtcpVal, const CEntity* pEntity);
	void SetRTCPValue(const AkRtpcID akRtcpID, const AkRtpcValue akRtcpVal);
	void SetRTCPValue(const AkRtpcID akRtcpID, const AkRtpcValue akRtcpVal, const CEntity* pEntity);

	// States
	void SetState(const char* szStateGroupName, const char* szStateName);
	void SetState(const AkStateGroupID akStateGroupID, const AkStateID akStateID);

	// Switches
	void SetSwitch(const AkSwitchGroupID akSwitchGroupID, const AkSwitchStateID akStateID, const CEntity* pEntity = nullptr);

	// Triggers
	void PostTrigger(const char* szTriggerName);
	void PostTrigger(const AkTriggerID akTriggerID);
	void PostTrigger(const char* szTriggerName, const CEntity* pEntity);
	void PostTrigger(const AkTriggerID akTriggerID, const CEntity* pEntity);

	//set the base Path for sound banks
	void SetBasePath(const wchar_t* strPath);
	bool LoadSoundBank(const wchar_t* szBankName);
	bool UnloadSoundBank(const wchar_t* szBankName);

	//set world scale
	void SetWorldScale(float fScale);

	bool GetInitialized() const { return m_bInitialized; }

#ifdef _DEBUG
	// post a message in the wwise capture list. ERROR and DEBUGGING Purposes only.
	void SendMessage(const char* message);
#endif

protected:
	AudioSystemWwise& operator=(AudioSystemWwise&);

	static AudioSystemWwise*	m_spAudioSystem;

	std::vector<CEntity*> m_RegisteredListeners;		// entities registered as Listeners
	std::vector<CEntity*> m_RegisteredEntities;			// entities registered as game objects
	std::map<std::wstring, AkBankID> m_RegisteredSoundBanks;

	float m_fWorldScale; //world scaling factor
	bool m_bInitialized;
};
//--------------------------------------------------------------------------------
#endif // AudioSystemWwise_h
//--------------------------------------------------------------------------------