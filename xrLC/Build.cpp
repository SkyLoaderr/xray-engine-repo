// Build.cpp: implementation of the CBuild class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Build.h"
#include "Image.h"

vector<OGF_Base *>		g_tree;
b_params				g_params;
HANDLE					hLargeHeap=0;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class T>
void transfer(const char *name, vector<T> &dest, CStream& F, DWORD chunk)
{
	CStream*	O		= F.OpenChunk(chunk);
	DWORD		count	= O?(O->Length()/sizeof(T)):0;
	Msg			("* %16s: %d",name,count);
	if (count)  
	{
		dest.reserve(count);
		dest.insert	(dest.begin(), (T*)O->Pointer(), (T*)O->Pointer() + count);
	}
	if (O)		O->Close	();
}

extern DWORD*	Surface_Load	(char* name, DWORD& w, DWORD& h);
extern void		Surface_Init	();

CBuild::CBuild	(b_params& Params, CStream& FS)
{
	HANDLE			hLargeHeap	= HeapCreate(0,64*1024*1024,0);
	Msg				("* <LargeHeap> handle: %X",hLargeHeap);

	DWORD			i			= 0;

	float			p_total		= 0;
	float			p_cost		= 1.f/3.f;
	
	CStream*		F			= 0;

	// 
	shaders.Load				("gamedata\\shaders_xrlc.xr");
	/*
	{
		Shader_xrLCVec&	S = shaders.Library();
		for (Shader_xrLCIt I = S.begin(); I!=S.end(); I++)
		{
			Msg("|%s|",I->Name);
		}
	}
	*/
	
	//*******
	Status					("Vertices...");
	{
		F = FS.OpenChunk		(EB_Vertices);
		DWORD v_count			=	F->Length()/sizeof(b_vertex);
		g_vertices.reserve		(3*v_count/2);
		scene_bb.invalidate		();
		for (i=0; i<v_count; i++)
		{
			Vertex*	pV			= new Vertex;
			F->Rvector			(pV->P);
			pV->N.set			(0,0,0);
			pV->Color.set		(0,0,0,0);
			scene_bb.modify		(pV->P);
		}
		Progress			(p_total+=p_cost);
		Msg					("* %16s: %d","vertices",g_vertices.size());
		F->Close			();
	}

	//*******
	Status					("Faces...");
	{
		F = FS.OpenChunk		(EB_Faces);
		R_ASSERT				(F);
		DWORD f_count			=	F->Length()/sizeof(b_face);
		g_faces.reserve			(f_count);
		for (i=0; i<f_count; i++)
		{
			try 
			{
				Face*	_F			= new Face;
				b_face	B;
				F->Read				(&B,sizeof(B));

				_F->dwMaterial		= WORD(B.dwMaterial);

				// Vertices and adjacement info
				for (DWORD it=0; it<3; it++)
				{
					int id			= B.v[it];
					R_ASSERT		(id<(int)g_vertices.size());
					_F->SetVertex	(it,g_vertices[id]);
				}

				// transfer TC
				UVpoint				uv1,uv2,uv3;
				uv1.set				(B.t[0].x,B.t[0].y);
				uv2.set				(B.t[1].x,B.t[1].y);
				uv3.set				(B.t[2].x,B.t[2].y);
				_F->AddChannel		( uv1, uv2, uv3 );
			} catch (...)
			{
				Msg			("* ERROR: Can't process face #%d",i);
				R_ASSERT	(0);
			}
		}
		Progress			(p_total+=p_cost);
		Msg					("* %16s: %d","faces",g_faces.size());
		F->Close			();
	}

	//*******
	Status	("Other transfer...");
	transfer("materials",	materials,			FS,		EB_Materials);
	transfer("shaders",		shader_render,		FS,		EB_Shaders_Render);
	transfer("shaders_xrlc",shader_compile,		FS,		EB_Shaders_Compile);
	transfer("glows",		glows,				FS,		EB_Glows);
	transfer("portals",		portals,			FS,		EB_Portals);
	transfer("LODs",		lods,				FS,		EB_LOD_models);

	// Load lights
	Status	("Loading lights...");
	{
		// Controlles/Layers
		{
			F = FS.OpenChunk		(EB_Light_control);
			L_control_data.assign	(LPBYTE(F->Pointer()),LPBYTE(F->Pointer())+F->Length());

			R_Layer			temp;
			
			while (!F->Eof())
			{
				F->Read				(temp.control.name,sizeof(temp.control.name));
				DWORD cnt			= F->Rdword();
				temp.control.data.resize(cnt);
				F->Read				(temp.control.data.begin(),cnt*sizeof(DWORD));

				L_layers.push_back	(temp);
			}

			F->Close		();
		}
		// Static
		{
			F = FS.OpenChunk(EB_Light_static);
			b_light_static	temp;
			DWORD cnt		= F->Length()/sizeof(temp);
			for				(i=0; i<cnt; i++)
			{
				R_Light		RL;
				F->Read		(&temp,sizeof(temp));
				Flight&		L = temp.data;

				// type
				if			(L.type == D3DLIGHT_DIRECTIONAL)	RL.type	= LT_DIRECT;
				else											RL.type = LT_POINT;

				// generic properties
				RL.diffuse.normalize_rgb	(L.diffuse);
				RL.position.set				(L.position);
				RL.direction.normalize_safe	(L.direction);
				RL.range				=	L.range*1.1f;
				RL.range2				=	RL.range*RL.range;
				RL.attenuation0			=	L.attenuation0;
				RL.attenuation1			=	L.attenuation1;
				RL.attenuation2			=	L.attenuation2;
				RL.energy				=	L.diffuse.magnitude_rgb	();

				// place into layer
				R_ASSERT	(temp.controller_ID<L_layers.size());
				L_layers	[temp.controller_ID].lights.push_back	(RL);
			}
			F->Close		();
		}

		// Dynamic
		transfer("d-lights",	L_dynamic,			FS,		EB_Light_dynamic);
	}
	
	// process textures
	Status			("Processing textures...");
	{
		Surface_Init	();
		F = FS.OpenChunk(EB_Textures);
		DWORD tex_count	= F->Length()/sizeof(b_texture);
		for (DWORD t=0; t<tex_count; t++)
		{
			Progress		(float(t)/float(tex_count));

			b_texture		TEX;
			F->Read			(&TEX,sizeof(TEX));

			b_BuildTexture	BT;
			CopyMemory		(&BT,&TEX,sizeof(TEX));

			// load thumbnail
			LPSTR N			= BT.name;
			if (strchr(N,'.')) *(strchr(N,'.')) = 0;
			char th_name[256]; strconcat(th_name,"\\\\x-ray\\stalkerdata$\\textures\\",N,".thm");
			CCompressedStream THM	(th_name,THM_SIGN);

			// analyze thumbnail information
			R_ASSERT(THM.ReadChunk(THM_CHUNK_TEXTUREPARAM,&BT.THM));

			// load surface if it has an alpha channel or has "implicit lighting" flag
			BT.dwWidth	= BT.THM.width;
			BT.dwHeight	= BT.THM.height;
			BT.bHasAlpha= BT.THM.HasAlphaChannel();
			if (BT.bHasAlpha || (BT.THM.flag&STextureParams::flImplicitLighted))	
			{
				Msg			("- loading: %s",N);
				DWORD w=0,	h=0;
				BT.pSurface = Surface_Load(N,w,h);
				BT.Vflip	();
			} else {
				// Free surface memory
			}

			// save all the stuff we've created
			textures.push_back	(BT);
		}
	}

	// post-process materials
	Status	("Post-process materials...");
	for (DWORD m=0; m<materials.size(); m++)
	{
		b_material &M = materials[m];
		if (65535==M.shader_xrlc)	{
			// No compiler shader
			M.reserved	= WORD(-1);
		} else {
			int id = shaders.GetID(shader_compile[M.shader_xrlc].name);
			if (id<0) {
				Msg("ERROR: Shader '%s' not found in library",shader_compile[M.shader].name);
				R_ASSERT(id>=0);
			}
			M.reserved = WORD(id);
		}
	}
	Progress(p_total+=p_cost);

	// Parameter block
	CopyMemory(&g_params,&Params,sizeof(b_params));
	g_params.m_SS_DedicateCached	= 8;
	g_params.m_bConvertProgressive	= FALSE;
}

