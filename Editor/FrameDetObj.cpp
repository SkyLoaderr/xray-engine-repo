#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameDetObj.h"
#include "Scene.h"
#include "ui_main.h"
#include "ChoseForm.h"
#include "Library.h"
#include "DetailObjects.h"
#include "EditObject.h"
#include "LeftBar.h"
#include "Texture.h"
#include "DOShuffle.h"
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
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
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

void __fastcall TfraDetailObject::ExtBtn1Click(TObject *Sender)
{
	LPCSTR T = TfrmChoseItem::SelectTexture(false,(Scene->m_DetailObjects->m_pBaseTexture)?Scene->m_DetailObjects->m_pBaseTexture->name():0);
    if (T){
		Scene->m_DetailObjects->Initialize(T);
    	Scene->UndoSave();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ExtBtn5Click(TObject *Sender)
{
	if (TfrmDOShuffle::Run())
    	Scene->UndoSave();
}
//---------------------------------------------------------------------------


void __fastcall TfraDetailObject::ebUpdateObjectsClick(TObject *Sender)
{
	if (Scene->m_DetailObjects->UpdateObjects(true,false))
		Scene->UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ebReinitializeSlotObjectsClick(
      TObject *Sender)
{
	if (Scene->m_DetailObjects->UpdateObjects(false,true))
		Scene->UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ExtBtn6Click(TObject *Sender)
{
	if (Scene->m_DetailObjects->Reinitialize())
		Scene->UndoSave();
}
//---------------------------------------------------------------------------

