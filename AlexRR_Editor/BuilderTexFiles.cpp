//----------------------------------------------------
// file: BuilderTexFiles.cpp
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

void SceneBuilder::AddUniqueTexName( char *name ){
	_ASSERTE( name );
	if( name[0] != 0 ){
		if( m_TexNames.empty() ){
			m_TexNames.push_back( SSTR(name) );
		} else {
			bool found = false;
			for( int i=0; i<m_TexNames.size(); i++)
				if( 0==stricmp(m_TexNames[i].filename,name) ){
					found = true;
					break;
				}
			if( !found )
				m_TexNames.push_back( SSTR(name) );
		}
	}
}

bool SceneBuilder::WriteTextures(){

	if( m_TexNames.empty() )
		return true;

	float file_cost = 100.f / (float) m_TexNames.size();
	float current_progress = 0;

	for( int i=0; i<m_TexNames.size(); i++){

		char out_filename[MAX_PATH];
		_splitpath(m_TexNames[i].filename,0,0,out_filename,0);
		strcat( out_filename, ".t" );
		m_TexPath.Update( out_filename );

		ETexture *t = new ETexture( m_TexNames[i].filename );
		if( !t->MakeT( out_filename ) ){
			delete t;
			return false;
		}

		delete t;

		current_progress += file_cost;
		BWindow.SetProgress( current_progress );
	}

	return true;
}
