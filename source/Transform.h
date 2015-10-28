#pragma once

//#include "Core\stdafx.h"
#include <DirectXMath.h>

class Transform
{
public:
	Transform();
	~Transform();

	void UpdateWorld() 
	{ 
		if (parent)
		{ DirectX::XMStoreFloat4x4(&World, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&Local),DirectX::XMLoadFloat4x4(&parent->GetWorld()))); }
		else { World = Local; } 
	}

	DirectX::XMFLOAT4X4& GetWorld()
	{
		UpdateWorld();
		return World;
	}

	/*const DirectX::XMFLOAT4X4& GetWorld() const
	{
		return World;
	}
	*/
	DirectX::XMFLOAT4X4 Local;
	Transform* parent;
private:
	DirectX::XMFLOAT4X4 World;

};

