//----------------------------------------------------
// file: BuilderCore.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
//#include "Landscape.h"
#include "SObject2.h"
#include "BuildProgress.h"
//----------------------------------------------------

bool SceneBuilder::PrepareFolders(){
    char lev_path[1024];
    char tex_path[1024];
    strcpy(lev_path, Scene.m_FNLevelPath);
    strcpy(tex_path, "maps");
    FS.m_GameData.Update(lev_path);
    FS.m_GameData.Update(tex_path);
	m_LevelPath.Init( lev_path, "\\", "", "");
	m_TexPath.Init( tex_path, "\\", "", "");

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

		float file_cost = 100.f / (float)filecount;

		// search & remove

		memset(&fdata,0,sizeof(fdata));
		fhandle = _findfirst(findmask,&fdata);
		do{
			char fullname[MAX_PATH];
            if (strcmp(fdata.name,".")==0 || strcmp(fdata.name,"..")==0) continue;
			strcpy( fullname, fdata.name );
			m_LevelPath.Update( fullname );
			unlink( fullname );

			frmBuildProgress->AddProgress( file_cost );

		} while( 0==_findnext(fhandle,&fdata) );
		_findclose( fhandle );
	}

	return true;
}

bool SceneBuilder::ResolveReferences(){

	int objcount = m_Scene->ObjectCount(OBJCLASS_SOBJECT2);
	if( objcount <= 0 )
		return true;

	float object_cost = 100.f / (float)objcount;
	float object_progress = 0.f;

	ObjectIt i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++){
		if((*i)->ClassID()==OBJCLASS_SOBJECT2){
			SObject2 *obj = (SObject2*)(*i);

			if( obj->m_MakeUnique )
				obj->ResolveReference();

			if( !obj->IsReference() )
				obj->TranslateToWorld();

			object_progress += object_cost;
			frmBuildProgress->SetProgress( object_progress );
		}
	}

	return true;
}

bool SceneBuilder::OptimizeObjects(){
	Sleep(2000);
	return true;
}

bool SceneBuilder::GetShift()
{
	int objcount = m_Scene->ObjectCount(OBJCLASS_NONE);
	if( objcount <= 0 )
		return true;

	float object_cost = 100.f / (float)objcount;

	ObjectIt i = m_Scene->FirstObj();

	for(;i!=m_Scene->LastObj();i++){
		frmBuildProgress->AddProgress( object_cost );
		if( (*i)->GetBox(m_LevelBox) )
			break;
	}

	for(;i!=m_Scene->LastObj();i++){
		IAABox box;
		if( (*i)->GetBox(box) ){
			m_LevelBox.growrange( box.vmin.x, box.vmin.y, box.vmin.z );
			m_LevelBox.growrange( box.vmax.x, box.vmax.y, box.vmax.z );
			frmBuildProgress->AddProgress( object_cost );
		}
	}

	m_LevelShift.mul( m_LevelBox.vmin, -1 );

	m_LevelBox.vmax.x -= m_LevelBox.vmin.x;
	m_LevelBox.vmax.y -= m_LevelBox.vmin.y;
	m_LevelBox.vmax.z -= m_LevelBox.vmin.z;
	m_LevelBox.vmin.x = 0;
	m_LevelBox.vmin.y = 0;
	m_LevelBox.vmin.z = 0;

	return true;
}

bool SceneBuilder::ShiftLevel(){
	int objcount = m_Scene->ObjectCount(OBJCLASS_NONE);
	if( objcount <= 0 )
		return true;

	float object_cost = 100.f / (float)objcount;

	ObjectIt i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++){
		(*i)->Move( m_LevelShift );
		frmBuildProgress->AddProgress( object_cost );
	}

	return true;
}

