//---------------------------------------------------------------------------

#ifndef EditShadersH
#define EditShadersH
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
#include "CloseBtn.hpp"
#include "ElHeader.hpp"
#include "RXCtrls.hpp"
#include <Grids.hpp>
#include "RxMenus.hpp"
#include <Menus.hpp>
#include "Placemnt.hpp"
//---------------------------------------------------------------------------
// refs
struct SH_ShaderDef;
struct st_Surface;
class CEditObject;
class CLibObject;
//---------------------------------------------------------------------------

class TfrmEditShaders : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
	TElTree *tvShaders;
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
	TExtBtn *ebNewShader;
	TExtBtn *ebRemoveShader;
	TExtBtn *ebPropertiesShader;
	TCheckBox *cbPreview;
	TExtBtn *ebCloneShader;
	TMenuItem *N1;
	TRadioGroup *rgTestObject;
	TGroupBox *gbExtern;
	TExtBtn *ebExternSelect;
	TPanel *Panel2;
	TExtBtn *ebCancel;
	TExtBtn *ebSave;
	TExtBtn *ebReloadShaders;
	TExtBtn *ebMerge;
	TLabel *lbSelectObject;
	TGroupBox *GroupBox2;
	TExtBtn *ebAssignShader;
	TLabel *lbExternSelMat;
	TRxLabel *RxLabel18;
	TRxLabel *RxLabel1;
	TLabel *lbExternSelShader;
	TExtBtn *ebDropper;
	TExtBtn *ebSaveExternObject;
	TFormStorage *fsStorage;
	TExtBtn *ExtBtn1;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebPropertiesShaderClick(TObject *Sender);
    void __fastcall ebRemoveShaderClick(TObject *Sender);
    void __fastcall tvShadersDblClick(TObject *Sender);
    void __fastcall ebNewShaderClick(TObject *Sender);
    void __fastcall miNewFolderClick(TObject *Sender);
    void __fastcall miEditFolderClick(TObject *Sender);
    void __fastcall miDeleteFolderClick(TObject *Sender);
    void __fastcall tvShadersDragDrop(TObject *Sender, TObject *Source,
          int X, int Y);
    void __fastcall tvShadersDragOver(TObject *Sender, TObject *Source,
          int X, int Y, TDragState State, bool &Accept);
    void __fastcall tvShadersStartDrag(TObject *Sender,
          TDragObject *&DragObject);
    void __fastcall tvShadersItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
    void __fastcall cbPreviewClick(TObject *Sender);
    void __fastcall ebValidateClick(TObject *Sender);
    void __fastcall tvShadersTryEdit(TObject *Sender, TElTreeItem *Item,
          TElHeaderSection *Section, TFieldTypes &CellType, bool &CanEdit);
    void __fastcall tvShadersItemChange(TObject *Sender, TElTreeItem *Item,
          TItemChangeMode ItemChangeMode);
	void __fastcall tvShadersKeyPress(TObject *Sender, char &Key);
	void __fastcall ebReloadShadersClick(TObject *Sender);
	void __fastcall ebCloneShaderClick(TObject *Sender);
	void __fastcall rgTestObjectClick(TObject *Sender);
	void __fastcall ebMergeClick(TObject *Sender);
	void __fastcall ebExternSelectClick(TObject *Sender);
	void __fastcall ebAssignShaderClick(TObject *Sender);
	void __fastcall ebSaveExternObjectClick(TObject *Sender);
	void __fastcall ExtBtn1Click(TObject *Sender);
private:	// User declarations
	TElTreeItem* FindShader(const char* s);
    TElTreeItem* FindFolder(const char* s);
    TElTreeItem* AddFolder(const char* s);
    TElTreeItem* AddShader(TElTreeItem* node, const char* name);
    TElTreeItem* FDragItem;
    TElTreeItem* FEditNode;
    void OnModified();
    SH_ShaderDef* m_SelectedShader;

    Fvector init_cam_hpb;
    Fvector init_cam_pos;

    CLibObject* m_TestObject;
    st_Surface* m_TestExternSurface;
    void CloseEditShaders(bool bReload);
public:		// User declarations
    __fastcall TfrmEditShaders(TComponent* Owner);
    void InitShaderFolder(const char* nm=0);
    void __fastcall EditShaders();
    void FinalClose();
    void __fastcall OnRender();
    void __fastcall OnIdle();
	void __fastcall ZoomObject();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmEditShaders *frmEditShaders;
extern void frmEditShadersEditShaders();
//---------------------------------------------------------------------------
#endif
