#include "StdAfx.h"
#include "light_db.h"
#include "..\xrLevel.h"

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
			light*		L				= xr_new<light>		();
			L->flags.type				= IRender_Light::POINT;
			L->flags.bStatic			= true;
			L->flags.bShadow			= false;
			L->flags.bActive			= true;
			L->direction.set			(-.5f,-1,0);L->direction.normalize	();
			L->cone						= deg2rad	(75.f);
			F->r						(&L->controller,4);
			F->r						(&Ldata,sizeof(Flight));
			if (Ldata.type==D3DLIGHT_DIRECTIONAL)
			{
				// directional
				v_static.push_back	(NULL);
				xr_delete			(L);
				sun_dir.set			(Ldata.direction);
				sun_dir.y			+= -1.f;
				sun_dir.normalize	();
				sun_color.set		(Ldata.diffuse.r,Ldata.diffuse.g,Ldata.diffuse.b);
			}
			else
			{
				// point
				v_static.push_back	(L);
				L->set_position		(Ldata.position);
				L->set_range		(Ldata.range);
				L->set_color		(Ldata.diffuse);
			}
		}

		F->close			();
	}
	// Msg	("* Layers/Lights : %d / %d",v_static_controls.size(),v_static.size());
}

void			CLight_DB::Unload	()
{
	for	(u32 it=0; it<v_static.size(); it++)	xr_delete(v_static[it]);
	v_static.clear			();
}

light*			CLight_DB::Create	()
{
	light*	L					= xr_new<light>	();
	L->flags.bStatic			= false;
	L->flags.bActive			= false;
	L->flags.bShadow			= true;
	return						L;
}

void			CLight_DB::Destroy	(light* L)
{
	xr_delete	(L);
}

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

void			CLight_DB::Update			()
{
	// Clear selection
	v_selected_shadowed.clear	();
	v_selected_unshadowed.clear	();

	// move point/spot lights
	if (0)
	{
		static float t		=	0;
		t					+=	.01f;
		for (u32 l=0; l<v_static.size(); l++)
		{
			light*  T	= v_static[l];
			if (0==T)	continue;

			Fvector move;
			move.set		(_sin(t),_sin(t),_sin(t));
			move.mul		(.005f);
			T->position.add	(move);
		}
	}
}
