///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains AABB-related code.
 *	\file		IceAABB.cpp
 *	\author		Pierre Terdiman
 *	\date		January, 29, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	AABB class.
 *	\class		AABB
 *	\author		Pierre Terdiman
 *	\version	1.0
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompiled Header
#include "Stdafx.h"
#pragma hdrstop

using namespace Meshmerizer;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Computes the sum of two AABBs.
 *	\param		aabb		[in] the other AABB
 *	\return		Self-Reference
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AABB& AABB::Add(const AABB& aabb)
{
	// Compute _new_ min & max values
	Point Min;	GetMin(Min);
	Point Tmp;	aabb.GetMin(Tmp);
	Min.Min(Tmp);

	Point Max;	GetMax(Max);
	aabb.GetMax(Tmp);
	Max.Max(Tmp);

	// Update this
	SetMinMax(Min, Max);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Checks a box is inside another box.
 *	\param		box		[in] the other AABB
 *	\return		true if current box is inside input box
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool AABB::IsInside(const AABB& box) const
{
	// ### to customize
	if(box.GetMin(0)>GetMin(0))	return false;
	if(box.GetMin(1)>GetMin(1))	return false;
	if(box.GetMin(2)>GetMin(2))	return false;
	if(box.GetMax(0)<GetMax(0))	return false;
	if(box.GetMax(1)<GetMax(1))	return false;
	if(box.GetMax(2)<GetMax(2))	return false;
	return true;
}

