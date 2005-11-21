//---------------------------------------------------------------------------

#ifndef MainH
#define MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "RxCtrls.hpp"
#include "RXSpin.hpp"
#include <Mask.hpp>
#include "RXCtrls.hpp"
#include "..\translator\tunyxml\tinyxml.h"
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <TabNotBk.hpp>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

typedef struct _DM_UNIT
{
    bool            m_Used;
    string          m_Category;
    string          m_Community;
    string          m_NPCCommunity;
    string          m_Relation;
    int             m_NPCRank;
    string          m_Level;
    string          m_Condlist;
    string          m_SmartTerrain;
    string          m_Text;
} DM_UNIT;

typedef map<string, DM_UNIT>                 dm_list;
typedef map<string, DM_UNIT>::iterator       dm_list_i;

//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
    TListBox *StringList;
    TMemo *Text;
    TEdit *AddString;
    TSpeedButton *AddStringButton;
    TSpeedButton *DeleteStringButton;
    TTabbedNotebook *Page;
    TGroupBox *GroupBox1;
    TRxLabel *RxLabel1;
    TRxLabel *RxLabel2;
    TRxLabel *RxLabel3;
    TRxLabel *RxLabel4;
    TRxLabel *RxLabel5;
    TRxLabel *RxLabel6;
    TRxLabel *RxLabel7;
    TRxLabel *RxLabel8;
    TComboBox *Category;
    TComboBox *NPCCommunity;
    TComboBox *Relation;
    TRxSpinEdit *NPCRank;
    TComboBox *Level;
    TEdit *Condlist;
    TEdit *SmartTerrain;
    TComboBox *Community;
    TGroupBox *GroupBox2;
    TRxLabel *RxLabel9;
    TRxLabel *RxLabel10;
    TRxLabel *RxLabel11;
    TRxLabel *RxLabel12;
    TRxLabel *RxLabel13;
    TRxLabel *RxLabel14;
    TRxLabel *RxLabel15;
    TRxLabel *RxLabel16;
    TComboBox *CategoryF;
    TComboBox *NPCCommunityF;
    TComboBox *RelationF;
    TRxSpinEdit *NPCRankF;
    TComboBox *LevelF;
    TEdit *CondlistF;
    TEdit *SmartTerrainF;
    TComboBox *CommunityF;
    void __fastcall StringListClick(TObject *Sender);
    void __fastcall CategoryChange(TObject *Sender);
    void __fastcall AddStringButtonClick(TObject *Sender);
    void __fastcall DeleteStringButtonClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall PageChange(TObject *Sender, int NewTab,
          bool &AllowChange);
    void __fastcall CategoryFChange(TObject *Sender);
private:	// User declarations
    dm_list         m_List;
    bool            m_bInternal;
    void            Find        ();
    dm_list         m_FList;
    dm_list        *m_Original;
    void            FillList        ();

public:		// User declarations
    __fastcall TMainForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif
