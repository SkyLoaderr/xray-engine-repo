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

#include "CustomObject.h"
#include "Library.h"
#include <Dialogs.hpp>
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include "mxPlacemnt.hpp"
#include <Menus.hpp>
//---------------------------------------------------------------------------
// refs
class TObjectPreview;
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
	TExtBtn *ebReloadObject;
	TCheckBox *cbPreview;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebPropertiesClick(TObject *Sender);
    void __fastcall ebDeleteItemClick(TObject *Sender);
    void __fastcall tvObjectsDblClick(TObject *Sender);
    void __fastcall ebLoadObjectClick(TObject *Sender);
    void __fastcall miNewFolderClick(TObject *Sender);
    void __fastcall miEditFolderClick(TObject *Sender);
    void __fastcall tvObjectsDragDrop(TObject *Sender, TObject *Source,
          int X, int Y);
    void __fastcall tvObjectsDragOver(TObject *Sender, TObject *Source,
          int X, int Y, TDragState State, bool &Accept);
    void __fastcall tvObjectsStartDrag(TObject *Sender,
          TDragObject *&DragObject);
//    void __fastcall ebEditObjectClick(TObject *Sender);
//    void __fastcall ebApplyEditClick(TObject *Sender);
//    void __fastcall ebCancelEditClick(TObject *Sender);
    void __fastcall cbPreviewClick(TObject *Sender);
    void __fastcall ebValidateClick(TObject *Sender);
    void __fastcall tvObjectsItemChange(TObject *Sender, TElTreeItem *Item,
          TItemChangeMode ItemChangeMode);
	void __fastcall ebMakeThmClick(TObject *Sender);
	void __fastcall ebUnloadClick(TObject *Sender);
	void __fastcall pcItemTypeClick(TObject *Sender);
	void __fastcall ebReloadObjectClick(TObject *Sender);
	void __fastcall tvObjectsKeyPress(TObject *Sender, char &Key);
	void __fastcall tvObjectsItemFocused(TObject *Sender);
private:	// User declarations
    void InitObjectFolder();
    TElTreeItem* FDragItem;
    TElTreeItem* FEditNode;
    CLibObject* m_SelectedObject;
    bool CloseEditLibrary(bool bReload);
public:		// User declarations
    __fastcall TfrmEditLibrary(TComponent* Owner);
    bool FinalClose();
    void __fastcall EditLibrary();
    void __fastcall OnRender();
    void __fastcall ZoomObject();
    void __fastcall OnModified();
	CEditableObject *RayPick(const Fvector& start, const Fvector& direction, SRayPickInfo* pinf);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmEditLibrary *frmEditLibrary;
extern void frmEditLibraryEditLibrary();
//---------------------------------------------------------------------------
#endif
