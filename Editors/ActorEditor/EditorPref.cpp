//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditorPref.h"
#include "texture.h"
#ifdef _LEVEL_EDITOR
	#include "cursor3d.h"	
	#include "ui_main.h"
#endif
#include "ColorPicker.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmEditorPreferences *frmEditorPreferences;
//---------------------------------------------------------------------------
__fastcall TfrmEditorPreferences::TfrmEditorPreferences(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditorPreferences::FormCreate(TObject *Sender)
{
	DEFINE_INI(fsStorage);
    fsStorage->RestoreFormPlacement();
}
//---------------------------------------------------------------------------
static TStaticText* temp_text=0;
void __fastcall TfrmEditorPreferences::stLMTexturesClick(TObject *Sender)
{
	if (temp_text) temp_text->Caption=((TMenuItem*)Sender)->Caption;
    temp_text = 0;
}
//---------------------------------------------------------------------------

static TTabSheet* TS=0;
int __fastcall TfrmEditorPreferences::Run(TTabSheet* ts){
    // activate
    TS = ts;
	int res=ShowModal();
    TS = 0;
    return res;
}

void __fastcall TfrmEditorPreferences::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
    if (Key==VK_RETURN) ebOk->Click();
}

//----------------------------------------------------

void __fastcall TfrmEditorPreferences::ebOkClick(TObject *Sender)
{
    Close();
    ModalResult = mrOk;
    fsStorageRestorePlacement(0);
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditorPreferences::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditorPreferences::fsStorageRestorePlacement(
      TObject *Sender)
{
	UI.m_AngleSnap = deg2rad(seSnapAngle->Value);
	UI.m_MoveSnap  = seSnapMove->Value;

    Device.m_Camera.SetViewport(seZNear->Value, seZFar->Value, seFOV->Value);

    UI.m_MouseSM= 0.2*(float(seSM->Value)/100.f)*(float(seSM->Value)/100.f);
    UI.m_MouseSR= 0.02f*(float(seSR->Value)/100.f)*(float(seSR->Value)/100.f);
    UI.m_MouseSS= 0.02f*(float(seSS->Value)/100.f)*(float(seSS->Value)/100.f);

    Device.m_Camera.SetSensitivity(float(seCameraSM->Value)/100.f, float(seCameraSR->Value)/100.f);
	Device.m_Camera.SetFlyParams(seCameraFlySpeed->Value,seCameraFlyAltitude->Value);

#ifdef _LEVEL_EDITOR
    UI.m_Cursor->SetBrushSegment(seBrushSegment->Value);
    UI.m_Cursor->SetBrushRadius(seBrushSize->Value);
    UI.m_Cursor->SetBrushDepth(seBrushUpDepth->Value,seBrushDnDepth->Value);
    Fcolor c; c.set_windows(mc3DCursorColor->Brush->Color); UI.m_Cursor->SetColor(c);
#endif

    UI.Command(COMMAND_UPDATE_GRID);

	if (TS) pcObjects->ActivePage=TS;
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditorPreferences::mcColorDialogClick(
      TObject *Sender, TMouseButton Button, TShiftState Shift, int X,
      int Y)
{
	DWORD color = ((TMultiObjColor*)Sender)->Brush->Color;
	if (SelectColorWin(&color)) ((TMultiObjColor*)Sender)->_Set(color);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditorPreferences::seNavNetConectDistChange(
      TObject *Sender)
{
	UI.UpdateScene();
}
//---------------------------------------------------------------------------




