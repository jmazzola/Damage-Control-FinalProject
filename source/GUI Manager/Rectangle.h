#ifndef _RECTANGLE_H
#define _RECTANGLE_H

#include "..\Core\stdafx.h"

#include "..\Math\vec3.h"

class CRectangle
{
public:
	float m_fLeft;
	float m_fRight;
	float m_fTop;
	float m_fBottom;
	/*****************************************************************
	* CRectangle(): Inilitizes a rectangle to zero
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
	CRectangle(void);

	/*****************************************************************
	* CRectangle(): Inilitizes a rectangle with the provided parameters
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
	CRectangle(float left, float right, float top, float bottom);

	// Computes the width of the rectangle
	float ComputeWidth(void) const { return m_fRight - m_fLeft; }
	// Computes the height of the rectangle
	float ComputeHeight(void) const { return m_fBottom - m_fTop; }	

	/*****************************************************************
	* Contains(): Returns true if the rectangle contains the point
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
	bool Contains(vec3f& point);

	/*****************************************************************
	* MoveCenter(): Centers the rectangle on 'x' and 'y'
	*
	* Ins: x - the x coordinate to place the rectangle
	*	   y - the y coordinate to place the rectangle
	*
	* Outs:
	*
	* Returns:
	*
	* Mod. Date:		      05/15/2015
	* Mod. Initials:
	*****************************************************************/
	void MoveCenter(float x, float y);

	/***************
	*  Accessors
	***************/

	// returns the left x-coordinate of the rectangle
	float GetLeft(void) const { return m_fLeft; }
	// returns the right x-coordinate of the rectangle
	float GetRight(void) const { return m_fRight; }
	// returns the upper y-coordinate of the rectangle
	float GetTop(void) const { return m_fTop; }
	// returns the lower y-coordinate of the rectangle
	float GetBottom(void) const { return m_fBottom; }

	/***************
	* Mutators
	***************/

	// sets the width of the rectangle
	void SetWidth(float width) { m_fRight = m_fLeft + width; }

	// sets the height of the rectangle
	void SetHeight(float height) { m_fBottom = m_fTop + height; }
};

#endif