#include "stdafx.h"
#include "build.h"
#include "xrThread.h"

extern void LightPoint(RAPID::XRCollide* DB, Fcolor &C, Fvector &P, Fvector &N, R_Light* begin, R_Light* end);

void CBuild::Light()
{
	vecDeflIt it;

	Phase	(
		(string("Raytracing lightmaps (")+
		string(g_params.m_bRadiosity?"Radiosity":"Direct") +
		string(" lighting)...")).c_str()
		);
	Status("Preparing...");
	for (it = g_deflectors.begin(); it!=g_deflectors.end(); it++)
		(*it)->Prepare();

	for (it = g_deflectors.begin(); it!=g_deflectors.end(); it++)
	{
		DWORD	N = it-g_deflectors.begin();
		float	P = float(N)/float(g_deflectors.size());
		Status("Calculating surface #%d...",N);
		Progress(P);

		(*it)->Light(P);
	}

	Phase("Saving shadowmaps...");
	for (it = g_deflectors.begin(); it!=g_deflectors.end(); it++)
	{
		DWORD	N = it-g_deflectors.begin();
		Status("Compressing surface #%d...",N);
		Progress(float(N)/float(g_deflectors.size()));
		(*it)->Save();
	}
}

//-----------------------------------------------------------------------
extern BOOL	hasImplicitLighting(Face* F);

class CVertexLightThread : public CThread
{
public:
	DWORD	faceStart, faceEnd;
	
	CVertexLightThread(DWORD ID, DWORD _start, DWORD _end) : CThread(ID)
	{
		faceStart	= _start;
		faceEnd		= _end;
	}
	virtual void		Execute	()
	{
		RAPID::XRCollide	DB;
		DB.RayMode			(RAY_CULL);
		
		vector<R_Light>	Lights = pBuild->lights_soften;
		if (Lights.empty())		return;
		
		for (DWORD I = faceStart; I<faceEnd; I++)
		{
			Face* F = g_faces[I];
			if (F->pDeflector)				continue;
			if (hasImplicitLighting(F))		continue;
			
			float v_amb	= F->Shader().cl_VC_Ambient;
			float v_inv = 1.f-v_amb;
			for (int v=0; v<3; v++)
			{
				Vertex* V = F->v[v];
				if (V->Color) continue;
				
				Fcolor		C,R,Lumel;
				C.set		(0,0,0,0);
				LightPoint	(&DB, C, V->P, V->N, Lights.begin(), Lights.end());
				
				R.r			= C.r*v_inv+v_amb;
				R.g			= C.g*v_inv+v_amb;
				R.b			= C.b*v_inv+v_amb;
				Lumel.lerp	(R,g_params.m_lm_amb_color,g_params.m_lm_amb_fogness);
				Lumel.a		= 1.f;
				
				V->Color	= Lumel.get();
			}
		}
		thProgress	= float(I - faceStart) / float(faceEnd-faceStart);
	}
};

#define NUM_THREADS	8
void CBuild::LightVertex()
{
	// Start threads, wait, continue --- perform all the work
	DWORD	start_time		= timeGetTime();
	CThreadManager			Threads;
	DWORD	stride			= g_faces.size()/NUM_THREADS;
	DWORD	last			= g_faces.size()-stride*(NUM_THREADS-1);
	for (DWORD thID=0; thID<NUM_THREADS; thID++)
		Threads.start(new CVertexLightThread(thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride)));
	Threads.wait			();
	Msg("%d seconds elapsed.",(timeGetTime()-start_time)/1000);
}
