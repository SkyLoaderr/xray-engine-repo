#include "stdafx.h"
#pragma hdrstop

#include "ui_customtools.h"
#include "ui_tools.h"
#include "ui_control.h"
#include "scene.h"
#include "ui_main.h"

#include "NumericVector.h"

TUI_CustomTools::TUI_CustomTools(EObjClass cls, bool bCreateDefaultControls)
{
	ClassID 	= cls;
	scene_tools	= Scene.GetMTools(ClassID);
    sub_target	= 0;
    pCurControl = 0;
    pFrame		= 0;
    if (bCreateDefaultControls) CreateDefaultControls();
}

TUI_CustomTools::~TUI_CustomTools()
{
	for (ControlsIt it=m_Controls.begin(); it!=m_Controls.end(); it++) 
    	xr_delete(*it);
    m_Controls.clear();
}

void TUI_CustomTools::CreateDefaultControls()
{
    for (int a=0; a<eaMaxActions; a++)
    	AddControlCB(xr_new<TUI_CustomControl>(estDefault,a,this));
}

void TUI_CustomTools::AddControlCB(TUI_CustomControl* c)
{
    R_ASSERT(c);
	for (ControlsIt it=m_Controls.begin(); it!=m_Controls.end(); it++){
    	if (((*it)->sub_target==c->sub_target)&&((*it)->action==c->action)){ 
			xr_delete(*it);
            m_Controls.erase(it);
        	break;
        }
    }
    m_Controls.push_back(c);
}

TUI_CustomControl* TUI_CustomTools::FindControl(int subtarget, int action)
{
	if (action==-1) return 0;
	for (ControlsIt it=m_Controls.begin(); it!=m_Controls.end(); it++)
    	if (((*it)->sub_target==subtarget)&&((*it)->action==action)) return *it;
    return 0;
}

void TUI_CustomTools::UpdateControl()
{
    if (pCurControl) pCurControl->OnExit();
    pCurControl=FindControl(sub_target,action);
    if (pCurControl) pCurControl->OnEnter();
}

void TUI_CustomTools::OnActivate  ()
{
	ResetSubTarget();
    if (pCurControl) pCurControl->OnEnter();
}
void TUI_CustomTools::OnDeactivate()
{
    if (pCurControl) pCurControl->OnExit();
}
void TUI_CustomTools::SetAction   (int act)
{
    action=act;
    UpdateControl();
}
void TUI_CustomTools::SetSubTarget(int tgt)
{
	sub_target=tgt;
    UpdateControl();
}
void TUI_CustomTools::ResetSubTarget()
{
	SetSubTarget(estDefault);
}

void TUI_CustomTools::SetNumPosition(CCustomObject* O)
{
	ObjectList objset;
    if(O)objset.push_back(O);
    else Scene.GetQueryObjects(objset,ClassID);
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

void TUI_CustomTools::SetNumRotation(CCustomObject* O)
{
	ObjectList objset;
    if(O)objset.push_back(O);
    else Scene.GetQueryObjects(objset,ClassID);
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

void TUI_CustomTools::SetNumScale(CCustomObject* O)
{
	ObjectList objset;
    if(O)objset.push_back(O);
    else Scene.GetQueryObjects(objset,ClassID);
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

