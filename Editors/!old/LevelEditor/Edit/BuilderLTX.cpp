//----------------------------------------------------
// file: BuilderLTX.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "Scene.h"
#include "EditObject.h"
#include "SceneObject.h"
#include "ELight.h"
#include "SpawnPoint.h"
#include "WayPoint.h"
#include "xr_ini.h"
#include "xr_efflensflare.h"
#include "GroupObject.h"
//----------------------------------------------------

//----------------------------------------------------
BOOL SceneBuilder::ParseLTX(CInifile* pIni, ObjectList& lst, LPCSTR prefix)
{
    return TRUE;
}
//----------------------------------------------------

BOOL SceneBuilder::BuildLTX()
{
	bool bResult=true;
	int objcount = Scene->ObjCount();
	if( objcount <= 0 ) return true;

	xr_string ltx_filename	= MakeLevelPath("level.ltx");

    if (FS.exist(ltx_filename.c_str()))
    	EFS.MarkFile(ltx_filename.c_str(),true);

	// -- defaults --           
    IWriter* F	= FS.w_open(ltx_filename.c_str());
    F->w_string( "; level script file");
	if(Scene->m_LevelOp.m_BOPText.size())
		F->w_stringZ( Scene->m_LevelOp.m_BOPText );
    FS.w_close	(F);

	return bResult;
}

