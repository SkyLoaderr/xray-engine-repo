#include "stdafx.h"
#include "compiler.h"

#include "cl_defs.h"
#include "cl_intersect.h"

const int	LIGHT_Count			=3;
const int	LIGHT_Total			=(2*LIGHT_Count+1)*(2*LIGHT_Count+1);

typedef	svector<R_Light,128>	LSelection;

IC bool RayPick(Fvector& P, Fvector& D, float r, R_Light& L)
{
	// 1. Check cached polygon
	float _u,_v,range;
	bool res = RAPID::TestRayTri(P,D,L.tri,_u,_v,range,true);
	if (res) {
		if (range>0 && range<r) return true;
	}

	// 2. Polygon doesn't pick - real database query
	XRC.RayPick(0,&Level,P,D,r);
	if (0==XRC.GetRayContactCount()) {
		return false;
	} else {
		// cache polygon
		RAPID::raypick_info&	rpinf	= XRC.RayContact[0];
		L.tri[0].set	(rpinf.p[0]);
		L.tri[1].set	(rpinf.p[1]);
		L.tri[2].set	(rpinf.p[2]);
		return true;
	}
}

float LightPoint(Fvector &P, Fvector &N, LSelection& SEL)
{
	Fvector		Ldir,Pnew;
	Pnew.direct(P,N,0.05f);

	R_Light	*L = SEL.begin(), *E = SEL.end();

	float	amount = 0;
	for (;L!=E; L++)
	{
		if (L->type==LT_DIRECT) {
			// Cos
			Ldir.invert	(L->direction);
			float D		= Ldir.dotproduct( N );
			if( D <=0 ) continue;

			// Raypick
			if (!RayPick(Pnew,Ldir,1000.f,*L))	amount+=D*L->amount;
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
			if (!RayPick(Pnew,Ldir,R,*L))
				amount += (D*L->amount)/(L->attenuation0 + L->attenuation1*R + L->attenuation2*sqD);
		}
	}
	return amount;
}

void	xrLight			()
{
	Fvector P,D,PLP;
	D.set	(0,1,0);
	float coeff = 0.5f*g_params.fPatchSize/float(LIGHT_Count);
	XRC.RayMode	(RAY_ONLYFIRST|RAY_CULL);

	LSelection	Selected;
	float		LperN	= float(g_lights.size());
	for (DWORD i=0; i<g_nodes.size(); i++)
	{
		Node& N = g_nodes[i];

		// select lights
		Selected.clear();
		for (DWORD L=0; L<g_lights.size(); L++)
		{
			R_Light&	R = g_lights[L];
			if (R.type==LT_DIRECT)	Selected.push_back(R);
			else {
				float dist = N.Pos.distance_to(R.position);
				if (dist-g_params.fPatchSize < R.range)
					Selected.push_back(R);
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
				amount += LightPoint(P,N.Plane.n,Selected);
			}
		}

		// calculation of luminocity
		N.LightLevel = amount/float(LIGHT_Total);

		if (0==i%32)	{
			Progress(float(i)/float(g_nodes.size()));
			Status("%d lights on level\n%2.1f lights per node",g_lights.size(),LperN);
		}
	}

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
