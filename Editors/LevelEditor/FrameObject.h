//---------------------------------------------------------------------------
#ifndef FrameObjectH
#define FrameObjectH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

#include "ui_customtools.h"
#include "ExtBtn.hpp"
#include "multi_edit.hpp"
#include "mxPlacemnt.hpp"

#include "ItemList.h"

// refs
class CEditableObject;
//---------------------------------------------------------------------------
class TfraObject : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paSelectObject;
    TPanel *paCommands;
	TExtBtn *ebMultiAppend;
    TLabel *Label5;
	TExtBtn *ebSelectByRefs;
	TExtBtn *ebDeselectByRefs;
	TPanel *paCurrentObject;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TLabel *APHeadLabel2;
	TLabel *APHeadLabel3;
	TExtBtn *ExtBtn3;
	TExtBtn *ebMultiSelectByRefMove;
	TMultiObjSpinEdit *seSelPercent;
	TBevel *Bevel1;
	TFormStorage *fsStorage;
	TLabel *Label1;
	TExtBtn *ebMultiSelectByRefAppend;
	TBevel *Bevel2;
	TPanel *paItems;
	TPanel *Panel1;
	TExtBtn *ExtBtn4;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ebSelectByRefsClick(TObject *Sender);
    void __fastcall ebDeselectByRefsClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ebMultiAppendClick(TObject *Sender);
	void __fastcall ebMultiSelectByRefMoveClick(TObject *Sender);
	void __fastcall ebMultiSelectByRefAppendClick(TObject *Sender);
	void __fastcall seSelPercentKeyPress(TObject *Sender, char &Key);
	void __fastcall ExtBtn4Click(TObject *Sender);
	void __fastcall paCurrentObjectResize(TObject *Sender);
private:	// User declarations
    void __fastcall MultiSelByRefObject ( bool clear_prev );
    void __fastcall SelByRefObject  	( bool flag );
    TItemList* m_Items;
    void __fastcall OnItemFocused		(ListItemsVec& items);
    LPCSTR m_Current;
	bool __fastcall OnDrawObjectThumbnail(ListItem* sender, TCanvas *Surface, TRect &R);
public:		// User declarations
        __fastcall TfraObject(TComponent* Owner);
    void __fastcall OnEnter	();
    void __fastcall OnExit	();
    LPCSTR 			Current	(){return m_Current;}
};
//---------------------------------------------------------------------------
#endif
