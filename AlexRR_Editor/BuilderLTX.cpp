//----------------------------------------------------
// file: BuilderLTX.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"
#include "Scene.h"
#include "SceneClassList.h"
#include "SObject2.h"
#include "SndPlane.h"
#include "Sound.h"
//----------------------------------------------------

extern "C" {
	void  (WINAPI * CompileLTX)( char *src, char *dest );
}

//----------------------------------------------------

bool SceneBuilder::BuildLTX(){

	int objcount = m_Scene->ObjectCount(OBJCLASS_SOBJECT2);
	if( objcount <= 0 )
		return true;

	float object_cost = 100.f / 
		(float)(objcount+
				m_Scene->ObjectCount(OBJCLASS_SNDPLANE)+
				m_Scene->ObjectCount(OBJCLASS_SOUND));
	float object_progress = 0.f;

	char ltx_filename[MAX_PATH];
	strcpy( ltx_filename, Scene.m_FNLtx );
	m_LevelPath.Update( ltx_filename );
	int handle = FS.create( ltx_filename );

	// -- defaults --

	if( m_Scene->m_BOPText[0] ){
		FS.write( handle, m_Scene->m_BOPText, 
			strlen( m_Scene->m_BOPText ) );
	}

	// -- mobile objects --

	SSTRLIST mobsection;

	ObjectIt i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++){
		if((*i)->ClassID()==OBJCLASS_SOBJECT2){
			SObject2 *obj = (SObject2*)(*i);

			if( obj->m_DynamicList ){

				mobsection.push_back( SSTR( obj->m_Name ) );
				
				FS.print( handle, "\r\n[%s]", obj->m_Name );

				if( obj->IsReference() ){
					FS.print( handle, "\r\nposition=%.3f,%.3f,%.3f",
						obj->m_Position.c.x,
						obj->m_Position.c.y,
						obj->m_Position.c.z );
					FS.print( handle, "\r\ndirection=%.3f,%.3f,%.3f",
						obj->m_Position.k.x,
						obj->m_Position.k.y,
						obj->m_Position.k.z );
					FS.print( handle, "\r\nnormal=%.3f,%.3f,%.3f",
						obj->m_Position.j.x,
						obj->m_Position.j.y,
						obj->m_Position.j.z );
				} else {
					if( obj->m_OGFCreated )
						FS.print( handle, "\r\nvisual=%s",obj->m_OGFName );
				}

				if( obj->m_ClassScript[0] )
					FS.print( handle, "\r\n%s", obj->m_ClassScript );

				FS.print( handle, "\r\n\r\n" );
			}

			object_progress += object_cost;
			BWindow.SetProgress( object_progress );
		}
	}

	if( !mobsection.empty() ){
		FS.print( handle, "\r\n[mobileobjects]" );
		for(int i=0;i<mobsection.size();i++)
			FS.print( handle, "\r\n%s=%s",
			mobsection[i].filename,
			mobsection[i].filename );
		FS.print( handle, "\r\n\r\n" );
	}

	// -- add sound cliping --
	if (m_Scene->ObjectCount(OBJCLASS_SNDPLANE)) {
		FS.print( handle, "\r\n[sound_environment]" );
		
		i = m_Scene->FirstObj(); int s_cnt=0;
		for(;i!=m_Scene->LastObj();i++){
			if((*i)->ClassID()==OBJCLASS_SNDPLANE){
				SndPlane *s = (SndPlane*)(*i);
				FS.print( handle, 
					"\r\nclip%d = [%f,%f,%f],[%f,%f,%f],[%f,%f,%f],%f",
					s_cnt,
					s->m_Points[0].x,s->m_Points[0].y,s->m_Points[0].z,
					s->m_Points[1].x,s->m_Points[1].y,s->m_Points[1].z,
					s->m_Points[3].x,s->m_Points[3].y,s->m_Points[3].z,
					s->m_Dencity
					);
				
				s_cnt++;
				object_progress += object_cost;
				BWindow.SetProgress( object_progress );
			}
		}

		FS.print( handle, "\r\n\r\n" );
	}

	// -- add static sounds --
	if (m_Scene->ObjectCount(OBJCLASS_SOUND)) {
		FS.print( handle, "\r\n[static_sounds]" );
		
		i = m_Scene->FirstObj();
		for(;i!=m_Scene->LastObj();i++){
			if((*i)->ClassID()==OBJCLASS_SOUND){
				CSound *s = (CSound *)(*i);
				FS.print( handle, 
					"\r\n%s = [%f,%f,%f],%f",
					s->m_fName,
					s->m_Position.x,s->m_Position.y,s->m_Position.z,
					s->m_Range
					);
				
				object_progress += object_cost;
				BWindow.SetProgress( object_progress );
			}
		}

		FS.print( handle, "\r\n\r\n" );
	}


	FS.close( handle );

	return true;
}

bool SceneBuilder::CompileText(){

	HMODULE hlib = LoadLibrary( "tools\\copt.dll" );
	if( hlib==0 ){
		NConsole.print( "Failed to load COPT.DLL" );
		return false;
	}

	CompileLTX = (void (WINAPI*)(char*,char*))
		GetProcAddress( hlib, "CompileLTX" );

	if( CompileLTX == 0 ){
		NConsole.print( "Invalid COPT.DLL" );
		FreeLibrary( hlib );
		return false;
	}

	char srcfile[MAX_PATH];
	strcpy(srcfile,m_Scene->m_FNLtx);
	m_LevelPath.Update( srcfile );
	
	char destfile[MAX_PATH];
	_splitpath(m_Scene->m_FNLtx,0,0,destfile,0);
	strcat(destfile,".btx" );
	m_LevelPath.Update( destfile );
	
	CompileLTX( srcfile, destfile );

	FreeLibrary( hlib );

	return true;
}
