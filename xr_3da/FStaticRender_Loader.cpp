#include "stdafx.h"
#include "fstaticrender.h"
#include "fbasicvisual.h"
#include "fmesh.h"
#include "xrLevel.h"
#include "cl_collector.h"
#include "xr_creator.h"

#include "x_ray.h"

extern Shader*	shDEBUG;

void CRender::level_Load()
{
	if (0==pCreator)	return;

	// Begin
	pApp->LoadBegin		();
	CStream*	fs		= pCreator->LL_Stream;
	CStream*	chunk;

	// VB
	pApp->LoadTitle		("Loading geometry...");
	LoadBuffers			(fs);
	
	// Visuals
	pApp->LoadTitle		("Loading spatialization...");
	chunk				= fs->OpenChunk(fsL_VISUALS);
	LoadVisuals			(chunk);
	chunk->Close		();
	
	// Lights
	pApp->LoadTitle		("Loading lights...");
	LoadLights			(fs);
	
	// Sectors
	pApp->LoadTitle		("Loading sectors & portals...");
	LoadSectors			(fs);
	
	// Details
	pApp->LoadTitle		("Loading details...");
	Details.Load		();

	// Streams
	vsPatches			= Device.Shader._CreateVS	(FVF::F_TL);
	
	// HOM
	HOM.Load			();
	
	// End
	pApp->LoadEnd		();
}

void CRender::level_Unload()
{
	if (0==pCreator)		return;

	u32 I;

	// HOM
	HOM.Unload				();

	//*** Streams
	Device.Shader._DeleteVS	(vsPatches);

	//*** Details
	Details.Unload			();

	//*** Sectors
	// 1.
	xr_delete					(rmPortals);
	pLastSector				= 0;
	vLastCameraPos.set		(0,0,0);
	// 2.
	for (I=0; I<Sectors.size(); I++)
		xr_delete(Sectors[I]);
	Sectors.clear			();
	// 3.
	Portals.clear			();

	//*** Lights
	Glows.Unload			();
	L_DB.Unload				();
	L_Dynamic.Destroy		();

	//*** Visuals
	for (I=0; I<Visuals.size(); I++)
	{
		Visuals[I]->Release();
		xr_delete(Visuals[I]);
	}
	Visuals.clear			();

	//*** VB/IB
	for (I=0; I<VB.size(); I++)	_RELEASE(VB[I]);
	for (I=0; I<IB.size(); I++)	_RELEASE(IB[I]);
	FVF.clear				();
	VB.clear				();
	IB.clear				();
}

void CRender::LoadBuffers	(CStream *base_fs)
{
	Device.Shader.Evict		();
	u32	dwUsage			= D3DUSAGE_WRITEONLY | (HW.Caps.vertex.bSoftware?D3DUSAGE_SOFTWAREPROCESSING:0);
	D3DPOOL	dwPool			= HW.Caps.vertex.bSoftware?D3DPOOL_SYSTEMMEM:D3DPOOL_DEFAULT;

	// Vertex buffers
	{
		destructor<CStream>		fs	(base_fs->OpenChunk(fsL_VBUFFERS));
		u32 count				= fs().Rdword();
		FVF.resize				(count);
		VB.resize				(count);
		for (u32 i=0; i<count; i++)
		{
			u32 vFVF			= fs().Rdword	();
			u32 vCount		= fs().Rdword	();
			u32 vSize			= D3DXGetFVFVertexSize(vFVF);
			Msg("* [Loading VB] %d verts, %d Kb",vCount,(vCount*vSize)/1024);

			FVF[i]				= vFVF;

			// Create and fill
			BYTE*	pData		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(vCount*vSize,dwUsage,vFVF,dwPool,&VB[i]));
			R_CHK				(VB[i]->Lock(0,0,&pData,0));
			PSGP.memCopy		(pData,fs().Pointer(),vCount*vSize);
			VB[i]->Unlock		();

			fs().Advance		(vCount*vSize);
		}
	}

	// Index buffers
	if (base_fs->FindChunk(fsL_IBUFFERS))
	{
		destructor<CStream>		fs	(base_fs->OpenChunk	(fsL_IBUFFERS));
		u32 count				= fs().Rdword();
		IB.resize				(count);
		for (u32 i=0; i<count; i++)
		{
			u32 iCount		= fs().Rdword	();
			Msg("* [Loading IB] %d indices, %d Kb",iCount,(iCount*2)/1024);

			// Create and fill
			BYTE*	pData		= 0;
			R_CHK				(HW.pDevice->CreateIndexBuffer(iCount*2,dwUsage,D3DFMT_INDEX16,dwPool,&IB[i]));
			R_CHK				(IB[i]->Lock(0,0,&pData,0));
			PSGP.memCopy		(pData,fs().Pointer(),iCount*2);
			IB[i]->Unlock		();

			fs().Advance		(iCount*2);
		}
	}
}

