#include "stdafx.h"
#pragma hdrstop

#include "ESceneCustomMTools.h"
#include "ui_control.h"
#include "ui_tools.h"

void ESceneCustomMTools::CreateControls()
{
	CreateDefaultControls();
}

void ESceneCustomMTools::RemoveControls()
{
    // controls
	for (ControlsIt it=m_Controls.begin(); it!=m_Controls.end(); it++) 
    	xr_delete	(*it);
    m_Controls.clear();
    xr_delete		(pFrame);
}

void ESceneCustomMTools::CreateDefaultControls()
{
    for (int a=0; a<eaMaxActions; a++)
    	AddControl(xr_new<TUI_CustomControl>(estDefault,a,this));
}

void ESceneCustomMTools::AddControl(TUI_CustomControl* c)
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

TUI_CustomControl* ESceneCustomMTools::FindControl(int subtarget, int action)
{
	if (action==-1) return 0;
	for (ControlsIt it=m_Controls.begin(); it!=m_Controls.end(); it++)
    	if (((*it)->sub_target==subtarget)&&((*it)->action==action)) return *it;
    return 0;
}

void ESceneCustomMTools::UpdateControl()
{
    if (pCurControl) pCurControl->OnExit();
    pCurControl=FindControl(sub_target,action);
    if (pCurControl) pCurControl->OnEnter();
}

void ESceneCustomMTools::OnActivate  ()
{
	ResetSubTarget();
    if (pCurControl) 	pCurControl->OnEnter();
//    if (pFrame)			pFrame->OnActivate();
}
void ESceneCustomMTools::OnDeactivate()
{
    if (pCurControl) 	pCurControl->OnExit();
//    if (pFrame)			pFrame->OnDeactivate();
}
void ESceneCustomMTools::SetAction   (int act)
{
    action=act;
    UpdateControl();
}
void ESceneCustomMTools::SetSubTarget(int tgt)
{
	sub_target=tgt;
    UpdateControl();
}
void ESceneCustomMTools::ResetSubTarget()
{
	SetSubTarget(estDefault);
}

 