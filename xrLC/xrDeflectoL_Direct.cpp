#include "stdafx.h"
#include "build.h"
#include "std_classes.h"
#include "xrThread.h"

extern void LightPoint		(RAPID::XRCollide* DB, Fcolor &C, Fvector &P, Fvector &N, R_Light* begin, R_Light* end);
extern void Jitter_Select	(UVpoint* &Jitter, DWORD& Jcount);

void CDeflector::L_Direct_Edge (UVpoint& p1, UVpoint& p2, Fvector& v1, Fvector& v2, Fvector& N, float texel_size)
{
	Fvector		vdir;
	vdir.sub	(v2,v1);

	UVpoint		size; 
	size.u		= p2.u-p1.u;
	size.v		= p2.v-p1.v;
	int	du		= iCeil(_abs(size.u)/texel_size);
	int	dv		= iCeil(_abs(size.v)/texel_size);
	int steps	= _max(du,dv);
	for (int I=0; I<=steps; I++)
	{
		float	time = float(I)/float(steps);
		UVpoint	uv;
		uv.u	= size.u*time+p1.u;
		uv.v	= size.v*time+p1.v;
		int	_x  = iFloor(uv.u*float(lm.dwWidth)); 
		int _y	= iFloor(uv.v*float(lm.dwHeight));

		if ((_x<0)||(_x>=(int)T->dwWidth))	continue;
		if ((_y<0)||(_y>=(int)T->dwHeight))	continue;
		
		DWORD& Lumel	= lm.pSurface[_y*lm.dwWidth+_x];
		if (RGBA_GETALPHA(Lumel))			continue;

		// ok - perform lighting
		Fcolor	C; C.set(0,0,0,0);
		Fvector	P; P.direct(v1,vdir,time);
		LightPoint	(&DB, C, P, N, LightsSelected.begin(), LightsSelected.end());

		Fcolor	R;
		R.lerp	(C,g_params.m_lm_amb_color,g_params.m_lm_amb_fogness);
		R.a		= 1.f;
		Lumel   = R.get();
	}
}

void CDeflector::L_Direct	(HASH& H)
{
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
	DB.RayMode	(0);
	
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
					Fvector wP,B;
					C[J].set	(0,0,0,0);
					FPU::m64r	();
					for (UVtri** it=space.begin(); it!=space.end(); it++)
					{
						if ((*it)->isInside(P,B)) {
							// We found triangle and have barycentric coords
							Face	*F	= (*it)->owner;
							Vertex	*V1 = F->v[0];
							Vertex	*V2 = F->v[1];
							Vertex	*V3 = F->v[2];
							wP.from_bary(V1->P,V2->P,V3->P,B);
							//			wN.from_bary(V1->N,V2->N,V3->N,B);
							//			wN.direct	(wN,F->N,0.5f);
							//			wN	= F->N;
							//			wN.normalize();
							LightPoint	(&DB, C[J], wP, F->N, LightsSelected.begin(), LightsSelected.end());
							Fcount		+= 1;
							break;
						}
					}
				} 
			} catch (...)
			{
				Msg("* Access violation. Possibly recovered. ");
			}
			
			FPU::m24r	();
			if (Fcount) {
				Fcolor	Lumel,R;
				float	cnt		= float(Fcount);
				R.r =		(C[0].r + C[1].r + C[2].r + C[3].r + C[4].r + C[5].r + C[6].r + C[7].r + C[8].r)/cnt;
				R.g =		(C[0].g + C[1].g + C[2].g + C[3].g + C[4].g + C[5].g + C[6].g + C[7].g + C[8].g)/cnt;
				R.b	=		(C[0].b + C[1].b + C[2].b + C[3].b + C[4].b + C[5].b + C[6].b + C[7].b + C[8].b)/cnt;
				Lumel.lerp	(R,g_params.m_lm_amb_color,g_params.m_lm_amb_fogness);
				Lumel.a		= 1.f;
				lm.pSurface	[V*lm.dwWidth+U] = Lumel.get();
			} else {
				lm.pSurface	[V*lm.dwWidth+U] = 0;
			}
		}
	}

	// *** Render Edges
	for (DWORD t=0; t<tris.size(); t++)
	{
		UVtri&		T	= tris[t];
		UVpoint&	p1	= T.uv[0]; int x1=iFloor(p1.u*float(lm.dwWidth)); int y1=iFloor(p1.v*float(lm.dwHeight));
		UVpoint&	p2	= T.uv[1]; int x2=iFloor(p2.u*float(lm.dwWidth)); int y2=iFloor(p2.v*float(lm.dwHeight));
		UVpoint&	p3	= T.uv[2]; int x3=iFloor(p3.u*float(lm.dwWidth)); int y3=iFloor(p3.v*float(lm.dwHeight));
		light_edge	(x1,y1,x2,y2,&lm,T);
		light_edge	(x2,y2,x3,y3,&lm,T);
		light_edge	(x3,y3,x1,y1,&lm,T);
	}
}
