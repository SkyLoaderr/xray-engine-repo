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

bool SceneBuilder::PreparePath(){
    char temp[1024];
    strcpy(temp, Scene.m_LevelOp.m_FNLevelPath.c_str());
    if(!ExtractFileDrive(Scene.m_LevelOp.m_FNLevelPath).Length())Engine.FS.m_GameLevels.Update(temp);
   	m_LevelPath.Init( temp, "\\", "", "");
    return true;
}
//------------------------------------------------------------------------------

bool SceneBuilder::PrepareFolders(){
	_finddata_t fdata;
	memset(&fdata,0,sizeof(fdata));

	char findmask[MAX_PATH];
	strcpy(findmask,"*.*" );
	m_LevelPath.Update( findmask );

	int fhandle = _findfirst(findmask,&fdata);
	if( fhandle > 0 ){

		// count files

		int filecount = 1;
		while( 0==_findnext(fhandle,&fdata) )
			filecount++;
		_findclose( fhandle );

		// search & remove
		memset(&fdata,0,sizeof(fdata));
		fhandle = _findfirst(findmask,&fdata);
		do{
			char fullname[MAX_PATH];
            if (strcmp(fdata.name,".")==0 || strcmp(fdata.name,"..")==0) continue;
			strcpy( fullname, fdata.name );
			m_LevelPath.Update( fullname );
			unlink( fullname );
		} while( 0==_findnext(fhandle,&fdata) );
		_findclose( fhandle );
	}

	return true;
}
//------------------------------------------------------------------------------

bool SceneBuilder::EvictResource(){
    UI.Command(COMMAND_EVICT_OBJECTS);
    UI.Command(COMMAND_EVICT_TEXTURES);

	int objcount = Scene.ObjCount(OBJCLASS_SCENEOBJECT);
	if( objcount <= 0 ) return true;

	UI.ProgressStart(objcount, "Evict objects...");
    // unload cform, point normals
    ObjectIt _F = Scene.FirstObj(OBJCLASS_SCENEOBJECT);
    ObjectIt _E = Scene.LastObj(OBJCLASS_SCENEOBJECT);
    for(;_F!=_E;_F++){
    	CSceneObject* O = (CSceneObject*)(*_F);
        if (UI.NeedAbort()) break; // break building
        O->EvictObject();
		UI.ProgressInc();
	}
	UI.ProgressEnd();

    return true;
}
//------------------------------------------------------------------------------

bool SceneBuilder::GetBounding(){
	return Scene.GetBox(m_LevelBox,OBJCLASS_SCENEOBJECT);
}
//------------------------------------------------------------------------------

bool SceneBuilder::RenumerateSectors(){
	m_iDefaultSectorNum	= -1;

	UI.ProgressStart(Scene.ObjCount(OBJCLASS_SECTOR), "Renumerate sectors...");

	int sector_num = 0;
    ObjectIt _F = Scene.FirstObj(OBJCLASS_SECTOR);
    ObjectIt _E = Scene.LastObj(OBJCLASS_SECTOR);
    for(;_F!=_E;_F++,sector_num++){
    	CSector* _S=(CSector*)(*_F);
        _S->sector_num = sector_num;
        if (_S->IsDefault()) m_iDefaultSectorNum=sector_num;
		UI.ProgressInc();
	}

	UI.ProgressEnd();

	if (m_iDefaultSectorNum<0) m_iDefaultSectorNum=Scene.ObjCount(OBJCLASS_SECTOR);
	return true;
}
//------------------------------------------------------------------------------

