#include "stdafx.h"
#pragma hdrstop

#include "HangingLamp.h"
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHangingLamp::CHangingLamp	()
{
}

CHangingLamp::~CHangingLamp	()
{
}

void CHangingLamp::Load		(LPCSTR section)
{
	inherited::Load			(section);
	R_ASSERT				(pVisual&&PKinematics(pVisual));
}

BOOL CHangingLamp::net_Spawn(LPVOID DC)
{
	inherited::net_Spawn	(DC);
	xrSE_HangingLamp* lamp	= (xrSE_HangingLamp*)(DC);
	R_ASSERT				(lamp);
	cNameVisual_set			(lamp->caModel);
	// set bone id
	light_bone_idx			= lamp->spot_bone[0]?PKinematics(pVisual)->LL_BoneID(lamp->spot_bone):-1;
	
//	lamp->animator;
//	lamp->color;
//	lamp->spot_cone_angle;
//	lamp->spot_range;
//	lamp->spot_texture;

	return TRUE;
}

void CHangingLamp::Update	(u32 dt)
{
	inherited::Update		(dt);
}

void CHangingLamp::UpdateCL		()
{
	inherited::UpdateCL		();
}

void CHangingLamp::OnVisible	()
{
	inherited::OnVisible();
}

void CHangingLamp::Hit(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse)
{
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse);
}