void CRender::LoadVisuals(CStream *fs)
{
	CStream*		chunk	= 0;
	u32			index	= 0;
	CVisual*		V		= 0;
	ogf_header		H;

	while ((chunk=fs->OpenChunk(index))!=0)
	{
		chunk->ReadChunkSafe		(OGF_HEADER,&H,sizeof(H));
		V = Models.Instance_Create	(H.type);
		V->Load(0,chunk,0);
		Visuals.push_back(V);

		chunk->Close();
		index++;
	}
}

void CRender::LoadLights(CStream *fs)
{
	// lights
	L_DB.Load	(fs);

	// glows
	CStream*	chunk = fs->OpenChunk(fsL_GLOWS);
	R_ASSERT	(chunk && "Can't find glows");
	Glows.Load(chunk);
	chunk->Close();
}

struct b_portal
{
	WORD				sector_front;
	WORD				sector_back;
	svector<Fvector,6>	vertices;
};

void CRender::LoadSectors(CStream* fs)
{
	// allocate memory for portals
	u32 size = fs->FindChunk(fsL_PORTALS); 
	R_ASSERT(0==size%sizeof(b_portal));
	u32 count = size/sizeof(b_portal);
	Portals.resize(count);

	// load sectors
	CStream* S = fs->OpenChunk(fsL_SECTORS);
	for (u32 i=0; ; i++)
	{
		CStream* P = S->OpenChunk(i);
		if (0==P) break;

		Sectors.push_back(new CSector(i));
		Sectors.back()->Load(*P);

		P->Close();
	}
	S->Close();

	// load portals
	if (count) 
	{
		CDB::Collector	CL;
		fs->FindChunk(fsL_PORTALS);
		for (i=0; i<count; i++)
		{
			b_portal	P;
			fs->Read(&P,sizeof(P));
			Portals[i].Setup(P.vertices.begin(),P.vertices.size(),
				getSector(P.sector_front),
				getSector(P.sector_back));
			for (u32 j=2; j<P.vertices.size(); j++)
				CL.add_face_packed(
				P.vertices[0],P.vertices[j-1],P.vertices[j],
				CDB::edge_open,CDB::edge_open,CDB::edge_open,
				0,0,u32(&Portals[i])
				);
		}
		if (CL.getTS()<2)
		{
			Fvector		v1,v2,v3;
			v1.set		(-20000.f,-20000.f,-20000.f);
			v2.set		(-20001.f,-20001.f,-20001.f);
			v3.set		(-20002.f,-20002.f,-20002.f);
			CL.add_face_packed(
				v1,v2,v3,
				CDB::edge_open,CDB::edge_open,CDB::edge_open,
				0,0,0
				);
		}

		// build portal model
		rmPortals = new CDB::MODEL;
		rmPortals->build	(CL.getV(),CL.getVS(),CL.getT(),CL.getTS());
	} else {
		rmPortals = 0;
	}

	// debug
	//	for (int d=0; d<Sectors.size(); d++)
	//		Sectors[d]->DebugDump	();

	pLastSector = 0;
}
