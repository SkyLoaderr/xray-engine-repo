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
		VERIFY			(count*element == size);
		v_static.reserve(count);
		for (u32 i=0; i<count; i++) 
		{
			Flight		Ldata;
			light*		L				= Create	();
			L->flags.bStatic			= true;
			L->set_type					(IRender_Light::POINT);

#if RENDER==R_R1
			L->set_shadow				(false);
#else
			L->set_shadow				(true);
#endif
			u32 controller				= 0;
			F->r						(&controller,4);
			F->r						(&Ldata,sizeof(Flight));
			if (Ldata.type==D3DLIGHT_DIRECTIONAL)	{
				Fvector tmp_R;		tmp_R.set(1,0,0);

				// directional (base)
				sun_base			= L;
				L->set_type			(IRender_Light::DIRECT);
				L->set_shadow		(true);
				L->set_rotation		(Ldata.direction,tmp_R);
				
				// copy to env-sun
				sun		=	L		= Create();
				L->flags.bStatic	= true;
				L->set_type			(IRender_Light::DIRECT);
				L->set_shadow		(true);
				L->set_rotation		(Ldata.direction,tmp_R);
			}
			else
			{
				// point
				v_static.push_back	(L);
				L->set_position		(Ldata.position		);
				L->set_range		(Ldata.range		);
				L->set_color		(Ldata.diffuse		);
				L->set_active		(true				);
				R_ASSERT			(L->spatial.sector	);
			}
		}

		F->close			();
	}
	R_ASSERT2(sun,"Where is sun?");

	// fake spot
	/*
	if (0)
	{
		Fvector	P;			P.set(-5.58f,	-0.00f + 2, -3.63f);
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
	*/
}

void			CLight_DB::Unload	()
{
	for	(u32 it=0; it<v_static.size(); it++)	Destroy(v_static[it]);
	v_static.clear			();
	Destroy					(sun);
	Destroy					(sun_base);
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
	xr_delete			(L);
}

#if RENDER==R_R1
void			CLight_DB::add_light		(light* L)
{
	if (Device.dwFrame==L->frame_render)	return;
	L->frame_render							=	Device.dwFrame;
	if (L->flags.bStatic)					return;	// skip static lighting, 'cause they are in lmaps

	RImplementation.L_Dynamic->add	(L);
}
#endif

#if RENDER==R_R2
void			CLight_DB::add_light		(light* L)
{
	if (Device.dwFrame==L->frame_render)	return;
	L->frame_render							=	Device.dwFrame;
	if (RImplementation.b_noshadows)		L->flags.bShadow	= FALSE;
	L->export								(package);
}
#endif

void			CLight_DB::Update			()
{
	// set sun params
	if (sun)
	{
		CEnvDescriptor&	E			= g_pGamePersistent->Environment.CurrentEnv;
		Fvector						P,Pbase,Dbase;
		Dbase.set(0,-1,0).add		(E.sun_dir).normalize	();
		Pbase.mad					(Device.vCameraPosition,Dbase,		-500.f);
		P.mad						(Device.vCameraPosition,E.sun_dir,	-500.f);
		sun->set_rotation			(E.sun_dir,sun->right);
		sun_base->set_rotation		(Dbase,sun->right);
		sun->set_color				(E.sun_color.x,E.sun_color.y,E.sun_color.z);
		sun_base->set_color			(E.sun_color.x,E.sun_color.y,E.sun_color.z);
		sun->set_position			(P);
		sun_base->set_position		(P);
		sun->set_range				(600.f);
		sun_base->set_range			(600.f);
	}

	// Clear selection
	package.clear	();
}
