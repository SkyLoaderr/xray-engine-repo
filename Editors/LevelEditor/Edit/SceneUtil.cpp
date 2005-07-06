//----------------------------------------------------
// file: SceneUtil.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Scene.h"
#include "ELight.h"
#include "SceneObject.h"
#include "ui_leveltools.h"
#include "PropertiesListTypes.h"
#include "Render.h"

//----------------------------------------------------
CCustomObject* EScene::FindObjectByName( LPCSTR name, ObjClassID classfilter )
{
	CCustomObject* object = 0;
    if (classfilter==OBJCLASS_DUMMY){
        SceneToolsMapPairIt _I = m_SceneTools.begin();
        SceneToolsMapPairIt _E = m_SceneTools.end();
        for (; _I!=_E; _I++){
            ESceneCustomOTools* mt = dynamic_cast<ESceneCustomOTools*>(_I->second);
            if (mt&&(0!=(object=mt->FindObjectByName(name)))) return object;
        }
    }else{
        ESceneCustomOTools* mt = GetOTools(classfilter); VERIFY(mt);
        if (mt&&(0!=(object=mt->FindObjectByName(name)))) return object;
    }
    return object;
}

CCustomObject* EScene::FindObjectByName( LPCSTR name, CCustomObject* pass_object )
{
	CCustomObject* object = 0;
    SceneToolsMapPairIt _I = m_SceneTools.begin();
    SceneToolsMapPairIt _E = m_SceneTools.end();
    for (; _I!=_E; _I++){
        ESceneCustomOTools* mt = dynamic_cast<ESceneCustomOTools*>(_I->second);
        if (mt&&(0!=(object=mt->FindObjectByName(name,pass_object)))) return object;
    }
    return 0;
}

bool EScene::FindDuplicateName()
{
// find duplicate name
    SceneToolsMapPairIt _I = m_SceneTools.begin();
    SceneToolsMapPairIt _E = m_SceneTools.end();
    for (; _I!=_E; _I++){
        ESceneCustomOTools* mt = dynamic_cast<ESceneCustomOTools*>(_I->second);
        if (mt){
        	ObjectList& lst = mt->GetObjects(); 
            for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
                if (FindObjectByName((*_F)->Name, *_F)){
                    ELog.DlgMsg(mtError,"Duplicate object name already exists: '%s'",(*_F)->Name);
                    return true;
                }
        }
    }
    return false;
}

void EScene::GenObjectName( ObjClassID cls_id, char *buffer, const char* pref )
{
	ESceneCustomOTools* ot = GetOTools(cls_id); VERIFY(ot);
    AnsiString result	= FHelper.GenerateName(pref&&pref[0]?pref:ot->ClassName(),4,fastdelegate::bind<TFindObjectByName>(this,&EScene::FindObjectByNameCB),true,true);
    strcpy				(buffer,result.c_str());
}
//------------------------------------------------------------------------------


