// Build.cpp: implementation of the CBuild class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrThread.h"
#include "xrSyncronize.h"

xr_vector<OGF_Base *>		g_tree;
BOOL					b_R2	= FALSE;

//////////////////////////////////////////////////////////////////////

CBuild::CBuild()
{
}

CBuild::~CBuild()
{
}
 
extern int RegisterString(std::string &T);


class CMUThread : public CThread
{
public:
	CMUThread	(DWORD ID) : CThread(ID)
	{
		thMessages	= FALSE;
	}
	virtual void	Execute()
	{
		u32 m;

		// Priority
		SetThreadPriority	(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
		Sleep				(0);

		// Light models
		for (m=0; m<pBuild->mu_models.size(); m++)
		{
			pBuild->mu_models[m]->calc_materials();
			pBuild->mu_models[m]->calc_lighting	();
		}

		// Light references
		for (m=0; m<pBuild->mu_refs.size(); m++)
		{
			pBuild->mu_refs[m]->calc_lighting	();
			thProgress					= (float(m)/float(pBuild->mu_refs.size()));
		}
	}
};

void CBuild::Run	(std::string& P)
{
	//****************************************** Open Level
	path						= P+"\\";
	IWriter* fs					= FS.w_open((path+"level.").c_str());
	fs->open_chunk				(fsL_HEADER);
	hdrLEVEL H;		ZeroMemory	(&H,sizeof(H));
	H.XRLC_version				= XRCL_PRODUCTION_VERSION;
	strcpy						(H.name,g_params.L_name);
	fs->w						(&H,sizeof(H));
	fs->close_chunk				();

	//****************************************** Tesselating
	FPU::m64r					();
	Phase						("Tesselating...");
	mem_Compact					();
	Tesselate					();
	
	//****************************************** Optimizing
	FPU::m64r					();
	Phase						("Optimizing...");
	mem_Compact					();
	PreOptimize					();

	//****************************************** RayCast model
	FPU::m64r					();
	Phase						("Building RayCast model...");
	mem_Compact					();
	BuildRapid					();

	//****************************************** Checking T-junctors
	FPU::m64r					();
	Phase						("Checking T-Junctions...");
	mem_Compact					();
	CorrectTJunctions			();

	//****************************************** Building normals
	FPU::m64r					();
	Phase						("Building normals...");
	mem_Compact					();
	CalcNormals					();

	//****************************************** T-Basis
	if (b_R2)	//****************************** R2 only
	{
		FPU::m64r					();
		Phase						("Building tangent-basis...");
		xrPhase_TangentBasis		();
		mem_Compact					();
	}

	//****************************************** Collision DB
	FPU::m64r();
	Phase						("Building collision database...");
	mem_Compact					();
	BuildCForm					(*fs);

	//****************************************** Portals
	FPU::m64r					();
	Phase						("Building portals...");
	mem_Compact					();
	BuildPortals				(*fs);

	//****************************************** Modify lighting
	// If it is R2 - leave just high quality identical hemisphere
	if (b_R2)					xrPhase_R2_Lights();

	//****************************************** Starting MU
	FPU::m64r					();
	Phase						("LIGHT: Starting MU...");
	mem_Compact					();
	for (vecFaceIt I=g_faces.begin(); I!=g_faces.end(); I++) (*I)->CacheOpacity();
	for (u32 m=0; m<mu_models.size(); m++) mu_models[m]->calc_faceopacity();

	CThreadManager				mu;
	mu.start					(xr_new<CMUThread> (0));

	//****************************************** Resolve materials
	FPU::m64r					();
	Phase						("Resolving materials...");
	mem_Compact					();
	xrPhase_ResolveMaterials	();
	IsolateVertices				();

	//****************************************** UV mapping
	if (!b_R2)	//****************************** R1 only
	{
		FPU::m64r					();
		Phase						("Build UV mapping...");
		mem_Compact					();
		xrPhase_UVmap				();
		IsolateVertices				();
	}

	//****************************************** Subdivide geometry
	FPU::m64r					();
	Phase						("Subdividing geometry...");
	mem_Compact					();
	xrPhase_Subdivide			();
	IsolateVertices				();

	//****************************************** All lighting + lmaps building and saving
	if (b_R2)					Light_R2		();
	else						Light			();

	//****************************************** Merge geometry
	FPU::m64r					();
	Phase						("Merging geometry...");
	mem_Compact					();
	xrPhase_MergeGeometry		();

	//****************************************** Convert to OGF
	FPU::m64r					();
	Phase						("Converting to OpenGraphicsFormat...");
	mem_Compact					();
	Flex2OGF					();

	//****************************************** Wait for MU
	FPU::m64r					();
	Phase						("LIGHT: Waiting for MU-thread...");
	mem_Compact					();
	mu.wait						(500);

	//****************************************** Export MU-models
	{
		u32 m;
		Status			("MU : Models...");
		for (m=0; m<mu_models.size(); m++)
		{
			mu_models[m]->calc_ogf			();
			mu_models[m]->export_geometry	();
		}

		Status			("MU : References...");
		for (m=0; m<mu_refs.size(); m++)
			mu_refs[m]->export_ogf		();
	}

	//****************************************** Destroy RCast-model
	FPU::m64r		();
	Phase			("Destroying ray-trace model...");
	mem_Compact		();
	xr_delete		(RCAST_Model);

	//****************************************** Build sectors
	FPU::m64r		();
	Phase			("Building sectors...");
	mem_Compact		();
	BuildSectors	();

	//****************************************** Saving MISC stuff
	FPU::m64r		();
	Phase			("Saving...");
	mem_Compact		();
	SaveLights		(*fs);

	fs->open_chunk	(fsL_GLOWS);
	for (u32 i=0; i<glows.size(); i++)
	{
		b_glow&	G	= glows[i];
		fs->w		(&G,4*sizeof(float));
		std::string T	= textures		[materials[G.dwMaterial].surfidx].name;
		std::string S	= shader_render	[materials[G.dwMaterial].shader].name;
		fs->w_u32	(RegisterString(T));
		fs->w_u32	(RegisterString(S));
	}
	fs->close_chunk	();

	SaveTREE		(*fs);
	SaveSectors		(*fs);

	err_save		();
}

void CBuild::err_save	()
{
	string256		log_name;
	strconcat		(log_name,"logs\\build_",Core.UserName,".err");

	IWriter*		fs	= FS.w_open(log_name);
	IWriter&		err = *fs;

	// t-junction
	err.open_chunk	(0);
	err.w_u32		(err_tjunction.size()/(1*sizeof(Fvector)));
	err.w			(err_tjunction.pointer(), err_tjunction.size());
	err.close_chunk	();

	// m-edje
	err.open_chunk	(1);
	err.w_u32		(err_multiedge.size()/(2*sizeof(Fvector)));
	err.w			(err_multiedge.pointer(), err_multiedge.size());
	err.close_chunk	();

	// invalid
	err.open_chunk	(2);
	err.w_u32		(err_invalid.size()/(3*sizeof(Fvector)));
	err.w			(err_invalid.pointer(), err_invalid.size());
	err.close_chunk	();
}
