//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditorPref.h"
#include "ui_main.h"
#include "cursor3d.h"
#include "texture.h"
#include "xr_func.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma link "RXCtrls"
#pragma link "RXSpin"
#pragma link "CloseBtn"
#pragma link "Placemnt"
#pragma link "RxMenus"
#pragma link "multi_color"
#pragma link "multi_edit"
#pragma resource "*.dfm"
TfrmEditorPreferences *frmEditorPreferences;
//---------------------------------------------------------------------------
__fastcall TfrmEditorPreferences::TfrmEditorPreferences(TComponent* Owner)
    : TForm(Owner)
{
	m_LMTexture = 0;
    bActive = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditorPreferences::FormCreate(TObject *Sender)
{
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsEditorPref->IniFileName = buf;
    fsEditorPref->RestoreFormPlacement();
}
//---------------------------------------------------------------------------
static TStaticText* temp_text=0;
void __fastcall TfrmEditorPreferences::stLMTexturesClick(TObject *Sender)
{
	if (temp_text) temp_text->Caption=((TMenuItem*)Sender)->Caption;
    temp_text = 0;
    // reload image
    if (bActive){
        _DELETE(m_LMTexture);
        char Name[64]; strcpy(Name,stLMTextures->Caption.c_str());
        if (strext(Name)) *strext(Name)=0;
        m_LMTexture = new ETextureCore(Name);
        pbImagePaint(0); paImage->Repaint();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditorPreferences::stLMTexturesMouseDown(
      TObject *Sender, TMouseButton Button, TShiftState Shift, int X,
      int Y)
{
    POINT pt;
    GetCursorPos(&pt);
    temp_text = (TStaticText*)Sender;
	pmLMTextures->TrackButton = tbLeftButton;
	pmLMTextures->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------
static TTabSheet* TS=0;
int __fastcall TfrmEditorPreferences::Run(TTabSheet* ts){
    bActive = true;
    // fill LM tex List
    AnsiString nm = "$LM*.*";
    FS.m_Textures.Update(nm);
    pmLMTextures->Items->Clear();
    LPCSTR T=0;
	if (T=FS.FindFirst(nm.c_str())){
    	do{ 
            TMenuItem* mi = new TMenuItem(0);
            mi->Caption = T;
            mi->OnClick = stLMTexturesClick;
            pmLMTextures->Items->Add(mi);
        }while(T=FS.FindNext());
	}

    // activate
    TS = ts;
	int res=ShowModal();
    TS = 0;

    bActive = false;

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

    fsEditorPrefRestorePlacement(0);
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditorPreferences::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditorPreferences::fsEditorPrefRestorePlacement(
      TObject *Sender)
{
	UI->m_AngleSnap = deg2rad(seSnapAngle->Value);
	UI->m_MoveSnap  = seSnapMove->Value;

	UI->Device.m_Camera.SetStyle(ECameraStyle(rgCameraStyle->ItemIndex));
    UI->Device.m_Camera.SetViewport(seZNear->Value, seZFar->Value, seFOV->Value);

    UI->m_MouseSM= 0.2*(float(seSM->Value)/100.f)*(float(seSM->Value)/100.f);
    UI->m_MouseSR= 0.02f*(float(seSR->Value)/100.f)*(float(seSR->Value)/100.f);
    UI->m_MouseSS= 0.02f*(float(seSS->Value)/100.f)*(float(seSS->Value)/100.f);

    UI->Device.m_Camera.SetSensitivity(float(seCameraSM->Value)/100.f, float(seCameraSR->Value)/100.f);
	UI->Device.m_Camera.SetFlyParams(seCameraFlySpeed->Value,seCameraFlyAltitude->Value);

    UI->m_Cursor->SetBrushSegment(seBrushSegment->Value);
    UI->m_Cursor->SetBrushRadius(seBrushSize->Value);
    UI->m_Cursor->SetBrushDepth(seBrushUpDepth->Value,seBrushDnDepth->Value);
    Fcolor c; c.set_windows(mc3DCursorColor->Brush->Color); UI->m_Cursor->SetColor(c);

    // reload image
    if (bActive){
        _DELETE(m_LMTexture);
        if (!stLMTextures->Caption.IsEmpty()){
            char Name[64]; strcpy(Name,stLMTextures->Caption.c_str());
            if (strext(Name)) *strext(Name)=0;
            m_LMTexture = new ETextureCore(Name);
            pbImagePaint(0); paImage->Repaint();
        }
    }

	if (TS) pcObjects->ActivePage=TS;
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditorPreferences::mcColorDialogClick(
      TObject *Sender, TMouseButton Button, TShiftState Shift, int X,
      int Y)
{
	DWORD color = ((TMultiObjColor*)Sender)->Brush->Color;
	if (SelectColorWin(&color,&color)) ((TMultiObjColor*)Sender)->_Set(color);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditorPreferences::seNavNetConectDistChange(
      TObject *Sender)
{
	UI->UpdateScene();
}
//---------------------------------------------------------------------------


void __fastcall TfrmEditorPreferences::FormDestroy(TObject *Sender)
{
    _DELETE(m_LMTexture);
}
//---------------------------------------------------------------------------

void __fastcall TfrmEditorPreferences::pbImagePaint(TObject *Sender)
{
    if (bActive&&m_LMTexture){
        RECT r; r.left = 1; r.top = 1;
        float w, h;
        w = m_LMTexture->width();
        h = m_LMTexture->height();
        if (w>h){   r.right = pbImage->Width; r.bottom = h/w*pbImage->Height;
        }else{      r.right = h/w*pbImage->Width; r.bottom = pbImage->Height;}
        m_LMTexture->StretchThumbnail(paImage->Handle, &r);
    }
}
//---------------------------------------------------------------------------



