// xrCPU_Pipe.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern xrSkin1W		xrSkin1W_x86;
extern xrSkin1W		xrSkin1W_3DNow;
extern xrSkin1W		xrSkin1W_SSE;
extern xrSkin2W		xrSkin2W_x86;
extern xrBoneLerp	xrBoneLerp_x86;
extern xrBoneLerp	xrBoneLerp_3DNow;
extern xrM44_Mul	xrM44_Mul_x86;
extern xrM44_Mul	xrM44_Mul_3DNow;
extern xrM44_Mul	xrM44_Mul_SSE;
extern xrTransfer	xrTransfer_x86;

extern "C" {
	__declspec(dllexport) void	__cdecl	xrBind_PSGP	(xrDispatchTable* T)
	{
		// analyze features
		DWORD dwFeatures = CPU::ID.feature & CPU::ID.os_support;

		if(strstr(strlwr(GetCommandLine()),"-x87"))	dwFeatures &= ~(_CPU_FEATURE_SSE+_CPU_FEATURE_3DNOW);

		// generic
		T->skin1W	= xrSkin1W_x86;
		T->skin2W	= NULL;
		T->blerp	= xrBoneLerp_x86;
		T->m44_mul	= xrM44_Mul_x86;
		T->transfer = xrTransfer_x86;

		// sse
		if (dwFeatures & _CPU_FEATURE_SSE) {
			//not jet ready
			//T->skin1W	= xrSkin1W_SSE;
		}

		// 3dnow!
		if (dwFeatures & _CPU_FEATURE_3DNOW) {
 			T->skin1W	= xrSkin1W_3DNow;
			T->blerp	= xrBoneLerp_3DNow;
//			T->m44_mul	= xrM44_Mul_3DNow;
		}
	}
};
