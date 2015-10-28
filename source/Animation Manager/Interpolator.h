/***********************************************
* Filename:			Interpolator.h
* Date:      		05/12/2015
* Mod. Date:		05/18/2015
* Mod. Initials:	JPM
* Author:    		Justin Mazzola
* Purpose:			This class will be in charge
*					of interpolating keyframes or
*					channels' in a animation/pose.
************************************************/

#ifndef _CINTERPOLATOR_H_
#define _CINTERPOLATOR_H_

#include "..\Core\stdafx.h"
#include "..\Animation Manager\Animation.h"
#include <math.h>

template<typename T>
class CInterpolator
{
public:
	CInterpolator();
	~CInterpolator();

	/*****************************************************************
	* Process():	A blackboxed function to process the current
	*				animation's keyframes and channels.
	*
	* Returns:				void
	*
	* Mod. Date				05/18/2015
	* Mod. Initials:		JPM
	*****************************************************************/
	virtual void Process();

	/*****************************************************************
	* InterpolateMatrix():	A math helper function that does the magic
	*						in interpolating two matrices smoothly with
	*						a given alpha/lambda
	*
	* Ins:					float4x4Prev
	*						float4x4Next
	*						fLambda
	*
	* Outs:					float4x4Out
	*
	* Returns:				void
	*
	* Mod. Date				05/18/2015
	* Mod. Initials:		JPM
	*****************************************************************/
	__forceinline void InterpolateMatrix(/*CAnimation<DirectX::XMFLOAT4X4>::TKey& prevKey,
										 CAnimation<DirectX::XMFLOAT4X4>::TKey& nextKey,
										 float& fLambda,
										 DirectX::XMFLOAT4X4& float4x4Out*/

										 DirectX::XMFLOAT4X4& float4x4Prev,
										 DirectX::XMFLOAT4X4& float4x4Next,
										 float& fLambda,
										 DirectX::XMFLOAT4X4& float4x4Out)
	{
		DirectX::XMMATRIX prevMatrix = DirectX::XMLoadFloat4x4(&float4x4Prev);
		DirectX::XMMATRIX nextMatrix = DirectX::XMLoadFloat4x4(&float4x4Next);

		DirectX::XMVECTOR prevScale, prevRotQuat, prevTrans;
		DirectX::XMVECTOR nextScale, nextRotQuat, nextTrans;

		DirectX::XMMatrixDecompose(&prevScale, &prevRotQuat, &prevTrans, prevMatrix);
		DirectX::XMMatrixDecompose(&nextScale, &nextRotQuat, &nextTrans, nextMatrix);
		DirectX::XMVECTOR scaling = DirectX::XMVectorLerp(prevScale, nextScale, fLambda);
		DirectX::XMVECTOR rotQuat = DirectX::XMQuaternionSlerp(prevRotQuat, nextRotQuat, fLambda);
		DirectX::XMVECTOR trans = DirectX::XMVectorLerp(prevTrans, nextTrans, fLambda);

		/*DirectX::XMVECTOR prevScale, nextScale;

		prevScale = nextScale = DirectX::g_XMOne;
		DirectX::XMVECTOR scaling = DirectX::XMVectorLerp(prevScale, nextScale, fLambda);
		DirectX::XMVECTOR rotQuat = DirectX::XMQuaternionSlerp(DirectX::XMLoadFloat4(&prevKey.m_vQuaternion), DirectX::XMLoadFloat4(&nextKey.m_vQuaternion), fLambda);
		DirectX::XMVECTOR trans = DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&prevKey.m_vPosition), DirectX::XMLoadFloat3(&nextKey.m_vPosition), fLambda);*/
		DirectX::XMVECTOR z = DirectX::g_XMZero;

		DirectX::XMStoreFloat4x4(&float4x4Out, DirectX::XMMatrixAffineTransformation(scaling, z, rotQuat, trans));

		return;
	}

	/***************
	*  Accessors
	***************/

	__forceinline CAnimation<T>* GetAnim() const
	{
		return m_pAnim;
	}

	__forceinline float GetTime() const
	{
		return m_fCurrentTime;
	}

	__forceinline std::vector<T>& GetPose()
	{
		return m_vPose;
	}

	/***************
	* Mutators
	***************/

	__forceinline void SetAnimation(CAnimation<T>* pAnim)
	{
		m_pAnim = pAnim;
	}

	__forceinline void SetTime(float fTime)
	{
		if (m_pAnim != nullptr)
		{
			if (fTime < 0.0f)
			{
				m_fCurrentTime += m_pAnim->GetDuration();
			}
			else if (fTime > m_pAnim->GetDuration())
			{
				m_fCurrentTime -= m_pAnim->GetDuration();
			}
			else
			{
				m_fCurrentTime = fTime;
			}
		}
	}

	__forceinline void AddTime(float fTime)
	{
		SetTime(m_fCurrentTime + fTime);
	}



protected:

	/*****************************************************************
	* ProcessChannels():	Processes the given animation's channels
	*
	* Ins:					vChannels
	*
	* Returns:				void
	*
	* Mod. Date				05/18/2015
	* Mod. Initials:		JPM
	*****************************************************************/
	void ProcessChannels(const std::vector<typename CAnimation<T>::TChannel>& vChannels);

