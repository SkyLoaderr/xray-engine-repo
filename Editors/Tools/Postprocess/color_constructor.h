//---------------------------------------------------------------------------

#ifndef color_constructorH
#define color_constructorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <stdio.h>

#include "color_param.h"
#include <ExtCtrls.hpp>

#include <vector>
#include <xrCore.h>
#include "PostprocessAnimator.h"

using namespace std;

//---------------------------------------------------------------------------
class TForm6 : public TForm
{
__published:	// IDE-managed Components
    TButton *Button1;
    TButton *Button2;
    TColorDialog *ColorDialog;
    TOpenDialog *OpenDialog;
    TSaveDialog *SaveDialog;
private:	// User declarations
    void    RecalcSize          ();
    void    ResetPositions      ();
    int     m_iTag;
public:		// User declarations
    TForm7* AddEntryTemplate    (int iInsertAfter);
    TColor                  m_InitColor;
    vector<TForm7*>         m_Entries;
    __fastcall TForm6(TComponent* Owner);
//    __fastcall TForm6 (HWND handle);
    void    __fastcall  OnAddButtonClick   (TObject *Sender);
    void    __fastcall  OnDelButtonClick   (TObject *Sender);
    void    __fastcall  SaveData           (FILE *file);
    void    __fastcall  LoadData           (FILE *file);
    void    __fastcall  Reset              ();
    TForm7* __fastcall  GetEntry           (size_t index);
    int     __fastcall  ShowModal          (CPostprocessAnimator* animator);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm6 *Form6;
//---------------------------------------------------------------------------
#endif
