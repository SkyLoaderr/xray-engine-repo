#include "stdafx.h"
#include "compiler.h"
#include "communicate.h"
#include "levelgamedef.h"
#include "level_graph.h"
#include "AIMapExport.h"

IC	const Fvector vertex_position(const CLevelGraph::CPosition &Psrc, const Fbox &bb, const SAIParams &params)
{
	Fvector				Pdest;
	int	x,z, row_length;
	row_length			= iFloor((bb.max.z - bb.min.z)/params.fPatchSize + EPS_L + 1.5f);
	x					= Psrc.xz() / row_length;
	z					= Psrc.xz() % row_length;
	Pdest.x =			float(x)*params.fPatchSize + bb.min.x;
	Pdest.y =			(float(Psrc.y())/65535)*(bb.max.y-bb.min.y) + bb.min.y;
	Pdest.z =			float(z)*params.fPatchSize + bb.min.z;
	return				(Pdest);
}

struct CNodePositionConverter {
	IC		CNodePositionConverter(const SNodePositionOld &Psrc, hdrNODES &m_header, NodePosition &np);
};

IC CNodePositionConverter::CNodePositionConverter(const SNodePositionOld &Psrc, hdrNODES &m_header, NodePosition &np)
{
	Fvector		Pdest;
	Pdest.x		= float(Psrc.x)*m_header.size;
	Pdest.y		= (float(Psrc.y)/65535)*m_header.size_y + m_header.aabb.min.y;
	Pdest.z		= float(Psrc.z)*m_header.size;
	CNodePositionCompressor(np,Pdest,m_header);
	np.y		(Psrc.y);
}

void xrLoad(LPCSTR name)
{
	// Load CFORM
	string256				N;
	{
		strconcat			(N,name,"level.cform");
		IReader				*fs = FS.r_open(N);
		R_ASSERT			(fs);
		
		hdrCFORM			H;
		fs->r				(&H,sizeof(hdrCFORM));
		R_ASSERT			(CFORM_CURRENT_VERSION==H.version);
		
		Fvector*	verts	= (Fvector*)fs->pointer();
		CDB::TRI*	tris	= (CDB::TRI*)(verts+H.vertcount);
		Level.build			( verts, H.vertcount, tris, H.facecount );
		Msg("* Level CFORM: %dK",Level.memory()/1024);
		fs->close			();

		LevelBB.set			(H.aabb);
	}

	// Load CFORM ("lighting")
	{
		strconcat			(N,name,"build.cform");
		IReader				*F = FS.r_open(N);
		
		hdrCFORM			H;
		IReader				*fs	= F->open_chunk(0);
		fs->r				(&H,sizeof(hdrCFORM));
		R_ASSERT			(CFORM_CURRENT_VERSION==H.version);
		
		//fs					= F->open_chunk(1);
		Fvector*	verts	= (Fvector*)fs->pointer();
		CDB::TRI*	tris	= (CDB::TRI*)(verts+H.vertcount);
		LevelLight.build	( verts, H.vertcount, tris, H.facecount );
		fs->close			();
		F->close			();
		Msg("* Level CFORM(L): %dK",LevelLight.memory()/1024);
	}
	
//	// Load emitters
//	{
//		strconcat			(N,name,"level.game");
//		IReader				*F = FS.r_open(N);
//		IReader				*O = 0;
//		if (0!=(O = F->open_chunk	(AIPOINT_CHUNK))) {
//			for (int id=0; O->find_chunk(id); id++) {
//				Emitters.push_back(Fvector());
//				O->r_fvector3	(Emitters.back());
//			}
//			O->close();
//		}
//	}
//
	// Load lights
	{
		strconcat				(N,name,"build.prj");

		IReader*	F			= FS.r_open(N);
		IReader					&fs= *F;

		// Version
		u32 version;
		fs.r_chunk				(EB_Version,&version);
		R_ASSERT				(XRCL_CURRENT_VERSION==version);

		// Header
		b_params				Params;
		fs.r_chunk				(EB_Parameters,&Params);

		// Lights (Static)
		{
			F = fs.open_chunk(EB_Light_static);
			b_light_static	temp;
			u32 cnt		= F->length()/sizeof(temp);
			for				(u32 i=0; i<cnt; i++)
			{
				R_Light		RL;
				F->r		(&temp,sizeof(temp));
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
			F->close		();
		}
	}

	// Init params
//	g_params.Init		();
	
	// Load AlexMX initial map from the Level Editor
	{
		strconcat			(N,name,"build.aimap");
		IReader				*F = FS.r_open(N);
		IReader				*O = 0;

		R_ASSERT			(F->open_chunk(E_AIMAP_CHUNK_VERSION));
		R_ASSERT			(F->r_u16() == E_AIMAP_VERSION);

		R_ASSERT			(F->open_chunk(E_AIMAP_CHUNK_BOX));
		F->r				(&LevelBB,sizeof(LevelBB));

		R_ASSERT			(F->open_chunk(E_AIMAP_CHUNK_PARAMS));
		F->r				(&g_params,sizeof(g_params));

		R_ASSERT			(F->open_chunk(E_AIMAP_CHUNK_NODES));
		u32					N = F->r_u32();
		R_ASSERT2			(N < ((u32(1) << u32(MAX_NODE_BIT_COUNT)) - 2),"Too many nodes!");
		g_nodes.resize		(N);

		hdrNODES			H;
		H.version			= XRAI_CURRENT_VERSION;
		H.count				= N+1;
		H.size				= g_params.fPatchSize;
		H.size_y			= 1.f;
		H.aabb				= LevelBB;
		
		typedef BYTE NodeLink[3];
		for (u32 i=0; i<N; i++) {
			NodeLink			id;
			u16 				pl;
			SNodePositionOld 	_np;
			NodePosition 		np;
			
			for (int j=0; j<4; ++j) {
				F->r			(&id,3);
				g_nodes[i].n[j]	= (*LPDWORD(&id)) & 0x00ffffff;
			}

			pl				= F->r_u16();
			pvDecompress	(g_nodes[i].Plane.n,pl);
			F->r			(&_np,sizeof(_np));
			CNodePositionConverter(_np,H,np);
			g_nodes[i].Pos	= vertex_position(np,LevelBB,g_params);

			g_nodes[i].Plane.build(g_nodes[i].Pos,g_nodes[i].Plane.n);
		}

		F->close			();
	}
}
