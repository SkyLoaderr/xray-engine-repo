#include "stdafx.h"
#include "xr_tokens.h"
#include "xr_object.h"
#include "scripting\script.h"

/*
namespace IQ {
#include "iq.h"
};
*/

#ifdef DEBUG
#undef malloc
#undef free
#endif

// Utility pack
int __cdecl xrRand()
{
	return rand();
}
void __cdecl xrIdentity(float *p)
{
	Fmatrix* pM = (Fmatrix*)p;
	pM->identity();
}

//using namespace IQ;
void SAPI_Export()
{
	/*
	static  char *xray_hdr_mem = NULL;
	char	xray_hdr[32768];

	strcpy(xray_hdr,
		"\n"
		"struct Fmatrix\n"
		"{\n"
		"float _11, _12, _13, _14;\n"
		"float _21, _22, _23, _24;\n"
		"float _31, _32, _33, _34;\n"
		"float _41, _42, _43, _44;\n"
		"};\n"

		"typedef unsigned int DWORD;\n"

		"c_import void  msg(char *,...);\n"
		"c_import void  xrIdentity(Fmatrix *p);\n"
		"c_import int   xrRand();\n"
		"import float fTimeDelta;\n"
		"import DWORD dwNumTStages;\n"
	);

	xray_hdr_mem = strdup(xray_hdr);
	scAdd_Internal_Header  ("xray",xray_hdr_mem);

	CScript::RegisterSymbol("fTimeDelta",	&Device.fTimeDelta);
	CScript::RegisterSymbol("dwNumTStages",	&HW.Caps.dwNumBlendStages);
	CScript::RegisterSymbol("msg",			Msg			 );
	CScript::RegisterSymbol("xrIdentity",	xrIdentity	 );
	CScript::RegisterSymbol("xrRand",		xrRand		 );
	CScript::RegisterSymbol("iqVisible_set_clsid",	IQ::iqVisible_set_clsid );
	CScript::RegisterSymbol("iqTimer_init_rand",	IQ::iqTimer_init_rand	);
	CScript::RegisterSymbol("iqTimer_init_fixed",	IQ::iqTimer_init_fixed	);
	CScript::RegisterSymbol("iqBrain_navi_jump",	IQ::iqBrain_navi_jump	);
	CScript::RegisterSymbol("iqAnim_play_cycle",	IQ::iqAnim_play_cycle	);
	CScript::RegisterSymbol("iqAnim_play_fx",		IQ::iqAnim_play_fx		);
	*/
}
