#include "stdafx.h"
#include "build.h"

#define HEMI1_LIGHTS	26
#define HEMI2_LIGHTS	91

const double hemi_1[HEMI1_LIGHTS][3] = 
{
	{0.00000,	1.00000,	0.00000	},
	{0.52573,	0.85065,	0.00000	},
	{0.16246,	0.85065,	0.50000	},
	{-0.42533,	0.85065,	0.30902	},
	{-0.42533,	0.85065,	-0.30902},
	{0.16246,	0.85065,	-0.50000},
	{0.89443,	0.44721,	0.00000	},
	{0.27639,	0.44721,	0.85065	},
	{-0.72361,	0.44721,	0.52573	},
	{-0.72361,	0.44721,	-0.52573},
	{0.27639,	0.44721,	-0.85065},
	{0.68819,	0.52573,	0.50000	},
	{-0.26287,	0.52573,	0.80902	},
	{-0.85065,	0.52573,	-0.00000},
	{-0.26287,	0.52573,	-0.80902},
	{0.68819,	0.52573,	-0.50000},
	{0.95106,	0.00000,	0.30902	},
	{0.58779,	0.00000,	0.80902	},
	{-0.00000,	0.00000,	1.00000	},
	{-0.58779,	0.00000,	0.80902	},
	{-0.95106,	0.00000,	0.30902	},
	{-0.95106,	0.00000,	-0.30902},
	{-0.58779,	0.00000,	-0.80902},
	{0.00000,	0.00000,	-1.00000},
	{0.58779,	0.00000,	-0.80902},
	{0.95106,	0.00000,	-0.30902}
};

