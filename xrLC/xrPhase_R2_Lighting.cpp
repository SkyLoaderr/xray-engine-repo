#include "stdafx.h"
#include "xrHemisphere.h"
#include "xrThread.h"
#include "xrSyncronize.h"

void __stdcall  hemi_callback	(float x, float y, float z, float E, LPVOID P)
{
	R_Light*	T				= (R_Light*)P;
	T->energy					= E;
	T->direction.set			(x,y,z);
	T->diffuse.set				(1,1,1,1);
	pBuild->L_hemi.push_back	(*T);
}

void CBuild::xrPhase_R2_Lights	()
{
	L_layers.clear				();
	L_hemi.clear				();

	R_Light						RL;
	RL.type                     = LT_DIRECT;
	RL.diffuse.set				(1,1,1,1);
	xrHemisphereBuild			(2,FALSE,1.f,1.f,hemi_callback,&RL);
}

#define NUM_THREADS	4
class CR2Light : public CThread
{
public:
	u32	vertStart, vertEnd;

	CR2Light			(u32 ID, u32 _start, u32 _end) : CThread(ID)
	{
		thMessages	= FALSE;
		vertStart	= _start;
		vertEnd		= _end;
	}
	virtual void		Execute	()
	{
		CDB::COLLIDER			DB;
		DB.ray_options			(0);

		xr_vector<R_Light>	Lights = pBuild->L_hemi;
		if (Lights.empty())		return;

		for (u32 I = vertStart; I<vertEnd; I++)
		{
			Vertex* V		= g_vertices[I];
			R_ASSERT		(V);

			Fcolor			C;
			C.set			(0,0,0,0);
			LightPoint		(&DB, RCAST_Model, C, V->P, V->N, &*Lights.begin(), &*Lights.end(), 0);
			V->C.set		(C);
			V->C.a			= 1.f;
			thProgress		= float(I - vertStart) / float(vertEnd-vertStart);
		}
	}
};

void CBuild::Light_R2			()
{
	FPU::m64r				();
	Phase					("LIGHT: Hemisphere...");
	mem_Compact				();

	// Start threads, wait, continue --- perform all the work
	Status					("Calculating... (%d lights)",L_hemi.size());
	u32	start_time			= timeGetTime();
	CThreadManager			Threads;
	u32	stride				= g_vertices.size()/NUM_THREADS;
	u32	last				= g_vertices.size()-stride*(NUM_THREADS-1);
	for (u32 thID=0; thID<NUM_THREADS; thID++)
		Threads.start(xr_new<CR2Light>(thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride)));
	Threads.wait			();
	clMsg					("%d seconds elapsed.",(timeGetTime()-start_time)/1000);
}
