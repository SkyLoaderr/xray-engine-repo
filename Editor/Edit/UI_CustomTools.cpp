#include "stdafx.h"
#pragma hdrstop

#include "ui_customtools.h"
#include "ui_tools.h"
#include "ui_control.h"
#include "ui_main.h"
#include "sceneclasslist.h"
#include "scene.h"

#include "PropertiesObject.h"
#include "PropertiesLight.h"
#include "PropertiesSound.h"
#include "PropertiesOccluder.h"
#include "PropertiesGlow.h"
#include "PropertiesSector.h"
#include "PropertiesPortal.h"
#include "PropertiesEvent.h"
#include "PropertiesPS.h"
#include "PropertiesRPoint.h"

#include "NumericVector.h"

TUI_CustomTools::TUI_CustomTools(EObjClass cls){
	objclass = cls;
    ResetSubTarget();
    pCurControl = 0;
    pFrame		= 0;
}

TUI_CustomTools::~TUI_CustomTools(){
    for (vector<TUI_CustomControl*>::iterator it=m_Controls.begin(); it!=m_Controls.end(); it++) _DELETE (*it);
    m_Controls.clear();
}

void TUI_CustomTools::AddControlCB(TUI_CustomControl* c){
    VERIFY(c);
    m_Controls.push_back(c);
}

TUI_CustomControl* TUI_CustomTools::FindControl(int subtarget, int action){
    for (DWORD i=0; i<m_Controls.size(); i++){
        if ((m_Controls[i]->sub_target==subtarget)&&(m_Controls[i]->action==action))
            return m_Controls[i];
    }
    return 0;
}

void TUI_CustomTools::UpdateControl(){
    if (pCurControl) pCurControl->OnExit();
    VERIFY(!sub_target_stack.empty());
    pCurControl=FindControl(sub_target_stack.back(),action);
    if (pCurControl) pCurControl->OnEnter();
}

void TUI_CustomTools::OnActivate  (){
    if (pCurControl) pCurControl->OnEnter();
}
void TUI_CustomTools::OnDeactivate(){
    if (pCurControl) pCurControl->OnExit();
}
void TUI_CustomTools::SetAction   (int act){
    action=act;
    UpdateControl();
}
void TUI_CustomTools::SetSubTarget(int tgt){
    sub_target_stack.push_back(tgt);
    UpdateControl();
}
void TUI_CustomTools::UnsetSubTarget(int tgt){
    if (!sub_target_stack.empty()){
        vector<int>::iterator it = find(sub_target_stack.begin(),sub_target_stack.end(),tgt);
        sub_target_stack.erase(it,sub_target_stack.end());
    }
    UpdateControl();
}
void TUI_CustomTools::ResetSubTarget(){
    sub_target_stack.clear();
    sub_target_stack.push_back(estSelf);
}

void TUI_CustomTools::ShowProperties(){
    ObjectList objset;
    Scene->GetQueryObjects(objset,objclass);
    bool bChange=false;
    if(!objset.empty()){
	    switch(objclass){
    	case OBJCLASS_EDITOBJECT:	frmPropertiesObjectRun(&objset,bChange); 	break;
	    case OBJCLASS_LIGHT:    	frmPropertiesLightRun(&objset,bChange);		break;
	    case OBJCLASS_SOUND:    	frmPropertiesSoundRun(&objset,bChange); 	break;
	    case OBJCLASS_OCCLUDER: 	frmPropertiesOccluderRun(&objset,bChange); 	break;
	    case OBJCLASS_GLOW:     	frmPropertiesGlowRun(&objset,bChange);		break;
	    case OBJCLASS_SECTOR:   	frmPropertiesSectorRun(&objset,bChange); 	break;
    	case OBJCLASS_PORTAL:   	frmPropertiesPortalRun(&objset,bChange); 	break;
	    case OBJCLASS_EVENT:   		frmPropertiesEventRun(&objset,bChange);		break;
	    case OBJCLASS_RPOINT:   	TfrmPropertiesRPoint::Run(&objset,bChange); break;	
//	    case OBJCLASS_AITRAFFIC:   	TfrmPropertiesAITraffic::Run(&objset,bChange);break;	
    	case OBJCLASS_DPATCH:   	break;
        case OBJCLASS_PS:			TfrmPropertiesPS::Run(&objset,bChange);		break;
    	default:{ Log->DlgMsg(mtError, "Can't find properties form."); throw -1;}
	    }
        if (bChange) Scene->UndoSave();
        UI->RedrawScene();
	}
}

void TUI_CustomTools::SetNumPosition(SceneObject* O){
	ObjectList objset;
    if(O)objset.push_back(O);
    else Scene->GetQueryObjects(objset,objclass);
	if( !objset.empty() ){
    	Fvector V;
        if (objset.front()->GetPosition(V)){
            Fvector RV; RV.set(0,0,0);
            if (mrOk==NumericVectorRun("Position",&V,3,&RV)){
                ObjectIt _F = objset.begin(); ObjectIt _E = objset.end();
                for(;_F!=_E;_F++){ 
                	(*_F)->SetPosition(V);
                    (*_F)->UpdateTransform();
                }
                Scene->UndoSave();
            }
        }
    }
}

void TUI_CustomTools::SetNumRotation(SceneObject* O){
	ObjectList objset;
    if(O)objset.push_back(O);
    else Scene->GetQueryObjects(objset,objclass);
	if( !objset.empty() ){
    	Fvector V;
        if (objset.front()->GetRotate(V)){
            V.set(rad2deg(V.x),rad2deg(V.y),rad2deg(V.z));
            Fvector RV; RV.set(0,0,0);
            Fvector MN; MN.set(-360,-360,-360);
            Fvector MX; MX.set( 360, 360, 360);
            if (mrOk==NumericVectorRun("Rotation",&V,1,&RV,&MN,&MX)){
                ObjectIt _F = objset.begin(); ObjectIt _E = objset.end();
	            V.set(deg2rad(V.x),deg2rad(V.y),deg2rad(V.z));
                for(;_F!=_E;_F++){ 
                	(*_F)->SetRotate(V);
                    (*_F)->UpdateTransform();
                }
                Scene->UndoSave();
            }
        }
    }
}

void TUI_CustomTools::SetNumScale(SceneObject* O){
	ObjectList objset;
    if(O)objset.push_back(O);
    else Scene->GetQueryObjects(objset,objclass);
	if( !objset.empty() ){
    	Fvector V;
        if (objset.front()->GetScale(V)){
            Fvector RV; RV.set(1,1,1);
            Fvector MN; MN.set(0.0001f,0.0001f,0.0001f);
            Fvector MX; MX.set(10000,10000,10000);
            if (mrOk==NumericVectorRun("Scale",&V,3,&RV,&MN,&MX)){
                ObjectIt _F = objset.begin(); ObjectIt _E = objset.end();
                for(;_F!=_E;_F++){ 
                	(*_F)->SetScale(V);
                    (*_F)->UpdateTransform();
                }
                Scene->UndoSave();
            }
        }
    }
}
//---------------------------------------------------------------------------

