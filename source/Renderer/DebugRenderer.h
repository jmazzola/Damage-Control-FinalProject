#ifndef _DEBUGRENDERER_H_
#define _DEBUGRENDERER_H_

class CDebugRenderer
{
	struct Line
	{
		DirectX::XMFLOAT3 _XMStart;
		DirectX::XMFLOAT3 _XMEnd;
		DirectX::XMFLOAT4 _XMColor;// = DirectX::XMFLOAT4(1, 1, 1, 1);
	};
	struct Square
	{
		DirectX::XMFLOAT3 _XMPosition;
		DirectX::XMFLOAT2 _XMRadius;
		DirectX::XMFLOAT4 _XMColor;// = DirectX::XMFLOAT4(1, 1, 1, 1);
	};
	struct Circle
	{
		DirectX::XMFLOAT3 _XMPosition;
		DirectX::XMFLOAT2 _XMRadius;
		DirectX::XMFLOAT4 _XMColor;// = DirectX::XMFLOAT4(1, 1, 1, 1);
	};

public:
	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	virtual ~CDebugRenderer(void);

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	static CDebugRenderer &GetReference( );

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	static void DeleteInstance( );

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void LineRenderer(DirectX::XMFLOAT3 _XMStart,
					  DirectX::XMFLOAT3 _XMEnd,
					  DirectX::XMFLOAT4 _XMColor = DirectX::XMFLOAT4(1, 1, 1, 1));

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void SquareRenderer(DirectX::XMFLOAT3 _XMPosition,
						DirectX::XMFLOAT2 _XMRadius,
						DirectX::XMFLOAT4 _XMColor = DirectX::XMFLOAT4(1, 1, 1, 1));

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void CircleRenderer(DirectX::XMFLOAT3 _XMPosition,
						DirectX::XMFLOAT2 _XMRadius,
						DirectX::XMFLOAT4 _XMColor = DirectX::XMFLOAT4(1, 1, 1, 1));

	void Execute( );

private:
	CDebugRenderer(void);
	CDebugRenderer(const CDebugRenderer &)
	{
	}
	CDebugRenderer &operator=(const CDebugRenderer &)
	{
		return *this;
	}

	bool m_bExecute = false;
	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void CreateBuffers( );

	/*****************************************************************
	* function:
	*
	* Ins:
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/28/2015
	* Mod. Initials:	      DG
	*****************************************************************/
	void ApplyDebug( );

	static CDebugRenderer*	instancePtr;

	ID3D11Buffer*			m_d3dLineVertexBuffer;
	ID3D11Buffer*			m_d3dSquareVertexBuffer;
	ID3D11Buffer*			m_d3dCircleVertexBuffer;

	int						m_nLine, m_nSquare, m_nCircle;

	std::vector<Line> m_vLines;
	std::vector<Square> m_vSquares;
	std::vector<Circle> m_vCircles;
};

#endif //_DEBUGRENDERER_H_
