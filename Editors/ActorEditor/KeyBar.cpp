#include "stdafx.h"
#pragma hdrstop

#include "KeyBar.h"
#include "UI_Tools.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElXPThemedControl"
#pragma link "ExtBtn"
#pragma link "Gradient"
#pragma link "ElTrackBar"
#pragma link "ElXPThemedControl"
#pragma link "MXCtrls"
#pragma link "multi_edit"
#pragma resource "*.dfm"
//---------------------------------------------------------------------------
__fastcall TfrmKeyBar::TfrmKeyBar(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
TfrmKeyBar* TfrmKeyBar::CreateKeyBar(TWinControl* parent)
{
	TfrmKeyBar* B = new TfrmKeyBar(parent);
    B->Parent= parent;
    B->Align = alBottom;
    B->Show	();
    return B;
}

void __fastcall TfrmKeyBar::seLODLWChange(TObject *Sender, int Val)
{
	Tools.m_RenderObject.m_fLOD = seLOD->Value;
}
//---------------------------------------------------------------------------

void __fastcall TfrmKeyBar::seLODKeyPress(TObject *Sender, char &Key)
{
	if (Key==VK_RETURN)	Tools.m_RenderObject.m_fLOD = seLOD->Value;
}
//---------------------------------------------------------------------------

void __fastcall TfrmKeyBar::seLODExit(TObject *Sender)
{
	Tools.m_RenderObject.m_fLOD = seLOD->Value;
}
//---------------------------------------------------------------------------

void TfrmKeyBar::UpdateBar()
{
	seLOD->Value 	= Tools.m_RenderObject.m_fLOD;
    float a,b,c;	Tools.GetStatTime(a,b,c);
    stStartTime->Caption   	= AnsiString().sprintf("%3.2f",a);
    stEndTime->Caption   	= AnsiString().sprintf("%3.2f",b);
    lbCurrentTime->Caption	= AnsiString().sprintf("%3.2f",c);
}
//---------------------------------------------------------------------------


