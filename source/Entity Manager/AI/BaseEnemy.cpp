/***********************************************
* Filename:			BaseEnemy.h
* Date:      		05/12/2015
* Mod. Date:		05/27/2015
* Mod. Initials:	JM
* Author:    		Jagoba Marcos
* Purpose:			Pure virtual class to store enemy shared members
*					and functions.
************************************************/

#include "BaseEnemy.h"
#include "..\..\Audio Manager\AudioSystemWwise.h"

#include "../../Renderer/DebugRenderer.h"
#include "..\..\State Machine\GameplayState.h"
#include <math.h>
#include "../Entities/Player.h"
#include "../StatusEffects.h"

using namespace MathHelper;

CBaseEnemy::CBaseEnemy()
{
	AudioSystemWwise::Get()->RegisterEntity(this, "Enemy");
	m_eRObjectType = RObjectType::ROBJECT_ANIMATED;

	ModelTransform.parent = &MyWorld;
	DirectX::XMStoreFloat4x4(&ModelTransform.Local, DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(180.0f)));
	m_fIdleDelay = float(rand() % 100) * 0.2f;
	
	m_pVariantIndicator = new CRObject;
	m_pVariantIndicator->SetTag("VariantPlane");
	m_pVariantIndicator->SetColor(DirectX::XMFLOAT4{ 1, 1, 1, 0.99f });


	m_pStatusEffects = new CStatusBank();
}
CBaseEnemy::~CBaseEnemy()
{

	ChangeTarget(nullptr);
	AudioSystemWwise::Get()->UnRegisterEntity(this);
	delete m_pVariantIndicator;
}

void CBaseEnemy::ChangeTarget(CEntity* pTarget)
{
	if (m_pTarget != nullptr)
	{
		m_pTarget = nullptr;
	}
	m_pTarget = pTarget;
}
bool CBaseEnemy::DealDamage(void)
{
	return DealDamage(m_fDamage);
}

bool CBaseEnemy::DealDamage(float fDamage)
{
	if (m_pTarget != nullptr)
	{
		if (static_cast<CLivingEntities*>(m_pTarget))
			static_cast<CLivingEntities*>(m_pTarget)->TakeDamage(fDamage);

		//printf("[BaseEnemy::Dealdamage] Enemy dealed %i damages. \n", (int)m_fDamage);
		return true;
	}
	return false;
}

//============================== PATHFINDING =======================

//void CBaseEnemy::CallPathFinder(DirectX::XMFLOAT3 goalPosition)
//{
//	if (m_vPath.size() > 0)
//		CPathSearch::GetInstance()->Enter(m_vPath.back().centroid, goalPosition);
//	else
//		CPathSearch::GetInstance()->Enter(this->GetPosition(), goalPosition);
//	CPathSearch::GetInstance()->Update();
//	m_vPath.clear();
//	m_vPath = CPathSearch::GetInstance()->GetSolution();
//	CPathSearch::GetInstance()->Exit();
//}

DirectX::XMFLOAT3 ClosestPointInSegment(DirectX::XMFLOAT3 p0, DirectX::XMFLOAT3 p1, DirectX::XMFLOAT3 myPoint)
{
	DirectX::XMFLOAT3 closest;
	DirectX::XMFLOAT3 edge0 = p1 - p0;
	DirectX::XMFLOAT3 norm = Normalize(edge0);
	DirectX::XMFLOAT3 V = myPoint - p0;
	float dote = Dot(norm, V);

	if (dote < 0)
	{
		closest = p0;
	}
	else if (dote > Length(p1 - p0))
	{
		closest = p1;
	}
	else
	{
		closest = p0 + (norm * dote);
	}

	return closest;
}

