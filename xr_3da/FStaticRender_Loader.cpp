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

	// shDEBUG			= Device.Shader.Create("debug\\wireframe");

	// Begin
	pApp->LoadBegin	();
	CStream*	fs = pCreator->LL_Stream;
	CStream*	chunk;

	// VB
	pApp->LoadTitle("Loading vertex buffers...");
	chunk = fs->OpenChunk(fsL_VBUFFERS);
	LoadBuffers(chunk);
	chunk->Close();
	
	// Visuals
	pApp->LoadTitle("Loading subdivisions...");
	chunk = fs->OpenChunk(fsL_VISUALS);
	LoadVisuals(chunk);
	chunk->Close();
	
	// Lights
	pApp->LoadTitle	("Loading lights...");
	LoadLights		(fs);
	
	// Sectors
	pApp->LoadTitle("Loading sectors & portals...");
	LoadSectors(fs);
	
	// Details
	pApp->LoadTitle("Loading details...");
	Details.Load();

	// Streams
	vsPatches = Device.Streams.Create(FVF::F_TL,max_patches*4);
	
	// HOM
	HOM.Load		();
	
	// End
	pApp->LoadEnd	();
}

void CRender::level_Unload()
{
	if (0==pCreator)	return;

	DWORD I;

	// HOM
	HOM.Unload		();

	//*** Streams
	vsPatches			= 0;

	//*** Details
	Details.Unload		();

	//*** Sectors
	// 1.
	_DELETE				(rmPortals);
	pLastSector			= 0;
	vLastCameraPos.set	(0,0,0);
	// 2.
	for (I=0; I<Sectors.size(); I++)
		_DELETE(Sectors[I]);
	Sectors.clear		();
	// 3.
	Portals.clear		();

	//*** Lights
	Glows.Unload		();
	L_DB.Unload			();
	L_Dynamic.Destroy	();

	//*** Visuals
	for (I=0; I<Visuals.size(); I++)
	{
		Visuals[I]->Release();
		_DELETE(Visuals[I]);
	}
	Visuals.clear		();

	//*** VB
	for (I=0; I<VB.size(); I++)
	{
		VB[I]->Release	();
	}
	FVF.clear			();
	VB.clear			();
}

void CRender::LoadBuffers(CStream *fs)
{
	DWORD count = fs->Rdword();
	FVF.resize(count);
	VB.resize(count);

	Device.Shader.Evict		();
	
	for (DWORD i=0; i<count; i++)
	{
		DWORD vFVF	= fs->Rdword	();
		DWORD vCount= fs->Rdword	();
		DWORD vSize	= D3DXGetFVFVertexSize(vFVF);
		Msg("* [Loading VB] %d verts, %d Kb",vCount,(vCount*vSize)/1024);

		FVF[i] = vFVF;

		DWORD dwUsage = D3DUSAGE_WRITEONLY;
		if (HW.Caps.vertex.bSoftware)	dwUsage|=D3DUSAGE_SOFTWAREPROCESSING;
		R_CHK(HW.pDevice->CreateVertexBuffer(
			vCount*vSize,
			dwUsage,
			vFVF,
			(dwUsage&D3DUSAGE_SOFTWAREPROCESSING)?D3DPOOL_SYSTEMMEM:D3DPOOL_DEFAULT,
			&VB[i]));

		BYTE* pData;
		R_CHK(VB[i]->Lock(0,0,&pData,0));

		PSGP.memCopy	(pData,fs->Pointer(),vCount*vSize);

		VB[i]->Unlock();

		fs->Advance(vSize*vCount);
	}
}

void CRender::LoadVisuals(CStream *fs)
{
	CStream*		chunk	= 0;
	DWORD			index	= 0;
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
	CStream* chunk;

	// keys
	/*
	chunk = fs->OpenChunk(fsL_LIGHT_KEYS);
	if (chunk) {
		Lights.LoadKeyframes(chunk);
		chunk->Close();
	}
	*/

	// lights
	chunk = fs->OpenChunk(fsL_LIGHTS);
	R_ASSERT	(chunk && "Can't find lights");
	L_DB.Load	(chunk);
	chunk->Close();

	// glows
	chunk = fs->OpenChunk(fsL_GLOWS);
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
	DWORD size = fs->FindChunk(fsL_PORTALS); 
	R_ASSERT(size && 0==size%sizeof(b_portal));
	DWORD count = size/sizeof(b_portal);
	Portals.resize(count);

	// load sectors
	CStream* S = fs->OpenChunk(fsL_SECTORS);
	for (DWORD i=0; ; i++)
	{
		CStream* P = S->OpenChunk(i);
		if (0==P) break;

		Sectors.push_back(new CSector(i));
		Sectors.back()->Load(*P);

		P->Close();
	}
	S->Close();

	// load portals
	CDB::Collector	CL;
	fs->FindChunk(fsL_PORTALS);
	for (i=0; i<count; i++)
	{
		b_portal	P;
		fs->Read(&P,sizeof(P));
		Portals[i].Setup(P.vertices.begin(),P.vertices.size(),
			getSector(P.sector_front),
			getSector(P.sector_back));
		for (DWORD j=2; j<P.vertices.size(); j++)
			CL.add_face_packed(
				P.vertices[0],P.vertices[j-1],P.vertices[j],
				CDB::edge_open,CDB::edge_open,CDB::edge_open,
				0,0,DWORD(&Portals[i])
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

	// debug
	//	for (int d=0; d<Sectors.size(); d++)
	//		Sectors[d]->DebugDump	();

	pLastSector = 0;
}
