#include "stdafx.h"
#include "..\fbasicvisual.h"
#include "..\fmesh.h"
#include "..\xrLevel.h"
#include "..\x_ray.h"
#include "..\IGame_Persistent.h"

void CRender::level_Load()
{
	R_ASSERT			(0!=g_pGameLevel);

	// Begin
	pApp->LoadBegin					();
	Device.Resources->DeferredLoad	(TRUE);
	IReader*	fs					= g_pGameLevel->LL_Stream;
	IReader*	chunk;

	// Shaders
	pApp->LoadTitle				("Loading shaders...");
	{
		chunk = fs->open_chunk		(fsL_SHADERS);
		R_ASSERT2					(chunk,"Level doesn't builded correctly.");
		u32 count = chunk->r_u32	();
		Shaders.resize				(count);
		for(u32 i=0; i<count; i++)	// skip first shader as "reserved" one
		{
			string512				n_sh,n_tlist;
			LPCSTR			n		= LPCSTR(chunk->pointer());
			chunk->skip_stringZ		();
			if (0==n[0])			continue;
			strcpy					(n_sh,n);
			LPSTR			delim	= strchr(n_sh,'/');
			*delim					= 0;
			strcpy					(n_tlist,delim+1);
			Shaders[i]				= Device.Resources->Create(n_sh,n_tlist);
		}
		chunk->close();
	}

	// Components
	Target						= xr_new<CRenderTarget>		();
	L_Shadows					= xr_new<CLightShadows>		();
	L_Projector					= xr_new<CLightProjector>	();
	L_DB						= xr_new<CLight_DB>			();
	L_Glows						= xr_new<CGlowManager>		();
	Wallmarks					= xr_new<CWallmarksEngine>	();
	Details						= xr_new<CDetailManager>	();

	// Static wallmarks
	string_path fn_wm;
	if (FS.exist(fn_wm, "$level$", "level.wallmarks"))
		Wallmarks->load_LevelWallmarks(fn_wm);

	rmFar						();
	rmNormal					();

	marker						= 0;

	if	(!g_pGamePersistent->bDedicatedServer)	{
		// SWIs
		LoadSWIs					(fs);

		// VB
		pApp->LoadTitle				("Loading geometry...");
		LoadBuffers					(fs);

		// Visuals
		pApp->LoadTitle				("Loading spatial-DB...");
		chunk						= fs->open_chunk(fsL_VISUALS);
		LoadVisuals					(chunk);
		chunk->close				();

		// Details
		pApp->LoadTitle				("Loading details...");
		Details->Load				();
	}
	
	// Sectors
	pApp->LoadTitle				("Loading sectors & portals...");
	LoadSectors					(fs);

	// Lights
	pApp->LoadTitle				("Loading lights...");
	LoadLights					(fs);

	//Sliding window
	pApp->LoadTitle				("Loading SWIs...");
	LoadLights					(fs);

	// HOM
	HOM.Load					();
	
	// End
	pApp->LoadEnd				();
}

void CRender::level_Unload()
{
	if (0==g_pGameLevel)		return;

	u32 I;

	// HOM
	HOM.Unload					();

	// walmmarks
	Wallmarks->unload_LevelWallmarks();


	//*** Details
	Details->Unload				();

	//*** Sectors
	// 1.
	xr_delete					(rmPortals);
	pLastSector					= 0;
	vLastCameraPos.set			(0,0,0);
	// 2.
	for (I=0; I<Sectors.size(); I++)	xr_delete(Sectors[I]);
	Sectors.clear_and_free		();
	// 3.
	for (I=0; I<Portals.size(); I++)	xr_delete(Portals[I]);
	Portals.clear_and_free		();

	//*** Lights
	L_Glows->Unload				();
	L_DB->Unload				();

	//*** Visuals
	for (I=0; I<Visuals.size(); I++)
	{
		Visuals[I]->Release();
		xr_delete(Visuals[I]);
	}
	Visuals.clear_and_free		();

	//*** VB/IB
	for (I=0; I<VB.size(); I++)	_RELEASE(VB[I]);
	for (I=0; I<IB.size(); I++)	_RELEASE(IB[I]);
	DCL.clear_and_free			();
	VB.clear_and_free			();
	IB.clear_and_free			();

	//*** Components
	xr_delete					(Details);
	xr_delete					(Wallmarks);
	xr_delete					(L_Glows);
	xr_delete					(L_DB);
	xr_delete					(L_Projector);
	xr_delete					(L_Shadows);
	xr_delete					(Target);

	//*** Shaders
	Shaders.clear_and_free		();
}

