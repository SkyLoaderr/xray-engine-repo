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

	// Controllers/Layers
	{
		F				= fs->open_chunk		(fsL_LIGHT_CONTROL);
		xrLIGHT_control	temp;

		while (!F->eof())
		{
			F->r						(temp.name,sizeof(temp.name));
			u32 cnt						= F->r_u32();
			temp.data.resize			(cnt);
			F->r						(temp.data.begin(),cnt*sizeof(u32));
			v_static_controls.push_back	(temp);
		}

		F->close		();
	}

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
			L->flags.bStatic			= true;
			L->flags.bShadow			= true;
			L->flags.bActive			= true;
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

				sun_tm_next			=	0;
				sun_dir_base		=	sun_dir;
				sun_dir_0			=	sun_dir;
				sun_dir_1			=	sun_dir;

				sun_color_base		=	sun_color;
				sun_color_0			=	sun_color;
				sun_color_1			=	sun_color;
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
	Msg	("* Layers/Lights : %d / %d",v_static_controls.size(),v_static.size());
}

void CLight_DB::Unload		()
{
	v_static_controls.clear	();
	for	(u32 it=0; it<v_static.size(); it++)	xr_delete(v_static[it]);
	v_static.clear			();
}

light*			CLight_DB::Create	()
{
	light*	L					= xr_new<light>	();
	L->flags.bStatic			= false;
	L->flags.bActive			= false;
	L->flags.bShadow			= true;
	v_dynamic_inactive.insert	(L);
	return						L;
}

void			CLight_DB::Destroy	(light* L)
{
	set<light*>::iterator	it;

	//
	it = v_dynamic_active.find	(L);
	if (it!=v_dynamic_active.end())	
	{
		v_dynamic_active.erase	(it);
		xr_delete		(L);
		return;
	}

	// 
	it = v_dynamic_active.find	(L);
	if (it!=v_dynamic_active.end())	
	{
		v_dynamic_active.erase(it);
		xr_delete	(L);
		return;
	}

	// ???
	xr_delete	(L);
	Msg			("! xrRENDER: unregistered light destroyed");
}

void			CLight_DB::Activate		(light* L)
{
	set<light*>::iterator	it			= v_dynamic_inactive.find	(L);
	R_ASSERT							(it!=v_dynamic_inactive.end());
	v_dynamic_inactive.erase			(it);

	v_dynamic_active.insert				(L);
}
void			CLight_DB::Deactivate	(light* L)
{
	set<light*>::iterator	it			= v_dynamic_active.find	(L);
	R_ASSERT							(it!=v_dynamic_active.end());
	v_dynamic_active.erase				(it);

	v_dynamic_inactive.insert			(L);
}

void			CLight_DB::add_sector_lights(vector<WORD> &L)
{
	for (vector<WORD>::iterator I=L.begin(); I!=L.end(); I++)
	{
		WORD ID		= *I;
		light*  T	= v_static[ID];
		if ((0==T) || (T->dwFrame==Device.dwFrame)) continue;
		
		if (RImplementation.View->testSphere_dirty	(T->sphere.P, T->sphere.R))
		{
			T->dwFrame				=Device.dwFrame;
			if (T->flags.bShadow)	v_selected_shadowed.push_back	(T);
			else					v_selected_unshadowed.push_back	(T);
		}
	}
}

void			CLight_DB::Update()
{
	// Light direction
	u32 t					= Device.TimerAsync();
	if (t>sun_tm_next) 
	{
		sun_tm_base				= t;
		sun_tm_next				= t+5000;

		sun_dir_0.set			(sun_dir_1);
		sun_dir_1.random_dir	();
		sun_dir_1.add			(sun_dir_base);
		sun_dir_1.normalize		();

		sun_color_0.set				(sun_color_1);
		sun_color_1.set				(::Random.randF(15.f),::Random.randF(15.f),::Random.randF(15.f));
		sun_color_1.normalize_safe	();
		sun_color_1.mul				(::Random.randF(.5,1.5f));
	}
	
	float f					= float(t-sun_tm_base)/float(sun_tm_next-sun_tm_base);

	sun_dir.lerp			(sun_dir_0,sun_dir_1,f);
	sun_dir.normalize		();
	sun_dir.lerp			(sun_dir,sun_dir_base,.6f);
	sun_dir.normalize		();

	sun_color.lerp			(sun_color_0,sun_color_1,f);
//	sun_color.lerp			(sun_color,sun_color_base,.5f);
}
