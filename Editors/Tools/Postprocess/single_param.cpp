//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "single_param.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXCtrls"
#pragma link "RXSpin"
#pragma resource "*.dfm"
TSingleParam *SingleParam;
//---------------------------------------------------------------------------
__fastcall TSingleParam::TSingleParam(TComponent* Owner, CPostprocessAnimator *pAnimator, pp_params param)
    : TForm(Owner)
{
    m_Constructor = new TForm8 (this);
    m_Animator = pAnimator;
    m_Param = param;
    m_bInternal = false;
}
//---------------------------------------------------------------------------
void __fastcall TSingleParam::FormDestroy(TObject *Sender)
{
    delete m_Constructor;
}
//---------------------------------------------------------------------------
void __fastcall TSingleParam::Button1Click(TObject *Sender)
{
    if (m_Constructor->ShowModal() == mrCancel) return;

    m_Animator->ResetParam (m_Param);
    
    if (m_Constructor->m_Entries.size () == 1)
       {
       FillList ();
       return;
       }


    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";
    float v;// = m_Constructor->InitValue->Value;
    //cparam->add_value (0.0f, v, 0.0f, 0.0f, 0.0f);
    float current = 0.0f;
    for (size_t a = 0; a < m_Constructor->m_Entries.size (); a++)
        {
        v = m_Constructor->m_Entries[a]->Value->Value;
        float time = m_Constructor->m_Entries[a]->WorkTime->Value;
        current += time;
        cparam->add_value (current, v, m_Constructor->m_Entries[a]->t, m_Constructor->m_Entries[a]->c, m_Constructor->m_Entries[a]->b);
        }
    FillList ();
    dynamic_cast<TMainForm*> (Parent->Parent)->UpdateGraph ();
}
//---------------------------------------------------------------------------
void            TSingleParam::FillList        ()
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
void __fastcall TSingleParam::PointListClick(TObject *Sender)
{
    if (PointList->ItemIndex == -1) return;
    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";
    m_bInternal = true;
    float time = cparam->get_key_time (PointList->ItemIndex);
    float v, t, c, b;
    cparam->get_value(time, v, t, c, b, 0);
    Value->Value = v;
    Tension->Value = t;
    Contin->Value = c;
    Bias->Value = b;
    m_bInternal = false;
    TMainForm *form = dynamic_cast<TMainForm*> (Parent->Parent);
    form->SetMarkerPosition (time);
}
//---------------------------------------------------------------------------
void __fastcall TSingleParam::ChangeParam(TObject *Sender)
{
    if (m_bInternal == true) return;
    if (PointList->ItemIndex == -1) return;
    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";
    float time = cparam->get_key_time (PointList->ItemIndex);
    cparam->update_value (time, Value->Value, Tension->Value, Contin->Value, Bias->Value);
    m_Constructor->m_Entries[PointList->ItemIndex]->t = Tension->Value;
    m_Constructor->m_Entries[PointList->ItemIndex]->c = Contin->Value;
    m_Constructor->m_Entries[PointList->ItemIndex]->b = Bias->Value;
    dynamic_cast<TMainForm*> (Parent->Parent)->UpdateGraph ();
}
//---------------------------------------------------------------------------
void    TSingleParam::UpdateAfterLoad ()
{
    FillList ();
    m_Constructor->Reset ();

    if (PointList->Items->Count == 0) return;
    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";
    float time, v, t, c, b;

    for (size_t a = 0; a < cparam->get_keys_count() - 1; a++)
        m_Constructor->AddEntryTemplate (a);

    float prev = 0.0f;
    for (a = 0; a < cparam->get_keys_count(); a++)
        {
        TForm9 *form = m_Constructor->GetEntry (a);
        time = cparam->get_key_time(a);
        cparam->get_value(time, v, t, c, b, 0);
        form->t = t;
        form->c = c;
        form->b = b;
        form->Value->Value = v;
        form->WorkTime->Value = time - prev;
        prev = time;
        }
}
//---------------------------------------------------------------------------
void    TSingleParam::Clear           ()
{
    m_bInternal = true;
    PointList->Clear ();
    Value->Value = 0.0f;
    Tension->Value = 0.0f;
    Contin->Value = 0.0f;
    Bias->Value = 0.0f;
    m_bInternal = false;
}
//---------------------------------------------------------------------------
void    TSingleParam::UpdateConstructor ()
{
    m_Constructor->Reset ();

    if (PointList->Items->Count == 0) return;
    CPostProcessParam* cparam = m_Animator->GetParam (m_Param);
    if (!cparam) throw "Error get parameter from animator";
    float time, v, t, c, b;

    for (size_t a = 0; a < cparam->get_keys_count() - 1; a++)
        m_Constructor->AddEntryTemplate (a);

    float prev = 0.0f;
    for (a = 0; a < cparam->get_keys_count(); a++)
        {
        TForm9 *form = m_Constructor->GetEntry (a);
        time = cparam->get_key_time(a);
        cparam->get_value(time, v, t, c, b, 0);
        form->t = t;
        form->c = c;
        form->b = b;
        form->Value->Value = v;
        form->WorkTime->Value = time - prev;
        prev = time;
        }

}

