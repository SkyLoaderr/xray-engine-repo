#include "stdafx.h"
#pragma hdrstop

#include "PropertiesPortal.h"
#include "SceneClassList.h"
#include "Portal.h"
#include "Sector.h"
#include "UI_Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma link "RXCtrls"
#pragma link "RXSpin"
#pragma link "CloseBtn"
#pragma link "ElHeader"
#pragma link "ElTree"
#pragma resource "*.dfm"
TfrmPropertiesPortal *frmPropertiesPortal=0;
//---------------------------------------------------------------------------
void frmPropertiesPortalRun(ObjectList* pObjects, bool& bChange){
	frmPropertiesPortal = new TfrmPropertiesPortal(0);
    frmPropertiesPortal->Run(pObjects,bChange);
    _DELETE(frmPropertiesPortal);
}

//---------------------------------------------------------------------------
__fastcall TfrmPropertiesPortal::TfrmPropertiesPortal(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------


void TfrmPropertiesPortal::GetObjectsInfo(){
}

bool TfrmPropertiesPortal::ApplyObjectsInfo(){
    return true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesPortal::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
}

//----------------------------------------------------
void __fastcall TfrmPropertiesPortal::FormShow(TObject *Sender)
{
    ebOk->Enabled       = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPortal::OnModified(TObject *Sender)
{
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPortal::ebOkClick(TObject *Sender)
{
    if (!ApplyObjectsInfo()) return;
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesPortal::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//----------------------------------------------------
int __fastcall TfrmPropertiesPortal::Run(ObjectList* pObjects, bool& bChange)
{
    m_Objects = pObjects;
    VERIFY(m_Objects);
    gbOrientation->Visible = (m_Objects->size()==1);
    m_CurPortal=(CPortal*)m_Objects->front();
    lbFrontSector->Caption = m_CurPortal->m_SectorFront->GetName();
    lbBackSector->Caption = m_CurPortal->m_SectorBack->GetName();
	GetObjectsInfo();
    int res = ShowModal();
    bChange = (res==mrOk);
    return res;
}


void __fastcall TfrmPropertiesPortal::ebInvertOrientationClick(TObject *Sender)
{
	m_CurPortal->InvertOrientation();
    m_CurPortal->Simplify();
    UI->RedrawScene();
	OnModified(Sender);
}
//---------------------------------------------------------------------------

