#include "stdafx.h"
#include "compiler.h"

#include "cl_defs.h"
#include "cl_intersect.h"

#include "xrThread.h"

const int	LIGHT_Count			=3;
const int	LIGHT_Total			=(2*LIGHT_Count+1)*(2*LIGHT_Count+1);

typedef	svector<R_Light*,128>	LSelection;

IC bool RayPick(RAPID::XRCollide& DB, Fvector& P, Fvector& D, float r, R_Light& L)
{
	// 1. Check cached polygon
	float _u,_v,range;
	bool res = RAPID::TestRayTri(P,D,L.tri,_u,_v,range,true);
	if (res) {
		if (range>0 && range<r) return true;
	}

	// 2. Polygon doesn't pick - real database query
	DB.RayPick(0,&LevelLight,P,D,r);
	if (0==DB.GetRayContactCount()) {
		return false;
	} else {
		// cache polygon
		RAPID::raypick_info&	rpinf	= DB.RayContact[0];
		L.tri[0].set	(rpinf.p[0]);
		L.tri[1].set	(rpinf.p[1]);
		L.tri[2].set	(rpinf.p[2]);
		return true;
	}
}

float LightPoint(RAPID::XRCollide& DB, Fvector &P, Fvector &N, LSelection& SEL)
{
	Fvector		Ldir,Pnew;
	Pnew.direct(P,N,0.05f);

	R_Light	**IT = SEL.begin(), **E = SEL.end();

	float	amount = 0;
	for (; IT!=E; IT++)
	{
		R_Light* L = *IT;
		if (L->type==LT_DIRECT) 
		{
			// Cos
			Ldir.invert	(L->direction);
			float D		= Ldir.dotproduct( N );
			if( D <=0 ) continue;

			// Raypick
			if (!RayPick(DB,Pnew,Ldir,1000.f,*L))	amount+=D*L->amount;
		} else {
			// Distance
			float sqD	= P.distance_to_sqr(L->position);
			if (sqD > L->range2) continue;
			
			// Dir
			Ldir.sub	(L->position,P);
			Ldir.normalize_safe();
			float D		= Ldir.dotproduct( N );
			if( D <=0 ) continue;
			
			// Raypick
			float R		= sqrtf(sqD);
			if (!RayPick(DB,Pnew,Ldir,R,*L))
				amount += (D*L->amount)/(L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);
		}
	}
	return amount;
}

class	LightThread : public CThread
{
	DWORD	Nstart, Nend;
public:
	LightThread			(DWORD ID, DWORD _start, DWORD _end) : CThread(ID)
	{
		Nstart	= _start;
		Nend	= _end;
	}
	virtual void		Execute()
	{
		RAPID::XRCollide DB;
		DB.RayMode		(RAY_ONLYFIRST|RAY_CULL);

		vector<R_Light>	Lights = g_lights;

		Fvector			P,D,PLP;
		D.set			(0,1,0);
		float coeff		= 0.5f*g_params.fPatchSize/float(LIGHT_Count);
		
		LSelection		Selected;
		float			LperN	= float(g_lights.size());
		for (DWORD i=Nstart; i<Nend; i++)
		{
			Node& N = g_nodes[i];
			
			// select lights
			Selected.clear();
			for (DWORD L=0; L<Lights.size(); L++)
			{
				R_Light&	R = g_lights[L];
				if (R.type==LT_DIRECT)	Selected.push_back(&R);
				else {
					float dist = N.Pos.distance_to(R.position);
					if (dist-g_params.fPatchSize < R.range)
						Selected.push_back(&R);
				}
			}
			LperN = 0.9f*LperN + 0.1f*float(Selected.size());
			
			// lighting itself
			float amount=0;
			for (int x=-LIGHT_Count; x<=LIGHT_Count; x++) 
			{
				P.x = N.Pos.x + coeff*float(x);
				for (int z=-LIGHT_Count; z<=LIGHT_Count; z++) 
				{
					// compute position
					P.z = N.Pos.z + coeff*float(z);
					P.y = N.Pos.y;
					N.Plane.intersectRayPoint(P,D,PLP);	// "project" position
					P.y = PLP.y;
					
					// light point
					amount += LightPoint(DB,P,N.Plane.n,Selected);
				}
			}
			
			// calculation of luminocity
			N.LightLevel	= amount/float(LIGHT_Total);
			
			thProgress		= float(i-Nstart)/float(Nend-Nstart);
		}
	}
};

#define NUM_THREADS	8
void	xrLight			()
{
	// Start threads, wait, continue --- perform all the work
	DWORD	start_time		= timeGetTime();
	CThreadManager			Threads;
	DWORD	stride			= g_nodes.size()/NUM_THREADS;
	DWORD	last			= g_nodes.size()-stride*(NUM_THREADS-1);
	for (DWORD thID=0; thID<NUM_THREADS; thID++)
		Threads.start(new LightThread(thID,thID*stride,thID*stride+((thID==(NUM_THREADS-1))?last:stride)));
	Threads.wait			();
	Msg("%d seconds elapsed.",(timeGetTime()-start_time)/1000);

	// Smooth
	Status("Smoothing lighting...");
	for (int pass=0; pass<3; pass++) {
		Nodes	Old = g_nodes;
		for (DWORD N=0; N<g_nodes.size(); N++)
		{
			Node&	Base		= Old[N];
			Node&	Dest		= g_nodes[N];
			
			float	val			= 2*Base.LightLevel;
			float	cnt			= 2;
			
			for (int nid=0; nid<4; nid++) {
				if (Base.n[nid]!=InvalidNode) {
					val		+=  Old[Base.n[nid]].LightLevel;
					cnt		+=	1.f;
				}
			}
			Dest.LightLevel		=  val/cnt;
			clamp(Dest.LightLevel,0.f,1.f);
		}
	}
}
