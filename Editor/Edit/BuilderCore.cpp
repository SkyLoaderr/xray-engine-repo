//----------------------------------------------------
// file: BuilderCore.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "EditObject.h"
#include "DPATCH.h"
#include "Sector.h"
//----------------------------------------------------
bool SceneBuilder::PreparePath(){
    char temp[1024];
    strcpy(temp, Scene->m_LevelOp.m_FNLevelPath.c_str());
    if(!ExtractFileDrive(Scene->m_LevelOp.m_FNLevelPath).Length())FS.m_GameLevels.Update(temp);
   	m_LevelPath.Init( temp, "\\", "", "");
    return true;
}

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

bool SceneBuilder::LightenObjects(){
	int objcount = Scene->ObjCount(OBJCLASS_EDITOBJECT);
	if( objcount <= 0 ) return true;

	UI->ProgressStart(objcount, "Lighten objects...");
    // unload cform, point normals, render buffers
    ObjectIt _F = Scene->FirstObj(OBJCLASS_EDITOBJECT);
    ObjectIt _E = Scene->LastObj(OBJCLASS_EDITOBJECT);
    for(;_F!=_E;_F++){
    	CEditObject* O = (CEditObject*)(*_F);
        if (O->IsReference()) O = O->GetRef();
        if (NeedAbort()) break; // break building
        O->LightenObject();
		UI->ProgressInc();
	}
	UI->ProgressEnd();
	// unload texture
    UI->Command(COMMAND_REFRESH_TEXTURES);
    
    return true;
}

bool SceneBuilder::UngroupAndUnlockObjects(){
	int objcount = Scene->ObjCount();
	if( objcount <= 0 ) return true;

	UI->ProgressStart(objcount, "Ungroup and unlock objects...");

    for(ObjectPairIt it=Scene->FirstClass(); it!=Scene->LastClass(); it++){
        ObjectList& lst = (*it).second;
        if (NeedAbort()) break; // break building
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
			UI->ProgressInc();
            if (NeedAbort()) break; // break building
            (*_F)->Ungroup();
            (*_F)->Lock(false);
        }
	}

	UI->ProgressEnd();

    Scene->m_DetailPatches->Lock( false );
    return true;
}

bool SceneBuilder::ResolveReferences(){
	int objcount = Scene->ObjCount(OBJCLASS_EDITOBJECT);
	if( objcount <= 0 )
		return true;

	UI->ProgressStart(objcount, "Resolve reference...");

    ObjectIt _F = Scene->FirstObj(OBJCLASS_EDITOBJECT);
    ObjectIt _E = Scene->LastObj(OBJCLASS_EDITOBJECT);
    for(;_F!=_E;_F++){
        if (NeedAbort()) break; // break building
        CEditObject *obj = (CEditObject*)(*_F);
    	Log->Msg(mtInformation,obj->GetName());
        if( !obj->IsDynamic() )  obj->ResolveReference();
        if( !obj->IsReference() )  obj->TranslateToWorld();
		UI->ProgressInc();
	}

	UI->ProgressEnd();

	return true;
}

bool SceneBuilder::GetShift(){
	if (!Scene->GetBox(m_LevelBox,OBJCLASS_EDITOBJECT)) return true;
    m_LevelBox.getcenter(m_LevelShift); m_LevelShift.mul(-1.f);
	m_LevelBox.offset(m_LevelShift);
	return true;
}

bool SceneBuilder::ShiftLevel(){
	int objcount = Scene->ObjCount();
	if( objcount <= 0 ) return true;

	UI->ProgressStart(objcount, "Shift level...");
/*
    for(ObjectPairIt it=Scene->FirstClass(); it!=Scene->LastClass(); it++){
        ObjectList& lst = (*it).second;
        if (NeedAbort()) break; // break building
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
            if (NeedAbort()) break; // break building
            (*_F)->Move( m_LevelShift );
			UI->ProgressInc();
        }
	}

    Scene->m_DetailPatches->MoveDPatches( m_LevelShift );
*/
  	UI->ProgressEnd();

	return true;
}                                     


bool SceneBuilder::RenumerateSectors(){
	m_iDefaultSectorNum	= -1;

	UI->ProgressStart(Scene->ObjCount(OBJCLASS_SECTOR), "Renumerate sectors...");

	int sector_num = 0;
    ObjectIt _F = Scene->FirstObj(OBJCLASS_SECTOR);
    ObjectIt _E = Scene->LastObj(OBJCLASS_SECTOR);
    for(;_F!=_E;_F++,sector_num++){
    	CSector* _S=(CSector*)(*_F);
        _S->sector_num = sector_num;
        if (_S->IsDefault()) m_iDefaultSectorNum=sector_num;
		UI->ProgressInc();
	}

	UI->ProgressEnd();

	if (m_iDefaultSectorNum<0) m_iDefaultSectorNum=Scene->ObjCount(OBJCLASS_SECTOR);
	return true;
}
