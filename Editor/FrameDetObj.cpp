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

void __fastcall TfraDetailObject::ExtBtn1Click(TObject *Sender)
{
	LPCSTR T = TfrmChoseItem::SelectTexture(false,(Scene->m_DetailObjects->m_pBaseTexture)?Scene->m_DetailObjects->m_pBaseTexture->name():0);
    if (T){
		Scene->m_DetailObjects->GenerateSlots(T);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraDetailObject::ExtBtn4Click(TObject *Sender)
{
    // !!!!!!!!!!!!!!!!!!!!!
	Scene->m_DetailObjects->AppendObject(0xFF0000,"flower_yellow");
	Scene->m_DetailObjects->AppendObject(0x00FF00,"grass");
	Scene->m_DetailObjects->AppendObject(0x0000FF,"flower_podor");
	Scene->m_DetailObjects->AppendObject(0xFF00FF,"flower_violet");
    // !!!!!!!!!!!!!!!!!!!!!
}
//---------------------------------------------------------------------------