void CBaseEnemy::MakeMovement(float fDt)
{

	//forward
	//CDebugRenderer::GetReference().LineRenderer(GetPosition(), (MathHelper::XMMatrixForward(m_XMWorldMatrix) * 20) + GetPosition(), DirectX::XMFLOAT4{ 1, 0, 0, 1 });

	//check if the agent is in the same triangle
	DirectX::XMFLOAT3 myPos = GetPosition();

	CPathSearch::GetInstance()->PointInTriangle(myPos, m_nMyTriangle);
	CPathSearch::GetInstance()->GetClosestTriangle(m_vDestination, m_nGoalTriangle);
	DirectX::XMFLOAT3 velocity;

	//debug
	//CPathSearch::GetInstance()->RenderMyTriangle(m_nMyTriangle);
	//CPathSearch::GetInstance()->RenderMyTriangle(m_nGoalTriangle);

	if (m_nMyTriangle == m_nGoalTriangle)
	{
		//go for it
		velocity = Normalize(m_vDestination - myPos);
	}
	else
	{
		DirectX::XMFLOAT3 pointToGo = SelectDirection();
		velocity = Normalize(pointToGo - myPos);

		//debug
		//CDebugRenderer::GetReference().CircleRenderer(pointToGo, DirectX::XMFLOAT2{ 20, 20 });
	}

	//turn
	TurnToPoint(fDt, velocity, m_fTurningSpeed);
	velocity = Normalize(velocity);

	DirectX::XMFLOAT3 ford = *reinterpret_cast<DirectX::XMFLOAT3*>(MyWorld.GetWorld().m[2]);
	float speedLimitator = Dot(ford, velocity);

	speedLimitator = (speedLimitator + 1) / 2;

	float fSpeed = m_fMovementSpeed;
	
	if (m_pStatusEffects)
	{
		fSpeed = m_pStatusEffects->GetEffectedStat(STAT_EFF_SLOW, fSpeed);
	}
	m_vVelocity = ford * fSpeed * speedLimitator * fDt;


	//CDebugRenderer::GetReference().CircleRenderer(m_vVelocity, DirectX::XMFLOAT2{ 10, 10 }, DirectX::XMFLOAT4{ 1, 0, 0, 1 });

	//CHECK BEFORE MOVING
	DirectX::XMFLOAT3 newPosition = GetPosition();
	newPosition = newPosition + m_vVelocity;

	int debugstuff;
	if (CPathSearch::GetInstance()->PointInTriangle(newPosition, debugstuff) == true)
	{
		//move only if is valid
		MyWorld.Local._41 = newPosition.x;
		MyWorld.Local._43 = newPosition.z;
	}
}

/*
void CBaseEnemy::MakeEvasionMovement(float fDt, DirectX::XMFLOAT3 goalposition, DirectX::XMFLOAT3 turnToThis)
{
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 myPos = GetPosition();

	CPathSearch::GetInstance()->PointInTriangle(myPos, m_nMyTriangle);
	CPathSearch::GetInstance()->GetClosestTriangle(goalposition, m_nGoalTriangle);

	//debug
	//CPathSearch::GetInstance()->RenderMyTriangle(m_nMyTriangle);
	//CPathSearch::GetInstance()->RenderMyTriangle(m_nGoalTriangle);

	if (m_nMyTriangle == m_nGoalTriangle)
	{
		//go for it
		velocity = Normalize(goalposition - myPos);
	}
	else
	{
		DirectX::XMFLOAT3 pointToGo = SelectDirection();
		if (CPathSearch::GetInstance()->EqualFloat(pointToGo, m_vDestination) == true)
		{
			pointToGo = goalposition;
		}

		velocity = Normalize(pointToGo - myPos);

		//CDebugRenderer::GetReference().CircleRenderer(pointToGo, DirectX::XMFLOAT2{ 20, 20 });
	}

	//================= TURN =============================
	DirectX::XMFLOAT3 forward = *reinterpret_cast<DirectX::XMFLOAT3*>(MyWorld.GetWorld().m[2]);
	DirectX::XMFLOAT3 turnThis = Normalize(turnToThis - myPos);
	TurnToPoint(fDt, turnThis, m_fTurningSpeed);

	//================ TURN END ===========================

	velocity = Normalize(velocity);

	DirectX::XMFLOAT3 ford = *reinterpret_cast<DirectX::XMFLOAT3*>(MyWorld.GetWorld().m[2]);
	//float speedLimitator = Dot(ford, velocity);
	//speedLimitator = (speedLimitator + 1) / 2;

	float fSpeed = m_fMovementSpeed;

	if (m_pStatusEffects)
	{
		fSpeed = m_pStatusEffects->GetEffectedStat(STAT_EFF_SLOW, fSpeed);
	}
	m_vVelocity = velocity * fSpeed /** speedLimitator * fDt;

	//CHECK BEFORE MOVING
	DirectX::XMFLOAT3 newPosition = GetPosition();
	newPosition = newPosition + m_vVelocity;

	int debugstuff;
	if (CPathSearch::GetInstance()->PointInTriangle(newPosition, debugstuff) == true)
	{
		//move only if is valid
		MyWorld.Local._41 = newPosition.x;
		MyWorld.Local._43 = newPosition.z;
	}
}

*/

