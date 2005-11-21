//---------------------------------------------------------------------------

#ifndef Unit7H
#define Unit7H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TStatsDialog : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TPanel *Panel1;
    TPanel *Panel2;
    TPanel *Panel3;
    TPanel *SrcFiles;
    TPanel *SrcStrings;
    TPanel *SrcWords;
    TGroupBox *GroupBox2;
    TPanel *Panel4;
    TPanel *Panel5;
    TPanel *Panel6;
    TPanel *DestFiles;
    TPanel *DestStrings;
    TPanel *DestWords;
    TButton *Button1;
private:	// User declarations
public:		// User declarations
    __fastcall TStatsDialog(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TStatsDialog *StatsDialog;
//---------------------------------------------------------------------------
#endif
