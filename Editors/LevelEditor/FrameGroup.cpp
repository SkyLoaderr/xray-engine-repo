#include "stdafx.h"
#pragma hdrstop

#include "ui_main.h"
#include "FrameGroup.h"
#include "GroupObject.h"
#include "Scene.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraGroup::TfraGroup(TComponent* Owner)
        : TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfraGroup::PaneMinClick(TObject *Sender)
{
    PanelMinMaxClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ExpandClick(TObject *Sender)
{
    PanelMaximizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebMakeGroupClick(TObject *Sender)
{
	string256 namebuffer;
	Scene.GenObjectName(OBJCLASS_GROUP, namebuffer);
    CGroupObject* group = xr_new<CGroupObject>((LPVOID)0,namebuffer);

	// validate objects
    ObjectList lst;
    if (Scene.GetQueryObjects(lst,OBJCLASS_DUMMY,1,1,0)) group->GroupObjects(lst);
    if (group->ObjectCount()){
	    ELog.DlgMsg(mtInformation,"Group '%s' successfully created.\nContain %d object(s)",group->Name,group->ObjectCount());
        Scene.AddObject(group,true);
    }else{
	    ELog.DlgMsg(mtError,"Group can't created.");
        xr_delete(group);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebUngroupClick(TObject *Sender)
{
	ObjectList lst;
	if (Scene.GetQueryObjects(lst,OBJCLASS_GROUP,1,1,0)){
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
            Scene.RemoveObject(*it,false);
			((CGroupObject*)(*it))->UngroupObjects();
        	xr_delete(*it);
        }
        Scene.UndoSave();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebCenterToGroupClick(TObject *Sender)
{
	ObjectList lst;
	if (Scene.GetQueryObjects(lst,OBJCLASS_GROUP,1,1,0)){
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
			((CGroupObject*)(*it))->UpdateBBoxAndPivot(false);
        }
        Scene.UndoSave();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraGroup::ebAlignToObjectClick(TObject *Sender)
{
	ObjectList lst;
	if (Scene.GetQueryObjects(lst,OBJCLASS_GROUP,1,1,0)){
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
			((CGroupObject*)(*it))->UpdateBBoxAndPivot(true);
        }
        Scene.UndoSave();
    }
}
//---------------------------------------------------------------------------

