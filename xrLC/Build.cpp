// Build.cpp: implementation of the CBuild class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Build.h"
#include "Image.h"

vector<OGF_Base *>		g_tree;

//////////////////////////////////////////////////////////////////////

CBuild::CBuild()
{
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

	// All lighting + lmaps building and saving
	Light			();

	FPU::m64r		();
	Phase			("Merging geometry...");
	mem_Compact		();
	xrPhase_MergeGeometry		();
	
	FPU::m64r		();
	Phase			("Converting to OpenGraphicsFormat...");
	mem_Compact		();
	Flex2OGF		();

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
