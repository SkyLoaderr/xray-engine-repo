//----------------------------------------------------
// file: BuilderRModel.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"

#include "Scene.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "SceneObject.h"
#include "ESceneAIMapTools.h"
//----------------------------------------------------
// some types
bool SceneBuilder::BuildHOMModel()
{
	CMemoryWriter F;

    F.open_chunk(0);
    F.w_u32(0);
    F.close_chunk();

    F.open_chunk(1);
    ObjectList& lst = Scene->ListObj(OBJCLASS_SCENEOBJECT);
    for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
    	CSceneObject* S 	= (CSceneObject*)(*it);
    	CEditableObject* E	= S->GetReference(); R_ASSERT(E);
    	if (E->m_Flags.is(CEditableObject::eoHOM)) E->ExportHOMPart(S->_Transform(),F);
    }
    BOOL bValid = !!F.chunk_size();
    F.close_chunk();
    if (bValid){
	    xr_string hom_name = MakeLevelPath("level.hom");
	    F.save_to(hom_name.c_str());
    }
	return bValid;
}

bool SceneBuilder::BuildAIMap()
{
	// build sky ogf
    if (Scene->GetMTools(OBJCLASS_AIMAP)->Valid()){
        return Scene->GetMTools(OBJCLASS_AIMAP)->Export(m_LevelPath.c_str());
    }
	return false;
}

bool SceneBuilder::BuildWallmarks()
{
	// build sky ogf
    if (Scene->GetMTools(OBJCLASS_WM)->Valid()){
        return Scene->GetMTools(OBJCLASS_WM)->Export(m_LevelPath.c_str());
    }
	return false;
}

