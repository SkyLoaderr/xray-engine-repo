#include "stdafx.h"
#include "compiler.h"
#include "communicate.h"
#include "levelgamedef.h"

void xrLoad(LPCSTR name)
{
	// Load CFORM
	string256				N;
	{
		strconcat			(N,name,"level.");
		CVirtualFileStream	FS(N);
		
		CStream* fs			= FS.OpenChunk(fsL_CFORM);
		R_ASSERT			(fs);
		
		hdrCFORM			H;
		fs->Read			(&H,sizeof(hdrCFORM));
		R_ASSERT			(CFORM_CURRENT_VERSION==H.version);
		
		Fvector*	verts	= (Fvector*)fs->Pointer();
		CDB::TRI*	tris	= (CDB::TRI*)(verts+H.vertcount);
		Level.build			( verts, H.vertcount, tris, H.facecount );
		Msg("* Level CFORM: %dK",Level.memory()/1024);
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
		CDB::TRI*	tris	= (CDB::TRI*)(verts+H.vertcount);
		LevelLight.build	( verts, H.vertcount, tris, H.facecount );
		Msg("* Level CFORM(L): %dK",LevelLight.memory()/1024);
	}
	
	// Load emitters
	{
		strconcat			(N,name,"level.game");
		CFileStream			F(N);
		CStream *O = 0;
		if (0!=(O = F.OpenChunk	(AIPOINT_CHUNK)))
		{
			for (int id=0; O->FindChunk(id); id++)
			{
				Fvector		pos;
				O->Rvector	(pos);
				Emitters.push_back(pos);
			}
			O->Close();
		}
	}

	// Load lights
	{
		strconcat					(N,name,"build.prj");

		string32	ID			= BUILD_PROJECT_MARK;
		string32	id;
		CStream*	F			= new CFileStream(N);
		F->Read		(&id,8);
		if (0==strcmp(id,ID))	{
			_DELETE		(F);
			F			= new CCompressedStream(N,ID);
		}
		CStream&				FS	= *F;

		// Version
		DWORD version;
		FS.ReadChunk			(EB_Version,&version);
		R_ASSERT				(XRCL_CURRENT_VERSION==version);

		// Header
		b_params				Params;
		FS.ReadChunk			(EB_Parameters,&Params);

		// Lights (Static)
		{
			F = FS.OpenChunk(EB_Light_static);
			b_light_static	temp;
			DWORD cnt		= F->Length()/sizeof(temp);
			for				(DWORD i=0; i<cnt; i++)
			{
				R_Light		RL;
				F->Read		(&temp,sizeof(temp));
				Flight&		L = temp.data;

				// type
				if			(L.type == D3DLIGHT_DIRECTIONAL)	RL.type	= LT_DIRECT;
				else											RL.type = LT_POINT;

				// generic properties
				RL.position.set				(L.position);
				RL.direction.normalize_safe	(L.direction);
				RL.range				=	L.range*1.1f;
				RL.range2				=	RL.range*RL.range;
				RL.attenuation0			=	L.attenuation0;
				RL.attenuation1			=	L.attenuation1;
				RL.attenuation2			=	L.attenuation2;
				RL.amount				=	L.diffuse.magnitude_rgb	();
				RL.tri[0].set			(0,0,0);
				RL.tri[1].set			(0,0,0);
				RL.tri[2].set			(0,0,0);

				// place into layer
				if (0==temp.controller_ID)	g_lights.push_back		(RL);
			}
			F->Close		();
		}
	}

	// Init params
	g_params.Init		();
}
