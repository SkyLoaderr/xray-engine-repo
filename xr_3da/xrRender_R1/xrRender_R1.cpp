// xrRender_R1.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "..\xr_ioconsole.h"
#include "..\xr_ioc_cmd.h"

#pragma comment(lib,"x:\\xr_3DA")

extern float		psDetail_w_rot1;
extern float		psDetail_w_rot2;
extern float		psDetail_w_speed;
extern float		psDetail_l_ambient;
extern float		psDetail_l_aniso;
extern float		psDetailDensity;
extern float		ssaDISCARD;
extern float		ssaDONTSORT;
extern float		ssaLOD_A;
extern float		ssaLOD_B;
extern float		ssaHZBvsTEX;
extern int			psGlowsPerFrame;
extern int			psSupersample;
extern int			psSH_Blur;

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		::Render			= &Render_Implementation;

		CMD4(CCC_Integer,	"rs_supersample",		&psSupersample,		0,		6	);
		CMD4(CCC_Integer,	"rs_sh_blur",			&psSH_Blur,			0,		2	);
		CMD4(CCC_Float,		"rs_detail_w_rot1",		&psDetail_w_rot1,	1.f,180.f	);
		CMD4(CCC_Float,		"rs_detail_w_rot2",		&psDetail_w_rot2,	1.f,180.f	);
		CMD4(CCC_Float,		"rs_detail_w_speed",	&psDetail_w_speed,	1.f,4.f		);
		CMD4(CCC_Float,		"rs_detail_l_ambient",	&psDetail_l_ambient,.5f,.95f	);
		CMD4(CCC_Float,		"rs_detail_l_aniso",	&psDetail_l_aniso,	.1f,.5f		);
		CMD4(CCC_Float,		"rs_detail_density",	&psDetailDensity,	.05f,0.3f	);
		CMD4(CCC_Float,		"rs_ssa_lod_a",			&ssaLOD_A,			16, 96		);
		CMD4(CCC_Float,		"rs_ssa_lod_b",			&ssaLOD_B,			32, 64		);
		CMD4(CCC_Float,		"rs_ssa_discard",		&ssaDISCARD,		1,  16		);
		CMD4(CCC_Float,		"rs_ssa_dontsort",		&ssaDONTSORT,		16, 128		);
		CMD4(CCC_Float,		"rs_ssa_hzb_vs_tex",	&ssaHZBvsTEX,		16,	512		);
		CMD4(CCC_Integer,	"rs_glows_per_frame",	&psGlowsPerFrame,	2,		32	);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}
