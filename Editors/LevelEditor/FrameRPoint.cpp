#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "ui_main.h"
#include "FrameRPoint.h"
#include "Scene.h"
#include "ChoseForm.h"
#include "SpawnPoint.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraSpawnPoint::TfraSpawnPoint(TComponent* Owner)
        : TFrame(Owner)
{
    DEFINE_INI(fsStorage);
    ebCurObj->Caption=NONE_CAPTION;
}
//---------------------------------------------------------------------------
void __fastcall TfraSpawnPoint::PaneMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawnPoint::ExpandClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

AnsiString TfraSpawnPoint::GetCurrentEntity(BOOL bForceSelect)
{
	if ((bForceSelect)||(ebCurObj->Caption==NONE_CAPTION)) ebCurObjClick(0);
    return (ebCurObj->Caption!=NONE_CAPTION)?ebCurObj->Caption:AnsiString("");
}
void __fastcall TfraSpawnPoint::ebCurObjClick(TObject *Sender)
{
	LPCSTR N;
    if (TfrmChoseItem::SelectItem(TfrmChoseItem::smEntity,N,1,(ebCurObj->Caption!=NONE_CAPTION)?ebCurObj->Caption.c_str():0)) ebCurObj->Caption = N;
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawnPoint::ebAttachObjectClick(TObject *Sender)
{
	if (ebAttachObject->Down) UI.Command(COMMAND_CHANGE_ACTION, eaAdd);
}
//---------------------------------------------------------------------------

void __fastcall TfraSpawnPoint::evDetachObjectClick(TObject *Sender)
{
    ObjectList lst;
    if (Scene.GetQueryObjects(lst,OBJCLASS_SPAWNPOINT,1,1,0)){
    	for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
	        CSpawnPoint* O = dynamic_cast<CSpawnPoint*>(*it); R_ASSERT(O);
        	O->DetachObject();
        }
    }
}
//---------------------------------------------------------------------------

