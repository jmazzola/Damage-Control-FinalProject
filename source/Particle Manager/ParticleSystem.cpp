/***********************************************
* Filename:			ParticleSystem.cpp
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/

#include "Particle.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/RenderStateManager.h"
#include "../Renderer/ShaderManager.h"
#include "../Renderer/VertexStructures.h"

#include "../TinyXML2/tinyxml2.h"

#include "../Entity Manager/Entities/Player.h"
#include "../Entity Manager/EntityManager.h"

using namespace std;
using namespace tinyxml2;
using namespace DirectX;

/*****************************************************************
* CParticleSystem():
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
CParticleSystem::CParticleSystem()
{
	m_liveList.reserve(MAX_PARTICLES);

	ZeroMemory(&m_particles[0], sizeof(m_particles));
	ZeroMemory(&m_copyParticle, sizeof(m_copyParticle));

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.ByteWidth = MAX_PARTICLES * sizeof(CParticle);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(CParticle);

	ID3D11Device* pDevice = CRenderer::GetReference().GetDevice();
	HRESULT hr = pDevice->CreateBuffer(&desc, NULL, &m_pPointBuffer);
	assert(hr == S_OK && "Failed to create particle buffer");

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	ZeroMemory(&srv_desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	srv_desc.Buffer.ElementWidth = sizeof(CParticle);
	srv_desc.Buffer.NumElements = MAX_PARTICLES;
	srv_desc.Format = DXGI_FORMAT_UNKNOWN;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;

	hr = pDevice->CreateShaderResourceView(m_pPointBuffer, &srv_desc, &m_pSrv);
	assert(hr == S_OK && "Failed to create particle SRV");

	//hr = CreateWICTextureFromFile(CRenderer::GetReference().GetDevice( ), L"Assets/Images/Particles/smoke1.png", (ID3D11Resource**)&m_pTexture, &m_pTextureSrv);
	//assert(hr == S_OK && "Failed to load particle system texture");
}

/*****************************************************************
* ~CParticleSystem():
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
CParticleSystem::~CParticleSystem()
{
	for (unsigned int i = 0; i < m_emitters.size(); i++)
	{
		SafeDelete(m_emitters[i]);
	}

	m_emitters.clear();
	m_liveList.clear();

}

/*****************************************************************
* Update() - Updates each particle in the particle system
*
* Ins: dt - the ammount of time in seconds since the last update
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
void CParticleSystem::Update(float dt)
{
	for (unsigned int i = 0; i < MAX_PARTICLES; i++)
	{
		if (m_particles[i].m_fLifeTime <= 0.f)
			continue;

		m_particles[i].Update(dt);
		m_liveList.push_back(m_particles[i]);
	}
}

/****************************************************************
* Render() - Updates each particle in the particle system
*
* Ins:
*
* Outs:
*
* Returns:
*
* Mod. Date:		      05/12/2015
* Mod. Initials:
*****************************************************************/
void CParticleSystem::Render(void)
{
#ifndef I_NEED_APP_VERIFIER

	if (!m_liveList.size())
		return;

	ID3D11DeviceContext* pContext = CRenderer::GetReference().GetContext();

	CRenderStateManager::GetReference().ApplyRenderState(CRenderStateManager::eRenderState::PARTICLES);
	XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, translation);
	CRenderer::GetReference().SetParticleData(world, CScene::GetCamera()->View(), CScene::GetCamera()->Projection(), XMFLOAT2{ 1.f, 1.f });

	UINT stride = 0;
	UINT offset = 0;
	static ID3D11Buffer* nullBuffer = nullptr;
	pContext->IASetVertexBuffers(0, 1, &nullBuffer, &stride, &offset);

	D3D11_MAPPED_SUBRESOURCE subresource;
	ZeroMemory(&subresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	pContext->Map(m_pPointBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);

	DirectX::XMFLOAT3* data = (DirectX::XMFLOAT3*)subresource.pData;

	memcpy(subresource.pData, &m_particles[0], MAX_PARTICLES * sizeof(CParticle));

	pContext->Unmap(m_pPointBuffer, 0);
	pContext->VSSetShaderResources(0, 1, &m_pSrv.p);
	pContext->GSSetShaderResources(0, 1, &m_pSrv.p);
	pContext->PSSetShaderResources(0, 1, &m_pTextureSrv.p);
	pContext->Draw(m_liveList.size(), 0);

	m_liveList.clear();
#endif
}

/*****************************************************************
* ReadLine() - reads a line out of the file stream and puts it
*			   into 'out_line'
*
* Ins: file - the file to read from
*
* Outs: out_line - the line to send the math to
*
* Returns: The particle system loaded from filename
*
* Mod. Date:		      05/20/2015
* Mod. Initials:
*****************************************************************/
void ReadLine(std::string& file, unsigned int& fileIndex, std::string& out_line)
{
	if (fileIndex >= file.size())
		return;
	char c = file[fileIndex];

	for (; c != '\n' && c != '\0'; fileIndex++)
	{
		out_line.push_back(c);
		c = file[fileIndex];
	}
}

/*****************************************************************
* SplitString() - cuts up the std::string based on delimiters and stores
*				  the result in 'output'
*
* Ins: delim - the characters to split the std::string
*
* Outs: output - the orginal std::string split up based on 'dlim'
*
* Returns: The particle system loaded from filename
*
* Mod. Date:		      05/20/2015
* Mod. Initials:
*****************************************************************/
void SplitString(std::string& s, std::vector<char>& delim, std::vector<std::string>* output)
{
	std::string currentString;
	for (unsigned int i = 0; i < s.size(); i++)
	{
		char c = s[i];

		bool foundValue = false;
		for (unsigned int ii = 0; ii < delim.size(); ii++)
		{
			if (delim[ii] == c)
			{
				output->push_back(currentString);
				currentString = "";
				foundValue = true;
				break;
			}
		}

		if (foundValue)
			continue;

		currentString.push_back(c);
	}

	output->push_back(currentString);
}

