////////////////////////////////////////////////////////////////////////////
//	Module 		: path_test.cpp
//	Created 	: 18.10.2003
//  Modified 	: 18.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Path tests
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "path_test.h"
#include "ai_map.h"

void path_test(LPCSTR caLevelName)
{
	CAI_Map		*l_tpMap = xr_new<CAI_Map>(caLevelName);

	l_tpMap->m_header.count;

	xr_delete	(l_tpMap);
}
