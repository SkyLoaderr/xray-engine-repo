//----------------------------------------------------
// file: BuilderTexFiles.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "Texture.h"
#include "EditObject.h"
//----------------------------------------------------

void SceneBuilder::AddUniqueTexName(CEditObject* obj){
	CEditObject *O = obj->IsReference()?obj->GetRef():obj;
    for (SurfaceIt s_it=O->FirstSurface(); s_it!=O->LastSurface(); s_it++)
        for (AStringIt t_it=(*s_it)->textures.begin(); t_it!=(*s_it)->textures.end(); t_it++)
            AddUniqueTexName(t_it->c_str());
}
//----------------------------------------------------

void SceneBuilder::AddUniqueTexName( const char *name ){
	VERIFY( name );
	if( name[0] != 0 ){
		if( m_TexNames.empty() ){
			m_TexNames.push_back( SSTR(name) );
		} else {
			bool found = false;
			for( DWORD i=0; i<m_TexNames.size(); i++)
				if( 0==stricmp(m_TexNames[i].filename,name) ){
					found = true;
					break;
				}
			if( !found )
				m_TexNames.push_back( SSTR(name) );
		}
	}
}
//----------------------------------------------------

//----------------------------------------------------
// only implicit lighted
//----------------------------------------------------
bool SceneBuilder::WriteTextures(){
	if( m_TexNames.empty() )
		return true;
    bool bRes = true;
    UI->ProgressStart(m_TexNames.size(),"Save textures...");
	for( DWORD i=0; i<m_TexNames.size(); i++){
        if (NeedAbort()) break;
		char out_filename[MAX_PATH];
		_splitpath(m_TexNames[i].filename,0,0,out_filename,0);
		ETextureCore *t = new ETextureCore( out_filename );
        if(!t->MakeGameTexture(out_filename)) bRes = false;
		_DELETE(t);
	    UI->ProgressInc();
	}
    UI->ProgressEnd();
	m_TexNames.clear();
	return bRes;
}
//----------------------------------------------------

