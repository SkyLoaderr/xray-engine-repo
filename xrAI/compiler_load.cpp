#include "stdafx.h"
#include "compiler.h"
#include "xr_func.h"
#include "xr_ini.h"
#include "communicate.h"

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

void xrLoad(LPCSTR name)
{
	// Load CFORM
	FILE_NAME			N;
	{
		strconcat			(N,name,"level.");
		CVirtualFileStream	FS(N);
		
		CStream* fs			= FS.OpenChunk(fsL_CFORM);
		R_ASSERT			(fs);
		
		hdrCFORM			H;
		fs->Read			(&H,sizeof(hdrCFORM));
		R_ASSERT			(CFORM_CURRENT_VERSION==H.version);
		
		Fvector*	verts	= (Fvector*)fs->Pointer();
		RAPID::tri*	tris	= (RAPID::tri*)(verts+H.vertcount);
		Level.BuildModel	( verts, H.vertcount, tris, H.facecount );
		Msg("* Level CFORM: %dK",Level.MemoryUsage()/1024);
		fs->Close			();
		
		LevelBB.set			(H.aabb);
	}

	// Load CFORM ("lighting")
	{
		strconcat			(N,name,"build.cform");
		CVirtualFileStream	FS(N);
		
		hdrCFORM			H;
		FS.Read				(&H,sizeof(hdrCFORM));
		R_ASSERT			(CFORM_CURRENT_VERSION==H.version);
		
		Fvector*	verts	= (Fvector*)FS.Pointer();
		RAPID::tri*	tris	= (RAPID::tri*)(verts+H.vertcount);
		LevelLight.BuildModel	( verts, H.vertcount, tris, H.facecount );
		Msg("* Level CFORM(L-Model): %dK",LevelLight.MemoryUsage()/1024);
	}
	
	// Load emitters
	{
		strconcat			(N,name,"level.ltx");
		CInifile			F(N);
		CInifile::Sect& S	= F.ReadSection("respawn_point");
		for (CInifile::SectIt I=S.begin(); I!=S.end(); I++) {
			Fvector		pos;
			const char*	sVal = I->second;
			sscanf(sVal,"%f,%f,%f",&pos.x,&pos.y,&pos.z);
			Emitters.push_back(pos);
		}
	}

	// Load lights
	{
		strconcat			(N,name,"build.prj");

		CVirtualFileStream	FS(N);
		void*				data = FS.Pointer();
		
		b_transfer	Header		= *((b_transfer *) data);
		R_ASSERT(XRCL_CURRENT_VERSION==Header._version);
		Header.lights			= (b_light*)	(DWORD(data)+DWORD(Header.lights));

		// Hemi setup
		int			h_count, h_table[3];
		const double (*hemi)[3] = 0;
		h_count		= HEMI1_LIGHTS;
		h_table[0]	= 0;
		h_table[1]	= 1;
		h_table[2]	= 2;
		hemi		= hemi_1;
		
		// Cycle thru
		for (DWORD l=0; l<Header.light_count; l++) 
		{
			b_light R = Header.lights[l];
			if (R.flags & XRLIGHT_LMAPS) 
			{
				R_Light	RL;
				if (R.type==D3DLIGHT_DIRECTIONAL) {
					RL.type				= LT_DIRECT;
				} else {
					RL.type				= LT_POINT;
				}
				RL.amount				=	R.diffuse.magnitude_rgb();
				RL.position.set				(R.position);
				RL.direction.normalize_safe	(R.direction);
				RL.range				=	R.range*1.2f;
				RL.range2				=	RL.range*RL.range;
				RL.attenuation0			=	R.attenuation0;
				RL.attenuation1			=	R.attenuation1;
				RL.attenuation2			=	R.attenuation2;
				RL.tri[0].set			(0,0,0);
				RL.tri[1].set			(0,0,0);
				RL.tri[2].set			(0,0,0);
				g_lights.push_back		(RL);

				if (RL.type==LT_DIRECT)	
				{
					R_Light	T			=	RL;
					T.amount			=	Header.params.areaDark.magnitude_rgb()*(Header.params.area_energy_summary)/float(h_count);
					for (int i=0; i<h_count; i++)
					{
						T.direction.set			(float(hemi[i][0]),float(hemi[i][1]),float(hemi[i][2]));
						T.direction.invert		();
						T.direction.normalize	();
						g_lights.push_back		(T);
					}
				}
			}
		}
	}

	// Init params
	g_params.Init		();
}
