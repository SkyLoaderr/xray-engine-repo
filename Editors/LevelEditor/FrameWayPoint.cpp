#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameWayPoint.h"
#include "Scene.h"
#include "WayPoint.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraWayPoint::TfraWayPoint(TComponent* Owner)
        : TFrame(Owner)
{
    DEFINE_INI(fsStorage);
}
//---------------------------------------------------------------------------
void __fastcall TfraWayPoint::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraWayPoint::ExpandClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraWayPoint::ebAdd1LinksClick(TObject *Sender)
{
	if (!ebPointMode->Down){
    	ELog.DlgMsg(mtInformation,"Before editing enter Point Mode.");
    	return;
    }
	ObjectList lst;
    int cnt = Scene.GetQueryObjects(lst, OBJCLASS_WAY, 1, 1, 0);
    for (ObjectIt it=lst.begin(); it!=lst.end(); it++)
    	((CWayObject*)(*it))->Add1Link();
	if (cnt) Scene.UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TfraWayPoint::ebAdd2LinkClick(TObject *Sender)
{
	if (!ebPointMode->Down){
    	ELog.DlgMsg(mtInformation,"Before editing enter Point Mode.");
    	return;
    }
	ObjectList lst;
    int cnt = Scene.GetQueryObjects(lst, OBJCLASS_WAY, 1, 1, 0);
    for (ObjectIt it=lst.begin(); it!=lst.end(); it++)
    	((CWayObject*)(*it))->Add2Link();
	if (cnt) Scene.UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TfraWayPoint::ebRemoveLinksClick(TObject *Sender)
{
	if (!ebPointMode->Down){
    	ELog.DlgMsg(mtInformation,"Before editing enter Point Mode.");
    	return;
    }
	ObjectList lst;
    int cnt = Scene.GetQueryObjects(lst, OBJCLASS_WAY, 1, 1, 0);
    for (ObjectIt it=lst.begin(); it!=lst.end(); it++)
    	((CWayObject*)(*it))->RemoveLink();
	if (cnt) Scene.UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TfraWayPoint::ebInvertLinkClick(TObject *Sender)
{
	if (!ebPointMode->Down){
    	ELog.DlgMsg(mtInformation,"Before editing enter Point Mode.");
    	return;
    }
	ObjectList lst;
    int cnt = Scene.GetQueryObjects(lst, OBJCLASS_WAY, 1, 1, 0);
    for (ObjectIt it=lst.begin(); it!=lst.end(); it++)
    	((CWayObject*)(*it))->InvertLink();
	if (cnt) Scene.UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TfraWayPoint::ebRemoveSelectedClick(TObject *Sender)
{
	if (!ebPointMode->Down){
    	ELog.DlgMsg(mtInformation,"Before editing enter Point Mode.");
    	return;
    }
	ObjectList lst;
    int cnt = Scene.GetQueryObjects(lst, OBJCLASS_WAY, 1, 1, 0);
    for (ObjectIt it=lst.begin(); it!=lst.end(); it++)
    	((CWayObject*)(*it))->RemoveSelectedPoints();
	if (cnt) Scene.UndoSave();
}
//---------------------------------------------------------------------------

