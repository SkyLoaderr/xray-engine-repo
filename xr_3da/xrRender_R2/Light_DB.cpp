#include "StdAfx.h"
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

	// Controllers/Layers
	{
		F				= fs->open_chunk		(fsL_LIGHT_CONTROL);
		xrLIGHT_control	temp;
		string128		c_name;

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
			F->r						(&L->controller,4);
			F->r						(&Ldata,sizeof(Flight));
			if (Ldata.type==D3DLIGHT_DIRECTIONAL)
			{
				// directional
				v_static.push_back	(NULL);
				xr_delete			(L);
				sun_dir.set			(Ldata.direction);
				sun_dir.normalize	();
				sun_color.set		(Ldata.diffuse.r,Ldata.diffuse.g,Ldata.diffuse.b,1.f);
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
