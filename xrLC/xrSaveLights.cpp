#include "stdafx.h"
#include "build.h"

void CBuild::SaveLights(CFS_Base &fs)
{
	fs.open_chunk(fsL_LIGHTS);
	for (DWORD i=0; i<lights_dynamic.size(); i++)
		fs.write(lights_dynamic.begin()+i, sizeof(xrLight));
	fs.close_chunk();
}
