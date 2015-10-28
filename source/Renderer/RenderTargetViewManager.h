#pragma once
class CRenderTargetViewManager
{
public:
	enum eRenderTargetView
	{
		COUNT
	};

	~CRenderTargetViewManager(void);

	static CRenderTargetViewManager &GetReference( );

	static void DeleteInstance( );

	bool ApplyRenderTargetView(eRenderTargetView state);

	eRenderTargetView GetCurrentRenderState( )
	{
		return m_tCurrentRenderState;
	}

	void ClearState( );

private:
	CRenderTargetViewManager(void);
	CRenderTargetViewManager(const CRenderTargetViewManager &)
	{
	}
	CRenderTargetViewManager &operator=(const CRenderTargetViewManager &)
	{
		return *this;
	}

	static CRenderTargetViewManager *instancePtr;

	eRenderTargetView m_tCurrentRenderState;
};

