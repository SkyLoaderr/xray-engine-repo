//----------------------------------------------------
// file: BuilderTexFiles.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "ImageThumbnail.h"
#include "EditObject.h"
#include "SceneObject.h"
//----------------------------------------------------

void SceneBuilder::AddUniqueTexName( const char *nm ){
	VERIFY( nm&&nm[0] );
    char name[255];
    strcpy(name,nm);
    strlwr(name);
	for (AStringIt it=m_TexNames.begin(); it!=m_TexNames.end(); it++)
    	if (*it==name) return;
	m_TexNames.push_back(name);
}
//----------------------------------------------------

//----------------------------------------------------
// write only implicit textures to level folder
//----------------------------------------------------
bool SceneBuilder::WriteTextures(){
	if( m_TexNames.empty() )
		return true;
    bool bRes = true;
    UI.ProgressStart(m_TexNames.size(),"Save textures...");
	for( DWORD i=0; i<m_TexNames.size(); i++){
        if (UI.NeedAbort()) break;
        AnsiString src_nm = m_TexNames[i];
        AnsiString dst_nm = src_nm;
        m_LevelPath.Update(dst_nm);
        EImageThumbnail* m_Thm = new EImageThumbnail(src_nm.c_str());
        STextureParams& F=m_Thm->_Format();
		if (F.flag.bImplicitLighted)
	        FS.CopyFileTo(src_nm.c_str(),dst_nm.c_str(),true);
	    UI.ProgressInc();
	}
    UI.ProgressEnd();
	m_TexNames.clear();
	return bRes;
}
//----------------------------------------------------