DirectX::XMFLOAT3 CBaseEnemy::SelectDirection()
{
	//find the edge
	DirectX::XMFLOAT3 myPos = GetPosition();
	TTriangleEdge myExit;
	CPathSearch::GetInstance()->FindExitEdge(m_nMyTriangle, m_nGoalTriangle, myPos, m_vDestination, myExit);

	//debug
	//CDebugRenderer::GetReference().CircleRenderer(myExit.vertexOne, DirectX::XMFLOAT2{ 20, 20 });
	//CDebugRenderer::GetReference().CircleRenderer(myExit.vertexTwo, DirectX::XMFLOAT2{ 20, 20 });

	DirectX::XMFLOAT3 forward = *reinterpret_cast<DirectX::XMFLOAT3*>(MyWorld.GetWorld().m[2]);
	//ray to segment test
	//if there is collision -> we are facing teh edge

	DirectX::XMFLOAT3 A, B, C, D, intersection, pointToGo;

	A = myExit.vertexOne;
	B = myExit.vertexTwo;
	C = myPos;
	D = (forward * 20000) + C;

	if (LinetoLineCollision(A, B, C, D, intersection) == false)
	{
		//find the closest vertex to my facing pointToGo
		DirectX::XMFLOAT3 s0 = Normalize(myExit.vertexOne - myPos);
		DirectX::XMFLOAT3 s1 = Normalize(myExit.vertexTwo - myPos);

		DirectX::XMFLOAT3 forward = *reinterpret_cast<DirectX::XMFLOAT3*>(MyWorld.GetWorld().m[2]);
		float dot0 = MathHelper::Dot(forward, s0);
		float dot1 = MathHelper::Dot(forward, s1);

		float myRadius = ((TSphere*)GetCollider())->fRadius;

		DirectX::XMFLOAT3 test;

		test = ClosestPointInSegment(myExit.vertexTwo, myExit.vertexOne, myPos);
		if (Length(test - myExit.vertexOne) < myRadius)
		{
			test = myExit.vertexOne + (Normalize(myExit.vertexTwo - myExit.vertexOne)*myRadius * 2.0f);
		}
		else if (Length(test - myExit.vertexTwo) < myRadius)
		{
			test = myExit.vertexTwo + (Normalize(myExit.vertexOne - myExit.vertexTwo)* myRadius * 2.0f);
		} 

		pointToGo = test;
	}
	else
	{
		D = (m_vDestination); 
		if (LinetoLineCollision(A, B, C, D, intersection) == false)
		{
			//find the closest vertex to my facing pointToGo
			DirectX::XMFLOAT3 s0 = Normalize(myExit.vertexOne - myPos);
			DirectX::XMFLOAT3 s1 = Normalize(myExit.vertexTwo - myPos);

			DirectX::XMFLOAT3 forward = *reinterpret_cast<DirectX::XMFLOAT3*>(MyWorld.GetWorld().m[2]);
			float dot0 = MathHelper::Dot(forward, s0);
			float dot1 = MathHelper::Dot(forward, s1);

			float myRadius = ((TSphere*)GetCollider())->fRadius;

			DirectX::XMFLOAT3 test;

			test = ClosestPointInSegment(myExit.vertexTwo, myExit.vertexOne, myPos);
			if (Length(test - myExit.vertexOne) < myRadius)
			{
				test = myExit.vertexOne + (Normalize(myExit.vertexTwo - myExit.vertexOne)*myRadius * 2.0f);
			}
			else if (Length(test - myExit.vertexTwo) < myRadius)
			{
				test = myExit.vertexTwo + (Normalize(myExit.vertexOne - myExit.vertexTwo)* myRadius * 2.0f);
			}

			pointToGo = test;
		}
		else
		{
			pointToGo = m_vDestination;
		}
	}

	return pointToGo;
}

