////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_monsters_misc.cpp
//	Created 	: 23.07.2002
//  Modified 	: 23.07.2002
//	Author		: Dmitriy Iassenev
//	Description : Miscellanious routines for monsters
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_MONSTERS_MISC__
#define __XRAY_AI_MONSTERS_MISC__

#include "..\\ai_PathNodes.h"

void vfCreateFastRealisticPath(vector<Fvector> &tpaPoints, DWORD dwStartNode, vector<Fvector> &tpaDeviations, vector<CTravelNode> &tpaPath, bool bUseDeviations = true, float fRoundedDistanceMin = 1.5f, float fRoundedDistanceMax = 3.0f, float fRadiusMin = 0.5f, float fRadiusMax = 1.5f, float fSuitableAngle = PI_DIV_8*.25f, float fSegmentSizeMin = Level().AI.GetHeader().size*.5f, float fSegmentSizeMax = Level().AI.GetHeader().size*2.f);

#endif
