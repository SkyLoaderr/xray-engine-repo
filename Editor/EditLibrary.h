//---------------------------------------------------------------------------

#ifndef EditLibraryH
#define EditLibraryH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>

#include "SceneObject.h"
#include "Library.h"
#include <Dialogs.hpp>
#include "ElTree.hpp"
#include "CloseBtn.hpp"
#include "ElHeader.hpp"
#include "RXCtrls.hpp"
#include <Grids.hpp>
#include "RxMenus.hpp"
#include <Menus.hpp>
#include "Placemnt.hpp"
//---------------------------------------------------------------------------
// refs
class TObjectPreview;
class CEditObject;
class CEditMesh;
//---------------------------------------------------------------------------

class TfrmEditLibrary : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *ebCancel;
    TElTree *tvObjects;
    TExtBtn *ebSave;
    TPopupMenu *pmObjects;
    TMenuItem *miNewFolder;
    TMenuItem *miEditFolder;
    TMenuItem *miDeleteFolder;
    TMenuItem *miObjectProperties;
    TMenuItem *LoadObject1;
    TMenuItem *DeleteObject1;
    TExtBtn *ebValidate;
	TMenuItem *Folder1;
	TExtBtn *ebMakeThm;
	TExtBtn *ebUnload;
	TFormStorage *fsStorage;
	TExtBtn *ebLoadObject;
	TExtBtn *ebDeleteObject;
	TExtBtn *ebProperties;
	TExtBtn *ebSaveObjectOGF;
	TExtBtn *ebSaveObjectVCF;
	TExtBtn *ebSaveObjectSkeletonOGF;
	TExtBtn *ebReloadObject;
	TCheckBox *cbPreview;
	TExtBtn *ebSaveObjectDO;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebPropertiesClick(TObject *Sender);
    void __fastcall ebDeleteObjectClick(TObject *Sender);
    void __fastcall tvObjectsDblClick(TObject *Sender);
    void __fastcall ebLoadObjectClick(TObject *Sender);
    void __fastcall miNewFolderClick(TObject *Sender);
    void __fastcall miEditFolderClick(TObject *Sender);
    void __fastcall miDeleteFolderClick(TObject *Sender);
    void __fastcall tvObjectsDragDrop(TObject *Sender, TObject *Source,
          int X, int Y);
    void __fastcall tvObjectsDragOver(TObject *Sender, TObject *Source,
          int X, int Y, TDragState State, bool &Accept);
    void __fastcall tvObjectsStartDrag(TObject *Sender,
          TDragObject *&DragObject);
//    void __fastcall ebEditObjectClick(TObject *Sender);
    void __fastcall tvObjectsItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
//    void __fastcall ebApplyEditClick(TObject *Sender);
//    void __fastcall ebCancelEditClick(TObject *Sender);
    void __fastcall cbPreviewClick(TObject *Sender);
    void __fastcall ebValidateClick(TObject *Sender);
    void __fastcall tvObjectsTryEdit(TObject *Sender, TElTreeItem *Item,
          TElHeaderSection *Section, TFieldTypes &CellType, bool &CanEdit);
    void __fastcall tvObjectsItemChange(TObject *Sender, TElTreeItem *Item,
          TItemChangeMode ItemChangeMode);
    void __fastcall ebSaveObjectOGFClick(TObject *Sender);
    void __fastcall ebSaveObjectVCFClick(TObject *Sender);
	void __fastcall ebMakeThmClick(TObject *Sender);
	void __fastcall ebUnloadClick(TObject *Sender);
	void __fastcall pcItemTypeClick(TObject *Sender);
	void __fastcall ebSaveObjectSkeletonOGFClick(TObject *Sender);
	void __fastcall ebReloadObjectClick(TObject *Sender);
	void __fastcall tvObjectsKeyPress(TObject *Sender, char &Key);
	void __fastcall ebSaveObjectDOClick(TObject *Sender);
private:	// User declarations
    void InitObjectFolder();
    TElTreeItem* FindFolder(const char* s);
    TElTreeItem* FindObject(void *obj);
    TElTreeItem* AddFolder(const char* s);
    TElTreeItem* AddObject(TElTreeItem* node, const char* name, void* obj);
    TElTreeItem* FDragItem;
    TElTreeItem* FEditNode;
    void OnModified();
    CLibObject* m_SelectedObject;
//    CEditObject* m_SelectedObject;
//    CEditObject* m_EditObject;
//    CEditObject* m_SaveObject;
//    bool bEditObjectMode;
//    void EnableEditingMode(bool flag);
    void CloseEditLibrary(bool bReload);
public:		// User declarations
    __fastcall TfrmEditLibrary(TComponent* Owner);
    void FinalClose();
    void __fastcall EditLibrary();
    void __fastcall OnRender();
    void __fastcall OnIdle();
    void __fastcall ZoomObject();
	SceneObject *RayPick(const Fvector& start, const Fvector& direction, SRayPickInfo* pinf);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmEditLibrary *frmEditLibrary;
extern void frmEditLibraryEditLibrary();
//---------------------------------------------------------------------------
#endif