void CBaseEnemy::Update(float fDt)
{
	if (nullptr != m_pPlayer)
	{
		CPlayer* help = (CPlayer*)m_pPlayer;
		if (help->GetCurrentHealth() <= 0)
		{
			m_vDestination = help->GetPosition();
		}
	}

	CLivingEntities::Update(fDt);

	XMStoreFloat4x4(&m_pVariantIndicator->GetLocalMatrix(),
		(/*insert rotation matrix here **/ DirectX::XMMatrixTranslation(m_xOffset.x, m_xOffset.y, m_xOffset.z)) *
		XMLoadFloat4x4(&MyWorld.Local));

	//// Animate
	//if (m_pAnimation)
	//{
	//	// Add the time to the animation to actually animate.
	//	m_cInterpolator.AddTime(fDt);

	//	// Set up the incoming keyframes and process their channels and keyframes
	//	m_cInterpolator.Process();

	//	// Process the skinning on the GPU
	//	m_cDeformer.Process(m_cInterpolator.GetPose());
	//}

	MyWorld.Local = MyWorld.GetWorld();

	if (m_fIdleDelay > 0)
	{
		m_fIdleDelay -= fDt;
		if (m_fIdleDelay < 0)
		{
			m_fIdleDelay = 0;
		}
	}
}

void CBaseEnemy::TakeDamage(float fAmount)
{
	CLivingEntities::TakeDamage(fAmount);
	//if (m_fCurrentHealth <= 0)
	//{
	//	m_bIsAlive = false;
	//	//Make death sound, independent of entity's death animation (which doesn't exist yet)
	//	DirectX::XMFLOAT3 pos;
	//	pos.x = GetPosition().x;
	//	pos.y = GetPosition().y;
	//	pos.z = GetPosition().z;
	//}
}

DirectX::XMFLOAT3 Truncate(DirectX::XMFLOAT3 vector, float maxValue)
{
	if (vector.x < 0 && abs(vector.x) > maxValue)
		vector.x = -maxValue;
	else if (vector.x > 0 && abs(vector.x) > maxValue)
		vector.x = maxValue;

	if (vector.y < 0 && abs(vector.y) > maxValue)
		vector.y = -maxValue;
	else if (vector.y > 0 && abs(vector.y) > maxValue)
		vector.y = maxValue;

	if (vector.z < 0 && abs(vector.z) > maxValue)
		vector.z = -maxValue;
	else if (vector.z > 0 && abs(vector.z) > maxValue)
		vector.z = maxValue;

	return vector;
}

void CBaseEnemy::Attack(void)
{
	Attack(m_fDamage);
}

