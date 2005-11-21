//---------------------------------------------------------------------------
#ifndef Unit3H
#define Unit3H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TLangDialog : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TGroupBox *GroupBox2;
    TListBox *SourceList;
    TListBox *DestList;
    TButton *Button1;
    TButton *Button2;
    void __fastcall SourceListClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TLangDialog(TComponent* Owner);
    void    InitializeData  (vsConfig *pConfig);
};
//---------------------------------------------------------------------------
extern PACKAGE TLangDialog *LangDialog;
//---------------------------------------------------------------------------
#endif
