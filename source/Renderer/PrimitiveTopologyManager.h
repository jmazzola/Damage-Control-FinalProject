#pragma once
class CPrimitiveTopologyManager
{
public:
	enum TTopology
	{
		TOPOLOGY_TRIANGLELIST,
		TOPOLOGY_POINTLIST,
		TOPOLOGY_LINELIST,
		TOPOLOGY_COUNT
	};

	~CPrimitiveTopologyManager(void);

	static CPrimitiveTopologyManager &GetReference( );

	static void DeleteInstance( );

	bool ApplyTopology(TTopology topology);

	TTopology GetCurrentTopology( )
	{
		return m_tCurrentTopology;
	}

	void ClearState( );

private:
	CPrimitiveTopologyManager(void);
	CPrimitiveTopologyManager(const CPrimitiveTopologyManager &)
	{
	}
	CPrimitiveTopologyManager &operator=(const CPrimitiveTopologyManager &)
	{
		return *this;
	}

	static CPrimitiveTopologyManager *instancePtr;

	TTopology m_tCurrentTopology;
};
