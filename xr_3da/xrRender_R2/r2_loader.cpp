#include "stdafx.h"
#include "r2.h"
#include "..\fbasicvisual.h"
#include "..\fmesh.h"
#include "..\xrLevel.h"
#include "..\x_ray.h"

void CRender::level_Load()
{
	if (0==pCreator)	return;

	// Begin
	pApp->LoadBegin		();
	IReader*	fs		= pCreator->LL_Stream;
	IReader*	chunk;

	// VB
	pApp->LoadTitle		("Loading geometry...");
	LoadBuffers			(fs);

	// Visuals
	pApp->LoadTitle		("Loading spatial-DB...");
	chunk				= fs->open_chunk(fsL_VISUALS);
	LoadVisuals			(chunk);
	chunk->close		();

	// Lights
	pApp->LoadTitle		("Loading lights...");
	LoadLights			(fs);

	// Sectors
	pApp->LoadTitle		("Loading sectors & portals...");
	LoadSectors			(fs);

	// Details
	pApp->LoadTitle		("Loading details...");
	Details.Load		();

	// Wallmarks
	Wallmarks			= xr_new<CWallmarksEngine>	();

	// Streams
	hGeomPatches		= Device.Shader.CreateGeom	(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);

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
	Device.Shader.DeleteGeom(hGeomPatches);

	// Wallmarks
	xr_delete				(Wallmarks);

	//*** Details
	Details.Unload			();

	//*** Sectors
	// 1.
	xr_delete				(rmPortals);
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
	DCL.clear				();
	VB.clear				();
	IB.clear				();
}

void CRender::LoadBuffers	(IReader *base_fs)
{
	Device.Shader.Evict		();
	u32	dwUsage				= D3DUSAGE_WRITEONLY | (HW.Caps.vertex.bSoftware?D3DUSAGE_SOFTWAREPROCESSING:0);

	// Vertex buffers
	if (base_fs->find_chunk(fsL_VBUFFERS_DX9))
	{
		// Use DX9-style declarators
		destructor<IReader>		fs	(base_fs->open_chunk(fsL_VBUFFERS_DX9));
		u32 count				= fs().r_u32();
		DCL.resize				(count);
		VB.resize				(count);
		for (u32 i=0; i<count; i++)
		{
			// decl
			D3DVERTEXELEMENT9*	dcl		= (D3DVERTEXELEMENT9*) fs().pointer();
			u32 dcl_len			= D3DXGetDeclLength		(dcl)+1;
			DCL[i].resize		(dcl_len);
			fs().r				(DCL[i].begin(),dcl_len*sizeof(D3DVERTEXELEMENT9));

			// count, size
			u32 vCount			= fs().r_u32	();
			u32 vSize			= D3DXGetDeclVertexSize	(dcl,0);
			Msg	("* [Loading VB] %d verts, %d Kb",vCount,(vCount*vSize)/1024);

			// Create and fill
			BYTE*	pData		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(vCount*vSize,dwUsage,0,D3DPOOL_MANAGED,&VB[i],0));
			R_CHK				(VB[i]->Lock(0,0,(void**)&pData,0));
			Memory.mem_copy		(pData,fs().pointer(),vCount*vSize);
			VB[i]->Unlock		();

			fs().advance		(vCount*vSize);
		}
	} else {
		// Use DX7-style FVFs
		destructor<IReader>		fs	(base_fs->open_chunk(fsL_VBUFFERS));
		u32 count				= fs().r_u32();
		DCL.resize				(count);
		VB.resize				(count);
		for (u32 i=0; i<count; i++)
		{
			u32 vFVF			= fs().r_u32	();
			u32 vCount			= fs().r_u32	();
			u32 vSize			= D3DXGetFVFVertexSize	(vFVF);
			Msg("* [Loading VB] %d verts, %d Kb",vCount,(vCount*vSize)/1024);

			D3DVERTEXELEMENT9*	dcl_dst	= DCL[i].begin();
			CHK_DX				(D3DXDeclaratorFromFVF	(vFVF,dcl_dst));

			// Create and fill
			BYTE*	pData		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(vCount*vSize,dwUsage,0,D3DPOOL_MANAGED,&VB[i],0));
			R_CHK				(VB[i]->Lock(0,0,(void**)&pData,0));
			Memory.mem_copy		(pData,fs().pointer(),vCount*vSize);
			VB[i]->Unlock		();

			fs().advance		(vCount*vSize);
		}
	}

	// Index buffers
	if (base_fs->find_chunk(fsL_IBUFFERS))
	{
		destructor<IReader>		fs	(base_fs->open_chunk	(fsL_IBUFFERS));
		u32 count				= fs().r_u32();
		IB.resize				(count);
		for (u32 i=0; i<count; i++)
		{
			u32 iCount		= fs().r_u32	();
			Msg("* [Loading IB] %d indices, %d Kb",iCount,(iCount*2)/1024);

			// Create and fill
			BYTE*	pData		= 0;
			R_CHK				(HW.pDevice->CreateIndexBuffer(iCount*2,dwUsage,D3DFMT_INDEX16,D3DPOOL_MANAGED,&IB[i],0));
			R_CHK				(IB[i]->Lock(0,0,(void**)&pData,0));
			Memory.mem_copy		(pData,fs().pointer(),iCount*2);
			IB[i]->Unlock		();

			fs().advance		(iCount*2);
		}
	}
}

