#include "stdafx.h"
#pragma hdrstop

#include "FrameDetObj.h"
#include "ui_main.h"
#include "ChoseForm.h"
#include "DetailObjects.h"
#include "Scene.h"
#include "DOShuffle.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "multi_color"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraDetailObject::TfraDetailObject(TComponent* Owner, EDetailManager* dm_tools)
        : TForm(Owner)
{
	DM = dm_tools; VERIFY(DM);
    DEFINE_INI(fsStorage);
}
//---------------------------------------------------------------------------
void __fastcall TfraDetailObject::PaneMinClick(TObject *Sender)
{
    PanelMinMaxClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ExpandClick(TObject *Sender)
{
    PanelMaximizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ExtBtn1Click(TObject *Sender)
{
	LPCSTR N;
    if (TfrmChoseItem::SelectItem(TfrmChoseItem::smTexture,N,1,(DM->m_Base.Valid())?DM->m_Base.GetName():0)){
		DM->Initialize(N);
    	Scene.UndoSave();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ExtBtn5Click(TObject *Sender)
{
	if (TfrmDOShuffle::Run())
    	Scene.UndoSave();
}
//---------------------------------------------------------------------------


void __fastcall TfraDetailObject::ebUpdateObjectsClick(TObject *Sender)
{
	if (DM->UpdateObjects(true,false))
		Scene.UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ebInvalidateCacheClick(TObject *Sender)
{
	DM->InvalidateCache();
	Scene.UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ebResetSlotsClick(TObject *Sender)
{
	if (ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure to reset slots?")==mrYes){
    	DM->ClearSlots();
		Scene.UndoSave();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ebClearDetailsClick(TObject *Sender)
{
	if (ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure to clear details?")==mrYes){
		DM->Clear();
		Scene.UndoSave();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ebReinitializeSlotObjectsClick(
      TObject *Sender)
{
	if (DM->UpdateObjects(false,true))
		Scene.UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ExtBtn6Click(TObject *Sender)
{
	if (DM->Reinitialize())
		Scene.UndoSave();
}
//---------------------------------------------------------------------------

