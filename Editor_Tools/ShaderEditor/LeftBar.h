//---------------------------------------------------------------------------


#ifndef LeftBarH
#define LeftBarH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "ExtBtn.hpp"
#include <Menus.hpp>
#include "ExtBtn.hpp"
#include "MxMenus.hpp"
#include "mxPlacemnt.hpp"
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"

#include "ShaderTools.h"

//---------------------------------------------------------------------------
class TfraLeftBar : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paLeftBar;
    TPanel *paScene;
	TFormStorage *fsStorage;
	TLabel *APHeadLabel2;
	TExtBtn *ebSceneMin;
	TExtBtn *ebSceneFile;
	TExtBtn *ebSceneCommands;
	TExtBtn *ebPreferences;
	TMxPopupMenu *pmSceneFile;
	TMenuItem *Save1;
	TMenuItem *Reload1;
	TMxPopupMenu *pmPreviewObject;
	TMenuItem *Plane1;
	TPanel *paShaders;
	TLabel *Label1;
	TPanel *paShaderList;
	TElTree *tvShaders;
	TPanel *Panel1;
	TExtBtn *ebShaderCreate;
	TExtBtn *ebShaderRemove;
	TPanel *paAction;
	TExtBtn *ebShaderProperties;
	TExtBtn *ExtBtn3;
	TMenuItem *Box1;
	TMenuItem *Ball1;
	TMenuItem *Teapot1;
	TMenuItem *Custom1;
	TMenuItem *N3;
	TExtBtn *ebShaderClone;
	TExtBtn *ebShaderImport;
	TExtBtn *ebShaderExport;
	TMxPopupMenu *pmShaderList;
	TMenuItem *ExpandAll1;
	TMenuItem *CollapseAll1;
	TMenuItem *N1;
	TMenuItem *CreateFolder1;
	TExtBtn *ebApplyChanges;
	TMxPopupMenu *pmBlenderList;
	TBevel *Bevel1;
	TBevel *Bevel2;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall ebReloadClick(TObject *Sender);
    void __fastcall PanelMimimizeClick(TObject *Sender);
    void __fastcall PanelMaximizeClick(TObject *Sender);
    void __fastcall ebEditorPreferencesClick(TObject *Sender);
    void __fastcall ebRefreshTexturesClick(TObject *Sender);
	void __fastcall ebResetAnimationClick(TObject *Sender);
	void __fastcall ebSceneFileMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ebSceneCommandsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall tvShadersMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ebShaderPropertiesClick(TObject *Sender);
	void __fastcall PreviewClick(TObject *Sender);
	void __fastcall ebApplyChangesClick(TObject *Sender);
	void __fastcall ebShaderCreateMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall CreateFolder1Click(TObject *Sender);
	void __fastcall ExpandAll1Click(TObject *Sender);
	void __fastcall CollapseAll1Click(TObject *Sender);
private:	// User declarations
	void __fastcall TemplateClick(TObject *Sender);
	void __fastcall ShowPPMenu(TMxPopupMenu* M, TObject* btn);
	TElTreeItem* 	FindFolder(LPCSTR full_name);
	TElTreeItem* 	AppendFolder(LPCSTR full_name);
	void __fastcall MakeFolderName(TElTreeItem* select_item, AnsiString& folder);
	void __fastcall GenerateFolderName(TElTreeItem* node,AnsiString& name);
	TElTreeItem* 	FindFolderItem(TElTreeItem* start_item, const AnsiString& name);
public:		// User declarations
        __fastcall TfraLeftBar(TComponent* Owner);
	void 			ChangeTarget	(int tgt);
    void 			UpdateBar		();
    void 			InitPalette	(TemplateVec& lst);
    void 			FillBlenderTree(BlenderMap* blenders);
	void 			AddBlender(LPCSTR full_name);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraLeftBar *fraLeftBar;
//---------------------------------------------------------------------------
#endif
