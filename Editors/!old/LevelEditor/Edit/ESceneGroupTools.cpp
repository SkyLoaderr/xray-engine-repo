#include "stdafx.h"
#pragma hdrstop

#include "ESceneGroupTools.h"
#include "ui_leveltools.h"
#include "FrameGroup.h"
#include "Scene.h"
#include "GroupObject.h"

void ESceneGroupTools::CreateControls()
{
	inherited::CreateDefaultControls(estDefault);
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

void ESceneGroupTools::CenterToGroup()
{
    ObjectList& lst 	= m_Objects;
    if (!lst.empty()){
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++)
			((CGroupObject*)(*it))->UpdateBBoxAndPivot(false);
        Scene->UndoSave();
    }
}
//----------------------------------------------------

void ESceneGroupTools::AlignToObject()
{
    ObjectList& lst 	= m_Objects;
    if (!lst.empty()){
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
			((CGroupObject*)(*it))->UpdateBBoxAndPivot(true);
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

