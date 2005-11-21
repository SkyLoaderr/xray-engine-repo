//---------------------------------------------------------------------------

#ifndef Unit6H
#define Unit6H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "RXCtrls.hpp"
//---------------------------------------------------------------------------
class TAddLangDialog : public TForm
{
__published:	// IDE-managed Components
    TRxLabel *RxLabel1;
    TEdit *LangShortcut;
    TButton *Button1;
    TButton *Button2;
    void __fastcall LangShortcutChange(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
    vsConfig *m_pConfig;
public:		// User declarations
    __fastcall TAddLangDialog(TComponent* Owner);
    void    InitializeData  (vsConfig *pConfig) { m_pConfig = pConfig; }
};
//---------------------------------------------------------------------------
extern PACKAGE TAddLangDialog *AddLangDialog;
//---------------------------------------------------------------------------
#endif
