#include "stdafx.h"
#include "fstaticrender.h"
#include "fmesh.h"
#include "xrLevel.h"
#include "collide\cl_collector.h"
#include "xr_creator.h"


#include "x_ray.h"

void CRender::level_Load()
{
	if (0==pCreator)	return;

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
	pApp->LoadTitle("Loading lights...");
	LoadLights(fs);
	Lights_Dynamic.Initialize();
	
	// Sectors
	pApp->LoadTitle("Loading sectors & portals...");
	LoadSectors(fs);
	
	// Details
	pApp->LoadTitle("Loading details...");
	Details.Load();

	// Streams
	vsPatches = Device.Streams.Create(FVF::F_TL,max_patches*4);
	
	// End
	pApp->LoadEnd	();
}

void CRender::level_Unload()
{
	if (0==pCreator)	return;

	DWORD I;

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
	Sectors.clear			();
	// 3.
	Portals.clear			();

	//*** Lights
	Glows.Unload			();
	Lights.Unload			();
	Lights_Dynamic.Destroy	();

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

	CHK_DX(HW.pDevice->ResourceManagerDiscardBytes(0));

	for (DWORD i=0; i<count; i++)
	{
		DWORD vFVF	= fs->Rdword	();
		DWORD vCount= fs->Rdword	();
		DWORD vSize	= D3DXGetFVFVertexSize(vFVF);
		Msg("* [Loading VB] %d verts, %d Kb",vCount,(vCount*vSize)/1024);

		FVF[i] = vFVF;

		DWORD dwUsage = D3DUSAGE_WRITEONLY;
		if (HW.Caps.bSoftware)	dwUsage|=D3DUSAGE_SOFTWAREPROCESSING;
		R_CHK(HW.pDevice->CreateVertexBuffer(
			vCount*vSize,
			dwUsage,
			vFVF,
			(dwUsage&D3DUSAGE_SOFTWAREPROCESSING)?D3DPOOL_SYSTEMMEM:D3DPOOL_DEFAULT,
			&VB[i]));

		BYTE* pData;
		R_CHK(VB[i]->Lock(0,0,&pData,D3DLOCK_NOSYSLOCK));

		CopyMemory(pData,fs->Pointer(),vCount*vSize);

		VB[i]->Unlock();

		fs->Advance(vSize*vCount);
	}
}

void CRender::LoadVisuals(CStream *fs)
{
	CStream*		chunk	= 0;
	DWORD			index	= 0;
	FBasicVisual*	V		= 0;
	ogf_header		H;

	while ((chunk=fs->OpenChunk(index))!=0)
	{
		chunk->ReadChunk(OGF_HEADER,&H);
		V = Models.Instance_Create(H.type);
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
	chunk = fs->OpenChunk(fsL_LIGHT_KEYS);
	if (chunk) {
		Lights.LoadKeyframes(chunk);
		chunk->Close();
	}

	// lights
	chunk = fs->OpenChunk(fsL_LIGHTS);
	Lights.Load(chunk);
	chunk->Close();

	// glows
	chunk = fs->OpenChunk(fsL_GLOWS);
	Glows.Load(chunk);
	chunk->Close();
}

struct b_portal
{
	WORD			sector_front;
	WORD			sector_back;
	DWORD			vert_count;
	Fvector			vertices[6];
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
	RAPID::Collector	CL;
	fs->FindChunk(fsL_PORTALS);
	for (i=0; i<count; i++)
	{
		b_portal	P;
		fs->Read(&P,sizeof(P));
		if (i!=63)	{
			Portals[i].Setup(P.vertices,P.vert_count,
				getSector(P.sector_front),
				getSector(P.sector_back));
		} else {
			Portals[i].Setup(P.vertices,P.vert_count,
				getSector(P.sector_back),
				getSector(P.sector_front));
		}
#pragma todo("portal 64")
		/*
			Portals[i].P.n.invert();
			Portals[i].P.d = -Portals[i].P.d;
		*/
		for (DWORD j=2; j<P.vert_count; j++)
			CL.add_face_packed(
				P.vertices[0],P.vertices[j-1],P.vertices[j],
				RAPID::edge_open,RAPID::edge_open,RAPID::edge_open,
				0,0,DWORD(&Portals[i])
			);
	}

	// build portal model
	rmPortals = new RAPID::Model;
	rmPortals->BuildModel(CL.getV(),CL.getVS(),CL.getT(),CL.getTS());

	pLastSector = 0;
}
