//---------------------------------------------------------------------------

#ifndef DOShuffleH
#define DOShuffleH
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
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"
//---------------------------------------------------------------------------
// refs
class TfrmOneColor;
class EImageThumbnail;

class TfrmDOShuffle : public TForm
{
friend class TfrmOneColor;
__published:	// IDE-managed Components
	TPanel *paTools;
	TFormStorage *fsStorage;
	TPanel *Panel3;
	TScrollBox *sbDO;
	TExtBtn *ebAppendIndex;
	TExtBtn *ebMultiClear;
	TPanel *Panel2;
	TElTree *tvItems;
	TPanel *Panel4;
	TLabel *lbItemName;
	TLabel *RxLabel1;
	TLabel *RxLabel3;
	TLabel *lbInfo;
	TPanel *paImage;
	TPaintBox *pbImage;
	TPanel *Panel5;
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
	TPanel *Panel1;
	TExtBtn *ebAddObject;
	TExtBtn *ebDelObject;
	TExtBtn *ebDOProperties;
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall pbImagePaint(TObject *Sender);
	void __fastcall tvItemsKeyPress(TObject *Sender, char &Key);
	void __fastcall tvMultiDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall tvMultiDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall tvMultiStartDrag(TObject *Sender,
          TDragObject *&DragObject);
	void __fastcall ebAddObjectClick(TObject *Sender);
	void __fastcall ebDelObjectClick(TObject *Sender);
	void __fastcall ebAppendIndexClick(TObject *Sender);
	void __fastcall ebMultiClearClick(TObject *Sender);
	void __fastcall tvItemsDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall tvItemsDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall tvItemsStartDrag(TObject *Sender,
          TDragObject *&DragObject);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ebDOPropertiesClick(TObject *Sender);
	void __fastcall tvItemsDblClick(TObject *Sender);
	void __fastcall tvItemsItemFocused(TObject *Sender);
private:	// User declarations
	static TfrmDOShuffle* form;
    TElTreeItem* FDragItem;

    EImageThumbnail* m_Thm;

    vector<TfrmOneColor*> color_indices;

    void InitItemsList(const char* nm=0);
	TElTreeItem* FindItem(const char* s);
    TElTreeItem* AddItem(TElTreeItem* node, const char* name, void* obj=(void*)1);

    void GetInfo();
    void ApplyInfo();

    bool bColorIndModif;
    void ModifColorInd(){bColorIndModif=true;}
public:		// User declarations
    __fastcall TfrmDOShuffle(TComponent* Owner);
// static function
    static bool __fastcall Run ();
    static bool __fastcall Visible(){return !!form;}
	static void __fastcall RemoveColorIndex(TfrmOneColor* p);
	static TfrmDOShuffle* __fastcall Form(){return form;}
};
//---------------------------------------------------------------------------
struct SDOData{
    AnsiString	m_RefName;
    float		m_fMinScale;
    float		m_fMaxScale;
    float 		m_fDensityFactor;
    DWORD		m_dwFlags;
    SDOData		();
};
DEFINE_VECTOR(SDOData*,DDVec,DDIt);
//---------------------------------------------------------------------------
#endif
