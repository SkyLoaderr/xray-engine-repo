//----------------------------------------------------
// file: SceneUtil.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Scene.h"
#include "ELight.h"
#include "SceneObject.h"
#include "ui_tools.h"
#include "PropertiesListTypes.h"
#include "Render.h"

//----------------------------------------------------
CCustomObject* EScene::FindObjectByName( LPCSTR name, EObjClass classfilter )
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

void EScene::GenObjectName( EObjClass cls_id, char *buffer, const char* pref )
{
    m_LastAvailObject = 0;
    string128 prefix; prefix[0]=0;
    if (pref&&pref[0]){
    	strcpy( prefix, pref );
        for (int i=strlen(prefix)-1; i>=0; i--) if (isdigit(prefix[i])) prefix[i]=0; else break;
		sprintf( buffer, "%s%04d", prefix, m_LastAvailObject++);
    }else        sprintf( buffer, "%s%04d", GetNameByClassID(cls_id), m_LastAvailObject++ );
    while (FindObjectByName( buffer, 0 )){
        if (pref&&pref[0])	sprintf( buffer, "%s%04d", prefix, m_LastAvailObject++ );
        else   	  			sprintf( buffer, "%s%04d", GetNameByClassID(cls_id), m_LastAvailObject++ );
	}
}
//------------------------------------------------------------------------------

void EScene::SetLights(){
    if (psDeviceFlags.is(rsLighting)){
        // scene lights
        int i=0;
        int l_cnt=0;
        ObjectList& lst = ListObj(OBJCLASS_LIGHT);
        for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
            CLight* l = (CLight*)(*_F);
            l_cnt++;
            if (l->Visible())
                if ((l->m_D3D.type==D3DLIGHT_DIRECTIONAL) || ::Render->ViewBase.testSphere_dirty(l->m_D3D.position,l->m_D3D.range)){
                    l->Set( i++ );
                    frame_light.push_back(l);
                    l->Enable(FALSE);
                }
        }
        Device.Statistic.dwTotalLight = l_cnt;
        Device.Statistic.dwLightInScene = frame_light.size();
    }
}

void EScene::TurnLightsForObject(CSceneObject* obj)
{
    for (u32 i=0; i<frame_light.size(); i++){
        CLight* l = frame_light[i];

        if (!l->m_Flags.is(CLight::flAffectDynamic)&&!l->m_Flags.is(CLight::flAffectStatic)) continue;
        if (obj->IsDynamic()&&!l->m_Flags.is(CLight::flAffectDynamic)) continue;
        if (!obj->IsDynamic()&&!l->m_Flags.is(CLight::flAffectStatic)) continue;

        if (l->m_D3D.type==D3DLIGHT_DIRECTIONAL){
            l->Enable(TRUE);
        }else{
        	Fbox bb; 	obj->GetBox(bb);
            Fvector C; 	float R; bb.getsphere(C,R);
            float d = C.distance_to(*((Fvector*)&l->m_D3D.position)) - l->m_D3D.range - R;
            if (d<0) l->Enable(TRUE);
            else  l->Enable(FALSE);
        }
    }
}

void EScene::ClearLights(){
    for (u32 i=0; i<frame_light.size(); i++)
        frame_light[i]->UnSet();
    frame_light.clear();
}

