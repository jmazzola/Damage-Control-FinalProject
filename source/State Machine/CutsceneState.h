#ifndef _CUTSCENESTATE_H_
#define _CUTSCENESTATE_H_

#include "BaseState.h"

class CFirstPersonCamera;
class CParticleSystem;

class CCutsceneState : public CBaseState
{
public:

	struct TFrame
	{
		DirectX::XMFLOAT3 m_xmPos;
		DirectX::XMFLOAT2 m_xmAngle;

		TFrame(DirectX::XMFLOAT3 xmPos, DirectX::XMFLOAT2 xmAng) : m_xmPos(xmPos), m_xmAngle(xmAng)
		{
		}
	};

	/*****************************************************************
	* Default Constructor
	*****************************************************************/
	CCutsceneState();

	/*****************************************************************
	* Disabled Copy Constructor
	*****************************************************************/
	CCutsceneState(const CCutsceneState& pOther) = delete;

	/*****************************************************************
	* Disabled Operator =
	*****************************************************************/
	CCutsceneState& operator= (const CCutsceneState& pOther) = delete;

	/*****************************************************************
	* Default Destructor
	*****************************************************************/
	//~CLoseState(void);

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
	virtual void Input(void);

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
	virtual void Update(float fDT);

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
	virtual void Render(CAssetManager* cRenderer);

	/*****************************************************************
	* Enter(): Called when switching to a state, initializes members
	*                and allocates memory
	*
	* Ins:			  CBaseState* cPrevState
	*
	* Outs:		      void
	*
	* Returns:		  bool, true if successfully entered
	*
	* Mod. Date:		      05/12/2015
	* Mod. Initials:	      MJG
	*****************************************************************/
	virtual bool FirstEnter(void);

	bool ReEnter(void);

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
	virtual bool Load(CAssetManager* pAssets, eStateLoadCommands* pLoadCommands = nullptr);

	/*****************************************************************
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
	virtual void UnLoad(eStateLoadCommands* pLoadCommands = nullptr);

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
	virtual bool Exit(void);

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
	virtual eStateType CheckState(void);


	bool TurnToPoint(float fDt, DirectX::XMFLOAT3 turnTo, float TurnSpeed);

	void DetermineReason();

private:

	CFirstPersonCamera* m_pFPSCam;
	std::vector<TFrame> m_vNodes;

	DirectX::XMFLOAT3 m_xmSmokePositions[5];
	CParticleSystem* m_pSmokeEffects[5];

	DirectX::XMFLOAT3 m_xmBloodPositions[5];
	CParticleSystem* m_pBloodEffects[5];

	DirectX::XMFLOAT3 m_xmSparkPositions[10];
	CParticleSystem* m_pSparkEffects[10];

	enum Reason { REASON_BLOWNUP, REASON_SUFFOCATE, REASON_EATEN, REASON_WIN, REASON_NUM };
	Reason m_Reason;

	float m_fTimer;

	int m_iCurrentNode;
};

#endif // _CUTSCENESTATE_H_
