//----------------------------------------------------
// file: SceneUtil.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "Scene.h"
#include "SceneClassList.h"
#include "ELight.h"
#include "EditObject.h"
#include "ui_main.h"
#include "Frustum.h"
#include "Statistic.h"
#include "ui_tools.h"

//----------------------------------------------------

bool EScene::SearchName( char *name ){
    for(ObjectPairIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
        ObjectList& lst = (*it).second;
    	for(ObjectIt _F = lst.begin();_F!=lst.end();_F++)
    		if( 0==stricmp((*_F)->GetName(),name) ) return true;
    }
	return false;
}

void EScene::GenObjectName( EObjClass cls_id, char *buffer, const char* prefix ){
    m_LastAvailObject = ListObj(cls_id).size();
	do{
        if (prefix)
       		sprintf( buffer, "%s_%04d", prefix, m_LastAvailObject );
        else
       		sprintf( buffer, "%s_%04d", GetNameByClassID(cls_id), m_LastAvailObject );
		m_LastAvailObject++;
	} while( SearchName( buffer ) );
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
    if (UI->bRenderLights){
        // scene lights
        int i=0;
        int l_cnt=0;
        ObjectList& lst = ListObj(OBJCLASS_LIGHT);
        for(ObjectIt _F = lst.begin();_F!=lst.end();_F++){
            CLight* l = (CLight*)(*_F);
            l_cnt++;
            if (l->Visible())
                if ((l->m_D3D.type==D3DLIGHT_DIRECTIONAL) || UI->Device.m_Frustum.testSphere(l->m_D3D.position,l->m_D3D.range)){
                    l->Set( i++ );
                    frame_light.push_back(l);
                    l->Enable(FALSE);
                }
        }
        UI->Device.m_Statistic.dwTotalLight = l_cnt;
        UI->Device.m_Statistic.dwLightInScene = frame_light.size();
    }
}

void EScene::TurnLightsForObject(CEditObject* obj){
    for (DWORD i=0; i<frame_light.size(); i++){
        CLight* l = frame_light[i];
        
        if (!(l->m_D3D.flags&XRLIGHT_MODELS)&&!(l->m_D3D.flags&XRLIGHT_LMAPS)) continue;
        if (obj->IsDynamic()&&!(l->m_D3D.flags&XRLIGHT_MODELS)) continue;
        if (!obj->IsDynamic()&&!(l->m_D3D.flags&XRLIGHT_LMAPS)) continue;

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

