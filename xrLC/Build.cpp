// Build.cpp: implementation of the CBuild class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Build.h"
#include "Image.h"

vector<OGF_Base *>		g_tree;
b_params				g_params;
extern u32				dwInvalidFaces;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class T>
void transfer(const char *name, vector<T> &dest, CStream& F, u32 chunk)
{
	CStream*	O		= F.OpenChunk(chunk);
	u32		count	= O?(O->Length()/sizeof(T)):0;
	Msg			("* %16s: %d",name,count);
	if (count)  
	{
		dest.reserve(count);
		dest.insert	(dest.begin(), (T*)O->Pointer(), (T*)O->Pointer() + count);
	}
	if (O)		O->Close	();
}

extern u32*	Surface_Load	(char* name, u32& w, u32& h);
extern void		Surface_Init	();

CBuild::CBuild	(b_params& Params, CStream& FS)
{
	// HANDLE		hLargeHeap	= HeapCreate(0,64*1024*1024,0);
	// Msg			("* <LargeHeap> handle: %X",hLargeHeap);

	u32			i			= 0;

	float			p_total		= 0;
	float			p_cost		= 1.f/3.f;
	
	CStream*		F			= 0;

	// 
	shaders.Load				("gamedata\\shaders_xrlc.xr");
	
	//*******
	Status					("Vertices...");
	{
		F = FS.OpenChunk		(EB_Vertices);
		u32 v_count			=	F->Length()/sizeof(b_vertex);
		g_vertices.reserve		(3*v_count/2);
		scene_bb.invalidate		();
		for (i=0; i<v_count; i++)
		{
			Vertex*	pV			= VertexPool.create();
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
		u32 f_count			=	F->Length()/sizeof(b_face);
		g_faces.reserve			(f_count);
		for (i=0; i<f_count; i++)
		{
			try 
			{
				Face*	_F			= FacePool.create();
				b_face	B;
				F->Read				(&B,sizeof(B));

				_F->dwMaterial		= WORD(B.dwMaterial);
				_F->dwMaterialGame	= B.dwMaterialGame;

				// Vertices and adjacement info
				for (u32 it=0; it<3; it++)
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
				err_save	();
				Fatal		("* ERROR: Can't process face #%d",i);
			}
		}
		Progress			(p_total+=p_cost);
		Msg					("* %16s: %d","faces",g_faces.size());
		F->Close			();

		if (dwInvalidFaces)	
		{
			err_save		();
			Fatal			("* FATAL: %d invalid faces. Compilation aborted",dwInvalidFaces);
		}
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
				u32 cnt			= F->Rdword();
				temp.control.data.resize(cnt);
				F->Read				(temp.control.data.begin(),cnt*sizeof(u32));

				L_layers.push_back	(temp);
			}

			F->Close		();
		}
		// Static
		{
			F = FS.OpenChunk(EB_Light_static);
			b_light_static	temp;
			u32 cnt		= F->Length()/sizeof(temp);
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
		u32 tex_count	= F->Length()/sizeof(b_texture);
		for (u32 t=0; t<tex_count; t++)
		{
			Progress		(float(t)/float(tex_count));

			b_texture		TEX;
			F->Read			(&TEX,sizeof(TEX));

			b_BuildTexture	BT;
			CopyMemory		(&BT,&TEX,sizeof(TEX));

			// load thumbnail
			LPSTR N			= BT.name;
			if (strchr(N,'.')) *(strchr(N,'.')) = 0;
			strlwr			(N);
			char th_name[256]; strconcat(th_name,"\\\\x-ray\\stalkerdata$\\textures\\",N,".thm");
			CCompressedStream THM	(th_name,THM_SIGN);

			// analyze thumbnail information
			R_ASSERT		(THM.ReadChunk(THM_CHUNK_TEXTUREPARAM,&BT.THM));
			BOOL			bLOD=FALSE;
			if (N[0]=='l' && N[1]=='o' && N[2]=='d' && N[3]=='\\') bLOD = TRUE;

			// load surface if it has an alpha channel or has "implicit lighting" flag
			BT.dwWidth	= BT.THM.width;
			BT.dwHeight	= BT.THM.height;
			BT.bHasAlpha= BT.THM.HasAlphaChannel();
			if (!bLOD) 
			{
				if (BT.bHasAlpha || BT.THM.flags.test(STextureParams::flImplicitLighted))
				{
					Msg			("- loading: %s",N);
					u32			w=0, h=0;
					BT.pSurface = Surface_Load(N,w,h);
					BT.Vflip	();
				} else {
					// Free surface memory
				}
			}

			// save all the stuff we've created
			textures.push_back	(BT);
		}
	}

	// post-process materials
	Status	("Post-process materials...");
	for (u32 m=0; m<materials.size(); m++)
	{
		b_material &M	= materials[m];

		if (65535==M.shader_xrlc)	{
			// No compiler shader
			M.reserved	= WORD(-1);
			Msg		("*** %20s",shader_render[M.shader].name);
		} else {
			Msg		("*** %20s / %-20s",shader_render[M.shader].name, shader_compile[M.shader_xrlc].name);
			int id = shaders.GetID(shader_compile[M.shader_xrlc].name);
			if (id<0) {
				Msg		("ERROR: Shader '%s' not found in library",shader_compile[M.shader].name);
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

	// 
	Msg	("* sizes: V(%d),F(%d)",sizeof(Vertex),sizeof(Face));
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
	for (u32 i=0; i<glows.size(); i++)
	{
		b_glow&	G = glows[i];
		fs.write(&G,4*sizeof(float));
		string T = textures		[materials[G.dwMaterial].surfidx].name;
		string S = shader_render[materials[G.dwMaterial].shader].name;
		fs.Wdword(RegisterString(T));
		fs.Wdword(RegisterString(S));
	}
	fs.close_chunk	();

	FPU::m64r		();
	Phase			("Saving static geometry...");
	mem_Compact		();
	SaveTREE		(fs);

	Phase			("Saving sectors...");
	mem_Compact		();
	SaveSectors		(fs);

	err_save		();
}

void CBuild::err_save	()
{
	string256		log_name,log_user;
	u32			buffer_size		= 128;
	GetUserName		(log_user,(DWORD*)&buffer_size);
	strconcat		(log_name,"build_",strlwr(log_user),".err");

	CFS_File		err(log_name);

	// t-junction
	err.open_chunk	(0);
	err.Wdword		(err_tjunction.size()/(1*sizeof(Fvector)));
	err.write		(err_tjunction.pointer(), err_tjunction.size());
	err.close_chunk	();

	// m-edje
	err.open_chunk	(1);
	err.Wdword		(err_multiedge.size()/(2*sizeof(Fvector)));
	err.write		(err_multiedge.pointer(), err_multiedge.size());
	err.close_chunk	();

	// invalid
	err.open_chunk	(2);
	err.Wdword		(err_invalid.size()/(3*sizeof(Fvector)));
	err.write		(err_invalid.pointer(), err_invalid.size());
	err.close_chunk	();
}
