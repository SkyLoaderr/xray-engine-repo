#include "stdafx.h"
#include "build.h"
#include "xrThread.h"

extern void LightPoint(RAPID::XRCollide* DB, Fcolor &C, Fvector &P, Fvector &N, R_Light* begin, R_Light* end);

class CLMThread : public CThread
{
public:
	CDeflector *	volatile defl;
	BOOL			volatile bContinue;
public:
	CLMThread	(DWORD ID, CDeflector* D) : CThread(ID)
	{
		defl		= D;
		thMessages	= FALSE;
		bContinue	= TRUE;
	}

	virtual void	Execute()
	{
		while (bContinue) 
		{
			defl->Light	();
			defl = 0;
			
			while ((0==defl) && bContinue) Sleep(1);
		}
	}
};

void CBuild::Light()
{
	Phase	(
		(string("Raytracing lightmaps (")+
		string(g_params.m_bRadiosity?"Radiosity":"Direct") +
		string(" lighting)...")).c_str()
		);

	// Main process (4 threads)
	const	DWORD			thNUM = 4;
	DWORD	dwTimeStart = timeGetTime();
	CLMThread*	threads[thNUM];
	ZeroMemory	(threads,sizeof(threads));
	DWORD		N=0;
	for (;;) {
		for (int L=0; L<thNUM; L++) {
			if ((0==threads[L]) || (0==threads[L]->defl))
			{
				if (N>=g_deflectors.size())		continue;
				if (0==threads[L])	{
					// Start NEW thread
					threads[L]			= new CLMThread(N,g_deflectors[N]);	N++;
					threads[L]->Start	();
				} else {
					// Use existing one
					threads[L]->defl	= g_deflectors[N];
				}
				
				// Info
				float	P = float(N)/float(g_deflectors.size());
				Progress(P);
				Status	("Calculating surface up to #%d...",N);
			}
		}
		DWORD	thOK	= 0;
		for (L=0; L<thNUM; L++)	thOK += (threads[L]?0:1);
		if		(thOK==thNUM)	break;
		Sleep	(50);
	}
	for (int L=0; L<thNUM; L++)	{
		threads[L]->bContinue	= FALSE;
		while	(!threads[L]->thCompleted) Sleep(1);
		_DELETE	(threads[L]);
	}
	Msg("%d seconds",(timeGetTime()-dwTimeStart)/1000);
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