	/*****************************************************************
	* ProcessChannels():	Processes the given animation's keyframes
	*
	* Ins:					vKeyFrames
	*
	* Returns:				void
	*
	* Mod. Date				05/18/2015
	* Mod. Initials:		JPM
	*****************************************************************/
	void ProcessKeyFrames(const std::vector<typename CAnimation<T>::TKeyFrame>& vKeyFrames);

	CAnimation<T>* m_pAnim;
	float m_fCurrentTime;

	std::vector<T> m_vPose;


};

template <typename T>
CInterpolator< T >::CInterpolator()
{
	m_pAnim = NULL;
	m_fCurrentTime = 0.0f;
}

template <typename T>
CInterpolator< T >::~CInterpolator()
{
}

template<typename T>
void CInterpolator<T>::Process()
{
	if (!m_pAnim)
	{
		m_vPose.clear();
		return;
	}

	if (m_pAnim->GetChannels().size() > 0)
	{
		ProcessChannels(m_pAnim->GetChannels());
	}
	else
	{
		ProcessKeyFrames(m_pAnim->GetKeyFrames());
	}
}

template<typename T>
void CInterpolator<T>::ProcessChannels(const std::vector<typename CAnimation<T>::TChannel>& vChannels)
{
	if (m_vPose.size() != vChannels.size())
	{
		m_vPose.clear();
		m_vPose.resize(vChannels.size());
	}

	size_t channel = 0;
	for (; channel < vChannels.size(); channel++)
	{
		int key = 0;
		for (; key < (int)vChannels[channel].m_vKeys.size(); key++)
		{
			if (m_fCurrentTime <= vChannels[channel].m_vKeys[key].m_fTime)
			{
				int prevKey, nextKey;

				nextKey = key;

				if ((key - 1) >= 0)
				{
					prevKey = key - 1;
				}
				else
				{
					prevKey = vChannels[channel].m_vKeys.size() - 1;
				}

				float tweenTime;
				float frameTime;
				float alpha;

				frameTime = m_fCurrentTime - vChannels[channel].m_vKeys[prevKey].m_fTime;
				if (frameTime <= 0)
				{
					frameTime = m_fCurrentTime;
					tweenTime = vChannels[channel].m_vKeys[nextKey].m_fTime;

					alpha = frameTime / tweenTime;
				}
				else
				{
					tweenTime = vChannels[channel].m_vKeys[nextKey].m_fTime - vChannels[channel].m_vKeys[prevKey].m_fTime;
					alpha = (m_fCurrentTime - vChannels[channel].m_vKeys[prevKey].m_fTime) / tweenTime;
				}

				auto prevNode = vChannels[channel].m_vKeys[prevKey].m_Node;
				auto nextNode = vChannels[channel].m_vKeys[nextKey].m_Node;

				/*auto prevNode = vChannels[channel].m_vKeys[prevKey];
				auto nextNode = vChannels[channel].m_vKeys[nextKey];*/

				InterpolateMatrix(prevNode, nextNode, alpha, m_vPose[channel]);

				break;
			}
		}
	}
}

template<typename T>
void CInterpolator<T>::ProcessKeyFrames(const std::vector<typename CAnimation<T>::TKeyFrame>& vKeyframes)
{
	CAnimation<T>::TKeyFrame nextKeyFrame;
	CAnimation<T>::TKeyFrame prevKeyFrame;

	size_t frame = 0;
	for (; frame < vKeyframes.size(); frame++)
	{
		if (m_fCurrentTime <= vKeyframes[frame].m_fTime)
		{
			int nextFrame = frame;
			nextKeyFrame = vKeyframes[nextFrame];

			if ((nextFrame - 1) >= 0)
			{
				prevKeyFrame = vKeyframes[nextFrame - 1];
			}
			else
			{
				prevKeyFrame = vKeyframes[vKeyframes.size() - 1];
			}

			break;
		}
	}

	if (!m_vPose.size())
	{
		m_vPose.resize(nextKeyFrame.m_vNodes.size());
	}

	float tweenTime;
	float frameTime;
	float alpha;

	frameTime = m_fCurrentTime - prevKeyFrame.m_fTime;
	if (frameTime <= 0)
	{
		frameTime = m_fCurrentTime;
		tweenTime = nextKeyFrame.m_fTime;

		alpha = frameTime / tweenTime;
	}
	else
	{
		tweenTime = nextKeyFrame.m_fTime - prevKeyFrame.m_fTime;
		alpha = (m_fCurrentTime - prevKeyFrame.m_fTime) / tweenTime;
	}

	for (size_t i = 0; i < nextKeyFrame.m_vNodes.size(); i++)
	{
		auto prevNode = prevKeyFrame.m_vNodes[i];
		auto nextNode = nextKeyFrame.m_vNodes[i];

		//InterpolateMatrix(prevNode, nextNode, alpha, m_vPose[i]);
	}
}


#endif //_CINTERPOLATOR_H_
