#include "stdafx.h"
#include "build.h"
#include "cl_defs.h"
#include "std_classes.h"

extern void LightPoint		(RAPID::XRCollide* DB, Fcolor &C, Fvector &P, Fvector &N, R_Light* begin, R_Light* end);
extern void Jitter_Select	(UVpoint* &Jitter, DWORD& Jcount);

void CDeflector::L_Direct(float progress)
{
	// Setup variables
	UVpoint		dim,guard,half;
	dim.set		(float(lm.dwWidth),float(lm.dwHeight));
	guard.set	(1.f/dim.u,1.f/dim.v);
	half.set	(.5f/dim.u,.5f/dim.v);

	// Lighting itself
	XRC.RayMode	(0);
	// XRC.RayMode	(RAY_CULL /*| RAY_ONLYFIRST*/);

	// Jitter data
	UVpoint JS;
	JS.set(g_params.m_lm_jitter/dim.u, g_params.m_lm_jitter/dim.v);

	DWORD		Jcount;
	UVpoint*	Jitter;
	Jitter_Select(Jitter, Jcount);

	// Main raytracing cycle
	if (!g_params.m_bLightMaps) {
		FillMemory(lm.pSurface,lm.dwHeight*lm.dwWidth*4,0xff);
		return;
	}

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
					vecPT &space = HASH[slot(P.v)][slot(P.u)];
					
					// World space
					Fvector wP,wN,B;
					C[J].set(0,0,0,0);
					for (vecPTIT it=space.begin(); it!=space.end(); it++)
					{
						if ((*it)->isInside(P,B)) {
							// We found triangle and have barycentric coords
							Face	*F	= (*it)->owner;
							Vertex	*V1 = F->v[0];
							Vertex	*V2 = F->v[1];
							Vertex	*V3 = F->v[2];
							wP.from_bary(V1->P,V2->P,V3->P,B);
							wN.from_bary(V1->N,V2->N,V3->N,B);
							//						wN.direct	(wN,F->N,0.5f);
							wN = F->N;
							wN.normalize();
							LightPoint	(&XRC, C[J], wP, wN, Lights.begin(), Lights.end());
							Fcount++;
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
		Progress(progress+(float(V)/float(lm.dwHeight))/float(g_deflectors.size()));
	}
}
