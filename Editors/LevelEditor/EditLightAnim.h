//---------------------------------------------------------------------------

#ifndef EditLightAnimH
#define EditLightAnimH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include "mxPlacemnt.hpp"
#include "multi_edit.hpp"
#include "Gradient.hpp"
#include "ElTrackBar.hpp"
#include "ElTreeAdvEdit.hpp"
#include "MxMenus.hpp"
#include <Menus.hpp>
#include "MXCtrls.hpp"

//---------------------------------------------------------------------------
// refs
class CLAItem;
class TProperties;
class PropValue;
//---------------------------------------------------------------------------

class TfrmEditLightAnim : public TForm
{
__published:	// IDE-managed Components
	TPanel *paItemProps;
	TFormStorage *fsStorage;
	TPanel *paColor;
	TPanel *Panel3;
	TElTree *tvItems;
	TPanel *Panel2;
	TExtBtn *ebAddAnim;
	TExtBtn *ebDeleteAnim;
	TBevel *Bevel1;
	TExtBtn *ebSave;
	TExtBtn *ebReload;
	TPaintBox *pbG;
	TMultiObjSpinEdit *sePointer;
	TStaticText *stStartFrame;
	TStaticText *stEndFrame;
	TExtBtn *ebDeleteKey;
	TMxPopupMenu *pmActionList;
	TMenuItem *Rename1;
	TMenuItem *N4;
	TMenuItem *CreateFolder1;
	TMenuItem *N1;
	TMenuItem *ExpandAll1;
	TMenuItem *CollapseAll1;
	TElTreeInplaceAdvancedEdit *InplaceTextEdit;
	TExtBtn *ebCreateKey;
	TExtBtn *ebPrevKey;
	TExtBtn *ebFirstKey;
	TExtBtn *ebNextKey;
	TExtBtn *ebLastKey;
	TExtBtn *ebMoveKeyLeft;
	TExtBtn *ebMoveKeyRight;
	TExtBtn *ebFirstFrame;
	TExtBtn *ebLastFrame;
	TMxLabel *lbCurFrame;
	TPanel *paProps;
	TMenuItem *N2;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall tvItemsKeyPress(TObject *Sender, char &Key);
	void __fastcall tvItemsItemFocused(TObject *Sender);
	void __fastcall ebAddAnimClick(TObject *Sender);
	void __fastcall ebDeleteAnimClick(TObject *Sender);
	void __fastcall ebSaveClick(TObject *Sender);
	void __fastcall ebReloadClick(TObject *Sender);
	void __fastcall pbGPaint(TObject *Sender);
	void __fastcall pbGMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall sePointerKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall sePointerExit(TObject *Sender);
	void __fastcall ebDeleteKeyClick(TObject *Sender);
	void __fastcall tvItemsDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall tvItemsDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall tvItemsStartDrag(TObject *Sender,
          TDragObject *&DragObject);
	void __fastcall tvItemsMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall InplaceTextEditValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall CreateFolder1Click(TObject *Sender);
	void __fastcall ExpandAll1Click(TObject *Sender);
	void __fastcall CollapseAll1Click(TObject *Sender);
	void __fastcall ebCreateKeyClick(TObject *Sender);
	void __fastcall pbGMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall pbGMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall sePointerChange(TObject *Sender);
	void __fastcall ebFirstKeyClick(TObject *Sender);
	void __fastcall ebPrevKeyClick(TObject *Sender);
	void __fastcall ebNextKeyClick(TObject *Sender);
	void __fastcall ebLastKeyClick(TObject *Sender);
	void __fastcall ebMoveKeyLeftClick(TObject *Sender);
	void __fastcall ebMoveKeyRightClick(TObject *Sender);
	void __fastcall ebLastFrameClick(TObject *Sender);
	void __fastcall ebFirstFrameClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
private:	// User declarations
    void InitItems();
    bool bFinalClose;
    bool IsClose();
	static TfrmEditLightAnim *form;
	static AnsiString m_LastSelection;
    CLAItem* 		m_CurrentItem;
    void	SetCurrentItem(CLAItem* item);
    void 	GetItemData();
    void	UpdateView();
    int		iMoveKey;
    int 	iTgtMoveKey;
    void __fastcall OnRenameItem(LPCSTR p0, LPCSTR p1);

    TProperties*		m_Props;
    void __fastcall		OnModified		(void);
	void __fastcall 	NameOnAfterEdit	(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall 	NameOnBeforeEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall 	NameOnDraw		(PropValue* sender, LPVOID draw_val);
public:		// User declarations
    __fastcall TfrmEditLightAnim(TComponent* Owner);
    static bool FinalClose();
    static void __fastcall ShowEditor();
    static void __fastcall OnIdle();
};
//---------------------------------------------------------------------------
#endif
