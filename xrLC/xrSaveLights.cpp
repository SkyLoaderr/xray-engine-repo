#include "stdafx.h"
#include "build.h"

void CBuild::SaveLights(CFS_Base &fs)
{
	fs.open_chunk(fsL_LIGHTS);
	for (DWORD i=0; i<lights_dynamic.size(); i++)
		fs.write(lights_dynamic.begin()+i, sizeof(xrLIGHT));
	fs.close_chunk();

	fs.write_chunk(fsL_LIGHT_KEYS,
		lights_keys.begin(),
		lights_keys.size()*sizeof(Flight)
		);
}