void CBaseEnemy::ApplyVariant(eEnemyVariant eTier)
{
	SetVariant(eTier);
	if (eTier == eBronze)
	{
		//White, thus normal color
		m_XMColor = DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f };

		m_pVariantIndicator->SetColor(DirectX::XMFLOAT4{ 0.8f, 0.5f, 0.2f, 0.99f });
	}
	else if (eTier == eSilver)
	{
		//Silver
		m_XMColor = m_XMBaseColor = DirectX::XMFLOAT4{ 0.663f, 0.663f, 0.663f, 1.0f };

		m_pVariantIndicator->SetColor(DirectX::XMFLOAT4{ 0.75f, 0.75f, 0.75f, 0.99f });
	}
	else if (eTier == eGold)
	{
		//Gold
		m_XMColor = DirectX::XMFLOAT4{ 1.0f, 0.843f, 0.0f, 1.0f };

		m_pVariantIndicator->SetColor(DirectX::XMFLOAT4{ 1.0f, 0.94f, 0.0f, 0.99f });
	}
	else
	{
		//Black, because why not
		m_XMColor = DirectX::XMFLOAT4{ 0.0f, 0.0f, 0.0f, 1.0f };
	}

	DirectX::XMFLOAT4 c = m_pVariantIndicator->GetColor();
	XMFLOAT3 c2 = XMFLOAT3( c.x, c.y, c.z ) * 3.f;
	m_pVariantIndicator->SetColor(DirectX::XMFLOAT4(c2.x, c2.y, c2.z, c.w));

}


bool CBaseEnemy::LinetoLineCollision(const DirectX::XMFLOAT3 A, const DirectX::XMFLOAT3 B,
	const DirectX::XMFLOAT3 C, const DirectX::XMFLOAT3 D, DirectX::XMFLOAT3 &point)
{
	float a1 = Signed2DTriArea(A, B, D); //Compute winding of abd(+or-)
	float a2 = Signed2DTriArea(A, B, C);//To intersect,must have sign opposite of a1

	//If c and d are on different sides of ab, areas have different signs
	if (a1*a2 < 0.0f) {
		//Compute signs for a and b with respect to segment cd
		float a3 = Signed2DTriArea(C, D, A); //Compute winding of cda(+or-)
		//Since area is constant a1 -a2=a3-a4, or a4=a3+a2-a1
		//	floata4 = Signed2DTriArea(c,d, b); //Must have opposite sign of a3
		float a4 = a3 + a2 - a1;
		//Points a and b on different sides of cd if areas have different signs
		if (a3*a4 < 0.0f)
		{
			float t = a3 / (a3 - a4);
			point = A + t*(B - A);
			return true;
		}
	}
	//Segments not intersecting (or collinear)
	return false;
}

float CBaseEnemy::Signed2DTriArea(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 c)
{
	return(a.x - c.x)*(b.y - c.y) - (a.y - c.y)*(b.x - c.x);
}


bool CBaseEnemy::TurnToPoint(float fDt, DirectX::XMFLOAT3 turnTo, float TurnSpeed)
{
	DirectX::XMFLOAT3 forward = *reinterpret_cast<DirectX::XMFLOAT3*>(MyWorld.GetWorld().m[2]);
	float angle = acos(MathHelper::Dot(forward, Normalize(turnTo)));

	DirectX::XMFLOAT3 myRight = Normalize(Cross(forward, DirectX::XMFLOAT3{ 0, 1, 0 }));
	float Rangle = Dot(myRight, turnTo);

	if (Rangle > 0)
		angle = -angle;

	if (angle < 0.0f)
	{
		if (angle < -TurnSpeed)
			angle = -TurnSpeed;
	}
	else
	{
		if (angle > TurnSpeed)
			angle = TurnSpeed;
	}

	if (angle == angle)
	{
		DirectX::XMMATRIX help;
		help = XMLoadFloat4x4(&MyWorld.GetWorld());
		help = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(angle * fDt * 10), help);
		XMStoreFloat4x4(&MyWorld.Local, help);
		return true;
	}

	return false;
}
