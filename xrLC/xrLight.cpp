#include "stdafx.h"
#include "build.h"
#include "xrThread.h"
#include "xrSyncronize.h"

xrCriticalSection					task_CS;
xr_vector<int>						task_pool;

class CLMThread : public CThread
{
public:
	HASH							H;
	CDB::COLLIDER					DB;
	xr_vector<R_Light>				LightsSelected;
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
typedef	mapVert::iterator			mapVertIt;
mapVert*							g_trans;
xrCriticalSection					g_trans_CS;

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
	// clMsg		("K:%f, L:%f, U:%f",key,it->first,it2->first);
	
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

vecFace*	VL_faces;

class CVertexLightThread : public CThread
{
public:
	u32	faceStart, faceEnd;
	
	CVertexLightThread(u32 ID, u32 _start, u32 _end) : CThread(ID)
	{
		thMessages	= FALSE;
		faceStart	= _start;
		faceEnd		= _end;
	}
	virtual void		Execute	()
	{
		CDB::COLLIDER	DB;
		DB.ray_options	(0);
		
		xr_vector<R_Light>	Lights = pBuild->L_layers.front().lights;
		if (Lights.empty())		return;
		
		for (u32 I = faceStart; I<faceEnd; I++)
		{
			Face* F		= (*VL_faces)[I];
			R_ASSERT	(F);
			
			float v_amb	= F->Shader().vert_ambient;
			float v_inv = 1.f-v_amb;
			for (int v=0; v<3; v++)
			{
				Vertex* V		= F->v[v];
				R_ASSERT		(V);
				if (!fis_zero(V->Color.a))	continue;
				
				Fcolor			C;
				C.set			(0,0,0,0);
				LightPoint		(&DB, RCAST_Model, C, V->P, V->N, &*Lights.begin(), &*Lights.end(), F);
				
				V->Color.r		= C.r*v_inv+v_amb;
				V->Color.g		= C.g*v_inv+v_amb;
				V->Color.b		= C.b*v_inv+v_amb;
				V->Color.a		= F->Shader().vert_translucency;
				g_trans_register(V);
			}

			thProgress	= float(I - faceStart) / float(faceEnd-faceStart);
		}
	}
};

#define NUM_THREADS	12
void CBuild::LightVertex()
{
	VL_faces				= xr_new<vecFace>	();
	g_trans					= xr_new<mapVert>	();

	// Select faces
	Status					("Selecting...");
	VL_faces->reserve		(g_faces.size()/2);
	for (u32 I = 0; I<g_faces.size(); I++)
	{
		Face* F = g_faces[I];
		if (F->pDeflector)					continue;
		if (hasImplicitLighting(F))			continue;
		if (!F->Shader().flags.bRendering)	continue;

		VL_faces->push_back					(F);
	}
	clMsg	("%d/%d selected.",VL_faces->size(),g_faces.size());

	// Start threads, wait, continue --- perform all the work
	Status					("Calculating...");
	u32	start_time		= timeGetTime();
	CThreadManager			Threads;
	u32	stride			= VL_faces->size()/NUM_THREADS;
	u32	last			= VL_faces->size()-stride*(NUM_THREADS-1);
	for (u32 thID=0; thID<NUM_THREADS; thID++)
		Threads.start(xr_new<CVertexLightThread>(thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride)));

	// Wait other threads
	Threads.wait			();
	clMsg("%d seconds elapsed.",(timeGetTime()-start_time)/1000);

	// Process all groups
	Status					("Transluenting...");
	for (mapVertIt it=g_trans->begin(); it!=g_trans->end(); it++)
	{
		// Unique
		vecVertex&	VL		= it->second;
		std::sort	(VL.begin(),VL.end());
		VL.erase	(std::unique(VL.begin(),VL.end()),VL.end());

		// Calc summary color
		Fcolor		C;
		C.set		(0,0,0,0);
		for (int v=0; v<int(VL.size()); v++)
		{
			C.r = _max(C.r,VL[v]->Color.r);
			C.g = _max(C.g,VL[v]->Color.g);
			C.b = _max(C.b,VL[v]->Color.b);
		}

		// Calculate final vertex color
		for (v=0; v<int(VL.size()); v++)
		{
			// trans-level
			float	level		= VL[v]->Color.a;

			// 
			Fcolor				R;
			R.lerp				(VL[v]->Color,C,level);
			R.r					= _max(R.r,VL[v]->Color.r);
			R.g					= _max(R.g,VL[v]->Color.g);
			R.b					= _max(R.b,VL[v]->Color.b);
			VL[v]->Color.lerp	(R,g_params.m_lm_amb_color,g_params.m_lm_amb_fogness);
			VL[v]->Color.mul_rgb(.5f);
			VL[v]->Color.a		= 1.f;
		}
	}

	xr_delete(VL_faces);
	xr_delete(g_trans);
}
