#include "Pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "AddObject.h"

#include "Library.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma link "multiobj"
#pragma link "RXCtrls"
#pragma link "RXSpin"
#pragma resource "*.dfm"
TfraAddObject *fraAddObject;
//---------------------------------------------------------------------------
__fastcall TfraAddObject::TfraAddObject(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etObject,eaAdd,this);
// use brush
    cbUseBrush->Checked = false;
    paBrush->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TfraAddObject::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraAddObject::sbSelectClick(TObject *Sender)
{
	SceneObject *t = 0;
    t = UI.SelectSObject();
    if( t ) Lib.oset( t );
    lbSelectObjectName->Caption = Lib.o()?Lib.o()->GetName():"no object";
}
//---------------------------------------------------------------------------

void __fastcall TfraAddObject::Label2Click(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraAddObject::cbUseBrushClick(TObject *Sender)
{
    paBrush->Visible = cbUseBrush->Checked;
}
//---------------------------------------------------------------------------

