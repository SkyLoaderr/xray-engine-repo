#include "stdafx.h"
#pragma hdrstop

#include "HangingLamp.h"
#include "..\LightAnimLibrary.h"
#include "PhysicsShell.h"
 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHangingLamp::CHangingLamp	()
{
	body					= 0;
	light_bone_idx			= -1;
	lanim					= 0;
	light_render			= ::Render->light_create();
	light_render->set_shadow(true);
}

CHangingLamp::~CHangingLamp	()
{
	::Render->light_destroy	(light_render);
}

void CHangingLamp::Load		(LPCSTR section)
{
	inherited::Load			(section);
}

BOOL CHangingLamp::net_Spawn(LPVOID DC)
{
	xrSE_HangingLamp* lamp	= (xrSE_HangingLamp*)(DC);
	R_ASSERT				(lamp);
	cNameVisual_set			(lamp->visual_name);
	inherited::net_Spawn	(DC);

	// set bone id
	light_bone_idx			= lamp->spot_bone[0]?PKinematics(pVisual)->LL_BoneID(lamp->spot_bone):-1;
	light_render->set_range	(lamp->spot_range);
	light_render->set_color	(color_get_R(lamp->color),color_get_G(lamp->color),color_get_B(lamp->color));
	light_render->set_active(true);
	light_render->set_cone	(lamp->spot_cone_angle);
	light_render->set_texture(lamp->spot_texture[0]?lamp->spot_texture:0);

	R_ASSERT				(pVisual&&PKinematics(pVisual));
	PKinematics(pVisual)->PlayCycle("idle");
	lanim					= LALib.FindItem(lamp->animator);

	CreateBody				();

	setVisible(true);
	setEnabled(true);

	return TRUE;
}

void CHangingLamp::Update	(u32 dt)
{
	inherited::Update		(dt);
}

void CHangingLamp::UpdateCL	()
{
	inherited::UpdateCL		();
}

void CHangingLamp::OnVisible()
{
	inherited::OnVisible	();

	Fmatrix& M = (light_bone_idx>=0)?PKinematics(pVisual)->LL_GetTransform(light_bone_idx):clXFORM();
	light_render->set_direction	(M.k);
	light_render->set_position	(M.c);
	if (lanim){
		int frame;
		u32 clr		= lanim->Calculate(Device.fTimeGlobal,frame); // возвращает в формате BGR
		light_render->set_color(color_get_B(clr),color_get_G(clr),color_get_R(clr));
	}
}

void CHangingLamp::Hit(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse)
{
	inherited::Hit(P,dir,who,element,p_in_object_space,impulse);
	light_render->set_active(false);
}

void CHangingLamp::CreateBody()
{
//	body					= PCreateShell xr_new<CPhysicsShell>();
}
