#include "stdafx.h"
#include "xrHemisphere.h"
#include "xrThread.h"
#include "xrSyncronize.h"

#pragma todo ("REMOVE THIS FILE");

/*
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
*/

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
		// Priority
		SetThreadPriority		(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
		Sleep					(0);

		// 
		CDB::COLLIDER			DB;
		DB.ray_options			(0);

		for (u32 I = vertStart; I<vertEnd; I++)
		{
			Vertex* V		= g_vertices[I];
			R_ASSERT		(V);

			base_color_c	C,tmp;
			LightPoint		(&DB, RCAST_Model, C, V->P, V->N, pBuild->L_static, LP_dont_rgb+LP_dont_sun,0);
			V->C._set		(C);
			thProgress		= float(I - vertStart) / float(vertEnd-vertStart);
		}
	}
};

void CBuild::Light_R2			()
{
	// start V-light
	/*
	CThreadManager			Threads;
	u32	stride				= g_vertices.size()/NUM_THREADS;
	u32	last				= g_vertices.size()-stride*(NUM_THREADS-1);
	for (u32 thID=0; thID<NUM_THREADS; thID++)
		Threads.start(xr_new<CR2Light>(thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride)));
	*/

	// impl
	FPU::m64r				();
	Phase					("LIGHT: Implicit...");
	ImplicitLighting		();
	mem_Compact				();

	// hemi
	/*
	FPU::m64r				();
	Phase					("LIGHT: Hemisphere...");
	Status					("Calculating... (%d lights)",L_static.hemi.size());
	CTimer	start_time;		start_time.Start();				
	Threads.wait			();
	clMsg					("%f seconds",start_time.GetElapsed_sec());
	*/
}
