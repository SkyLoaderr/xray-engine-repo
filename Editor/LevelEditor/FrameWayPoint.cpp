#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameWayPoint.h"
#include "Scene.h"
#include "ui_main.h"
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
	ObjectList lst;
    int cnt = Scene.GetQueryObjects(lst, OBJCLASS_WAY, 1, 1, 0);
	if (cnt==1){
		ebRemoveLinksClick(Sender);
    	CWayObject* O=(CWayObject*)lst.back();
        WPVec objects;
        if (O->GetSelectedPoints(objects)){
            WPIt _A0=objects.begin();
            WPIt _A1=objects.end(); _A1--;
            WPIt _B1=objects.end();
            for (WPIt _A=_A0; _A!=_A1; _A++){
                CWayPoint* A = (CWayPoint*)(*_A);
                WPIt _B=_A; _B++;
                for (; _B!=_B1; _B++){
                    CWayPoint* B = (CWayPoint*)(*_B);
                    if (A->AddSingleLink(B)) cnt++;
                }
            }
            Scene.UndoSave();
        }
    }else{
    	ELog.DlgMsg(mtInformation,"Select one way.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraWayPoint::ebAdd2LinkClick(TObject *Sender)
{
	ObjectList lst;
    int cnt = Scene.GetQueryObjects(lst, OBJCLASS_WAY, 1, 1, 0);
	if (cnt==1){
		ebRemoveLinksClick(Sender);
    	CWayObject* O=(CWayObject*)lst.back();
        WPVec objects;
        if (O->GetSelectedPoints(objects)){
            WPIt _A0=objects.begin();
            WPIt _A1=objects.end(); _A1--;
            WPIt _B1=objects.end();
            for (WPIt _A=_A0; _A!=_A1; _A++){
                CWayPoint* A = (CWayPoint*)(*_A);
                WPIt _B=_A; _B++;
                for (; _B!=_B1; _B++){
                    CWayPoint* B = (CWayPoint*)(*_B);
                    if (A->AddDoubleLink(B)) cnt++;
                }
            }
            Scene.UndoSave();
        }
    }else{
    	ELog.DlgMsg(mtInformation,"Select one way.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraWayPoint::ebRemoveLinksClick(TObject *Sender)
{
	ObjectList lst;
    int cnt = Scene.GetQueryObjects(lst, OBJCLASS_WAY, 1, 1, 0);
	if (cnt==1){
    	CWayObject* O=(CWayObject*)lst.back();
        WPVec objects;
        if (O->GetSelectedPoints(objects)){
            WPIt _A0=objects.begin();
            WPIt _A1=objects.end(); _A1--;
            WPIt _B1=objects.end();
            for (WPIt _A=_A0; _A!=_A1; _A++){
                CWayPoint* A = (CWayPoint*)(*_A);
                WPIt _B=_A; _B++;
                for (; _B!=_B1; _B++){
                    CWayPoint* B = (CWayPoint*)(*_B);
                    if (A->RemoveLink(B)) cnt++;
                }
            }
            Scene.UndoSave();
        }
    }else{
    	ELog.DlgMsg(mtInformation,"Select one way.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraWayPoint::ebInvertLinkClick(TObject *Sender)
{
	ObjectList lst;
    int cnt = Scene.GetQueryObjects(lst, OBJCLASS_WAY, 1, 1, 0);
	if (cnt==1){
    	CWayObject* O=(CWayObject*)lst.back();
        WPVec objects;
        if (O->GetSelectedPoints(objects)){
            WPIt _A0=objects.begin();
            WPIt _A1=objects.end(); _A1--;
            WPIt _B1=objects.end();
            for (WPIt _A=_A0; _A!=_A1; _A++){
                CWayPoint* A = (CWayPoint*)(*_A);
                WPIt _B=_A; _B++;
                for (; _B!=_B1; _B++){
                    CWayPoint* B = (CWayPoint*)(*_B);
                    A->InvertLink(B);
                }
            }
            Scene.UndoSave();
        }
    }else{
    	ELog.DlgMsg(mtInformation,"Select one way.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraWayPoint::ebRemoveSelectedClick(TObject *Sender)
{
	ObjectList lst;
    int cnt = Scene.GetQueryObjects(lst, OBJCLASS_WAY, 1, 1, 0);
	if (cnt==1){
    	CWayObject* O=(CWayObject*)lst.back();
        O->RemoveSelectedPoints();
		Scene.UndoSave();
    }else{
    	ELog.DlgMsg(mtInformation,"Select one way.");
    }
}
//---------------------------------------------------------------------------

