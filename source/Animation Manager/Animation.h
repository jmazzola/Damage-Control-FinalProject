/***********************************************
* Filename:			Animation.h
* Date:      		05/12/2015
* Mod. Date:		05/13/2015
* Mod. Initials:	JM
* Author:    		Justin Mazzola
* Purpose:			This templated class will handle all the
*					properties of a Animation object
*					such as Keyframes, Keys and Channels
************************************************/

#ifndef _CANIMATION_H_
#define _CANIMATION_H_

#include "..\Core\stdafx.h"

#include <string>
#include <vector>

template < typename T >
class CAnimation
{
public:

	/***********************************************
	* Author:    		Justin Mazzola
	* Purpose:			A key that holds on to a data node
	*					and a particular time
	************************************************/
	struct TKey
	{
		float m_fTime;
		T m_Node;

		//quaternion & position TODO: Beta
		/*DirectX::XMFLOAT4 m_vQuaternion;
		DirectX::XMFLOAT3 m_vPosition;*/
	};

	/***********************************************
	* Author:    		Justin Mazzola
	* Purpose:			A list of keys
	************************************************/
	struct TChannel
	{
		std::vector<TKey> m_vKeys;
	};

	/***********************************************
	* Author:    		Justin Mazzola
	* Purpose:			A group of nodes played at a 
	*					particular time.
	************************************************/
	struct TKeyFrame
	{
		float m_fTime;
		std::vector<T> m_vNodes;
	};

	CAnimation();
	~CAnimation();

	/***************
	* Accessors
	****************/

	std::string& GetName() const;
	float GetDuration() const;
	std::vector<TChannel> GetChannels() const;
	std::vector<TKeyFrame> GetKeyFrames() const;


	std::string m_szName;
	std::vector<TChannel> m_vChannels;
	std::vector<TKeyFrame> m_vKeyframes;
	float m_fDuration;

};


template<typename T>
CAnimation<T>::CAnimation()
{

}

template<typename T>
CAnimation<T>::~CAnimation()
{

}

template <typename T>
std::string& CAnimation<T>::GetName() const
{
	return m_szName;
}

template <typename T>
float CAnimation<T>::GetDuration() const
{
	/*if (m_vChannels.size() > 0)
	{
		return m_vChannels.back().m_vKeys.back().m_fTime;
	}
	else
	{
		return m_vKeyframes.back().m_fTime;
	}*/

	return m_fDuration;
}

template<typename T>
std::vector<typename CAnimation<T>::TChannel> CAnimation<T>::GetChannels() const
{
	return m_vChannels;
}

template <typename T>
std::vector< typename CAnimation<T>::TKeyFrame> CAnimation<T>::GetKeyFrames() const
{
	return m_vKeyframes;
}

#endif // _CANIMATION_H_

