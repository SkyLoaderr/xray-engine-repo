// Build.cpp: implementation of the CBuild class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Build.h"
#include "Image.h"
#include "xrShaderLib.h"
#include "xr_func.h"

//OGF_Base*	g_TREE_ROOT = 0;

vector<OGF_Base *>	g_tree;
vector<DetailPatch>	g_pathes;
b_params			g_params;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
template <class T>
void transfer(const char *name, vector<T> &dest, T* src, DWORD cnt )
{
	Msg("* %16s: %d",name,cnt);
	dest.reserve(cnt);
	dest.insert	(dest.begin(), src, src+cnt);
}

extern DWORD*	Surface_Load	(char* name, DWORD& w, DWORD& h);
extern void		Surface_Init	();

CBuild::CBuild(b_transfer * L)
{
	DWORD	i=0;

	float	p_total = 0;
	float	p_cost  = 1.f/3.f;

	Phase	("Loading shaders...");
	shLibrary_Load("x:\\game\\shaders.xr",shader_defs);

	Phase	("Converting data structures...");

	//*******
	Status	("Vertices...");
	scene_bb.invalidate	();
	for (i=0; i<L->vertex_count; i++)
	{
		Vertex*	pV  = new Vertex;
		pV->P.set(L->vertices[i]);
		pV->N.set(0,0,0);
		pV->Color	= 0;
		scene_bb.modify(pV->P);
	}
	Progress(p_total+=p_cost);
	Msg("%d vertices processed.",g_vertices.size());

	//*******
	Status	("Faces...");
	for (i=0; i<L->face_count; i++)
	{
		Face*	F		= new Face;
		b_face*	B		= &(L->faces[i]);

		F->dwMaterial	= WORD(B->dwMaterial);

		// Vertices and adjacement info
		for (DWORD it=0; it<3; it++)
		{
			int id = B->v[it];
			R_ASSERT(id<(int)g_vertices.size());
			F->SetVertex(it,g_vertices[id]);
		}

		// transfer TC
		DWORD	tCnt	= L->material[B->dwMaterial].dwTexCount;
		for (it=0; it<tCnt; it++) {
			UVpoint uv1,uv2,uv3;

			uv1.set(B->t[it][0].tu,B->t[it][0].tv);
			uv2.set(B->t[it][1].tu,B->t[it][1].tv);
			uv3.set(B->t[it][2].tu,B->t[it][2].tv);
			F->AddChannel( uv1, uv2, uv3 );
		}
	}
	Progress(p_total+=p_cost);
	Msg("%d faces processed.",g_faces.size());

	for (i=0; i<L->particle_count; i++)
	{
		DetailPatch DP;
		CopyMemory(&DP,&(L->particles[i]),sizeof(b_particle));
		g_pathes.push_back(DP);
	}
	Msg("%d detail patches processed.",g_pathes.size());

	//*******
	Status	("Other transfer...");
	transfer("materials",	materials,	L->material,	L->mtl_count);
	transfer("shaders",		shader_names,	L->shaders,	L->shader_count);
	transfer("glows",		glows,		L->glows,		L->glow_count);
	transfer("occluders",	occluders,	L->occluders,	L->occluder_count);
	transfer("portals",		portals,	L->portals,		L->portal_count);
	transfer("light_keys",	lights_keys,L->light_keys,	L->light_keys_count);

	Status	("Post-process lights...");
	// post-process lights
	for (DWORD l=0; l<L->light_count; l++) {
		b_light R = L->lights[l];
		R.direction.normalize_safe();
		if (R.flags & XRLIGHT_LMAPS)	lights_lmaps.push_back(R);
		if ((R.flags & XRLIGHT_MODELS)||(R.flags & XRLIGHT_PROCEDURAL)) lights_dynamic.push_back(R);
	}

	// process textures
	Status		("Processing textures...");
	Surface_Init();
	for (DWORD t=0; t<L->tex_count; t++)
	{
		Progress(float(t)/float(L->tex_count));

		b_texture&		TEX = L->textures[t];
		b_BuildTexture	BT;
		CopyMemory		(&BT,&TEX,sizeof(TEX));

		// load thumbnail
		LPSTR N			= BT.name;
		if (strchr(N,'.')) *(strchr(N,'.')) = 0;
		char th_name[256]; strconcat(th_name,"x:\\textures\\thumbnail\\",N,".thm");
		Msg("- THM: %s",N);
		CCompressedStream THM(th_name,"THM");

		// analyze thumbnail information
		R_ASSERT(THM.ReadChunk(THM_CHUNK_TEXTUREPARAM,&BT.THM));
		
		// load surface if it has an alpha channel or has "implicit lighting" flag
		BT.pSurface = 0;
		if (BT.bHasAlpha || (BT.THM.flag&EF_IMPLICIT_LIGHTED))	
		{
			Msg("- loaded: %s",N);
			// load & flip
			DWORD		w=0,h=0;
			BT.pSurface = Surface_Load(N,w,h);
			if ((BT.dwWidth!=w) || (BT.dwHeight!=h))
			{
				Msg	("* ERROR: Surface marked as [%d x %d], actually [%d x %d]",
					BT.dwWidth,BT.dwHeight,
					w,h
					);
				BT.dwWidth	= w;
				BT.dwHeight	= h;
			}

			BT.Vflip	();
		}
		
		// save all the stuff we've created
		textures.push_back	(BT);
	}

	// post-process materials
	Status	("Post-process materials...");
	for (DWORD m=0; m<materials.size(); m++)
	{
		b_material &M = materials[m];
		int id = shLibrary_Find(shader_names[M.shader].name,shader_defs);
		if (id<0) {
			Msg("ERROR: Shader '%s' not found in library",shader_names[M.shader].name);
			R_ASSERT(id>=0);
		}
		M.reserved = DWORD(id);
	}

	Progress(p_total+=p_cost);
	 
	// Parameter block
	CopyMemory(&g_params,&L->params,sizeof(b_params));
}

