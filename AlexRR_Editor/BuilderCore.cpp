//----------------------------------------------------
// file: BuilderCore.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "Landscape.h"
#include "SObject2.h"
//----------------------------------------------------

bool SceneBuilder::ConvertLandscape(){

	int objcount = m_Scene->ObjectCount(OBJCLASS_LANDSCAPE);
	if( objcount <= 0 )
		return true;

	float object_cost = 100.f / (float)objcount;
	float object_progress = 0.f;


	bool no_more_landscapes = false;
	while( !no_more_landscapes ){
		no_more_landscapes = true;
		ObjectIt i = m_Scene->FirstObj();
		while( i!=m_Scene->LastObj() ){
			if((*i)->ClassID()==OBJCLASS_LANDSCAPE){

				SObject2 *obj = new SObject2();
				if( !obj->CreateFromLandscape( (SLandscape*)(*i) ) ){
					delete obj;
					return false;
				}

				delete (*i);
				m_Scene->RemoveObject( (*i) );
				m_Scene->AddObject( obj );

				object_progress += object_cost;
				BWindow.SetProgress( object_progress );
				no_more_landscapes = false;
				
				break;

			} else {
				i++;
			}
		}
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
			BWindow.SetProgress( object_progress );
		}
	}

	return true;
}

bool SceneBuilder::PrepareFolders(){

	m_LevelPath.Init( Scene.m_FNLevelPath, "\\", "", "");
	m_TexPath.Init( Scene.m_FNTexPath, "\\", "", "");

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
		float cur_progress = 0.f;

		// search & remove

		memset(&fdata,0,sizeof(fdata));
		fhandle = _findfirst(findmask,&fdata);
		do{
			char fullname[MAX_PATH];
			strcpy( fullname, fdata.name );
			m_LevelPath.Update( fullname );
			unlink( fullname );

			cur_progress += file_cost;
			BWindow.SetProgress( cur_progress );

		} while( 0==_findnext(fhandle,&fdata) );
		_findclose( fhandle );
	}

	return true;
}

bool SceneBuilder::OptimizeObjects(){
	Sleep(2000);
	return true;
}


bool SceneBuilder::GetShift(){
	
	int objcount = m_Scene->ObjectCount(-1);
	if( objcount <= 0 )
		return true;
	
	float object_cost = 100.f / (float)objcount;
	float object_progress = 0.f;

	ObjectIt i = m_Scene->FirstObj();
	
	for(;i!=m_Scene->LastObj();i++){
		object_progress += object_cost;
		BWindow.SetProgress( object_progress );
		if( (*i)->GetBox(m_LevelBox) )
			break;
	}

	for(;i!=m_Scene->LastObj();i++){
		IAABox box;
		if( (*i)->GetBox(box) ){
			m_LevelBox.growrange( box.vmin.x, box.vmin.y, box.vmin.z );
			m_LevelBox.growrange( box.vmax.x, box.vmax.y, box.vmax.z );
			object_progress += object_cost;
			BWindow.SetProgress( object_progress );
		}
	}

	m_LevelShift.inverse( m_LevelBox.vmin );
	m_LevelShift.x += m_Scene->m_BOPIndentLow.x;
	m_LevelShift.z += m_Scene->m_BOPIndentLow.z;
	m_LevelShift.y = 0;

	m_LevelBox.vmax.x -= m_LevelBox.vmin.x;
	m_LevelBox.vmax.z -= m_LevelBox.vmin.z;
	m_LevelBox.vmax.x += m_Scene->m_BOPIndentLow.x;
	m_LevelBox.vmax.z += m_Scene->m_BOPIndentLow.z;
	m_LevelBox.vmax.x += m_Scene->m_BOPIndentHigh.x;
	m_LevelBox.vmax.z += m_Scene->m_BOPIndentHigh.z;
	m_LevelBox.vmin.x = 0;
	m_LevelBox.vmin.z = 0;

	return true;
}

bool SceneBuilder::ShiftLevel(){
	int objcount = m_Scene->ObjectCount(-1);
	if( objcount <= 0 )
		return true;
	
	float object_cost = 100.f / (float)objcount;
	float object_progress = 0.f;

	ObjectIt i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++){
		(*i)->Move( m_LevelShift );
		object_progress += object_cost;
		BWindow.SetProgress( object_progress );
	}

	return true;
}
