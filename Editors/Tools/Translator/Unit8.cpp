//---------------------------------------------------------------------------

#include <vcl.h>
#include <string>
#pragma hdrstop

#include "Unit8.h"
using namespace std;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRemovedStringsDialog *RemovedStringsDialog;
//---------------------------------------------------------------------------
__fastcall TRemovedStringsDialog::TRemovedStringsDialog(TComponent* Owner)
    : TForm(Owner)
{
    StringsList->DefaultColWidth = StringsList->Width / 2 - 10;
}
//---------------------------------------------------------------------------
void        TRemovedStringsDialog::Add (string file, string strid)
{
    StringsList->Cells[0][StringsList->RowCount - 1] = file.c_str ();
    StringsList->Cells[1][StringsList->RowCount - 1] = strid.c_str ();
    StringsList->RowCount++;
}
//---------------------------------------------------------------------------
void        TRemovedStringsDialog::Clear   ()
{
    StringsList->RowCount = 1;
    StringsList->Cells[0][0] = "";
    StringsList->Cells[1][0] = "";
}

