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

#include "ExtBtn.hpp"
#include "multi_edit.hpp"
#include "mxPlacemnt.hpp"

#include "ItemList.h"
#include "ESceneCustomMTools.h"

// refs
class CEditableObject;
//---------------------------------------------------------------------------
class TfraObject : public TForm
{
__published:	// IDE-managed Components
    TPanel *paCommands;
	TExtBtn *ebMultiAppend;
	TPanel *paCurrentObject;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TLabel *APHeadLabel2;
	TFormStorage *fsStorage;
	TBevel *Bevel2;
	TPanel *paItems;
	TPanel *Panel1;
	TExtBtn *ExtBtn4;
	TLabel *Label5;
	TExtBtn *ebSelectByRefs;
	TExtBtn *ebDeselectByRefs;
	TExtBtn *ebMultiSelectByRefMove;
	TBevel *Bevel1;
	TLabel *Label2;
	TExtBtn *ebMultiSelectByRefAppend;
	TMultiObjSpinEdit *seSelPercent;
	TLabel *Label3;
	TPanel *paSelectObject;
	TLabel *Label1;
	TExtBtn *ExtBtn1;
	TExtBtn *ExtBtn3;
	TLabel *APHeadLabel3;
	TExtBtn *ExtBtn5;
	TExtBtn *ExtBtn6;
	TBevel *Bevel3;
	TLabel *Label4;
	TExtBtn *ExtBtn7;
	TLabel *Label6;
	TMultiObjSpinEdit *MultiObjSpinEdit1;
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
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
private:	// User declarations
    void __fastcall MultiSelByRefObject ( bool clear_prev );
    void __fastcall SelByRefObject  	( bool flag );
    TItemList* m_Items;
    void __fastcall OnItemFocused		(ListItemsVec& items);
    LPCSTR m_Current;
	bool __fastcall OnDrawObjectThumbnail(ListItem* sender, TCanvas *Surface, TRect &R);
public:		// User declarations
        __fastcall TfraObject(TComponent* Owner);
    LPCSTR 			Current	(){return m_Current;}
};
//---------------------------------------------------------------------------
#endif
