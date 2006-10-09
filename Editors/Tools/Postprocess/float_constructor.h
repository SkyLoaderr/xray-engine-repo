//---------------------------------------------------------------------------

#ifndef float_constructorH
#define float_constructorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <stdio.h>

#include "float_param.h"
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Mask.hpp>

#include <vector>

using namespace std;
//---------------------------------------------------------------------------
class TForm8 : public TForm
{
__published:	// IDE-managed Components
    TButton *Button1;
    TButton *Button2;
    TOpenDialog *OpenDialog;
    TSaveDialog *SaveDialog;
private:	// User declarations
    void    RecalcSize          ();
    void    ResetPositions      ();
    int     m_iTag;
public:		// User declarations
    vector<TForm9*>         m_Entries;
    __fastcall TForm8(TComponent* Owner);
    void    AddEntryTemplate    (int iInsertAfter);
    void    __fastcall  OnAddButtonClick   (TObject *Sender);
    void    __fastcall  OnDelButtonClick   (TObject *Sender);
    void    __fastcall  Reset              ();
    TForm9* __fastcall  GetEntry           (size_t index);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm8 *Form8;
//---------------------------------------------------------------------------
#endif
