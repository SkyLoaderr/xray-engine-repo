#include "stdafx.h"
#include "build.h"
#include "std_classes.h"
#include "xrThread.h"

extern void Jitter_Select	(UVpoint* &Jitter, DWORD& Jcount);

void CDeflector::L_Direct_Edge (CDB::COLLIDER* DB, LSelection* LightsSelected, UVpoint& p1, UVpoint& p2, Fvector& v1, Fvector& v2, Fvector& N, float texel_size, Face* skip)
{
	Fvector		vdir;
	vdir.sub	(v2,v1);
	
	b_texture&	lm = layers.back().lm;
	
	UVpoint		size; 
	size.u		= p2.u-p1.u;
	size.v		= p2.v-p1.v;
	int	du		= iCeil(_abs(size.u)/texel_size);
	int	dv		= iCeil(_abs(size.v)/texel_size);
	int steps	= _max(du,dv);
	if (steps<=0)	return;
	
	for (int I=0; I<=steps; I++)
	{
		float	time = float(I)/float(steps);
		UVpoint	uv;
		uv.u	= size.u*time+p1.u;
		uv.v	= size.v*time+p1.v;
		int	_x  = iFloor(uv.u*float(lm.dwWidth)); 
		int _y	= iFloor(uv.v*float(lm.dwHeight));
		
		if ((_x<0)||(_x>=(int)lm.dwWidth))	continue;
		if ((_y<0)||(_y>=(int)lm.dwHeight))	continue;
		
		DWORD& Lumel	= lm.pSurface[_y*lm.dwWidth+_x];
		if (RGBA_GETALPHA(Lumel))			continue;
		
		// ok - perform lighting
		Fcolor	C; C.set(0,0,0,0);
		Fvector	P; P.mad(v1,vdir,time);
		LightPoint	(DB, C, P, N, LightsSelected->begin(), LightsSelected->end(),skip);
		
		Fcolor		R;
		R.lerp		(C,g_params.m_lm_amb_color,g_params.m_lm_amb_fogness);
		R.mul_rgb	(.5f);
		R.a			= 1.f;
		Lumel		= R.get() | RGBA_MAKE(0,0,0,255);
	}
}

void CDeflector::L_Direct	(CDB::COLLIDER* DB, LSelection* LightsSelected, HASH& H)
{
	R_ASSERT	(DB);
	R_ASSERT	(LightsSelected);

	b_texture&	lm = layers.back().lm;

	// Setup variables
	UVpoint		dim,half;
	dim.set		(float(lm.dwWidth),float(lm.dwHeight));
	half.set	(.5f/dim.u,.5f/dim.v);
	
	// Jitter data
	UVpoint		JS;
	JS.set		(.499f/dim.u, .499f/dim.v);
	
	DWORD		Jcount;
	UVpoint*	Jitter;
	Jitter_Select(Jitter, Jcount);
	
	// Lighting itself
	DB->ray_options	(0);
	
	Fcolor		C[9];
	for (DWORD J=0; J<9; J++)	C[J].set(0,0,0,0);
	for (DWORD V=0; V<lm.dwHeight; V++)
	{
		for (DWORD U=0; U<lm.dwWidth; U++)
		{
			DWORD		Fcount	= 0;
			
			try {
				for (J=0; J<Jcount; J++) 
				{
					// LUMEL space
					UVpoint P;
					P.u = float(U)/dim.u + half.u + Jitter[J].u * JS.u;
					P.v = float(V)/dim.v + half.v + Jitter[J].v * JS.v;
					
					vector<UVtri*>&	space	= H.query(P.u,P.v);
					
					// World space
					Fvector wP,wN,B;
					C[J].set	(0,0,0,0);
					for (UVtri** it=space.begin(); it!=space.end(); it++)
					{
						if ((*it)->isInside(P,B)) {
							// We found triangle and have barycentric coords
							Face	*F	= (*it)->owner;
							Vertex	*V1 = F->v[0];
							Vertex	*V2 = F->v[1];
							Vertex	*V3 = F->v[2];
							wP.from_bary(V1->P,V2->P,V3->P,B);
							if (F->Shader().flags.bLIGHT_Sharp)	{ wN.set(F->N); }
							else								{ wN.from_bary(V1->N,V2->N,V3->N,B); wN.normalize(); }
							try {
								LightPoint	(DB, C[J], wP, wN, LightsSelected->begin(), LightsSelected->end(), F);
								Fcount		+= 1;
							} catch (...) {
								Msg("* ERROR (CDB). Recovered. ");
							}
							break;
						}
					}
				} 
			} catch (...) {
				Msg("* ERROR (Light). Recovered. ");
			}
			
			if (Fcount) {
				Fcolor	Lumel,R;
				float	cnt		= float(Fcount);
				R.r =			(C[0].r + C[1].r + C[2].r + C[3].r + C[4].r + C[5].r + C[6].r + C[7].r + C[8].r)/cnt;
				R.g =			(C[0].g + C[1].g + C[2].g + C[3].g + C[4].g + C[5].g + C[6].g + C[7].g + C[8].g)/cnt;
				R.b	=			(C[0].b + C[1].b + C[2].b + C[3].b + C[4].b + C[5].b + C[6].b + C[7].b + C[8].b)/cnt;
				Lumel.lerp		(R,g_params.m_lm_amb_color,g_params.m_lm_amb_fogness);
				Lumel.mul_rgb	(.5f);
				Lumel.a			= 1.f;
				lm.pSurface	[V*lm.dwWidth+U] = Lumel.get();
			} else {
				lm.pSurface	[V*lm.dwWidth+U] = 0;
			}
		}
	}

	// *** Render Edges
	float texel_size = (1.f/float(_max(lm.dwWidth,lm.dwHeight)))/8.f;
	for (DWORD t=0; t<UVpolys.size(); t++)
	{
		UVtri&		T	= UVpolys[t];
		Face*		F	= T.owner;
		R_ASSERT	(F);
		try {
			L_Direct_Edge	(DB,LightsSelected, T.uv[0], T.uv[1], F->v[0]->P, F->v[1]->P, F->N, texel_size,F);
			L_Direct_Edge	(DB,LightsSelected, T.uv[1], T.uv[2], F->v[1]->P, F->v[2]->P, F->N, texel_size,F);
			L_Direct_Edge	(DB,LightsSelected, T.uv[2], T.uv[0], F->v[2]->P, F->v[0]->P, F->N, texel_size,F);
		} catch (...)
		{
			Msg("* ERROR (Edge). Recovered. ");
		}
	}
}
