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

void SceneBuilder::PF_ProcessOne(_finddata_t& F, const char* path)
{
	string256	N;
	strcpy		(N,path);
	strcat		(N,F.name);

	if (F.attrib&_A_SUBDIR) {
		if (0==strcmp(F.name,"."))	return;
		if (0==strcmp(F.name,"..")) return;
		strcat(N,"\\");
		PF_Recurse(N);
        RmDir(N);
	} else {
    	unlink(N);
	}
}

void SceneBuilder::PF_Recurse(const char* path)
{
    _finddata_t		sFile;
    int				hFile;

	string256		N;
	string256		dst;

    strcpy			(N,path);
    strcat			(N,"*.*");
    R_ASSERT		((hFile=_findfirst(N, &sFile)) != -1);
    PF_ProcessOne	(sFile,path);
    while			( _findnext( hFile, &sFile ) == 0 )
        PF_ProcessOne(sFile,path);
    _findclose		( hFile );
}

bool SceneBuilder::PrepareFolders()
{
	PF_Recurse		(m_LevelPath.m_Path);
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

bool SceneBuilder::GetBounding()
{
	Fbox b0;
    bool r0 = Scene.GetBox(m_LevelBox,OBJCLASS_SCENEOBJECT);
    bool r1 = Scene.GetBox(b0,OBJCLASS_GROUP);
    m_LevelBox.merge(b0);
	return (r0||r1);
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

