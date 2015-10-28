/***********************************************
* Filename:			HealthBar.h
* Date:      		05/12/2015
* Mod. Date:
* Mod. Initials:
* Author:    		Jagoba Marcos
* Purpose:
************************************************/
#include "Rectangle.h"

#include "../Renderer/Renderer.h"

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
CRectangle::CRectangle(void)
{
	m_fLeft = m_fRight = m_fTop = m_fBottom = 0.f;
}

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
CRectangle::CRectangle(float left, float right, float top, float bottom)
{
	m_fLeft = left;
	m_fRight = right;
	m_fTop = top;
	m_fBottom = bottom;
}

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
bool CRectangle::Contains(vec3f& point)
{
	float top = m_fTop, bottom = m_fBottom;
	float left = m_fLeft, right = m_fRight;
	if (CRenderer::GetReference( ).m_bFullScreen == FALSE)
	{
		top += 30.0f;
		bottom += 30.0f;

		left += 5;
		right += 7;
	}

	return point.x < right && point.x > left &&
		point.y < bottom && point.y > top;
}

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
void CRectangle::MoveCenter(float x, float y)
{
	float width = ComputeWidth( );
	float height = ComputeHeight( );

	m_fLeft = x - width / 2.f;
	m_fRight = x + width / 2.f;
	m_fTop = y - height / 2.f;
	m_fBottom = y + height / 2.f;
}