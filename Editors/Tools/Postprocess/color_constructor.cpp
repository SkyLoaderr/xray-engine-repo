//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdlib.h>
#include <stdio.h>
#pragma hdrstop
#include "color_constructor.h"

#define HEIGHT  26
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm6 *Form6;
//---------------------------------------------------------------------------
__fastcall TForm6::TForm6(TComponent* Owner)
    : TForm(Owner)
{
    m_iTag = 0;
    AddEntryTemplate (-1);
    m_Entries[0]->WorkTime->Value = 0.0f;
    m_Entries[0]->WorkTime->Enabled = false;
    m_Entries[0]->ColorPanel->Color = clBlack;
    m_Entries[0]->DeleteButton->Enabled = false;
}
//---------------------------------------------------------------------------
TForm7*    TForm6::AddEntryTemplate    (int iInsertAfter)
{
    TForm7 *form = new TForm7 (this);
    form->Parent = this;
    form->Left = 0;
    form->Tag = m_iTag++;
    form->Visible = true;
    form->AddButton->Tag = form->Tag;
    form->DeleteButton->Tag = form->Tag;
    form->AddButton->OnClick = OnAddButtonClick;
    form->DeleteButton->OnClick = OnDelButtonClick;
    vector<TForm7*> temp;
    iInsertAfter++;
    if (!m_Entries.size () || m_Entries.size () == (size_t)iInsertAfter)
       {
       m_Entries.push_back (form);
       }
    else
       {
       for (int a = 0; a < (int)m_Entries.size (); a++)
           {
           if (a == iInsertAfter)
              temp.push_back (form);
           temp.push_back (m_Entries[a]);
           }
       m_Entries.assign (temp.begin (), temp.end ());
       }
    ResetPositions ();
    return form;
}
//---------------------------------------------------------------------------
void    TForm6::ResetPositions      ()
{
    int top = 0;
    for (size_t a = 0; a < m_Entries.size (); a++)
        {
        m_Entries[a]->Left = 0;
        m_Entries[a]->Top = (m_Entries[a]->Height + 4) * a;
        top = m_Entries[a]->Top;
        }
    Button1->Top = top + m_Entries[0]->Height + 4;
    Button2->Top = Button1->Top;
    if (m_Entries.size () <= 6)
       ClientHeight = Button1->Top + Button1->Height;
}
//---------------------------------------------------------------------------
void    __fastcall  TForm6::OnAddButtonClick   (TObject *Sender)
{
    int tag = dynamic_cast<TComponent*> (Sender)->Tag;
    for (size_t a = 0; a < m_Entries.size (); a++)
        if (m_Entries[a]->Tag == tag)
           {
           AddEntryTemplate (a);
           return;
           }
}
//---------------------------------------------------------------------------
void    __fastcall  TForm6::OnDelButtonClick   (TObject *Sender)
{
    if (m_Entries.size () == 1) return;
    int tag = dynamic_cast<TControl*> (Sender)->Parent->Parent->Tag;
    vector<TForm7*>::iterator s = m_Entries.begin(), e = m_Entries.end ();
    for (; s != e; ++s)
        if ((*s)->Tag == tag)
           {
           TForm7 *form = (*s);
           delete form;
           m_Entries.erase (s);
           RecalcSize ();
           return;
           }
}
//---------------------------------------------------------------------------
void                TForm6::RecalcSize          ()
{
    int top;
    for (int a = 0; a < (int)m_Entries.size (); a++)
        {
        m_Entries[a]->Top = a * m_Entries[a]->Height + 2;
        top = m_Entries[a]->Top;
        }
    Button1->Top = top + m_Entries[0]->Height + 4;
    Button2->Top = Button1->Top;
    if (m_Entries.size () <= 6)
       ClientHeight = Button1->Top + Button1->Height;
}
//---------------------------------------------------------------------------
void    __fastcall  TForm6::Reset              ()
{
    vector<TForm7*>::iterator s = m_Entries.begin(), e = m_Entries.end ();
    for (; s != e; ++s)
        {
        TForm7 *form = (*s);
        delete form;
        }
    m_Entries.clear();
    m_InitColor = clBlack;
    AddEntryTemplate (-1);
    m_Entries[0]->WorkTime->Value = 0.0f;
    m_Entries[0]->WorkTime->Enabled = false;
    m_Entries[0]->ColorPanel->Color = clBlack;
}
//---------------------------------------------------------------------------
TForm7* __fastcall  TForm6::GetEntry           (size_t index)
{
    if (index >= m_Entries.size())
       throw Exception ("Color constructor: invalid index");
    return m_Entries[index];
}
//---------------------------------------------------------------------------
int     __fastcall  TForm6::ShowModal           (CPostprocessAnimator* animator)
{
    
    return TForm::ShowModal ();
}

