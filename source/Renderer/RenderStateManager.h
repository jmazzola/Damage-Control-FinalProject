#pragma once
class CRenderStateManager
{
public:
	enum eRenderState
	{
		HUD,
		FONT,

		GEOMETRY,
		LIGHTING,
		MATERIALS,

		BACKFACE_TRANSPARENT,
		FRONTFACE_TRANSPARENT,

		ANIMATION,
		INSTANCE_ANIMATION,

		SKYBOX,

		DEBUG_LINE,

		PARTICLES,

		COUNT
	};

	~CRenderStateManager(void);

	static CRenderStateManager &GetReference( );

	static void DeleteInstance( );

	bool ApplyRenderState(eRenderState state);

	eRenderState GetCurrentRenderState( )
	{
		return m_tCurrentRenderState;
	}

	void ClearState( );

private:
	CRenderStateManager(void);
	CRenderStateManager(const CRenderStateManager &)
	{
	}
	CRenderStateManager &operator=(const CRenderStateManager &)
	{
		return *this;
	}

	void ApplyTransparency( );
	void Apply2D( );

	static CRenderStateManager *instancePtr;

	eRenderState m_tCurrentRenderState;
};
