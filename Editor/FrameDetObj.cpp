#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameDetObj.h"
#include "Light.h"
#include "Scene.h"
#include "ui_main.h"
#include "ChoseForm.h"
#include "Library.h"
#include "DetailObjects.h"
#include "EditObject.h"
#include "LeftBar.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma link "RXCtrls"
#pragma link "multi_edit"
#pragma link "Placemnt"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraDetailObject::TfraDetailObject(TComponent* Owner)
        : TFrame(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TfraDetailObject::PaneMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ExpandClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}

//---------------------------------------------------------------------------
void __fastcall TfraDetailObject::ebObjectNameClick(TObject *Sender){
	LPCSTR N = TfrmChoseItem::SelectObject(false, true, DETAILOBJECT_FOLDER, lbObjectName->Caption.c_str());
    if (!N) return;
	CLibObject* O = Lib->SearchObject(N);
    if (O)	lbObjectName->Caption = O->GetRefName();
	else	lbObjectName->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::fsStorageRestorePlacement(
      TObject *Sender)
{
	if (!lbObjectName->Caption.IsEmpty())
		if (!Lib->SearchObject(lbObjectName->Caption.c_str())) lbObjectName->Caption = "";
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ebModeClusterClick(TObject *Sender)
{
	if (UI->m_Tools->GetAction()==eaAdd){
		paObjectBrush->Hide();
		paClusterBrush->Show();
	    UI->Command(COMMAND_UPDATE_TOOLBAR);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ebModeObjectsClick(TObject *Sender)
{
	if (UI->m_Tools->GetAction()==eaAdd){
		paClusterBrush->Hide();
		paObjectBrush->Show();
	    UI->Command(COMMAND_UPDATE_TOOLBAR);
    }
}
//---------------------------------------------------------------------------

