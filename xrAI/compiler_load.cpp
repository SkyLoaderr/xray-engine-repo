#include "stdafx.h"
#include "compiler.h"
#include "xr_func.h"
#include "xr_ini.h"
#include "communicate.h"

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

		for (DWORD l=0; l<Header.light_count; l++) 
		{
			b_light R = Header.lights[l];
			R.direction.normalize_safe();
			if (R.flags & XRLIGHT_LMAPS) 
			{
				R_Light	RL;
				if (R.type==D3DLIGHT_DIRECTIONAL) {
					RL.type			= LT_DIRECT;
				} else {
					RL.type			= LT_POINT;
				}
				RL.amount				=	R.diffuse.magnitude_rgb();
				RL.position.set				(R.position);
				RL.direction.set			(R.direction);
				RL.range				=	R.range*1.2f;
				RL.range2				=	RL.range*RL.range;
				RL.attenuation0			=	R.attenuation0;
				RL.attenuation1			=	R.attenuation1;
				RL.attenuation2			=	R.attenuation2;
				RL.tri[0].set			(0,0,0);
				RL.tri[1].set			(0,0,0);
				RL.tri[2].set			(0,0,0);
				g_lights.push_back		(RL);
			}
		}

	}

	// Init params
	g_params.Init		();
}
