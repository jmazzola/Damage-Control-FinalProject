/***********************************************
* Filename:			AnimationBlender.h
* Date:      		05/12/2015
* Mod. Date:		05/15/2015
* Mod. Initials:	JPM
* Author:    		Justin Mazzola
* Purpose:			This class will be in charge
*					of smoothly/non-smoothly
*					blending/transitioning into
*					and from animations
************************************************/

#ifndef _CANIMATIONBLENDER_H_
#define _CANIMATIONBLENDER_H_

#include "..\Core\stdafx.h"
#include "..\Animation Manager\Animation.h"
#include "..\Animation Manager\Interpolator.h"
#include "..\Animation Manager\Deformer.h"

#include <algorithm>

template<typename T>
class CAnimationBlender
{
public:

	CAnimationBlender();
	~CAnimationBlender();

	/*****************************************************************
	* SnapTo():			Non-smoothly switch the current animation to the
	*					given animation giving a 'snapping' look.
	*
	* Ins:				pAnimation
	*
	* Returns:			void
	*
	* Mod. Date			05/18/2015
	* Mod. Initials: JPM
	*****************************************************************/
	void SnapTo(CAnimation<T>* pAnimation);

	/*****************************************************************
	* TransitionTo():			Smoothly transition the current animation to the
	*					given animation.
	*
	* Ins:				pAnimation
	*					fTransTime
	*
	* Returns:			void
	*
	* Mod. Date			05/18/2015
	* Mod. Initials: JPM
	*****************************************************************/
	void TransitionTo(CAnimation<T>* pAnimation, 
					  float fTransTime);

	/*****************************************************************
	* Process():		Blackbox function to handle all processing of
	*					interpolation and pose management in the blending
	*
	* Ins:				fDt
	*
	* Returns:			void
	*
	* Mod. Date			05/18/2015
	* Mod. Initials: JPM
	*****************************************************************/
	void Process(float fDt);

	/***************
	*  Accessors
	***************/

	__forceinline std::vector<T>& GetPose()
	{
		return m_vPose;
	}

	__forceinline bool IsBlending() const
	{
		return (m_fTransTime > 0.0f);
	}

	__forceinline CInterpolator<T> GetInterpolator(int iNum)
	{
		return m_Interpolators[iNum];
	}

private:

	CInterpolator<T> m_Interpolators[2];
	std::vector<T> m_vPose;

	unsigned int m_unCurrentInterpolator;

	float m_fTransTime;
	float m_fElapsedTransTime;

};

template <typename T>
CAnimationBlender<T>::CAnimationBlender()
{
	m_unCurrentInterpolator = 0;
	m_fTransTime = 0.0f;
	m_fElapsedTransTime = 0.0f;
}


template <typename T>
CAnimationBlender<T>::~CAnimationBlender()
{
}

template<typename T>
void CAnimationBlender<T>::SnapTo(CAnimation<T>* pAnimation)
{
	m_fElapsedTransTime = m_fTransTime = 0.0f;
	m_Interpolators[m_unCurrentInterpolator].SetAnimation(pAnimation);
}

template<typename T>
void CAnimationBlender<T>::TransitionTo(CAnimation<T>* pAnimation, float fTransTime)
{
	unsigned int nextInterp;

	if (!m_Interpolators[m_unCurrentInterpolator].GetAnim())
	{
		SnapTo(pAnimation);
	}
	else
	{
		nextInterp = m_unCurrentInterpolator ^ 1;
		m_fTransTime = fTransTime;
		m_fElapsedTransTime = 0.0f;

		m_Interpolators[nextInterp].SetAnimation(pAnimation);
	}
}

template<typename T>
void CAnimationBlender<T>::Process(float fDt)
{
	/*if (m_Interpolators[0]== NULL) return;
	if (m_Interpolators[1] == NULL) return;*/

	int nextInterp = -1;

	if (m_unCurrentInterpolator == 0)
	{
		nextInterp = 1;
	}
	else if (m_unCurrentInterpolator == 1)
	{
		nextInterp = 0;
	}

	if (IsBlending())
	{
		m_fElapsedTransTime += fDt;

		if (m_fElapsedTransTime >= m_fTransTime)
		{
			m_unCurrentInterpolator = nextInterp;
			m_fTransTime = m_fElapsedTransTime = 0;
		}
	}

	m_Interpolators[m_unCurrentInterpolator].AddTime(fDt);
	m_Interpolators[m_unCurrentInterpolator].Process();

	size_t poseSize = m_Interpolators[m_unCurrentInterpolator].GetPose().size();
	if (m_vPose.size() != poseSize)
	{
		m_vPose.clear();
		m_vPose.resize(poseSize);
	}

	if (m_fTransTime > 0.0f)
	{
		m_Interpolators[nextInterp].AddTime(fDt);
		m_Interpolators[nextInterp].Process();

		for (size_t i = 0; i < poseSize; i++)
		{
			m_Interpolators->InterpolateMatrix(
				m_Interpolators[m_unCurrentInterpolator].GetPose()[i],
				m_Interpolators[nextInterp].GetPose()[i], m_fElapsedTransTime,
				m_vPose[i]);
		}

		/*for (size_t channel = 0; channel < m_Interpolators[m_unCurrentInterpolator].GetAnim()->GetChannels().size(); channel++)
		{
			for (size_t key = 0; key < m_Interpolators[m_unCurrentInterpolator].GetAnim()->GetChannels()[channel].m_vKeys.size(); key++)
			{
				auto animation = m_Interpolators[m_unCurrentInterpolator].GetAnim();
				auto prevKey = m_Interpolators[m_unCurrentInterpolator].GetAnim()->GetChannels()[channel].m_vKeys[key];
				auto nextKey = m_Interpolators[nextInterp].GetAnim()->GetChannels()[channel].m_vKeys[key];
				m_Interpolators->InterpolateMatrix(
					prevKey,
					nextKey,
					m_fElapsedTransTime,
					m_vPose[channel]);
			}
		}*/
	}
	else
	{
		for (size_t i = 0; i < poseSize; i++)
		{
			m_vPose[i] = (m_Interpolators[m_unCurrentInterpolator].GetPose()[i]);
		}
	}

}

#endif // _CANIMATIONBLENDER_H_
