//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;


//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
    : TForm(Owner)
{
    Image->Picture->Bitmap->Width = Image->Width;
    Image->Picture->Bitmap->Height = Image->Height;

    m_Animator = new CPostprocessAnimator (0, false);
    m_ShowForm = NULL;

    m_BaseColor = new TColorForm (this, m_Animator, pp_base_color);
    m_BaseColor->Parent = TabControl;
    m_BaseColor->Visible = true;

    m_AddColor = new TColorForm (this, m_Animator, pp_add_color);
    m_AddColor->Parent = TabControl;

    m_GrayColor = new TColorForm (this, m_Animator, pp_gray_color);
    m_GrayColor->Parent = TabControl;

    m_Blur = new TSingleParam (this, m_Animator, pp_blur);
    m_Blur->Parent = TabControl;

    m_Gray = new TSingleParam (this, m_Animator, pp_gray_value);
    m_Gray->Parent = TabControl;

    m_DualH = new TSingleParam (this, m_Animator, pp_dual_h);
    m_DualH->Parent = TabControl;

    m_DualV = new TSingleParam (this, m_Animator, pp_dual_v);
    m_DualV->Parent = TabControl;

    m_NoiseI = new TSingleParam (this, m_Animator, pp_noise_i);
    m_NoiseI->Parent = TabControl;

    m_NoiseG = new TSingleParam (this, m_Animator, pp_noise_g);
    m_NoiseG->Parent = TabControl;

    m_NoiseF = new TSingleParam (this, m_Animator, pp_noise_f);
    m_NoiseF->Parent = TabControl;

    m_Marker = 0.0f;

    TabControl->TabIndex = 0;
    TabControlChange (TabControl);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormDestroy(TObject *Sender)
{
    delete m_Animator;
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormResize(TObject *Sender)
{
    Image->Picture->Bitmap->Width = 0;
    Image->Picture->Bitmap->Height = 0;
    Image->Picture->Bitmap->Width = Image->Width;
    Image->Picture->Bitmap->Height = Image->Height;
    UpdateGraph ();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::NewEffectButtonClick(TObject *Sender)
{
    if (Application->MessageBox ("Do you wish to create a new effect ?", "Warning", MB_YESNO | MB_ICONSTOP) == IDNO)
       return;
       
    m_Animator->Create ();   
    m_BaseColor->Clear ();
    m_AddColor->Clear ();
    m_GrayColor->Clear ();
    m_Blur->Clear ();
    m_Gray->Clear ();
    m_DualH->Clear ();
    m_DualV->Clear ();
    m_NoiseI->Clear ();
    m_NoiseG->Clear ();
    m_NoiseF->Clear ();
    UpdateGraph ();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::FormPaint(TObject *Sender)
{
    UpdateGraph ();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::ImageMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
//    m_pEffect->add_point (X, Y);
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::GrayColorClick(TObject *Sender)
{
//    m_pEffect->get_add_color()->show_constructor ();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::TabControlChange(TObject *Sender)
{
    if (m_ShowForm)
       {
       m_ShowForm->Visible = false;
       m_ShowForm = NULL;
       }
    TForm *forms[10] = {m_BaseColor, m_AddColor, m_GrayColor, m_Blur, m_Gray, m_DualH, m_DualV, m_NoiseI, m_NoiseG, m_NoiseF};
    m_ShowForm = forms[TabControl->TabIndex];
    TRect rc = TabControl->DisplayRect;
    m_ShowForm->Left = rc.left;
    m_ShowForm->Top = rc.top;
    m_ShowForm->Width = rc.right - rc.left;
    m_ShowForm->Height = rc.Bottom - rc.top;
    m_ShowForm->Visible = true;
    UpdateGraph ();
}
//---------------------------------------------------------------------------
void            TMainForm::SetMarkerPosition   (float time)
{
    m_Marker = time;
    UpdateGraph ();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::LoadButtonClick(TObject *Sender)
{
    if (m_Animator->GetLength() != 0.0f)
       if (Application->MessageBox ("Do you wish to save current effect ?", "Warning", MB_YESNO | MB_ICONSTOP) == IDYES)
          SaveButtonClick (Sender);
    if (OpenDialog->Execute () == false) return;
    m_Marker = 0.0f;
    m_Animator->Load (OpenDialog->FileName.c_str ());
    m_BaseColor->UpdateAfterLoad();
    m_AddColor->UpdateAfterLoad();
    m_GrayColor->UpdateAfterLoad();
    m_Blur->UpdateAfterLoad();
    m_Gray->UpdateAfterLoad();
    m_DualH->UpdateAfterLoad();
    m_DualV->UpdateAfterLoad();
    m_NoiseI->UpdateAfterLoad();
    m_NoiseG->UpdateAfterLoad();
    m_NoiseF->UpdateAfterLoad();
    TabControl->TabIndex = 0;
    UpdateGraph ();
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::SaveButtonClick(TObject *Sender)
{
    if (m_Animator->GetLength() == 0.0f) return;
    if (SaveDialog->Execute () == false) return;
    m_Animator->Save (SaveDialog->FileName.c_str ());
}
//---------------------------------------------------------------------------
void            TMainForm::UpdateGraph         ()
{
    TCanvas *canvas = Image->Picture->Bitmap->Canvas;
    canvas->Brush->Color = clWhite;
    canvas->FillRect (TRect (0, 0, Image->Picture->Bitmap->Width, Image->Picture->Bitmap->Height));
    canvas->Pen->Color = clBlack;
    canvas->MoveTo (0, Image->Picture->Bitmap->Height / 2);
    canvas->LineTo (Image->Picture->Bitmap->Width, Image->Picture->Bitmap->Height / 2);
    canvas->MoveTo (0, 0);
    canvas->LineTo (Image->Picture->Bitmap->Width, 0);
    canvas->MoveTo (0, Image->Picture->Bitmap->Height - 1);
    canvas->LineTo (Image->Picture->Bitmap->Width, Image->Picture->Bitmap->Height - 1);
    canvas->TextOutA (0, Image->Picture->Bitmap->Height / 2 - 14, "0.0");
    canvas->TextOutA (0, Image->Picture->Bitmap->Height - 14, "-1.0");
    canvas->TextOutA (0, 2, "1.0");


    float alltime = m_Animator->GetLength ();
    char buf[64];
    sprintf (buf, "Effect time : %.3f", alltime);
    StatusBar->Panels->Items[0]->Text = buf;
    if (alltime == 0.0f) return;
    float width = (float)Image->Picture->Bitmap->Width, height = (float)Image->Picture->Bitmap->Height * 0.5f;
    //draw marker
    int left = (int)(width / alltime * m_Marker);
    canvas->MoveTo (left, 0);
    canvas->Pen->Style = psDot;
    canvas->LineTo (left, Image->Picture->Bitmap->Height);
    canvas->Pen->Style = psSolid;

    SPPInfo	m_EffectorParams;
    ZeroMemory (&m_EffectorParams, sizeof (SPPInfo));

    float increment = alltime / (float)Image->Width;

    for (float t = 0.0f; t < alltime; t += increment)
        {
        m_Animator->Process (t, m_EffectorParams);
        int x = (int)(width / alltime * t);
        switch (TabControl->TabIndex)
               {
               case 0:
                    canvas->Pixels[x][(int)(-m_EffectorParams.color_base.r * height + height)] = clRed;
                    canvas->Pixels[x][(int)(-m_EffectorParams.color_base.g * height + height)] = clGreen;
                    canvas->Pixels[x][(int)(-m_EffectorParams.color_base.b * height + height)] = clBlue;
                    break;
               case 1:
                    canvas->Pixels[x][(int)(-m_EffectorParams.color_add.r * height + height)] = clRed;
                    canvas->Pixels[x][(int)(-m_EffectorParams.color_add.g * height + height)] = clGreen;
                    canvas->Pixels[x][(int)(-m_EffectorParams.color_add.b * height + height)] = clBlue;
                    break;
               case 2:
                    canvas->Pixels[x][(int)(-m_EffectorParams.color_gray.r * height + height)] = clRed;
                    canvas->Pixels[x][(int)(-m_EffectorParams.color_gray.g * height + height)] = clGreen;
                    canvas->Pixels[x][(int)(-m_EffectorParams.color_gray.b * height + height)] = clBlue;
                    break;
               case 3:
                    canvas->Pixels[x][(int)(-m_EffectorParams.blur * height + height)] = clBlack;
                    break;
               case 4:
                    canvas->Pixels[x][(int)(-m_EffectorParams.gray * height + height)] = clBlack;
                    break;
               case 5:
                    canvas->Pixels[x][(int)(-m_EffectorParams.duality.h * height + height)] = clBlack;
                    break;
               case 6:
                    canvas->Pixels[x][(int)(-m_EffectorParams.duality.v * height + height)] = clBlack;
                    break;
               case 7:
                    canvas->Pixels[x][(int)(-m_EffectorParams.noise.intensity * height + height)] = clBlack;
                    break;
               case 8:
                    canvas->Pixels[x][(int)(-m_EffectorParams.noise.grain * height + height)] = clBlack;
                    break;
               case 9:
                    canvas->Pixels[x][(int)(-m_EffectorParams.noise.fps * height + height)] = clBlack;
                    break;
               }
        }
}