void CRender::LoadVisuals(IReader *fs)
{
	IReader*		chunk	= 0;
	u32			index	= 0;
	IVisual*		V		= 0;
	ogf_header		H;

	while ((chunk=fs->open_chunk(index))!=0)
	{
		chunk->r_chunk_safe		(OGF_HEADER,&H,sizeof(H));
		V = Models.Instance_Create	(H.type);
		V->Load(0,chunk,0);
		Visuals.push_back(V);

		chunk->close();
		index++;
	}
}

void CRender::LoadLights(IReader *fs)
{
	// lights
	L_DB.Load	(fs);

	// glows
	IReader*	chunk = fs->open_chunk(fsL_GLOWS);
	R_ASSERT	(chunk && "Can't find glows");
	Glows.Load(chunk);
	chunk->close();
}

struct b_portal
{
	WORD				sector_front;
	WORD				sector_back;
	svector<Fvector,6>	vertices;
};

void CRender::LoadSectors(IReader* fs)
{
	// allocate memory for portals
	u32 size = fs->find_chunk(fsL_PORTALS); 
	R_ASSERT(0==size%sizeof(b_portal));
	u32 count = size/sizeof(b_portal);
	Portals.resize	(count);
	for (u32 c=0; c<count; c++)
		Portals[c]	= xr_new<CPortal> ();

	// load sectors
	IReader* S = fs->open_chunk(fsL_SECTORS);
	for (u32 i=0; ; i++)
	{
		IReader* P = S->open_chunk(i);
		if (0==P) break;

		CSector* __S		= xr_new<CSector> (i);
		__S->Load			(*P);
		Sectors.push_back	(__S);

		P->close();
	}
	S->close();

	// load portals
	if (count) 
	{
		CDB::Collector	CL;
		fs->find_chunk	(fsL_PORTALS);
		for (i=0; i<count; i++)
		{
			b_portal	P;
			fs->r		(&P,sizeof(P));
			CPortal*	__P	= (CPortal*)Portals[i];
			__P->Setup	(P.vertices.begin(),P.vertices.size(),
				(CSector*)getSector(P.sector_front),
				(CSector*)getSector(P.sector_back));
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
		rmPortals = xr_new<CDB::MODEL> ();
		rmPortals->build	(CL.getV(),CL.getVS(),CL.getT(),CL.getTS());
	} else {
		rmPortals = 0;
	}

	// debug
	//	for (int d=0; d<Sectors.size(); d++)
	//		Sectors[d]->DebugDump	();

	pLastSector = 0;
}
