////////////////////////////////////////////////////////////////////////////
//	Module 		: xrCrossTable.cpp
//	Created 	: 25.01.2003
//  Modified 	: 25.01.2003
//	Author		: Dmitriy Iassenev
//	Description : Building cross table for AI nodes
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defines.h"
#include "xrCrossTable.h"
#include "ai_map.h"

void xrBuildCrossTable(LPCSTR caProjectName)
{
	FILE_NAME		caFileName;
	strconcat		(caFileName,caProjectName,"level.graph");
	
	CGraph			tGraph	(caFileName);
	CAI_Map			tMap	(caProjectName);

}
