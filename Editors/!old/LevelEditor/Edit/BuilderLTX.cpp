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
	int objcount = Scene.ObjCount();
	if( objcount <= 0 ) return true;

	AnsiString ltx_filename	= m_LevelPath+"level.ltx";

    if (FS.exist(ltx_filename.c_str()))
    	EFS.MarkFile(ltx_filename,true);

	// -- defaults --
    CMemoryWriter F;
	if( !Scene.m_LevelOp.m_BOPText.IsEmpty() )
		F.w_stringZ( Scene.m_LevelOp.m_BOPText.c_str() );
	F.save_to(ltx_filename.c_str());

    CInifile* pIni = xr_new<CInifile>(ltx_filename.c_str(),false);
/*
//.
    // required strings
    pIni->w_string("static_sounds",		"; sounds","");
    pIni->w_string("sound_environment",	"; sound environment","");

    for(ObjectPairIt it=Scene.FirstClass(); it!=Scene.LastClass(); it++)
        if (!ParseLTX(pIni,(*it).second)){bResult = FALSE; break;}
*/

    xr_delete(pIni);

	return bResult;
}

