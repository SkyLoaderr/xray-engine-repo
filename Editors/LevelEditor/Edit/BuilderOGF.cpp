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
#include "fmesh.h"
#include "std_classes.h"
//----------------------------------------------------
// some types
bool SceneBuilder::BuildSkyModel(){
	// build sky ogf
    if (Scene.m_SkyDome){
	    AnsiString ogf_name = m_LevelPath+Scene.m_SkyDome->Name+".ogf";
        CEditableObject* O = Scene.m_SkyDome->GetReference(); R_ASSERT(O);
        return O->ExportObjectOGF(ogf_name.c_str());
    }
	return false;
}

bool SceneBuilder::BuildHOMModel()
{
	CMemoryWriter F;

    F.open_chunk(0);
    F.w_u32(0);
    F.close_chunk();

    F.open_chunk(1);
    ObjectList& lst = Scene.ListObj(OBJCLASS_SCENEOBJECT);
    for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
    	CSceneObject* S 	= (CSceneObject*)(*it);
    	CEditableObject* E	= S->GetReference(); R_ASSERT(E);
    	if (E->m_Flags.is(CEditableObject::eoHOM)) E->ExportHOMPart(S->_Transform(),F);
    }
    BOOL bValid = !!F.chunk_size();
    F.close_chunk();
    if (bValid){
	    AnsiString hom_name = m_LevelPath+"level.hom";
	    F.save_to(hom_name.c_str());
    }
	return bValid;
}

