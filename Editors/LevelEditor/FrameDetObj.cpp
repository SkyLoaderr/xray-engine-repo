#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameDetObj.h"
#include "Scene.h"
#include "ChoseForm.h"
#include "Library.h"
#include "DetailObjects.h"
#include "EditObject.h"
#include "LeftBar.h"
#include "Texture.h"
#include "DOShuffle.h"
#include "BottomBar.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "multi_color"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraDetailObject::TfraDetailObject(TComponent* Owner)
        : TFrame(Owner)
{
    DEFINE_INI(fsStorage);
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
	LPCSTR N;
    if (TfrmChoseItem::SelectItem(TfrmChoseItem::smTexture,N,1,(Scene.m_DetailObjects->m_Base.Valid())?Scene.m_DetailObjects->m_Base.GetName():0)){
		Scene.m_DetailObjects->Initialize(N);
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
	if (Scene.m_DetailObjects->UpdateObjects(true,false))
		Scene.UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ebInvalidateCacheClick(TObject *Sender)
{
	Scene.m_DetailObjects->InvalidateCache();
	Scene.UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ebResetSlotsClick(TObject *Sender)
{
	if (ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure to reset slots?")==mrYes){
    	Scene.m_DetailObjects->ClearSlots();
		Scene.UndoSave();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ebClearDetailsClick(TObject *Sender)
{
	if (ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure to clear details?")==mrYes){
		Scene.m_DetailObjects->Clear();
		Scene.UndoSave();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ebReinitializeSlotObjectsClick(
      TObject *Sender)
{
	if (Scene.m_DetailObjects->UpdateObjects(false,true))
		Scene.UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ExtBtn6Click(TObject *Sender)
{
	if (Scene.m_DetailObjects->Reinitialize())
		Scene.UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::OnEnter()
{
	seDensity->Value = Scene.m_DetailObjects->m_fDensity;
	fsStorage->RestoreFormPlacement();
}

void __fastcall TfraDetailObject::OnExit()
{
	fsStorage->SaveFormPlacement();
}

void __fastcall TfraDetailObject::seDensityExit(TObject *Sender)
{
	Scene.m_DetailObjects->m_fDensity = seDensity->Value;
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::seDensityKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	Scene.m_DetailObjects->m_fDensity = seDensity->Value;
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::seDensityLWChange(TObject *Sender,
      int Val)
{
	Scene.m_DetailObjects->m_fDensity = seDensity->Value;
}
//---------------------------------------------------------------------------




