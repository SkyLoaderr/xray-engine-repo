#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameWayPoint.h"
#include "Scene.h"
#include "ui_main.h"
#include "AITraffic.h"
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
void __fastcall TfraWayPoint::PaneMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraWayPoint::ExpandClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraWayPoint::ebAddLinksClick(TObject *Sender)
{
	int cnt=0;
	ObjectList objset;
	if (Scene.GetQueryObjects(objset, OBJCLASS_AITPOINT, 1, 1, 0)){
    	ObjectIt _A0=objset.begin();
    	ObjectIt _A1=objset.end(); _A1--;
    	ObjectIt _B1=objset.end();
		for (ObjectIt _A=_A0; _A!=_A1; _A++){
        	CAITPoint* A = (CAITPoint*)(*_A);
            ObjectIt _B=_A; _B++;
            for (; _B!=_B1; _B++){
	        	CAITPoint* B = (CAITPoint*)(*_B);
        		if (A->AddLink(B)) cnt++;
            }
        }
    }
    if (cnt) Scene.UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TfraWayPoint::ebRemoveLinksClick(TObject *Sender)
{
	int cnt=0;
	ObjectList objset;
	if (Scene.GetQueryObjects(objset, OBJCLASS_AITPOINT, 1, 1, 0)){
    	ObjectIt _A0=objset.begin();
    	ObjectIt _A1=objset.end(); _A1--;
    	ObjectIt _B1=objset.end();
		for (ObjectIt _A=_A0; _A!=_A1; _A++){
        	CAITPoint* A = (CAITPoint*)(*_A);
            ObjectIt _B=_A; _B++;
            for (; _B!=_B1; _B++){
	        	CAITPoint* B = (CAITPoint*)(*_B);
        		if (A->RemoveLink(B)) cnt++;
            }
        }
    }
    if (cnt) Scene.UndoSave();
}
//---------------------------------------------------------------------------

