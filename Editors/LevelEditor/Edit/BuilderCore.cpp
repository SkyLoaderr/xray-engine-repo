//------------------------------------------------------------------------------
// file: BuilderCore.cpp
//------------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "Scene.h"
#include "SceneObject.h"
#include "Sector.h"
#include "ui_main.h"
//------------------------------------------------------------------------------

bool SceneBuilder::PreparePath()
{
	if (Scene.m_LevelOp.m_FNLevelPath.IsEmpty()) return false;
    FS.update_path	(m_LevelPath,_game_levels_,Scene.m_LevelOp.m_FNLevelPath.c_str());
    m_LevelPath		+= "\\";
    return true;
}
//------------------------------------------------------------------------------

bool SceneBuilder::PrepareFolders()
{
	FS.dir_delete	(m_LevelPath.c_str(),TRUE);
	return true;
}
//------------------------------------------------------------------------------

bool SceneBuilder::EvictResource()
{
	UI->Command(COMMAND_EVICT_OBJECTS);
    UI->Command(COMMAND_EVICT_TEXTURES);

	int objcount = Scene.ObjCount(OBJCLASS_SCENEOBJECT);
	if( objcount <= 0 ) return true;

	UI->ProgressStart(objcount, "Evict objects...");
    // unload cform, point normals
    ObjectIt _F = Scene.FirstObj(OBJCLASS_SCENEOBJECT);
    ObjectIt _E = Scene.LastObj(OBJCLASS_SCENEOBJECT);
    for(;_F!=_E;_F++){
    	CSceneObject* O = (CSceneObject*)(*_F);
        if (UI->NeedAbort()) break; // break building
        O->EvictObject();
		UI->ProgressInc();
	}
	UI->ProgressEnd();

    return true;
}
//------------------------------------------------------------------------------

bool SceneBuilder::GetBounding()
{
	Fbox b0;
    bool r0 = Scene.GetBox(m_LevelBox,OBJCLASS_SCENEOBJECT);
    bool r1 = Scene.GetBox(b0,OBJCLASS_GROUP);
    m_LevelBox.merge(b0);
	return (r0||r1);
}
//------------------------------------------------------------------------------

bool SceneBuilder::RenumerateSectors()
{
	m_iDefaultSectorNum	= -1;

	UI->ProgressStart(Scene.ObjCount(OBJCLASS_SECTOR), "Renumerate sectors...");

	int sector_num = 0;
    ObjectIt _F = Scene.FirstObj(OBJCLASS_SECTOR);
    ObjectIt _E = Scene.LastObj(OBJCLASS_SECTOR);
    for(;_F!=_E;_F++,sector_num++){
    	CSector* _S=(CSector*)(*_F);
        _S->sector_num = sector_num;
        if (_S->IsDefault()) m_iDefaultSectorNum=sector_num;
		UI->ProgressInc();
	}

	UI->ProgressEnd();

	if (m_iDefaultSectorNum<0) m_iDefaultSectorNum=Scene.ObjCount(OBJCLASS_SECTOR);
	return true;
}
//------------------------------------------------------------------------------

