//----------------------------------------------------
// file: SceneUtil.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Log.h"
#include "Scene.h"
#include "SceneClassList.h"
#include "Light.h"
#include "SObject2.h"
#include "ui_main.h"
#include "VisibilityTester.h"
#include "Statistic.h"

//----------------------------------------------------

bool EScene::SearchName( char *name ){
	ObjectIt obj = m_Objects.begin();
	for(;obj!=m_Objects.end();obj++)
		if( 0==stricmp((*obj)->GetName(),name) )
			return true;
	return false;
}

void EScene::GenObjectName( char *buffer ){
	do{
		sprintf( buffer, "obj_%04d", m_LastAvailObject );
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
    if (UI.bRenderLights){
        // scene lights
        int i=0;
        int l_cnt=0;
        ObjectIt _F = m_Objects.begin();
        for(;_F!=m_Objects.end();_F++)
            if( (*_F)->ClassID()==OBJCLASS_LIGHT ){
                Light* l = (Light*)(*_F);
                l_cnt++;
                if (l->Visible())
                    if ((l->m_Type==ltSun) || UI.frustum->visibleSphere(*((Fvector*)&l->m_Position),l->m_Range)){
                        l->Set( i++ );
                        frame_light.push_back(l);
                        l->Enable(FALSE);
                    }
            }
        UI.stat->lTotalLight = l_cnt;
        UI.stat->lLightInScene = frame_light.size();
    }
}

void EScene::TurnLightsForObject(SObject2* obj)
{
    for (DWORD i=0; i<frame_light.size(); i++){
        Light* l = frame_light[i];
        if (l->m_Type==ltSun){
            l->Enable(TRUE);
        }else{
            float d = obj->m_Center.distance_to(*((Fvector*)&l->m_Position)) - l->m_Range - obj->m_fRadius;
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


void EScene::RebuildAllObjects(){
    VERIFY(0);
//	ObjectIt _F = m_Objects.begin();
//	for(;_F!=m_Objects.end();_F++)
//		if( (*_F)->ClassID()==OBJCLASS_SOBJECT2 )
//			((SObject2*)(*_F))->FillD3DPoints();
}
