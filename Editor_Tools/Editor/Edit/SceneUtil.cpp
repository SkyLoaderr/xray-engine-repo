//----------------------------------------------------
// file: SceneUtil.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Scene.h"
#include "SceneClassList.h"
#include "ELight.h"
#include "SceneObject.h"
#include "ui_main.h"
#include "Frustum.h"
#include "ui_tools.h"

//----------------------------------------------------

CCustomObject* EScene::FindObjectByName( char *name, EObjClass classfilter ){
	ObjectIt _F = FirstObj(classfilter);
    ObjectIt _E = LastObj(classfilter);
	for(;_F!=_E;_F++) if(0==stricmp((*_F)->GetName(),name)) return (*_F);
    return 0;
}

CCustomObject* EScene::FindObjectByName( char *name, CCustomObject* pass_object ){
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	ObjectIt _F = lst.begin();
        ObjectIt _E = lst.end();
    	for(;_F!=_E;_F++){
            if( (0==strcmp((*_F)->GetName(),name)) && (pass_object!=(*_F)) )
                return (*_F);
        }
	}
    return 0;
}

bool EScene::FindDuplicateName(){
// find duplicate name
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
            if (FindObjectByName((*_F)->GetName(), *_F)){
//                char buf[1024];
//                GenObjectName((*_F)->ClassID(),buf);
//                strcpy((*_F)->GetName(),buf);
            	ELog.DlgMsg(mtError,"Duplicate object name already exists: '%s'",(*_F)->GetName());
                return true;
            }
	}
    return false;
}

void EScene::GenObjectName( EObjClass cls_id, char *buffer, const char* prefix ){
    m_LastAvailObject = ListObj(cls_id).size();
	do{
        if (prefix)
       		sprintf( buffer, "%s_%02d", prefix, m_LastAvailObject );
        else
       		sprintf( buffer, "%s_%02d", GetNameByClassID(cls_id), m_LastAvailObject );
		m_LastAvailObject++;
	} while( FindObjectByName( buffer, 0 ) );
}

//----------------------------------------------------
/*
void EScene::SmoothLandscape(){

	ObjectIt f = m_Objects.begin();

	for(;f!=m_Objects.end();f++)

		if( (*f)->ClassID() == OBJCLASS_LANDSCAPE ){

			SLandscape *l = (SLandscape *)(*f);
			l->ResetPointNormals();

			ObjectIt f1 = m_Objects.begin();
			for(;f1!=m_Objects.end();f1++)
				if( (f != f1) && (*f1)->ClassID() == OBJCLASS_LANDSCAPE )
					l->TryAddPointNormals((SLandscape *)(*f1));

			l->NormalizePointNormals();
			l->FillD3DPoints();
		}

}
*/
//----------------------------------------------------

void EScene::SetLights(){
    if (psDeviceFlags&rsLighting){
        // scene lights
        int i=0;
        int l_cnt=0;
        ObjectList& lst = ListObj(OBJCLASS_LIGHT);
        for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
            CLight* l = (CLight*)(*_F);
            l_cnt++;
            if (l->Visible())
                if ((l->m_D3D.type==D3DLIGHT_DIRECTIONAL) || Device.m_Frustum.testSphere(l->m_D3D.position,l->m_D3D.range)){
                    l->Set( i++ );
                    frame_light.push_back(l);
                    l->Enable(FALSE);
                }
        }
        Device.Statistic.dwTotalLight = l_cnt;
        Device.Statistic.dwLightInScene = frame_light.size();
    }
}

void EScene::TurnLightsForObject(CSceneObject* obj){
    for (DWORD i=0; i<frame_light.size(); i++){
        CLight* l = frame_light[i];

        if (!l->m_Flags.bAffectDynamic&&!l->m_Flags.bAffectStatic) continue;
        if (obj->IsDynamic()&&!l->m_Flags.bAffectDynamic) continue;
        if (!obj->IsDynamic()&&!l->m_Flags.bAffectStatic) continue;

        if (l->m_D3D.type==D3DLIGHT_DIRECTIONAL){
            l->Enable(TRUE);
        }else{
            float d = obj->GetCenter().distance_to(*((Fvector*)&l->m_D3D.position)) - l->m_D3D.range - obj->GetRadius();
            if (d<0) l->Enable(TRUE);
            else  l->Enable(FALSE);
        }
    }
}

void EScene::ClearLights(){
    for (DWORD i=0; i<frame_light.size(); i++)
        frame_light[i]->UnSet();
    frame_light.clear();
}

