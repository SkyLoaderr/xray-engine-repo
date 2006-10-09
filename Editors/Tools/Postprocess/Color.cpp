//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#pragma hdrstop
#include "Color.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "multi_edit"
#pragma link "MXCtrls"
#pragma resource "*.dfm"
TColorForm *ColorForm;
//---------------------------------------------------------------------------
__fastcall TColorForm::TColorForm(TComponent* Owner, CPostprocessAnimator *pAnimator, pp_params param)
    : TForm(Owner)
{
    m_Constructor = new TForm6 (this);
    m_Animator = pAnimator;
    m_Param = param;
    m_bInternal = false;
}
//---------------------------------------------------------------------------
void __fastcall TColorForm::FormDestroy(TObject *Sender)
{
    delete m_Constructor;
}
//---------------------------------------------------------------------------
void __fastcall TColorForm::Button1Click(TObject *Sender)
{
    UpdateConstructor ();
    
    if (m_Constructor->ShowModal(m_Animator) == mrCancel) return;

    m_Animator->ResetParam (m_Param);

    if (m_Constructor->m_Entries.size () == 1)
       {
       FillList ();
       return;
       }
    
    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";

    float fmul = m_Param == pp_base_color ? 0.5f : 1.0f;

    DWORD color;
    float r, g, b;
    float current = 0.0f;
    for (size_t a = 0; a < m_Constructor->m_Entries.size (); a++)
        {
        color = m_Constructor->m_Entries[a]->ColorPanel->Color;
        r = (float)(color & 0x0000ff) / 255.0f * fmul;
        b = (float)((color & 0xff0000) >> 16) / 255.0f * fmul;
        g = (float)((color & 0x00ff00) >> 8) / 255.0f * fmul;
        float time = m_Constructor->m_Entries[a]->WorkTime->Value;
        current += time;
        cparam->add_value (current, r, m_Constructor->m_Entries[a]->tr, m_Constructor->m_Entries[a]->cr, m_Constructor->m_Entries[a]->br, 0);
        cparam->add_value (current, g, m_Constructor->m_Entries[a]->tg, m_Constructor->m_Entries[a]->cg, m_Constructor->m_Entries[a]->bg, 1);
        cparam->add_value (current, b, m_Constructor->m_Entries[a]->tb, m_Constructor->m_Entries[a]->cb, m_Constructor->m_Entries[a]->bb, 2);
        }
    FillList ();
    dynamic_cast<TMainForm*> (Parent->Parent)->UpdateGraph ();
}
//---------------------------------------------------------------------------
void            TColorForm::FillList        ()
{
    PointList->Clear ();
    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";
    size_t count = cparam->get_keys_count();
    if (!count) return;
    for (size_t a = 0; a < count; a++)
        {
        char buf[256];
        sprintf (buf, "Time : %.3f", cparam->get_key_time(a));
        PointList->Items->Add (buf);
        }
    if (PointList->Items->Count)
       {
       dynamic_cast<TMainForm*> (Parent->Parent)->SetMarkerPosition (0.0f);
       PointList->ItemIndex = 0;
       }         
}
//---------------------------------------------------------------------------
void __fastcall TColorForm::PointListClick(TObject *Sender)
{
    if (PointList->ItemIndex == -1) return;
    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";
    m_bInternal = true;
    float time = cparam->get_key_time (PointList->ItemIndex);
    float v, t, c, b;
    cparam->get_value(time, v, t, c, b, 0);
    RedValue->Value = v;
    RedTension->Value = t;
    RedContin->Value = c;
    RedBias->Value = b;
    cparam->get_value(time, v, t, c, b, 1);
    GreenValue->Value = v;
    GreenTension->Value = t;
    GreenContin->Value = c;
    GreenBias->Value = b;
    cparam->get_value(time, v, t, c, b, 2);
    BlueValue->Value = v;
    BlueTension->Value = t;
    BlueContin->Value = c;
    BlueBias->Value = b;
    m_bInternal = false;
    UpdateColor ();
    TMainForm *form = dynamic_cast<TMainForm*> (Parent->Parent);
    form->SetMarkerPosition (time);
}
//---------------------------------------------------------------------------
void __fastcall TColorForm::CnahgeRedParam(TObject *Sender)
{
    if (m_bInternal == true) return;
    if (PointList->ItemIndex == -1) return;
    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";
    float time = cparam->get_key_time (PointList->ItemIndex);
    cparam->update_value (time, RedValue->Value, RedTension->Value, RedContin->Value, RedBias->Value, 0);
    m_Constructor->m_Entries[PointList->ItemIndex]->tr = RedTension->Value;
    m_Constructor->m_Entries[PointList->ItemIndex]->cr = RedContin->Value;
    m_Constructor->m_Entries[PointList->ItemIndex]->br = RedBias->Value;
    UpdateColor ();
    dynamic_cast<TMainForm*> (Parent->Parent)->UpdateGraph ();
}
//---------------------------------------------------------------------------
void __fastcall TColorForm::ChangeGreenParam(TObject *Sender)
{
    if (m_bInternal == true) return;
    if (PointList->ItemIndex == -1) return;
    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";
    float time = cparam->get_key_time (PointList->ItemIndex);
    cparam->update_value (time, GreenValue->Value, GreenTension->Value, GreenContin->Value, GreenBias->Value, 1);
    m_Constructor->m_Entries[PointList->ItemIndex]->tg = GreenTension->Value;
    m_Constructor->m_Entries[PointList->ItemIndex]->cg = GreenContin->Value;
    m_Constructor->m_Entries[PointList->ItemIndex]->bg = GreenBias->Value;
    UpdateColor ();
    dynamic_cast<TMainForm*> (Parent->Parent)->UpdateGraph ();
}
//---------------------------------------------------------------------------
void __fastcall TColorForm::ChangeBlueParam(TObject *Sender)
{
    if (m_bInternal == true) return;
    if (PointList->ItemIndex == -1) return;
    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";
    float time = cparam->get_key_time (PointList->ItemIndex);
    cparam->update_value (time, BlueValue->Value, BlueTension->Value, BlueContin->Value, BlueBias->Value, 2);
    m_Constructor->m_Entries[PointList->ItemIndex]->tb = BlueTension->Value;
    m_Constructor->m_Entries[PointList->ItemIndex]->cb = BlueContin->Value;
    m_Constructor->m_Entries[PointList->ItemIndex]->bb = BlueBias->Value;
    UpdateColor ();
    dynamic_cast<TMainForm*> (Parent->Parent)->UpdateGraph ();
}
//---------------------------------------------------------------------------
void    TColorForm::UpdateColor     ()
{
    if (PointList->ItemIndex == -1) return;
    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";
    DWORD color;
    float time = cparam->get_key_time (PointList->ItemIndex);
    float v, t, c, b;
    float fmul = m_Param == pp_base_color ? 2.0f : 1.0f;
    cparam->get_value(time, v, t, c, b, 0);
    color = (DWORD)(v * 255.0f * fmul);
    cparam->get_value(time, v, t, c, b, 1);
    color |= (DWORD)(v * 255.0f * fmul) << 8;
    cparam->get_value(time, v, t, c, b, 2);
    color |= (DWORD)(v * 255.0f * fmul) << 16;
    Color->Color = (TColor)color;                                             
}
//---------------------------------------------------------------------------
void __fastcall TColorForm::ColorClick(TObject *Sender)
{
    if (PointList->ItemIndex == -1) return;
    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";
    ColorDialog->Color = Color->Color;
    if (ColorDialog->Execute () == false) return;
    float fmul = m_Param == pp_base_color ? 0.5f : 1.0f;
    float r = (float)((ColorDialog->Color & 0x0000ff) >> 0) / 255.0f * fmul;
    float b = (float)((ColorDialog->Color & 0xff0000) >> 16) / 255.0f * fmul;
    float g = (float)((ColorDialog->Color & 0x00ff00) >> 8) / 255.0f * fmul;
    RedValue->Value = r;
    GreenValue->Value = g;
    BlueValue->Value = b;
}
//---------------------------------------------------------------------------
void    TColorForm::Clear           ()
{
    m_bInternal = true;
    PointList->Clear ();
    RedValue->Value = 0.0f;
    RedTension->Value = 0.0f;
    RedContin->Value = 0.0f;
    RedBias->Value = 0.0f;
    GreenValue->Value = 0.0f;
    GreenTension->Value = 0.0f;
    GreenContin->Value = 0.0f;
    GreenBias->Value = 0.0f;
    BlueValue->Value = 0.0f;
    BlueTension->Value = 0.0f;
    BlueContin->Value = 0.0f;
    BlueBias->Value = 0.0f;
    m_bInternal = false;
}
//---------------------------------------------------------------------------
void    TColorForm::UpdateAfterLoad ()
{
    FillList ();
    m_Constructor->Reset ();

    if (PointList->Items->Count == 0) return;
    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";
    float time, v, t, c, b, fmul = m_Param == pp_base_color ? 2.0f : 1.0f;

    for (size_t a = 0; a < cparam->get_keys_count() - 1; a++)
        m_Constructor->AddEntryTemplate (a);

    float prev = 0.0f;
    for (a = 0; a < cparam->get_keys_count(); a++)
        {
        TColor color = clBlack;
        TForm7 *form = m_Constructor->GetEntry (a);
        time = cparam->get_key_time(a);
        cparam->get_value(time, v, t, c, b, 0);
        form->tr = t;
        form->cr = c;
        form->br = b;
        color |= (TColor)(v * 255.0f * fmul);
        cparam->get_value(time, v, t, c, b, 1);
        form->tg = t;
        form->cg = c;
        form->bg = b;
        color |= (TColor)(v * 255.0f * fmul) << 8;
        cparam->get_value(time, v, t, c, b, 2);
        form->tg = t;
        form->cg = c;
        form->bg = b;
        color |= (TColor)(v * 255.0f * fmul) << 16;
        form->ColorPanel->Color = color;
        form->WorkTime->Value = time - prev;
        prev = time;
        }

/*
    float       tr, cr, br;
    float       tg, cg, bg;
    float       tb, cb, bb;
*/
}
void TColorForm::UpdateConstructor ()
{
    m_Constructor->Reset ();

    if (PointList->Items->Count == 0) return;
    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";
    float time, v, t, c, b, fmul = m_Param == pp_base_color ? 2.0f : 1.0f;

    for (size_t a = 0; a < cparam->get_keys_count() - 1; a++)
        m_Constructor->AddEntryTemplate (a);

    float prev = 0.0f;
    for (a = 0; a < cparam->get_keys_count(); a++)
        {
        TColor color = clBlack;
        TForm7 *form = m_Constructor->GetEntry (a);
        time = cparam->get_key_time(a);
        cparam->get_value(time, v, t, c, b, 0);
        form->tr = t;
        form->cr = c;
        form->br = b;
        color |= (TColor)(v * 255.0f * fmul);
        cparam->get_value(time, v, t, c, b, 1);
        form->tg = t;
        form->cg = c;
        form->bg = b;
        color |= (TColor)(v * 255.0f * fmul) << 8;
        cparam->get_value(time, v, t, c, b, 2);
        form->tg = t;
        form->cg = c;
        form->bg = b;
        color |= (TColor)(v * 255.0f * fmul) << 16;
        form->ColorPanel->Color = color;
        form->ColorDialog->Color = color;
        form->WorkTime->Value = time - prev;
        prev = time;
        }

}
