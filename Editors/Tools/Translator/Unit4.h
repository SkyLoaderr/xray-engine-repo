//---------------------------------------------------------------------------

#ifndef Unit4H
#define Unit4H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "RXCtrls.hpp"
//---------------------------------------------------------------------------
class TDiffFilesDialog : public TForm
{
__published:	// IDE-managed Components
    TRxLabel *RxLabel1;
    TListBox *FilesList;
    TButton *Button1;
private:	// User declarations
public:		// User declarations
    __fastcall TDiffFilesDialog(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TDiffFilesDialog *DiffFilesDialog;
//---------------------------------------------------------------------------
#endif