/*****************************************************************
* LoadFrom() - loads a particle system out of a maya file
*
* Ins: filename - the name of the maya file to load
*
* Outs:
*
* Returns: The particle system loaded from filename
*
* Mod. Date:		      05/19/2015
* Mod. Initials:
*****************************************************************/
/*static*/ CParticleSystem* CParticleSystem::LoadFrom(const char* filename)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(filename);
	XMLElement* pRoot = doc.RootElement();

	CParticleSystem* outval = new CParticleSystem;

	XMLElement* texElement = pRoot->FirstChildElement("Texture");

	const char* texFilename = texElement->Attribute("Filename");
	wchar_t widename[MAX_PATH * 4];
	int ret = MultiByteToWideChar(CP_UTF8, 0, texFilename, -1, widename, MAX_PATH * 4);

	//ID3D11Resource* pResource = NULL;
	//HRESULT hr = CreateWICTextureFromFile(CRenderer::GetReference().GetDevice( ), widename, &pResource, &outval->m_pTextureSrv.p);
	//SafeRelease(pResource);
	//assert(hr == S_OK && "Failed to load particle system texture");
	CAssetManager::LoadTexture(texFilename, &outval->m_pTextureSrv.p);

	//outval->m_pTexture = pTexture;
	//outval->m_pTextureSrv = pSrv;

	for (XMLElement* emitterNode = pRoot->FirstChildElement("Emitter"); emitterNode; emitterNode = emitterNode->NextSiblingElement("Emitter"))
	{
		float minLifeTime = (float)atof(emitterNode->Attribute("MinLifeTime"));
		float maxLifeTime = (float)atof(emitterNode->Attribute("MaxLifeTime"));

		XMLElement* minSizeElement = emitterNode->FirstChildElement("MinSize");
		XMLElement* maxSizeElement = emitterNode->FirstChildElement("MaxSize");
		XMLElement* minSpeedElement = emitterNode->FirstChildElement("MinSpeed");
		XMLElement* maxSpeedElement = emitterNode->FirstChildElement("MaxSpeed");
		XMLElement* minRotationElement = emitterNode->FirstChildElement("MinRotation");
		XMLElement* maxRotationElement = emitterNode->FirstChildElement("MaxRotation");
		XMLElement* positionElement = emitterNode->FirstChildElement("Position");
		XMLElement* directionElement = emitterNode->FirstChildElement("Direction");
		XMLElement* startColorElement = emitterNode->FirstChildElement("StartColor");
		XMLElement* endColorElement = emitterNode->FirstChildElement("EndColor");
		XMLElement* rotationSpeedElement = emitterNode->FirstChildElement("RotationSpeed");

		float minSize = (float)atof(minSizeElement->GetText());
		float maxSize = (float)atof(maxSizeElement->GetText());

		float minSpeed = (float)atof(minSpeedElement->GetText());
		float maxSpeed = (float)atof(maxSpeedElement->GetText());

		float minRotation = (float)atof(minRotationElement->GetText());
		float maxRotation = (float)atof(maxRotationElement->GetText());

		float rotationSpeed = (float)atof(rotationSpeedElement->GetText());
		unsigned int ammount = (unsigned int)atoi(emitterNode->Attribute("EmitAmmount"));

		float3 position = float3
		{
			(float)atof(positionElement->Attribute("x")),
			(float)atof(positionElement->Attribute("y")),
			(float)atof(positionElement->Attribute("z")),
		};
		float3 direction = float3
		{
			(float)atof(directionElement->Attribute("x")),
			(float)atof(directionElement->Attribute("y")),
			(float)atof(directionElement->Attribute("z")),
		};

		float4 startColor
		{
			(float)atof(startColorElement->Attribute("r")),
			(float)atof(startColorElement->Attribute("g")),
			(float)atof(startColorElement->Attribute("b")),
			(float)atof(startColorElement->Attribute("a")),
		};
		float4 endColor =
		{
			(float)atof(endColorElement->Attribute("r")),
			(float)atof(endColorElement->Attribute("g")),
			(float)atof(endColorElement->Attribute("b")),
			(float)atof(endColorElement->Attribute("a")),
		};

		CParticle flyweight;
		ZeroMemory(&flyweight, sizeof(CParticle));

		memcpy(&flyweight.m_startColor, &startColor, sizeof(float4));

		memcpy(&flyweight.m_endColor, &endColor, sizeof(float4));

		flyweight.m_fRotationSpeed = rotationSpeed;
		flyweight.m_vVelocity = direction;

		CParticleEmitter* pEmitter = new CParticleEmitter(outval, flyweight);
		pEmitter->m_unAmmoutPerEmit = ammount;
		pEmitter->m_fMinLifeTime = minLifeTime;
		pEmitter->m_fMaxLifeTime = maxLifeTime;

		pEmitter->m_fMinRotation = XMConvertToRadians(minRotation);
		pEmitter->m_fMaxRotation = XMConvertToRadians(maxRotation);

		pEmitter->m_fMinSize = minSize;
		pEmitter->m_fMaxSize = maxSize;

		pEmitter->m_fMinSpeed = minSpeed;
		pEmitter->m_fMaxSpeed = maxSpeed;
	}

	return outval;
}
