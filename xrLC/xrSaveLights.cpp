#include "stdafx.h"
#include "build.h"
#include "light.h"

void CBuild::SaveLights(CFS_Base &fs)
{
	fs.open_chunk(fsL_LIGHTS);
	for (DWORD i=0; i<lights_dynamic.size(); i++) 
	{
		b_light&	BL = lights_dynamic[i];
		xrLIGHT		L;
		ZeroMemory	(&L,sizeof(L));
		CopyMemory	(&L,&BL,sizeof(Flight));
		L.flags.bAffectDynamic	= BL.flags.bAffectDynamic;
		L.flags.bAffectStatic	= BL.flags.bAffectStatic;
		L.flags.bProcedural		= BL.flags.bProcedural;
		strcpy		(L.name,BL.name);
		fs.write	(&L,sizeof(L));
	}
	fs.close_chunk();
}
