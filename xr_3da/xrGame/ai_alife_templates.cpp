////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_alife_templates.cpp
//	Created 	: 21.01.2003
//  Modified 	: 03.07.2003
//	Author		: Dmitriy Iassenev
//	Description : A-Life templates
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_alife_templates.h"

void delete_data(xr_vector<LPSTR> &tpVector)
{
	xr_vector<LPSTR>::iterator	I = tpVector.begin();
	xr_vector<LPSTR>::iterator	E = tpVector.end();
	for ( ; I != E; I++)
		xr_free					(*I);
	tpVector.clear				();
};