CBuild::~CBuild()
{
	Phase("Cleanup");
}
 
extern int	RegisterTexture	(string &T);
extern int	RegisterShader	(string &T);
void CBuild::Run()
{
	CFS_File fs((string(g_params.L_path)+"level.").c_str());
	fs.open_chunk(fsL_HEADER);
	hdrLEVEL H; ZeroMemory(&H,sizeof(H));
	H.XRLC_version = XRCL_PRODUCTION_VERSION;
	strcpy(H.name,g_params.L_name);
	fs.write(&H,sizeof(H));
	fs.close_chunk();

	FPU::m24r();
	Phase	("Optimizing...");
	PreOptimize		();

	FPU::m24r();
	Phase	("Checking T-Junctions...");
	CorrectTJunctions();

	FPU::m24r();
	Phase	("Building normals...");
	CalcNormals		();

	/*
	if (g_params.m_bTesselate) {
		FPU::m24r();
		Phase	("Tesselating curves...");
		Tesselate	();
		FPU::m24r();
		Phase	("Optimizing...");
		PreOptimize	();
		FPU::m24r();
		Phase	("Building normals...");
		CalcNormals	();
	}
	*/
	
	FPU::m24r();
	Phase	("Building collision database...");
	BuildCForm		(fs);

	FPU::m24r();
	Phase	("Building volume-pick database...");
	BuildPortals	(fs);

	FPU::m64r();
	Phase	("Building RayCast model...");
	BuildRapid		();

	FPU::m24r();
	Phase	("Resolving materials...");
	ResolveMaterials();
	
	FPU::m24r();
	Phase	("Build UV mapping...");
	BuildUVmap		();
	
	FPU::m24r();
	Phase	("Soften lights...");
	SoftenLights	();

	FPU::m24r();
	Phase	("Merging lightmaps...");
	MergeLM			();

	FPU::m24r();
	Phase	("Implicit lighting...");
	ImplicitLighting();

	FPU::m24r();
	Phase	("Raytracing...");
	Light			();
	
	FPU::m24r();
	Phase	("Merging geometry...");
	MergeGeometry();
	
	FPU::m24r();
	Phase	("Calculating vertex lighting...");
	LightVertex		();
	
	FPU::m24r();
	Phase	("Converting to OpenGraphicsFormat...");
	Flex2OGF		();

	FPU::m24r();
	Phase	("Processing DetailPatches...");
	LightPatches	();

	FPU::m24r();
	Phase	("Building sectors...");
	BuildSectors	();

	/*
	FPU::m24r();
	Phase	("Building hierrarhy...");
	BuildHierrarhy	();

	if (g_params.m_bTestOcclusion)
	{
		FPU::m24r();
		Phase	("Building PVS...");
		BuildPVS	();
	}

	FPU::m24r();
	Phase	("Building 'RelevantSet'...");
	BuildRelevance	(fs);
	*/

	FPU::m24r();
	Phase	("Saving lights, glows, occlusion planes...");
	SaveLights		(fs);

	// Glows
	fs.open_chunk(fsL_GLOWS);
	for (DWORD i=0; i<glows.size(); i++)
	{
		b_glow&	G = glows[i];
		fs.write(&G,4*sizeof(float));
		string T = textures[materials[G.dwMaterial].surfidx[0]].name;
		string S = shader_names[materials[G.dwMaterial].shader].name;
		fs.Wdword(RegisterTexture(T));
		fs.Wdword(RegisterShader(S));
	}
	fs.close_chunk();

	/*
	fs.open_chunk(fsL_PLANES);
	fs.write(occluders.begin(),occluders.size()*sizeof(b_occluder));
	fs.close_chunk();
	*/

	FPU::m24r();
	Phase	("Saving static geometry...");
	SaveTREE		(fs);

	Phase	("Saving sectors...");
	SaveSectors		(fs);
}
