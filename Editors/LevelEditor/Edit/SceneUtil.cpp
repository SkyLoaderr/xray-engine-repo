//----------------------------------------------------
// file: SceneUtil.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Scene.h"
#include "ELight.h"
#include "SceneObject.h"
#include "Frustum.h"
#include "ui_tools.h"
#include "PropertiesListTypes.h"
#include "Render.h"

//----------------------------------------------------
CCustomObject* EScene::FindObjectByName( LPCSTR name, EObjClass classfilter ){
	ObjectIt _F = FirstObj(classfilter);
    ObjectIt _E = LastObj(classfilter);
	for(;_F!=_E;_F++) if(0==stricmp((*_F)->Name,name)) return (*_F);
    return 0;
}

CCustomObject* EScene::FindObjectByName( LPCSTR name, CCustomObject* pass_object ){
    for(ObjectPairIt it=FirstClass(); it!=LastClass(); it++){
        ObjectList& lst = (*it).second;
    	ObjectIt _F = lst.begin();
        ObjectIt _E = lst.end();
    	for(;_F!=_E;_F++){
            if( (0==stricmp((*_F)->Name,name)) && (pass_object!=(*_F)) )
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
            if (FindObjectByName((*_F)->Name, *_F)){
//                char buf[1024];
//                GenObjectName((*_F)->ClassID(),buf);
//                strcpy((*_F)->GetName(),buf);
            	ELog.DlgMsg(mtError,"Duplicate object name already exists: '%s'",(*_F)->Name);
                return true;
            }
	}
    return false;
}

void EScene::GenObjectName( EObjClass cls_id, char *buffer, const char* pref ){
    m_LastAvailObject = 0;
    string128 prefix; prefix[0]=0;
    if (pref&&pref[0]){                
    	strcpy( prefix, pref );
        for (int i=strlen(prefix)-1; i>=0; i--) if (isdigit(prefix[i])) prefix[i]=0; else break;
		sprintf( buffer, "%s%04d", prefix, m_LastAvailObject++);
    }else        sprintf( buffer, "%s%04d", GetNameByClassID(cls_id), m_LastAvailObject++ );
    while (FindObjectByName( buffer, 0 )){
        if (pref)	sprintf( buffer, "%s%04d", prefix, m_LastAvailObject++ );
        else   		sprintf( buffer, "%s%04d", GetNameByClassID(cls_id), m_LastAvailObject++ );
	}
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
    for (DWORD i=0; i<frame_light.size(); i++){
        CLight* l = frame_light[i];

        if (!l->m_dwFlags&CLight::flAffectDynamic&&!l->m_dwFlags&CLight::flAffectStatic) continue;
        if (obj->IsDynamic()&&!l->m_dwFlags&CLight::flAffectDynamic) continue;
        if (!obj->IsDynamic()&&!l->m_dwFlags&CLight::flAffectStatic) continue;

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
    for (DWORD i=0; i<frame_light.size(); i++)
        frame_light[i]->UnSet();
    frame_light.clear();
}

