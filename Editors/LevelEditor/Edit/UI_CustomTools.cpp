#include "stdafx.h"
#pragma hdrstop

#include "ui_customtools.h"
#include "ui_tools.h"
#include "ui_control.h"
#include "ui_main.h"
#include "sceneclasslist.h"
#include "scene.h"

#include "PropertiesSceneObject.h"
#include "PropertiesGroup.h"
#include "PropertiesLight.h"
#include "PropertiesSound.h"
#include "PropertiesGlow.h"
#include "PropertiesSector.h"
#include "PropertiesPortal.h"
#include "PropertiesEvent.h"
#include "PropertiesPS.h"
#include "PropertiesRPoint.h"
#include "PropertiesWayPoint.h"

#include "PropertiesList.h"
#include "NumericVector.h"

TUI_CustomTools::TUI_CustomTools(EObjClass cls){
	objclass = cls;
    ResetSubTarget();
    pCurControl = 0;
    pFrame		= 0;
    for (int a=0; a<eaMaxActions; a++)
        m_Controls[EAction(a)]= new TUI_CustomControl(estSelf,a,this);
}

TUI_CustomTools::~TUI_CustomTools(){
	for (ControlsPairIt it=m_Controls.begin(); it!=m_Controls.end(); it++) _DELETE(it->second);
    m_Controls.clear();
}

void TUI_CustomTools::AddControlCB(TUI_CustomControl* c){
    VERIFY(c);
	ControlsPairIt it=m_Controls.find(EAction(c->action));
    VERIFY(it!=m_Controls.end());
    _DELETE(it->second);
    it->second = c;
}

TUI_CustomControl* TUI_CustomTools::FindControl(int subtarget, int action){
	if (action==-1) return 0;
	ControlsPairIt it=m_Controls.find(EAction(action));
    VERIFY(it!=m_Controls.end());
	return it->second;
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
    Scene.GetQueryObjects(objset,objclass);
    bool bChange=false;
    if(!objset.empty()){
//    	TfrmProperties* P = new TfrmProperties(0);
	    switch(objclass){
    	case OBJCLASS_GROUP:		TfrmPropertiesGroup::Run(&objset,bChange); break;
    	case OBJCLASS_SCENEOBJECT:	TfrmPropertiesSceneObject::Run(&objset,bChange); break;
	    case OBJCLASS_LIGHT:    	frmPropertiesLightRun(&objset,bChange);		break;
	    case OBJCLASS_SOUND:    	frmPropertiesSoundRun(&objset,bChange); 	break;
	    case OBJCLASS_GLOW:     	frmPropertiesGlowRun(&objset,bChange);		break;
	    case OBJCLASS_SECTOR:   	frmPropertiesSectorRun(&objset,bChange); 	break;
	    case OBJCLASS_EVENT:   		frmPropertiesEventRun(&objset,bChange);		break;
	    case OBJCLASS_RPOINT:   	TfrmPropertiesRPoint::Run(&objset,bChange); break;
	    case OBJCLASS_WAY:   		TfrmPropertiesWayPoint::Run(&objset,bChange);break;
        case OBJCLASS_PS:			TfrmPropertiesPS::Run(&objset,bChange);		break;
    	default:{ ELog.Msg(mtInformation, "Can't find properties form.");}
	    }
//        if (P->IsModified()) Scene.UndoSave();
        if (bChange) Scene.UndoSave();
        UI.RedrawScene();
//        _DELETE(P);
	}
}

void TUI_CustomTools::SetNumPosition(CCustomObject* O){
	ObjectList objset;
    if(O)objset.push_back(O);
    else Scene.GetQueryObjects(objset,objclass);
	if( !objset.empty() ){
    	Fvector V = objset.front()->PPosition;
        {
            Fvector RV; RV.set(0,0,0);
            if (mrOk==NumericVectorRun("Position",&V,3,&RV)){
                ObjectIt _F = objset.begin(); ObjectIt _E = objset.end();
                for(;_F!=_E;_F++)
                	(*_F)->NumSetPosition(V);
                Scene.UndoSave();
            }
        }
    }
}

void TUI_CustomTools::SetNumRotation(CCustomObject* O){
	ObjectList objset;
    if(O)objset.push_back(O);
    else Scene.GetQueryObjects(objset,objclass);
	if( !objset.empty() ){
    	Fvector V=objset.front()->PRotation;
        {
            V.set(rad2deg(V.x),rad2deg(V.y),rad2deg(V.z));
            Fvector RV; RV.set(0,0,0);
            Fvector MN; MN.set(-10000,-10000,-10000);
            Fvector MX; MX.set( 10000, 10000, 10000);
            if (mrOk==NumericVectorRun("Rotation",&V,1,&RV,&MN,&MX)){
                ObjectIt _F = objset.begin(); ObjectIt _E = objset.end();
	            V.set(deg2rad(V.x),deg2rad(V.y),deg2rad(V.z));
                for(;_F!=_E;_F++)
                	(*_F)->NumSetRotation(V);
                Scene.UndoSave();
            }
        }
    }
}

void TUI_CustomTools::SetNumScale(CCustomObject* O){
	ObjectList objset;
    if(O)objset.push_back(O);
    else Scene.GetQueryObjects(objset,objclass);
	if( !objset.empty() ){
    	Fvector V=objset.front()->PScale;
        {
            Fvector RV; RV.set(1,1,1);
            Fvector MN; MN.set(0.0001f,0.0001f,0.0001f);
            Fvector MX; MX.set(10000,10000,10000);
            if (mrOk==NumericVectorRun("Scale",&V,3,&RV,&MN,&MX)){
                ObjectIt _F = objset.begin(); ObjectIt _E = objset.end();
                for(;_F!=_E;_F++)
                	(*_F)->NumSetScale(V);
                Scene.UndoSave();
            }
        }
    }
}
//---------------------------------------------------------------------------

