#include "stdafx.h"
#include "build.h"

void CBuild::SoftenLights()
{
	Status	("Jittering lights...");
	b_light*	L = lights_lmaps.begin();
	R_Light		RL;
	for (;L!=lights_lmaps.end(); L++)
	{
		Progress				(float(L-lights_lmaps.begin())/float(lights_lmaps.size()));

		// generic properties
		RL.diffuse.normalize_rgb	(L->diffuse);
		RL.position.set				(L->position);
		RL.direction.normalize_safe	(L->direction);
		RL.range				=	L->range*1.1f;
		RL.range2				=	RL.range*RL.range;
		RL.attenuation0			=	L->attenuation0;
		RL.attenuation1			=	L->attenuation1;
		RL.attenuation2			=	L->attenuation2;
		RL.energy				=	L->diffuse.magnitude_rgb();
		
		// select destination container
		vector<R_Light>* dest	=	0;
		if (L->flags.bProcedural)	{
			// one of the procedural lights
			lights.push_back		( b_LightLayer() );
			lights.back().original	= L;
			dest					= &(lights.back().lights);		
			RL.diffuse.set			(1,1,1,1);
			RL.energy				= RL.diffuse.magnitude_rgb();
			RL.diffuse.normalize_rgb();
		} else {
			// ambient (fully-static)
			dest					= &(lights.front().lights);
		}

		if (L->type==D3DLIGHT_DIRECTIONAL) 
		{
			RL.type				= LT_DIRECT;
			R_Light	T			= RL;
			Fmatrix				rot_y;

			Fvector				v_top,v_right,v_dir;
			v_top.set			(0,1,0);
			v_dir.set			(RL.direction);
			v_right.crossproduct(v_top,v_dir);
			v_right.normalize	();

			// Build jittered light
			T.energy			= RL.energy/14.f;
			float angle			= deg2rad(g_params.area_dispersion);
			{
				// *** center
				dest->push_back	(T);

				// *** left
				rot_y.rotateY			(3*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotateY			(2*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotateY			(1*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				// *** right
				rot_y.rotateY			(-1*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotateY			(-2*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotateY			(-3*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);
				
				// *** top 
				rot_y.rotation			(v_right, 3*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotation			(v_right, 2*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotation			(v_right, 1*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				// *** bottom
				rot_y.rotation			(v_right,-1*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotation			(v_right,-2*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);

				rot_y.rotation			(v_right,-3*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				dest->push_back	(T);
			}

			// Build area-lights
			if (g_params.area_quality)	
			{
				int		h_count, h_table[3];
				const double (*hemi)[3] = 0;
				if	(g_params.area_quality==1)
				{
					h_count = HEMI1_LIGHTS;
					h_table[0]	= 0;
					h_table[1]	= 1;
					h_table[2]	= 2;
					hemi	= hemi_1;
				} else {
					h_count = HEMI2_LIGHTS;
					h_table[0]	= 0;
					h_table[1]	= 2;
					h_table[2]	= 1;
					hemi	= hemi_2;
				}

				T.energy				= (g_params.area_energy_summary)/float(h_count);
				Fcolor	cLight;
				cLight.normalize_rgb	(L->diffuse);
				
				Fvector		base		= RL.direction;
				base.normalize			();
				for (int i=0; i<h_count; i++)
				{
					T.direction.set			(float(hemi[i][h_table[0]]),float(hemi[i][h_table[1]]),float(hemi[i][h_table[2]]));
					T.direction.invert		();
					T.direction.normalize	();
					
					T.diffuse.set			(g_params.area_color);
					
					dest->push_back	(T);
				}
			}
		} else {
			RL.type			= LT_POINT;
			if (g_params.fuzzy_enable)	{
				// Perform jittering
				R_Light	T		= RL;
				T.energy		= RL.energy/float(g_params.fuzzy_samples);
				
				XRC.RayMode		(RAY_ONLYNEAREST);
				for (int i=0; i<g_params.fuzzy_samples; i++)
				{
					// dir & dist
					Fvector			R;
					R.random_dir	();
					float dist		= ::Random.randF(g_params.fuzzy_min,g_params.fuzzy_max);
					
					// check collision
					XRC.RayPick		(0,&RCAST_Model,RL.position,R,dist);
					if (XRC.GetRayContactCount()) {
						const RAPID::raypick_info* I = XRC.GetMinRayPickInfo();
						dist = I->range;
					}
					
					// calculate point
					T.position.direct		(RL.position,R,dist*.5f);
					dest->push_back	(T);
				}
			} else {
				dest->push_back(RL);
			}
		}
	}
	Msg("* Total light-layers: %d",lights.size());

	Status("Caching faces opacity...");
	for (vecFaceIt I=g_faces.begin(); I!=g_faces.end(); I++) (*I)->CacheOpacity();
}