CBuild::~CBuild()
{
	Phase("Cleanup");
}
 
extern int RegisterString(string &T);

void CBuild::Run	(string& P)
{
	path			= P+"\\";

	CFS_File fs		((path+"level.").c_str());

	fs.open_chunk	(fsL_HEADER);
	hdrLEVEL H;		ZeroMemory(&H,sizeof(H));
	H.XRLC_version = XRCL_PRODUCTION_VERSION;
	strcpy			(H.name,g_params.L_name);
	fs.write		(&H,sizeof(H));
	fs.close_chunk	();

	FPU::m64r		();
	Phase			("Tesselating...");
	mem_Compact		();
	Tesselate		();
	
	FPU::m64r		();
	Phase			("Optimizing...");
	mem_Compact		();
	PreOptimize		();

	FPU::m64r		();
	Phase			("Building RayCast model...");
	mem_Compact		();
	BuildRapid		();

	FPU::m64r		();
	Phase			("Checking T-Junctions...");
	mem_Compact		();
	CorrectTJunctions();

	FPU::m64r		();
	Phase			("Building normals...");
	mem_Compact		();
	CalcNormals		();
	
	FPU::m64r();
	Phase			("Building collision database...");
	mem_Compact		();
	BuildCForm		(fs);

	FPU::m64r		();
	Phase			("Building volume-pick database...");
	mem_Compact		();
	BuildPortals	(fs);

	FPU::m64r		();
	Phase			("Resolving materials...");
	mem_Compact		();
	xrPhase_ResolveMaterials	();
	IsolateVertices	();
	
	FPU::m64r		();
	Phase			("Build UV mapping...");
	mem_Compact		();
	xrPhase_UVmap	();
	IsolateVertices	();
	
	FPU::m64r		();
	Phase			("Subdividing geometry...");
	mem_Compact		();
	xrPhase_Subdivide();
	IsolateVertices	();

	// Caching opacity
	Status("Caching faces opacity...");
	for (vecFaceIt I=g_faces.begin(); I!=g_faces.end(); I++) (*I)->CacheOpacity();

	// Implicit
	FPU::m64r		();
	Phase			("Implicit lighting...");
	mem_Compact		();
	ImplicitLighting();

	// All lighting
	FPU::m64r		();
	Phase			("Lighting...");
	mem_Compact		();
	Light			();
	
	FPU::m64r		();
	Phase			("Calculating vertex lighting...");
	mem_Compact		();
	LightVertex		();
	
	FPU::m64r		();
	Phase			("Merging lightmaps...");
	mem_Compact		();
	xrPhase_MergeLM	();

	FPU::m64r		();
	Phase			("Merging geometry...");
	mem_Compact		();
	xrPhase_MergeGeometry		();
	
	FPU::m64r		();
	Phase			("Converting to OpenGraphicsFormat...");
	mem_Compact		();
	Flex2OGF		();

	FPU::m64r		();
	Phase			("Building LOD-models...");
	mem_Compact		();
	Flex2LOD		();

	FPU::m64r		();
	Phase			("Building sectors...");
	mem_Compact		();
	BuildSectors	();

	FPU::m64r		();
	Phase			("Saving lights, glows, occlusion planes...");
	mem_Compact		();
	SaveLights		(fs);

	// Glows
	fs.open_chunk(fsL_GLOWS);
	for (DWORD i=0; i<glows.size(); i++)
	{
		b_glow&	G = glows[i];
		fs.write(&G,4*sizeof(float));
		string T = textures		[materials[G.dwMaterial].surfidx].name;
		string S = shader_render[materials[G.dwMaterial].shader].name;
		fs.Wdword(RegisterString(T));
		fs.Wdword(RegisterString(S));
	}
	fs.close_chunk();

	FPU::m64r		();
	Phase			("Saving static geometry...");
	mem_Compact		();
	SaveTREE		(fs);

	Phase			("Saving sectors...");
	mem_Compact		();
	SaveSectors		(fs);
}
