//---------------------------------------------------------------------------

#ifndef ChoseFormH
#define ChoseFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>

#include "Library.h"
#include <Dialogs.hpp>
#include "ElTree.hpp"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"
#include "ElXPThemedControl.hpp"
#include "Gradient.hpp"
#include "ChooseTypes.H"

//---------------------------------------------------------------------------
// refs
class EImageThumbnail;
class TProperties;

class ECORE_API TfrmChoseItem : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
	TFormStorage *fsStorage;
	TPanel *paMulti;
	TElTree *tvMulti;
	TExtBtn *ebMultiUp;
	TExtBtn *ebMultiDown;
	TExtBtn *ebMultiRemove;
	TExtBtn *ebMultiClear;
	TPanel *Panel2;
	TExtBtn *sbSelect;
	TExtBtn *sbCancel;
	TPanel *Panel4;
	TElTree *tvItems;
	TPanel *paItemsCount;
	TGradient *grdFon;
	TLabel *mxLabel2;
	TLabel *lbHint;
	TLabel *lbItemName;
	TLabel *mxLabel1;
	TMxPanel *paImage;
	TExtBtn *ebExt;
	TPanel *paInfo;
	TBevel *Bevel1;
    void __fastcall sbSelectClick(TObject *Sender);
    void __fastcall sbCancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall tvItemsDblClick(TObject *Sender);
	void __fastcall tvItemsKeyPress(TObject *Sender, char &Key);
	void __fastcall tvItemsItemChange(TObject *Sender, TElTreeItem *Item,
          TItemChangeMode ItemChangeMode);
	void __fastcall ebMultiUpClick(TObject *Sender);
	void __fastcall ebMultiDownClick(TObject *Sender);
	void __fastcall tvMultiDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall tvMultiDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall tvMultiStartDrag(TObject *Sender,
          TDragObject *&DragObject);
	void __fastcall ebMultiRemoveClick(TObject *Sender);
	void __fastcall ebMultiClearClick(TObject *Sender);
	void __fastcall tvItemsItemFocused(TObject *Sender);
	void __fastcall paImagePaint(TObject *Sender);
	void __fastcall ebExtClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall tvItemsCompareItems(TObject *Sender, TElTreeItem *Item1,
          TElTreeItem *Item2, int &res);
private:	// User declarations
	static TfrmChoseItem* form;
    static AnsiString select_item;

    EImageThumbnail* 	m_Thm;
	ref_sound			m_Snd;

    TProperties*	m_Props;
    void 	InitItemsList(const char* nm=0);

    bool 	bMultiSel;
    int 	iMultiSelLimit;
// fill routines
    void __fastcall FillCustom		(ChooseItemVec* items);
    void __fastcall FillSoundSource	();
    void __fastcall FillSoundEnv	();
    void __fastcall FillObject		();
    void __fastcall FillLAnim		();
    void __fastcall FillEShader		();
    void __fastcall FillCShader		();
//    void __fastcall FillPS			();
    void __fastcall FillPE			();
    void __fastcall FillParticles	();
    void __fastcall FillTexture		();
    void __fastcall FillEntity		();
    void __fastcall FillGameObject	();
    void __fastcall FillGameAnim	();
    void __fastcall FillGameMaterial();

    void __fastcall AppendItem		(LPCSTR name, LPCSTR info="");
public:		// User declarations
protected:
    EChooseMode Mode;
    static AnsiString m_LastSelection[smMaxMode]; 
public:		// User declarations
    __fastcall 					TfrmChoseItem	(TComponent* Owner);
    static int	 	__fastcall 	SelectItem		(EChooseMode mode, LPCSTR& dest, int sel_cnt=1, LPCSTR init_name=0, ChooseItemVec* items=0);
    static bool 	__fastcall 	Visible			(){return !!form;}
};
//---------------------------------------------------------------------------
#endif
