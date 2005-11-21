//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "find.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXCtrls"
#pragma link "RXSpin"
#pragma resource "*.dfm"
TFindStringDialog *FindStringDialog;
//---------------------------------------------------------------------------
__fastcall TFindStringDialog::TFindStringDialog(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void        TFindStringDialog::FillList        ()
{
    m_List.clear ();
    StringList->Clear ();

    dm_list_i s = m_Original->begin (), e = m_Original->end ();
    for (; s != e; ++s)
        {
        if ((*s).second.m_Used == false) continue;
        StringList->Items->Add ((*s).first.c_str ());
        m_List[(*s).first] = (*s).second;
        }
}
//---------------------------------------------------------------------------
void __fastcall TFindStringDialog::Button1Click(TObject *Sender)
{
    if (StringList->ItemIndex == -1)
       m_Selected = "";
    else
       m_Selected = StringList->Items->Strings[StringList->ItemIndex].c_str ();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------


