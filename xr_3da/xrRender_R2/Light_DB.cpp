#include "StdAfx.h"
#include "..\_d3d_extensions.h"
#include "..\xrLevel.h"
#include "..\igame_persistent.h"
#include "..\environment.h"
#include "light_db.h"

CLight_DB::CLight_DB()
{
}

CLight_DB::~CLight_DB()
{
}

void CLight_DB::Load			(IReader *fs) 
{
	IReader* F	= 0;

	// Lights itself
	sun			= NULL;
	{
		F				= fs->open_chunk		(fsL_LIGHT_DYNAMIC);

		u32 size		= F->length();
		u32 element		= sizeof(Flight)+4;
		u32 count		= size/element;
		R_ASSERT		(count*element == size);
		v_static.reserve(count);
		for (u32 i=0; i<count; i++) 
		{
			Flight		Ldata;
			light*		L				= Create	();
			L->flags.bStatic			= true;
			L->set_type					(IRender_Light::POINT);
			L->set_shadow				(false);
			u32 controller				= 0;
			F->r						(&controller,4);
			F->r						(&Ldata,sizeof(Flight));
			if (Ldata.type==D3DLIGHT_DIRECTIONAL)
			{
				// directional
				sun					= L;
				L->set_type			(IRender_Light::DIRECT);
				L->set_shadow		(true);
			}
			else
			{
				// point
				v_static.push_back	(L);
				L->set_position		(Ldata.position);
				L->set_range		(Ldata.range);
				L->set_color		(Ldata.diffuse);
				L->set_active		(true);
				R_ASSERT			(L->spatial.sector);
			}
		}

		F->close			();
	}
	R_ASSERT2(sun,"Where is sun?");

	// fake spot
	{
		Fvector	P;			P.set(-140.93f,	-0.20f + 2, -217.43f);
		Fvector	D;			D.set(0,-1,0);
		light*	fake		= Create();
		fake->set_type		(IRender_Light::SPOT);
		fake->set_color		(1,1,1);
		fake->set_cone		(deg2rad(60.f));
		fake->set_direction	(D);
		fake->set_position	(P);
		fake->set_range		(3.f);
		fake->set_active	(true);
	}
}

void			CLight_DB::Unload	()
{
	for	(u32 it=0; it<v_static.size(); it++)	Destroy(v_static[it]);
	v_static.clear			();
	Destroy					(sun);
}

light*			CLight_DB::Create	()
{
	light*	L			= xr_new<light>	();
	L->flags.bStatic	= false;
	L->flags.bActive	= false;
	L->flags.bShadow	= true;
	return				L;
}

void			CLight_DB::Destroy	(light* L)
{
	xr_delete	(L);
}

#if RENDER==R_R1
void			CLight_DB::add_light		(light* L)
{
	if (Device.dwFrame==L->dwFrame)	return;
	L->dwFrame	=	Device.dwFrame;
	if (L->flags.bStatic)			return;	// skip static lighting, 'cause they are in lmaps

	RImplementation.L_Dynamic->add	(L);
}
#endif

#if RENDER==R_R2
void			CLight_DB::add_light		(light* L)
{
	if (Device.dwFrame==L->dwFrame)	return;
	L->dwFrame	=	Device.dwFrame;

	if (L->flags.bShadow)	
	{
		//$$$ nv3x codepath doesn't implement shadowed point lights
		if (RImplementation.b_nv3x && (IRender_Light::POINT==L->flags.type))
			v_selected_unshadowed.push_back	(L);
		else
			v_selected_shadowed.push_back	(L);
	}
	else	v_selected_unshadowed.push_back	(L);
}
#endif

void			CLight_DB::Update			()
{
	// set sun params
	if (sun)
	{
		CEnvDescriptor&	E			= g_pGamePersistent->Environment.CurrentEnv;
		Fvector						P;
		P.mad						(Device.vCameraPosition,E.sun_dir,-500.f);
		sun->set_direction			(E.sun_dir);
		sun->set_color				(E.sun_color.x,E.sun_color.y,E.sun_color.z);
		sun->set_position			(P);
		sun->set_range				(600.f);
	}

	// Clear selection
	v_selected_shadowed.clear	();
	v_selected_unshadowed.clear	();
}
