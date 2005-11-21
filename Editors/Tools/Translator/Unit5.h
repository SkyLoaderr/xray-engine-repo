//---------------------------------------------------------------------------

#ifndef Unit5H
#define Unit5H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "RXCtrls.hpp"
#include "xmlfile.h"

using namespace std;
//---------------------------------------------------------------------------
class TDiffStrDialog : public TForm
{
__published:	// IDE-managed Components
    TListBox *FilesList;
    TListBox *StringsList;
    TRxLabel *RxLabel1;
    TRxLabel *RxLabel2;
    TRxLabel *RxLabel3;
    TButton *Button1;
    TButton *GotoButton;
    void __fastcall FilesListClick(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall GotoButtonClick(TObject *Sender);
private:	// User declarations
    xml_diff_strings    *m_List;
public:		// User declarations
    string  m_File;
    string  m_String;
    __fastcall TDiffStrDialog(TComponent* Owner);
    void    InitializeList      (xml_diff_strings *list);
};
//---------------------------------------------------------------------------
extern PACKAGE TDiffStrDialog *DiffStrDialog;
//---------------------------------------------------------------------------
#endif
