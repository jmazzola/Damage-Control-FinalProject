

#ifndef _SYSTEMGUIDE_H_
#define _SYSTEMGUIDE_H_

#include "../../Renderer/RObject.h"

class CSubSystem;
class CScene;

namespace FloorGuides
{
	struct TGuideMarker
	{
		//The LED display that the arrows render on
		CRObject m_cDisplay;
		//The arrow showing on the display
		CRObject m_cFrame;
		//The Y rotation of each arrow used by this marker
		union
		{
			struct
			{
				//The reactor's arrow's rotation
				float m_fReactorRotation;
				//The life support's arrow's rotation
				float m_fLifeSupportRotation;
				//The engine's arrow's rotation
				float m_fEngineRotation;
			};
			//All subsystems' arrows' rotations in an array
			float m_fSystemRotation[3];
		};
	};

	//Serves as a container for the markers
	class CFloorGuide
	{
	public:
		CFloorGuide(void);
		CFloorGuide(const CFloorGuide& other) = delete;
		~CFloorGuide();

		//Resets the blinking and such to their starting state
		void Reset(void);

		//Updates the color and such
		void Update(float fDt);
		//Allows our dudes to render
		void AddToScene(CScene* pScene);

		//Adds a guide marker to the floor guide
		void AddGuide(TGuideMarker* pMarker);
		//Creates a guide marker with this information and adds it to the floor guide
		void AddGuide(DirectX::XMFLOAT3 xmPosition, float fReactorRotation, float fLifeSupportRotation, float fEngineRotation);

		//Add the subsystems 0 == Reactor, 1 == LifeSupport, 2 == Engine
		void AddSubSystem(CSubSystem* pSystem, unsigned char uWhich);

		//Accessors
		unsigned int GetMarkerCount(void) const { return m_lMarkers.size(); }
		TGuideMarker* GetMarker(unsigned int index) const;

	private:

		std::string GetCurrTag() const { return *(&m_szReactorName + m_uCurrentArrow); }
		std::string GetPrevTag() const { return *(&m_szReactorName + m_uPrevArrow); }

		//Calculates the color/tag/everything about the Markers
		//	Returns true if a new blink has begun (might need to update texture data)
		bool ProcessBlink(float fDt);
		//Changes color, texture, etc for the next blink
		void CalculateNextBlink(void);
		//Applies that info to the markers
		void ApplyBlink(bool bChangeTexture);

		void CalculateSystemStatus(void);

		enum ESubSystemAlert : unsigned int 
		{
			REACTOR_DAMAGED = 0x1,
			REACTOR_UNDER_ATTACK = 0x2,
			REACTOR_DOWN = 0x4,
			LIFESUPPORT_DAMAGED = 0x8,
			LIFESUPPORT_UNDER_ATTACK = 0x10,
			LIFESUPPORT_DOWN = 0x20,
			ENGINE_DAMAGED = 0x40,
			ENGINE_UNDER_ATTACK = 0x80,
			ENGINE_DOWN = 0x100,
		};
		//What's the status of everything
		unsigned short m_eSystemStatus = 0;
		//Which arrow should we use?
		// 0 == Reactor, 1 == Life Support, 2 == Engine
		unsigned char m_uCurrentArrow = 1;
		//What was the last arrow we showed
		unsigned char m_uPrevArrow = 0;
		//Blink Info
		//The progress of the current blink
		float m_fBlinkProgress = 0;
		//How long has this blink last
		float m_fBlinkElapsed = 0;
		//How long should this blink last
		float m_fBlinkDuration = 1.5f;
		//How may more times should we show this arrow before switching to a different one
		int m_nBlinkCount = 0;

		//What color should we render all the dudes as
		DirectX::XMFLOAT3 m_xmCurColor;
		//The brightest color we'll use
		DirectX::XMFLOAT3 m_xmTarColor;
		//Light range
		float m_fBrightness = 0;

		//A list of all the markers
		std::list<TGuideMarker*> m_lMarkers;
		//What are the tags of all the models we'll be using
		std::string m_szFrameName;
		std::string m_szReactorName;
		std::string m_szLifeSupportName;
		std::string m_szEngineName;

		//Holds the sub-systems so we can react to their crap
		union
		{
			struct
			{
				//Local reactor pointer
				CSubSystem* m_pReactor;
				//Local life support pointer
				CSubSystem* m_pLifeSupport;
				//Local engine pointer
				CSubSystem* m_pEngine;
			};
			//All the subsystems in an array
			CSubSystem* m_pSubSystems[3];
		};
	};
}

#endif