#include "stdafx.h"
#include "build.h"

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
void CBuild::LightVertex()
{
	vector<R_Light>	Lights = lights_soften;
	if (Lights.empty())		return;

	XRC.RayMode			(RAY_CULL);
	DWORD _VLighted		= 0;
	for (vecFaceIt I=g_faces.begin(); I!=g_faces.end(); I++)
	{
		Face* F = *I;
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
			LightPoint	(&XRC, C, V->P, V->N, Lights.begin(), Lights.end());

			R.r			= C.r*v_inv+v_amb;
			R.g			= C.g*v_inv+v_amb;
			R.b			= C.b*v_inv+v_amb;
			Lumel.lerp	(R,g_params.m_lm_amb_color,g_params.m_lm_amb_fogness);
			Lumel.a		= 1.f;

			V->Color	= Lumel.get();
			_VLighted++;
		}

		Progress(float(I-g_faces.begin())/float(g_faces.size()));
	}
	Msg("%d vertices raytraced",_VLighted);
}
