#include "stdafx.h"
#include "xrHemisphere.h"
#include "xrThread.h"
#include "xrSyncronize.h"

void __stdcall  hemi_callback	(float x, float y, float z, float E, LPVOID P)
{
	R_Light*	T				= (R_Light*)P;
	T->energy					= E;
	T->direction.set			(x,y,z);
	pBuild->L_hemi.push_back	(*T);
}

void CBuild::xrPhase_R2_Lights	()
{
	L_layers.clear			();
	L_hemi.clear			();

	R_Light						RL;
	RL.type                     = D3DLIGHT_DIRECTIONAL;
	RL.diffuse.set				(1,1,1,1);
	xrHemisphereBuild			(2,FALSE,0.5f,1.f,hemi_callback,&RL);
	for (u32 it=0; it<L_hemi.size(); it++)
	{
		L_hemi[it].diffuse.mul_rgba	(L_hemi[it].energy);
	}
}
#define NUM_THREADS	6
class CR2Light : public CThread
{
public:
	DWORD	vertStart, vertEnd;

	CR2Light			(DWORD ID, DWORD _start, DWORD _end) : CThread(ID)
	{
		thMessages	= FALSE;
		vertStart	= _start;
		vertEnd		= _end;
	}
	virtual void		Execute	()
	{
		CDB::COLLIDER			DB;
		DB.ray_options			(0);

		vector<R_Light>	Lights = pBuild->L_hemi;
		if (Lights.empty())		return;

		for (DWORD I = vertStart; I<vertEnd; I++)
		{
			Vertex* V		= g_vertices[I];
			R_ASSERT		(V);

			Fcolor			C;
			C.set			(0,0,0,0);
			LightPoint		(&DB, RCAST_Model, C, V->P, V->N, Lights.begin(), Lights.end(), 0);
			V->Color.set	(C);
			V->Color.a		= 1.f;
			thProgress		= float(I - vertStart) / float(vertEnd-vertStart);
		}
	}
};

void CBuild::Light_R2			()
{
	// Start threads, wait, continue --- perform all the work
	Status					("Calculating...");
	DWORD	start_time		= timeGetTime();
	CThreadManager			Threads;
	DWORD	stride			= g_vertices.size()/NUM_THREADS;
	DWORD	last			= g_vertices.size()-stride*(NUM_THREADS-1);
	for (DWORD thID=0; thID<NUM_THREADS; thID++)
		Threads.start(xr_new<CR2Light>(thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride)));
	Threads.wait			();
	clMsg					("%d seconds elapsed.",(timeGetTime()-start_time)/1000);
}
