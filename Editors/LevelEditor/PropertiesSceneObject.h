//---------------------------------------------------------------------------
#ifndef PropertiesSceneObjectH
#define PropertiesSceneObjectH
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include "multi_check.hpp"
#include <StdCtrls.hpp>
#include "multi_edit.hpp"
//---------------------------------------------------------------------------
#include <Forms.hpp>
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "ElPgCtl.hpp"
#include "ElXPThemedControl.hpp"
#include "ElTree.hpp"
#include "mxPlacemnt.hpp"
#include "ElTreeAdvEdit.hpp"
#include "MxMenus.hpp"
#include <Menus.hpp>

// refs
class CCustomObject;
class CSceneObject;
class COMotion;

class TfrmPropertiesSceneObject : public TForm
{
__published:	// IDE-managed Components
	TElPageControl *pcSceneProps;
	TElTabSheet *tsBasic;
	TPanel *paBasic;
	TElTabSheet *tsMotions;
	TLabel *RxLabel1;
	TEdit *edName;
	TExtBtn *ebReference;
	TLabel *RxLabel2;
	TPanel *paBottom;
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
	TFormStorage *fsStorage;
	TLabel *Label1;
	TPanel *paMotions;
	TBevel *Bevel1;
	TElTree *tvOMotions;
	TPanel *Panel1;
	TExtBtn *ebOMotionAppend;
	TExtBtn *ebOMotionRename;
	TExtBtn *ebOMotionDelete;
	TExtBtn *ebOMotionClear;
	TExtBtn *ebOMotionSave;
	TExtBtn *ebOMotionLoad;
	TGroupBox *GroupBox3;
	TLabel *RxLabel19;
	TLabel *lbOMotionCount;
	TLabel *RxLabel21;
	TLabel *lbActiveOMotion;
	TGroupBox *gbOMotion;
	TLabel *RxLabel20;
	TLabel *lbOMotionName;
	TLabel *RxLabel23;
	TLabel *lbOMotionFrames;
	TLabel *lbOMotionFPS;
	TLabel *RxLabel28;
	TElTreeInplaceAdvancedEdit *InplaceMotionEdit;
	TMxPopupMenu *pmMotions;
	TMenuItem *Rename1;
	TMenuItem *N4;
	TMenuItem *CreateFolder1;
	TMenuItem *N1;
	TMenuItem *ExpandAll1;
	TMenuItem *CollapseAll1;
	TGroupBox *gbFlags;
	TMultiObjCheck *cbDummy;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ebReferenceClick(TObject *Sender);
	void __fastcall tsMotionsShow(TObject *Sender);
	void __fastcall tvOMotionsItemFocused(TObject *Sender);
	void __fastcall ebOMotionAppendClick(TObject *Sender);
	void __fastcall ebOMotionDeleteClick(TObject *Sender);
	void __fastcall tvOMotionsItemChange(TObject *Sender, TElTreeItem *Item,
          TItemChangeMode ItemChangeMode);
	void __fastcall ebOMotionClearClick(TObject *Sender);
	void __fastcall ebOMotionSaveClick(TObject *Sender);
	void __fastcall ebOMotionLoadClick(TObject *Sender);
	void __fastcall tvOMotionsDragDrop(TObject *Sender, TObject *Source,
          int X, int Y);
	void __fastcall tvOMotionsDragOver(TObject *Sender, TObject *Source,
          int X, int Y, TDragState State, bool &Accept);
	void __fastcall tvOMotionsStartDrag(TObject *Sender,
          TDragObject *&DragObject);
	void __fastcall InplaceMotionEditValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall ebOMotionRenameClick(TObject *Sender);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall CreateFolder1Click(TObject *Sender);
	void __fastcall ExpandAll1Click(TObject *Sender);
	void __fastcall CollapseAll1Click(TObject *Sender);
	void __fastcall tvOMotionsMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
private:	// User declarations
	static TfrmPropertiesSceneObject* form;
    bool 					bLoadMode;
    CSceneObject* 			m_EditObject;
    vector<COMotion*>		m_OMotions;
    COMotion*				m_ActiveOMotion;
    list<CCustomObject*>* 	m_Objects;
    void GetObjectsInfo     ();
    bool ApplyObjectsInfo   ();

    void ClearObjectsInfo	();
    void SaveObjectsInfo	();
    void RestoreObjectsInfo	();
    AnsiString m_NewReference;
    IC bool IsMultiSelection(){return (m_Objects->size()!=1);}
	void __fastcall OnRenameItem(LPCSTR p0, LPCSTR p1);
	void __fastcall OnRemoveItem(LPCSTR name);
	void __fastcall ebOResetActiveMotion(TElTreeItem* ignore_item);
public:		// User declarations
    __fastcall TfrmPropertiesSceneObject(TComponent* Owner);
    static int __fastcall Run(list<CCustomObject*>* pObjects, bool& bChange);
    static bool __fastcall Visible(){return !!form;}
};
//---------------------------------------------------------------------------
#endif
