#include "stdafx.h"
#include "build.h"
#include "light.h"

void CBuild::SaveLights(CFS_Base &fs)
{
	fs.write_chunk	(fsL_LIGHT_CONTROL,L_control_data.begin(),L_control_data.size());

	fs.open_chunk	(fsL_LIGHT_DYNAMIC);
	for (DWORD i=0; i<L_dynamic.size(); i++) 
	{
		b_light_dynamic& L	= L_dynamic[i];
		fs.Wdword	(L.controller_ID);
		fs.write	(&L.data,sizeof(L.data));
	}
	fs.close_chunk();
}
