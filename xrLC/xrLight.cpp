#include "stdafx.h"
#include "build.h"
#include "xrThread.h"
#include "xrSyncronize.h"

xrCriticalSection	task_CS;
xr_vector<int>		task_pool;

class CLMThread		: public CThread
{
public:
	HASH			H;
	CDB::COLLIDER	DB;
	base_lighting	LightsSelected;
public:
	CLMThread	(u32 ID) : CThread(ID)
	{
		// thMonitor= TRUE;
		thMessages	= FALSE;
	}

	virtual void	Execute()
	{
		CDeflector* D	= 0;

		for (;;) 
		{
			// Get task
			task_CS.Enter		();
			thProgress			= 1.f - float(task_pool.size())/float(g_deflectors.size());
			if (task_pool.empty())	
			{
				task_CS.Leave		();
				return;
			}

			D					= g_deflectors[task_pool.back()];
			task_pool.pop_back	();
			task_CS.Leave		();

			// Perform operation
			try {
				D->Light	(&DB,&LightsSelected,H);
			} catch (...)
			{
				clMsg("* ERROR: CLMThread::Execute - light");
			}
		}
	}
};

void CBuild::Light()
{
	//****************************************** Implicit
	if (!b_R2)	//****************************** R1 only
	{
		FPU::m64r		();
		Phase			("LIGHT: Implicit...");
		mem_Compact		();
		ImplicitLighting();
	}

	//****************************************** Lmaps
	if (!b_R2)	//****************************** R1 only
	{
		FPU::m64r		();
		Phase			("LIGHT: LMaps...");
		mem_Compact		();

		// Randomize deflectors
		std::random_shuffle	(g_deflectors.begin(),g_deflectors.end());
		for					(u32 dit = 0; dit<g_deflectors.size(); dit++)	task_pool.push_back(dit);

		// Main process (4 threads)
		Status			("Lighting...");
		CThreadManager	threads;
		const	u32	thNUM	= 6;
		u32	dwTimeStart	= timeGetTime();
		for				(int L=0; L<thNUM; L++)	threads.start(xr_new<CLMThread> (L));
		threads.wait	(500);
		clMsg			("%d seconds",(timeGetTime()-dwTimeStart)/1000);
	}

	//****************************************** Vertex
	FPU::m64r		();
	Phase			("LIGHT: Vertex...");
	mem_Compact		();

	LightVertex		();

	//****************************************** Merge LMAPS
	if (!b_R2)	//****************************** R1 only
	{
		FPU::m64r		();
		Phase			("LIGHT: Merging lightmaps...");
		mem_Compact		();

		xrPhase_MergeLM	();
	}
}

//-----------------------------------------------------------------------
extern BOOL	hasImplicitLighting		(Face* F);

typedef	xr_multimap<float,vecVertex>	mapVert;
typedef	mapVert::iterator				mapVertIt;
mapVert*								g_trans;
xrCriticalSection						g_trans_CS;

void	g_trans_register_internal	(Vertex* V)
{
	R_ASSERT	(V);

	const float eps		= EPS_L;
	const float eps2	= 2.f*eps;
	
	// Search
	const float key		= V->P.x;
	mapVertIt	it		= g_trans->lower_bound	(key);
	mapVertIt	it2		= it;

	// Decrement to the start and inc to end
	while (it!=g_trans->begin() && ((it->first+eps2)>key)) it--;
	while (it2!=g_trans->end() && ((it2->first-eps2)<key)) it2++;
	if (it2!=g_trans->end())	it2++;
	
	// Search
	for (; it!=it2; it++)
	{
		vecVertex&	VL		= it->second;
		Vertex* Front		= VL.front();
		R_ASSERT			(Front);
		if (Front->P.similar(V->P,eps))
		{
			VL.push_back		(V);
			return;
		}
	}

	// Register
	mapVertIt	ins			= g_trans->insert(mk_pair(key,vecVertex()));
	ins->second.reserve		(32);
	ins->second.push_back	(V);
}
void	g_trans_register	(Vertex* V)
{
	g_trans_CS.Enter			();
	g_trans_register_internal	(V);
	g_trans_CS.Leave			();
}

class CVertexLightThread : public CThread
{
public:
	u32	vertStart, vertEnd;
	
	CVertexLightThread(u32 ID, u32 _start, u32 _end) : CThread(ID)
	{
		thMessages	= FALSE;
		vertStart	= _start;
		vertEnd		= _end;
	}
	virtual void		Execute	()
	{
		CDB::COLLIDER	DB;
		DB.ray_options	(0);
		
		for (u32 I = vertStart; I<vertEnd; I++)
		{
			Vertex* V		= g_vertices[I];
			R_ASSERT		(V);
			
			// Get ambient factor
			float		v_amb		= 0.f;
			float		v_trans		= 0.f;
			u32 		L_flags		= 0;
			for (u32 f=0; f<V->adjacent.size(); f++)
			{
				Face*	F								=	V->adjacent[f];
				v_amb									+=	F->Shader().vert_ambient;
				v_trans									+=	F->Shader().vert_translucency;
				if (!F->Shader().flags.bLIGHT_Vertex)	L_flags=LP_dont_rgb+LP_dont_sun;
			}
			v_amb				/=	float(V->adjacent.size());
			v_trans				/=	float(V->adjacent.size());
			float v_inv			=	1.f-v_amb;

			base_color			vC;
			LightPoint			(&DB, RCAST_Model, vC, V->P, V->N, pBuild->L_static, L_flags, 0);
			vC._tmp_			= v_trans;
			V->C				= vC;
			if (L_flags)		g_trans_register	(V);

			thProgress	= float(I - vertStart) / float(vertEnd-vertStart);
		}
	}
};

#define NUM_THREADS			12
void CBuild::LightVertex	()
{
	g_trans					= xr_new<mapVert>	();

	// Start threads, wait, continue --- perform all the work
	Status				("Calculating...");
	u32	start_time		= timeGetTime();
	CThreadManager		Threads;
	u32	stride			= g_vertices.size()/NUM_THREADS;
	u32	last			= g_vertices.size()-stride*(NUM_THREADS-1);
	for (u32 thID=0; thID<NUM_THREADS; thID++)
		Threads.start	(xr_new<CVertexLightThread>(thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride)));
	Threads.wait		();
	clMsg				("%d seconds elapsed.",(timeGetTime()-start_time)/1000);

	// Process all groups
	Status				("Transluenting...");
	for (mapVertIt it=g_trans->begin(); it!=g_trans->end(); it++)
	{
		// Unique
		vecVertex&	VL	= it->second;
		std::sort		(VL.begin(),VL.end());
		VL.erase		(std::unique(VL.begin(),VL.end()),VL.end());

		// Calc summary color
		base_color	C;
		for (int v=0; v<int(VL.size()); v++)
			C.max		(VL[v]->C);

		// Calculate final vertex color
		for (v=0; v<int(VL.size()); v++)
		{
			// trans-level
			float	level		= VL[v]->C._tmp_;

			// 
			base_color			R;
			R.lerp				(VL[v]->C,C,level);
			R.max				(VL[v]->C);
			VL[v]->C			= R;
			VL[v]->C.mul		(.5f);
		}
	}

	xr_delete(g_trans);
}
