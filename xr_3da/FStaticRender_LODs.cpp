#include "stdafx.h"
#include "fstaticrender.h"

void CRender::flush_LODs()
{
	mapLOD.getLR	(lstLODs);


	mapLOD.clear	();
	lstLODs.clear	();
}
