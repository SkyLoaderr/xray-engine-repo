#include "stdafx.h"
#pragma hdrstop

#include "ESceneGroupTools.h"
#include "ESceneGroupControls.h"
#include "ui_leveltools.h"
#include "FrameGroup.h"
#include "Scene.h"
#include "GroupObject.h"
#include "ChoseForm.h"

void ESceneGroupTools::CreateControls()
{
	inherited::CreateDefaultControls(estDefault);
    AddControl		(xr_new<TUI_ControlGroupAdd >(estDefault,etaAdd,		this));
	// frame
    pFrame 			= xr_new<TfraGroup>((TComponent*)0,this);
}
//----------------------------------------------------
 
void ESceneGroupTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

void ESceneGroupTools::UngroupObjects(bool bUndo)
{
    ObjectList lst 	= m_Objects;
    if (!lst.empty()){
        for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
        	if ((*it)->Selected()){
                Scene->RemoveObject(*it,false);
                ((CGroupObject*)(*it))->UngroupObjects();
                xr_delete(*it);
            }
        }
	    if (bUndo) Scene->UndoSave();
    }
}
//----------------------------------------------------

void ESceneGroupTools::GroupObjects(bool bUndo)
{
	string256 namebuffer;
	Scene->GenObjectName(OBJCLASS_GROUP, namebuffer);
    CGroupObject* group = xr_new<CGroupObject>((LPVOID)0,namebuffer);

	// validate objects
    ObjectList lst;
    if (Scene->GetQueryObjects(lst,OBJCLASS_DUMMY,1,1,0)) group->GroupObjects(lst);
    if (group->ObjectCount()){
	    ELog.DlgMsg(mtInformation,"Group '%s' successfully created.\nContain %d object(s)",group->Name,group->ObjectCount());
        Scene->AppendObject(group,bUndo);
    }else{
	    ELog.DlgMsg(mtError,"Group can't created.");
        xr_delete(group);
    }
}
//----------------------------------------------------

void ESceneGroupTools::OpenGroups(bool bUndo)
{
    ObjectList lst 	= m_Objects;
    if (!lst.empty()){
        for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
        	if ((*it)->Selected())
                ((CGroupObject*)(*it))->OpenGroup();
        }
	    if (bUndo) Scene->UndoSave();
    }
}
//----------------------------------------------------

void ESceneGroupTools::CloseGroups(bool bUndo)
{
    ObjectList lst 	= m_Objects;
    if (!lst.empty()){
        for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
        	if ((*it)->Selected())
                ((CGroupObject*)(*it))->CloseGroup();
        }
	    if (bUndo) Scene->UndoSave();
    }
}
//----------------------------------------------------

void ESceneGroupTools::CenterToGroup()
{
    ObjectList& lst 	= m_Objects;
    if (!lst.empty()){
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++)
			((CGroupObject*)(*it))->UpdatePivot(0,true);
        Scene->UndoSave();
    }
}
//----------------------------------------------------

void __stdcall  FillGroupItems(ChooseItemVec& items, void* param)
{
	CGroupObject* group = (CGroupObject*)param;
    for (ObjectIt it=group->GetObjects().begin(); it!=group->GetObjects().end(); it++){
	    items.push_back	(SChooseItem((*it)->Name,""));
    }
}

void ESceneGroupTools::AlignToObject()
{
    ObjectList& lst 	= m_Objects;
    if (!lst.empty()){
        LPCSTR nm;
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
        	if ((*it)->Selected()){
                if (TfrmChoseItem::SelectItem(smCustom,nm,1,nm,FillGroupItems,*it)){
                    ((CGroupObject*)(*it))->UpdatePivot(nm,false);
                }else break;
            }
        }
        Scene->UndoSave();
    }
}
//----------------------------------------------------

CCustomObject* ESceneGroupTools::CreateObject(LPVOID data, LPCSTR name)
{
	CCustomObject* O	= xr_new<CGroupObject>(data,name);
    O->ParentTools		= this;
    return O;
}
//----------------------------------------------------

void ESceneGroupTools::SaveSelectedObject()
{
	CGroupObject* obj	= 0;
	// find single selected object
    for (ObjectIt it=m_Objects.begin(); it!=m_Objects.end(); it++){
    	if ((*it)->Selected()){ 
        	if (obj!=0){
            	ELog.DlgMsg(mtError,"Select single object and try again.");
                return;
            }
        	obj 		= dynamic_cast<CGroupObject*>(*it);
        }
    }
    if (obj){
    	if (obj->IsOpened()){	
	        ELog.DlgMsg(mtError,"Can't save opened group. Close group and retry.");
        }else{
            xr_string fn;
            if (EFS.GetSaveName(_groups_,fn)){
                IWriter* W 	= FS.w_open(fn.c_str());
                obj->Save	(*W);
                FS.w_close	(W);
            }                                 	
        }
    }else{
        ELog.DlgMsg(mtError,"Nothing selected.");
    }
}
//----------------------------------------------------

void ESceneGroupTools::SetCurrentObject(LPCSTR nm)
{
	m_CurrentObject			= nm;
	TfraGroup* frame		=(TfraGroup*)pFrame;
    frame->lbCurrent->Caption = m_CurrentObject.c_str();
}
//----------------------------------------------------

void ESceneGroupTools::OnActivate()
{
	inherited::OnActivate	();
	TfraGroup* frame		=(TfraGroup*)pFrame;
    frame->lbCurrent->Caption = m_CurrentObject.c_str();
}
//----------------------------------------------------

