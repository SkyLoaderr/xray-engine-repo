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
		Lights.resize	(count);
		Distance.resize	(count);
		Enabled.resize	(count);

		for (u32 i=0; i<count; i++) 
		{

			F->r						(&Lights[i].dwController,4);
			F->r						(&Lights[i],sizeof(Flight));

			Lights[i].specular.set		(Lights[i].diffuse);
			Lights[i].specular.mul_rgb	(0.2f);
			if (Lights[i].type==D3DLIGHT_DIRECTIONAL)
			{
				Lights[i].position.invert	(Lights[i].direction);
				Lights[i].position.mul		(1000.f);
				Lights[i].range				= 1000.f;
			}
			Enabled[i]	= FALSE;
		}

		F->close		();
	}
	Msg	("* Layers/Lights : %d / %d",Layers.size(),Lights.size());
}

void CLightDB_Static::Unload		(void)
{
	for (u32 i=0; i<Lights.size(); i++) Disable(i);
	for (u32 L=0; L<Layers.size(); L++) Device.Shader._DeleteConstant(Layers[L].dest);

	Layers.clear			();
	Lights.clear			();
	Distance.clear			();
	Enabled.clear			();
}

