//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include "ToolEdit.hpp"
#include <Mask.hpp>
//---------------------------------------------------------------------------
class TConfigDialog : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TLabel *Label1;
    TLabel *Label2;
    TEdit *Lang1;
    TEdit *Lang2;
    TGroupBox *GroupBox2;
    TBevel *Bevel1;
    TLabel *Label3;
    TButton *OkButton;
    TButton *Button2;
    TDirectoryEdit *Path1;
    TDirectoryEdit *ImportPath;
    TLabel *Label4;
    void __fastcall TextChange(TObject *Sender);
    void __fastcall OkButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TConfigDialog(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TConfigDialog *ConfigDialog;
//---------------------------------------------------------------------------
#endif