const double hemi_2[HEMI2_LIGHTS][3] = 
{
	{0.00000,	0.00000,	1.00000},
	{0.52573,	0.00000,	0.85065},
	{0.16246,	0.50000,	0.85065},
	{-0.42533,	0.30902,	0.85065},
	{-0.42533,	-0.30902,	0.85065},
	{0.16246,	-0.50000,	0.85065},
	{0.89443,	0.00000,	0.44721},
	{0.27639,	0.85065,	0.44721},
	{-0.72361,	0.52573,	0.44721},
	{-0.72361,	-0.52573,	0.44721},
	{0.27639,	-0.85065,	0.44721},
	{0.68819,	0.50000	,	0.52573},
	{-0.26287,	0.80902	,	0.52573},
	{-0.85065,	-0.00000,	0.52573},
	{-0.26287,	-0.80902,	0.52573},
	{0.68819,	-0.50000,	0.52573},
	{0.95106,	0.30902	,	0.00000},
	{0.58779,	0.80902	,	0.00000},
	{-0.00000,	1.00000	,	0.00000},
	{-0.58779,	0.80902	,	0.00000},
	{-0.95106,	0.30902	,	0.00000},
	{-0.95106,	-0.30902,	0.00000},
	{-0.58779,	-0.80902,	0.00000},
	{0.00000,	-1.00000,	0.00000},
	{0.58779,	-0.80902,	0.00000},
	{0.95106,	-0.30902,	0.00000},
	{0.27327,	0.00000	,	0.96194},
	{0.08444,	0.25989	,	0.96194},
	{-0.22108,	0.16062	,	0.96194},
	{-0.22108,	-0.16062,	0.96194},
	{0.08444,	-0.25989,	0.96194},
	{0.36180,	0.26287	,	0.89443},
	{-0.13820,	0.42533	,	0.89443},
	{-0.44721,	-0.00000,	0.89443},
	{-0.13820,	-0.42533,	0.89443},
	{0.36180,	-0.26287,	0.89443},
	{0.73818,	0.00000	,	0.67461},
	{0.22811,	0.70205	,	0.67461},
	{-0.59720,	0.43389	,	0.67461},
	{-0.59720,	-0.43389,	0.67461},
	{0.22811,	-0.70205,	0.67461},
	{0.63820,	0.26287	,	0.72361},
	{-0.05279,	0.68819	,	0.72361},
	{-0.67082,	0.16246	,	0.72361},
	{-0.36180,	-0.58779,	0.72361},
	{0.44721,	-0.52573,	0.72361},
	{0.63820,	-0.26287,	0.72361},
	{0.44721,	0.52573	,	0.72361},
	{-0.36180,	0.58779	,	0.72361},
	{-0.67082,	-0.16246,	0.72361},
	{-0.05279,	-0.68819,	0.72361},
	{0.82262,	0.25989	,	0.50572},
	{0.00703,	0.86267	,	0.50572},
	{-0.81827,	0.27327	,	0.50572},
	{-0.51275,	-0.69378,	0.50572},
	{0.50138,	-0.70205,	0.50572},
	{0.82262,	-0.25989,	0.50572},
	{0.50138,	0.70205	,	0.50572},
	{-0.51275,	0.69378	,	0.50572},
	{-0.81827,	-0.27327,	0.50572},
	{0.00703,	-0.86267,	0.50572},
	{0.95925,	0.16062	,	0.23245},
	{0.86180,	0.42533	,	0.27639},
	{0.67082,	0.68819	,	0.27639},
	{0.44919,	0.86267	,	0.23245},
	{0.14366,	0.96194	,	0.23245},
	{-0.13820,	0.95106	,	0.27639},
	{-0.44721,	0.85065	,	0.27639},
	{-0.68164,	0.69378	,	0.23245},
	{-0.87046,	0.43389	,	0.23245},
	{-0.94721,	0.16246	,	0.27639},
	{-0.94721,	-0.16246,	0.27639},
	{-0.87046,	-0.43389,	0.23245},
	{-0.68164,	-0.69378,	0.23245},
	{-0.44721,	-0.85065,	0.27639},
	{-0.13820,	-0.95106,	0.27639},
	{0.14366,	-0.96194,	0.23245},
	{0.44919,	-0.86267,	0.23245},
	{0.67082,	-0.68819,	0.27639},
	{0.86180,	-0.42533,	0.27639},
	{0.95925,	-0.16062,	0.23245},
	{0.80902,	0.58779	,	0.00000},
	{0.30902,	0.95106	,	0.00000},
	{-0.30902,	0.95106	,	0.00000},
	{-0.80902,	0.58779	,	0.00000},
	{-1.00000,	-0.00000,	0.00000},
	{-0.80902,	-0.58779,	0.00000},
	{-0.30902,	-0.95106,	0.00000},
	{0.30902,	-0.95106,	0.00000},
	{0.80902,	-0.58779,	0.00000},
	{1.00000,	0.00000	,	0.00000}
};
void CBuild::SoftenLights()
{
	Status	("Jittering lights...");
	b_light*	L = lights_lmaps.begin();
	R_Light		RL;
	for (;L!=lights_lmaps.end(); L++)
	{
		Progress				(float(L-lights_lmaps.begin())/float(lights_lmaps.size()));

		RL.diffuse.normalize_rgb	(L->diffuse);
		RL.position.set				(L->position);
		RL.direction.set			(L->direction);
		RL.range				=	L->range*1.2f;
		RL.range2				=	RL.range*RL.range;
		RL.attenuation0			=	L->attenuation0;
		RL.attenuation1			=	L->attenuation1;
		RL.attenuation2			=	L->attenuation2;
		RL.energy				=	L->diffuse.magnitude_rgb();

		if (L->type==D3DLIGHT_DIRECTIONAL) 
		{
			RL.type			= LT_DIRECT;
			R_Light	T		= RL;
			Fmatrix			rot_y;

			Fvector			v_top,v_right,v_dir;
			v_top.set			(0,1,0);
			v_dir.set			(RL.direction);
			v_right.crossproduct(v_top,v_dir);
			v_right.normalize	();

			// Build jittered light
			T.energy		= RL.energy/14.f;
			float angle		= deg2rad(g_params.area_dispersion);
			{
				// *** center
				lights_soften.push_back	(T);

				// *** left
				/*
				rot_y.rotateY			(4*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				lights_soften.push_back	(T);
				*/

				rot_y.rotateY			(3*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				lights_soften.push_back	(T);

				rot_y.rotateY			(2*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				lights_soften.push_back	(T);

				rot_y.rotateY			(1*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				lights_soften.push_back	(T);

				// *** right
				rot_y.rotateY			(-1*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				lights_soften.push_back	(T);

				rot_y.rotateY			(-2*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				lights_soften.push_back	(T);

				rot_y.rotateY			(-3*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				lights_soften.push_back	(T);

				/*
				rot_y.rotateY			(-4*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				lights_soften.push_back	(T);
				*/
				
				// *** top 
				rot_y.rotation			(v_right, 3*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				lights_soften.push_back	(T);

				rot_y.rotation			(v_right, 2*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				lights_soften.push_back	(T);

				rot_y.rotation			(v_right, 1*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				lights_soften.push_back	(T);

				// *** bottom
				rot_y.rotation			(v_right,-1*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				lights_soften.push_back	(T);

				rot_y.rotation			(v_right,-2*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				lights_soften.push_back	(T);

				rot_y.rotation			(v_right,-3*angle/4);
				rot_y.transform_dir		(T.direction,RL.direction);
				lights_soften.push_back	(T);
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
					
					T.diffuse.set			(g_params.areaDark);
					
					/*
					float dot				= base.dotproduct(T.direction);
					float factor			= (dot+1)/2;
					
					Fcolor					cDirect;
					cDirect.mul_rgb			(cLight,factor);

					T.diffuse.r				= cDirect.r;
					T.diffuse.g				= cDirect.g;
					T.diffuse.b				= cDirect.b;
					*/
					
					lights_soften.push_back	(T);
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
					lights_soften.push_back	(T);
				}
			} else {
				lights_soften.push_back(RL);
			}
		}
	}
	Msg("* Total lights: %d",lights_soften.size());

	Status("Caching faces opacity...");
	for (vecFaceIt I=g_faces.begin(); I!=g_faces.end(); I++) (*I)->CacheOpacity();
}