void CRender::LoadBuffers	(IReader *base_fs)
{
	Device.Resources->Evict	();
	u32	dwUsage				= D3DUSAGE_WRITEONLY | (HW.Caps.geometry.bSoftware?D3DUSAGE_SOFTWAREPROCESSING:0);

	// Vertex buffers
	if (base_fs->find_chunk(fsL_VB))
	{
		// Use DX9-style declarators
		destructor<IReader>		fs	(base_fs->open_chunk(fsL_VB));
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
			//.????????? remove T&B from DCL[]

			// count, size
			u32 vCount			= fs().r_u32	();
			u32 vSize			= D3DXGetDeclVertexSize	(dcl,0);
			Msg	("* [Loading VB] %d verts, %d Kb",vCount,(vCount*vSize)/1024);

			// Create and fill
			BYTE*	pData		= 0;
			R_CHK				(HW.pDevice->CreateVertexBuffer(vCount*vSize,dwUsage,0,D3DPOOL_MANAGED,&VB[i],0));
			R_CHK				(VB[i]->Lock(0,0,(void**)&pData,0));
			Memory.mem_copy		(pData,fs().pointer(),vCount*vSize);	//.???? copy while skip T&B
			VB[i]->Unlock		();

			fs().advance		(vCount*vSize);
		}
	} else {
		Debug.fatal				("DX7-style FVFs unsupported");
	}

	// Index buffers
	if (base_fs->find_chunk(fsL_IB))
	{
		destructor<IReader>		fs	(base_fs->open_chunk	(fsL_IB));
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
	IRender_Visual*		V		= 0;
	ogf_header		H;

	while ((chunk=fs->open_chunk(index))!=0)
	{
		chunk->r_chunk_safe			(OGF_HEADER,&H,sizeof(H));
		V = Models->Instance_Create	(H.type);
		V->Load						(0,chunk,0);
		Visuals.push_back			(V);

		chunk->close();
		index++;
	}
}

void CRender::LoadLights(IReader *fs)
{
	// lights
	L_DB->Load	(fs);

	// glows
	IReader*		chunk = fs->open_chunk(fsL_GLOWS);
	R_ASSERT		(chunk && "Can't find glows");
	L_Glows->Load	(chunk);
	chunk->close	();
}

struct b_portal
{
	u16				sector_front;
	u16				sector_back;
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

		CSector* __S		= xr_new<CSector> ();
		__S->load			(*P);
		Sectors.push_back	(__S);

		P->close();
	}
	S->close();

	// load portals
	if (count) 
	{
		u32	i;
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
				CL.add_face_packed_D	( P.vertices[0],P.vertices[j-1],P.vertices[j], u32(i) );
		}
		if (CL.getTS()<2)
		{
			Fvector		v1,v2,v3;
			v1.set		(-20000.f,-20000.f,-20000.f);
			v2.set		(-20001.f,-20001.f,-20001.f);
			v3.set		(-20002.f,-20002.f,-20002.f);
			CL.add_face_packed_D		( v1,v2,v3, 0 );
		}

		// build portal model
		rmPortals = xr_new	<CDB::MODEL> ();
		rmPortals->build	(CL.getV(),int(CL.getVS()),CL.getT(),int(CL.getTS()));
	} else {
		rmPortals = 0;
	}

	// debug
	//	for (int d=0; d<Sectors.size(); d++)
	//		Sectors[d]->DebugDump	();

	pLastSector = 0;
}

void CRender::LoadSWIs(IReader* base_fs)
{
	// allocate memory for portals
	if (base_fs->find_chunk(fsL_SWIS)){
		destructor<IReader>	fs	(base_fs->open_chunk(fsL_SWIS));
		u32 item_count		= fs().r_u32();	
		SWIs.resize			(item_count);
		for (u32 c=0; c<item_count; c++){
			FSlideWindowItem& swi = SWIs[c];
			swi.reserved[0]	= fs().r_u32();	
			swi.reserved[1]	= fs().r_u32();	
			swi.reserved[2]	= fs().r_u32();	
			swi.reserved[3]	= fs().r_u32();	
			swi.count		= fs().r_u32();	
			swi.sw			= xr_alloc<FSlideWindow> (swi.count);
			fs().r			(swi.sw,sizeof(FSlideWindow)*swi.count);
		}
	}
}
