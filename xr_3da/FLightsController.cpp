#include "stdafx.h"
#include "flightscontroller.h"
#include "..\xrLevel.h"

// Disables all lights
void CLightDB_Static::UnselectAll	(void) 
{
	vecI_it it;
	for (it=Selected.begin(); it!=Selected.end(); it++)
		Disable(*it);
	Selected.clear();
}

// for dynamic
void	CLightDB_Static::Select		(Fvector &pos, float fRadius)
{
	// for all dynamic objects we apply not only dynamic but static lights too.
	for (vecI_it it=Selected.begin(); it!=Selected.end(); it++)
	{
		int		num	= *it;
		xrLIGHT &L	= Lights[num];
		if (L.type == D3DLIGHT_DIRECTIONAL)		Enable(num);
		else {
			float	R	= fRadius+L.range;
			if (pos.distance_to_sqr(L.position) < R*R)	Enable(num);
			else										Disable(num);
		}
	}
}
void	CLightDB_Static::Select		(Fvector &pos, float fRadius, vector<xrLIGHT*>& dest)
{
	// for all dynamic objects we apply not only dynamic but static lights too.
	for (vecI_it it=Selected.begin(); it!=Selected.end(); it++)
	{
		xrLIGHT &L	= Lights[*it];
		if (L.type == D3DLIGHT_DIRECTIONAL)				dest.push_back(&L);
		else {
			float	R	= fRadius+L.range;
			if (pos.distance_to_sqr(L.position) < R*R)	dest.push_back(&L);
		}
	}
}

void CLightDB_Static::Load			(IReader *fs) 
{
	IReader* F	= 0;

	// Controlles/Layers
	{
		F				= fs->open_chunk		(fsL_LIGHT_CONTROL);
		xrLIGHT_control	temp;
		string128		c_name;

		while (!F->eof())
		{
			F->r				(temp.name,sizeof(temp.name));
			u32 cnt				= F->r_u32();
			temp.data.resize	(cnt);
			F->r				(temp.data.begin(),cnt*sizeof(u32));
			strconcat			(c_name,"$light$",temp.name);
			temp.dest			= Device.Shader._CreateConstant(c_name);
			Layers.push_back	(temp);
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

IC float spline(float *m, float p0, float p1, float p2, float p3)
{	return p0 * m[0] + p1 * m[1] + p2 * m[2] + p3 * m[3]; }

IC void t_spline(float t, float *m) 
{
	float     t2  = t * t;
	float     t3  = t2 * t;
	
	m[0] = ( 0.5f * ( (-1.0f * t3) + ( 2.0f * t2) + (-1.0f * t) ) );
	m[1] = ( 0.5f * ( ( 3.0f * t3) + (-5.0f * t2) + ( 0.0f * t) + 2.0f ) );
	m[2] = ( 0.5f * ( (-3.0f * t3) + ( 4.0f * t2) + ( 1.0f * t) ) );
	m[3] = ( 0.5f * ( ( 1.0f * t3) + (-1.0f * t2) + ( 0.0f * t) ) );
}
void	CLightDB_Static::add_sector_lights(vector<WORD> &L)
{
	for (vector<WORD>::iterator I=L.begin(); I!=L.end(); I++)
	{
		WORD ID		= *I;
		xrLIGHT&  T	= Lights[ID];
		if (T.dwFrame==Device.dwFrame) continue;
		
		if ((T.type == D3DLIGHT_DIRECTIONAL)||(::Render->View->testSphere_dirty	(T.position, T.range))) 
		{
			Selected.push_back(ID);
			T.dwFrame=Device.dwFrame;
		}
	}
};

