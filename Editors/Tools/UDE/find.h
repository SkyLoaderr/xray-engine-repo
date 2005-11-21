//---------------------------------------------------------------------------

#ifndef findH
#define findH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "RXCtrls.hpp"
#include "RXSpin.hpp"
#include <Mask.hpp>
#include "main.h"


//---------------------------------------------------------------------------
class TFindStringDialog : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TRxLabel *RxLabel1;
    TRxLabel *RxLabel2;
    TRxLabel *RxLabel3;
    TRxLabel *RxLabel4;
    TRxLabel *RxLabel5;
    TRxLabel *RxLabel6;
    TRxLabel *RxLabel7;
    TRxLabel *RxLabel8;
    TComboBox *CategoryF;
    TComboBox *NPCCommunityF;
    TComboBox *RelationF;
    TRxSpinEdit *NPCRankF;
    TComboBox *LevelF;
    TEdit *CondlistF;
    TEdit *SmartTerrainF;
    TComboBox *CommunityF;
    TButton *Button1;
    TButton *Button2;
    TListBox *StringList;
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    string      m_Selected;
    __fastcall TFindStringDialog(TComponent* Owner);
    virtual int __fastcall  ShowModal   (dm_list *list);
};
//---------------------------------------------------------------------------
extern PACKAGE TFindStringDialog *FindStringDialog;
//---------------------------------------------------------------------------
#endif
