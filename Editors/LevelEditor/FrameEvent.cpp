#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameEvent.h"
#include "Event.h"
#include "Scene.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "mxPlacemnt"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraEvent::TfraEvent(TComponent* Owner)
        : TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfraEvent::PaneMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraEvent::ExpandClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraEvent::ebAppendFormClick(TObject *Sender)
{
    ObjectList lst;
	if (Scene.GetQueryObjects(lst,OBJCLASS_EVENT,1,1,0)!=1)
    	ELog.DlgMsg(mtInformation,"Select one Event.");
    else{
    	CEvent* E=(CEvent*)lst.back();
        E->AppendForm(CEvent::efBox);
    }
    UI.RedrawScene();
}
//---------------------------------------------------------------------------


void __fastcall TfraEvent::ebRemoveFormClick(TObject *Sender)
{
    ObjectList lst;
	if (Scene.GetQueryObjects(lst,OBJCLASS_EVENT,1,1,0)!=1)
    	ELog.DlgMsg(mtInformation,"Select one Event.");
    else{
    	CEvent* E=(CEvent*)lst.back();
        E->RemoveSelectedForm();
    }
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

