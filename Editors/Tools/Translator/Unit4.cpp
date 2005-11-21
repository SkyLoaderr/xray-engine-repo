//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit4.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "RXCtrls"
#pragma resource "*.dfm"
TDiffFilesDialog *DiffFilesDialog;
//---------------------------------------------------------------------------
__fastcall TDiffFilesDialog::TDiffFilesDialog(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------
