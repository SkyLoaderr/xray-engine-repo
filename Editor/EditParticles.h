//---------------------------------------------------------------------------

#ifndef EditParticlesH
#define EditParticlesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>

#include <Dialogs.hpp>
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"
#include <Menus.hpp>
#include "MxMenus.hpp"
//---------------------------------------------------------------------------
// refs
class CParticleSystem;
class CPSObject;
//---------------------------------------------------------------------------
#include "ParticleSystem.h"

class TfrmEditParticles : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
	TPopupMenu *pmShaders;
    TMenuItem *miNewFolder;
    TMenuItem *miEditFolder;
    TMenuItem *miDeleteFolder;
    TMenuItem *miObjectProperties;
    TMenuItem *N2;
    TMenuItem *LoadObject1;
    TMenuItem *DeleteObject1;
	TMenuItem *Object1;
	TGroupBox *GroupBox1;
	TExtBtn *ebNewPS;
	TExtBtn *ebRemovePS;
	TExtBtn *ebPropertiesPS;
	TExtBtn *ebClonePS;
	TMenuItem *N1;
	TPanel *Panel2;
	TExtBtn *ebCancel;
	TExtBtn *ebSave;
	TExtBtn *ebReloadPS;
	TExtBtn *ebMerge;
	TFormStorage *fsStorage;
	TGroupBox *gbCurrentPS;
	TExtBtn *ExtBtn2;
	TExtBtn *ExtBtn5;
	TMxLabel *lbCurState;
	TMxLabel *lbParticleCount;
	TMxLabel *RxLabel1;
	TMxLabel *RxLabel2;
	TElTree *tvItems;
	TMxPopupMenu *pmEnum;
	TMenuItem *N4;
	TMenuItem *CreateFolder1;
	TMenuItem *N5;
	TMenuItem *Rename1;
	TMenuItem *PS1;
	TMenuItem *Properties1;
	TMenuItem *N3;
	TMenuItem *Remove1;
	TMenuItem *Append1;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebPropertiesPSClick(TObject *Sender);
    void __fastcall ebRemovePSClick(TObject *Sender);
    void __fastcall tvItemsDblClick(TObject *Sender);
    void __fastcall ebAppendPSClick(TObject *Sender);
    void __fastcall miNewFolderClick(TObject *Sender);
    void __fastcall miEditFolderClick(TObject *Sender);
    void __fastcall tvItemsDragDrop(TObject *Sender, TObject *Source,
          int X, int Y);
    void __fastcall tvItemsDragOver(TObject *Sender, TObject *Source,
          int X, int Y, TDragState State, bool &Accept);
    void __fastcall tvItemsStartDrag(TObject *Sender,
          TDragObject *&DragObject);
    void __fastcall tvItemsItemChange(TObject *Sender, TElTreeItem *Item,
          TItemChangeMode ItemChangeMode);
	void __fastcall tvItemsKeyPress(TObject *Sender, char &Key);
	void __fastcall ebReloadPSClick(TObject *Sender);
	void __fastcall ebClonePSClick(TObject *Sender);
	void __fastcall ebMergeClick(TObject *Sender);
	void __fastcall ExtBtn2Click(TObject *Sender);
	void __fastcall ExtBtn5Click(TObject *Sender);
	void __fastcall tvItemsItemFocused(TObject *Sender);
private:
// list functions
    void InitItemsList(const char* nm=0);
	TElTreeItem* FindItem(const char* s);
    TElTreeItem* FindFolder(const char* s);
    TElTreeItem* AddFolder(const char* s);
    TElTreeItem* AddItem(TElTreeItem* node, const char* name);
    TElTreeItem* AddItemToFolder(const char* folder, const char* name);
    TElTreeItem* FDragItem;
    TElTreeItem* FEditNode;
private:	// User declarations
	static TfrmEditParticles* form;

    PS::SDef* 	m_SelectedPS;
    CPSObject*  m_TestObject;

    Fvector init_cam_hpb;
    Fvector init_cam_pos;
    void OnModified();
    void 		SetCurrent(PS::SDef* ps);
    void		ResetCurrent();
public:		// User declarations
    __fastcall TfrmEditParticles(TComponent* Owner);

    static void __fastcall OnRender();
    static void __fastcall OnIdle();
	static void __fastcall ZoomObject();
// static function
    static void __fastcall ShowEditor();
    static bool __fastcall HideEditor(bool bNeedReload);
    static bool __fastcall FinalClose();
    static bool __fastcall Visible(){return !!form;}
    static TfrmEditParticles* GetForm(){VERIFY(form); return form;}
    static PS::SDef* __fastcall GetSelectedPS(){if(!Visible()) return 0; else return form->m_SelectedPS;}
    static void __fastcall Modified(){if(!Visible()) return; form->OnModified();}
    static void __fastcall UpdateCurrent();
    static void __fastcall UpdateEmitter();
    static void __fastcall OnNameUpdate();
    static PS::SDef* __fastcall FindPS(LPCSTR name);
};
//---------------------------------------------------------------------------
#endif
