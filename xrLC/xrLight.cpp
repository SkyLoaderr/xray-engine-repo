#include "stdafx.h"
#include "build.h"
#include "xrThread.h"
#include "xrSyncronize.h"

class CLMThread : public CThread
{
public:
	CDeflector *		volatile	defl;
	BOOL				volatile	bContinue;
	HASH							H;
	CDB::COLLIDER					DB;
	vector<R_Light>					LightsSelected;
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
			try {
				defl->Light	(&DB,&LightsSelected,H);
			} catch (...)
			{
				Msg("* ERROR: CLMThread::Execute - light");
			}
			defl = 0;
			
			while ((0==defl) && bContinue) Sleep(1);
		}
	}
};

void CBuild::Light()
{
	// Randomize deflectors
	random_shuffle	(g_deflectors.begin(),g_deflectors.end());

	// Main process (4 threads)
	const	DWORD			thNUM = 5;
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
					threads[L]			= new CLMThread	(N,g_deflectors[N]);	N++;
					threads[L]->Start	();
				} else {
					// Use existing one
					threads[L]->defl	= g_deflectors[N]; N++;
				}
				
				// Info
				float	P = float(N)/float(g_deflectors.size());
				Progress(P);
				Status	("Calculating surface up to #%d (%d)...",N,g_deflectors.size());
			}
		}
		if	(N>=g_deflectors.size())	
		{
			DWORD	thOK	= 0;
			for		(L=0; L<thNUM; L++)	if ((0==threads[L]) || (0==threads[L]->defl))	thOK ++;
			if		(thOK==thNUM)	break;
		}
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

vector<vecVertex>			g_trans;
CCriticalSection			g_trans_CS;

void	g_trans_reg			(Vertex* V)
{
	// Search
	for (int it=0; it<g_trans.size(); it++)
	{
		vecVertex&	VL		= g_trans[it];
		if (VL.front()->P.similar(V->P,EPS_L))
		{
			VL.push_back	(V);
			return;
		}
	}

	// Register
	g_trans.push_back		(vecVertex());
	g_trans.back().reserve	(32);
	g_trans.back().push_back(V);
}

void	g_trans_register	(Vertex* V)
{
	g_trans_CS.Enter	();
	g_trans_reg			(V);
	g_trans_CS.Leave	();
}

class CVertexLightThread : public CThread
{
public:
	DWORD	faceStart, faceEnd;
	
	CVertexLightThread(DWORD ID, DWORD _start, DWORD _end) : CThread(ID)
	{
		thMessages	= FALSE;
		faceStart	= _start;
		faceEnd		= _end;
	}
	virtual void		Execute	()
	{
		CDB::COLLIDER	DB;
		DB.ray_options	(CDB::OPT_CULL);
		
		vector<R_Light>	Lights = pBuild->lights[0].lights;
		if (Lights.empty())		return;
		
		for (DWORD I = faceStart; I<faceEnd; I++)
		{
			Face* F = g_faces[I];
			if (F->pDeflector)				continue;
			if (hasImplicitLighting(F))		continue;
			
			float v_amb	= F->Shader().vert_ambient;
			float v_inv = 1.f-v_amb;
			for (int v=0; v<3; v++)
			{
				Vertex* V		= F->v[v];
				
				Fcolor			C;
				C.set			(0,0,0,0);
				LightPoint		(&DB, C, V->P, V->N, Lights.begin(), Lights.end(), F);
				
				V->Color.r		= C.r*v_inv+v_amb;
				V->Color.g		= C.g*v_inv+v_amb;
				V->Color.b		= C.b*v_inv+v_amb;
				V->Color.a		= 1.f;
				g_trans_register(V);
			}
		}
		thProgress	= float(I - faceStart) / float(faceEnd-faceStart);
	}
};

#define NUM_THREADS	12
void CBuild::LightVertex()
{
	// Start threads, wait, continue --- perform all the work
	Status					("Calculating...");
	DWORD	start_time		= timeGetTime();
	CThreadManager			Threads;
	DWORD	stride			= g_faces.size()/NUM_THREADS;
	DWORD	last			= g_faces.size()-stride*(NUM_THREADS-1);
	for (DWORD thID=0; thID<NUM_THREADS; thID++)
		Threads.start(new CVertexLightThread(thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride)));
	Threads.wait			();
	Msg("%d seconds elapsed.",(timeGetTime()-start_time)/1000);

	// Process all groups
	Status					("Transluenting...");
	for (int it=0; it<g_trans.size(); it++)
	{
		// Unique
		vecVertex&	VL		= g_trans[it];
		std::sort	(VL.begin(),VL.end());
		VL.erase	(unique(VL.begin(),VL.end()),VL.end());

		// Calc summary color
		Fcolor		C;
		C.set		(0,0,0,0);
		for (int v=0; v<VL.size(); v++)
		{
			C.r = _max(C.r,VL[v]->Color.r);
			C.g = _max(C.g,VL[v]->Color.g);
			C.b = _max(C.b,VL[v]->Color.b);
		}

		// Calculate final vertex color
		for (v=0; v<VL.size(); v++)
		{
			Fcolor				R;
			R.lerp				(VL[v]->Color,C,.5f);
			R.r					= _max(R.r,VL[v]->Color.r);
			R.g					= _max(R.g,VL[v]->Color.g);
			R.b					= _max(R.b,VL[v]->Color.b);
			VL[v]->Color.lerp	(R,g_params.m_lm_amb_color,g_params.m_lm_amb_fogness);
			VL[v]->Color.mul_rgb(.5f);
			VL[v]->Color.a		= 1.f;
		}
	}
}
