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

#include "UI_Tools.h"
#include "ElTreeAdvEdit.hpp"
#include "ElPgCtl.hpp"

//---------------------------------------------------------------------------
class TfraLeftBar : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paLeftBar;
    TPanel *paScene;
	TFormStorage *fsStorage;
	TLabel *APHeadLabel2;
	TExtBtn *ebSceneMin;
	TExtBtn *ebPreferences;
	TMxPopupMenu *pmEngineShadersFile;
	TMenuItem *Save1;
	TMenuItem *Reload1;
	TPanel *paEngineShaders;
	TLabel *Label1;
	TMxPopupMenu *pmListCommand;
	TMenuItem *ExpandAll1;
	TMenuItem *CollapseAll1;
	TMenuItem *N1;
	TMenuItem *CreateFolder1;
	TMenuItem *Rename1;
	TMenuItem *N2;
	TMenuItem *Import1;
	TMenuItem *Export1;
	TElPageControl *pcShaders;
	TElTabSheet *tsEngine;
	TBevel *Bevel1;
	TPanel *Panel1;
	TExtBtn *ebEngineShaderRemove;
	TExtBtn *ebEngineShaderClone;
	TExtBtn *ebEngineShaderFile;
	TElTree *tvEngine;
	TElTabSheet *tsCompiler;
	TPanel *Panel4;
	TExtBtn *ebCompilerShaderRemove;
	TExtBtn *ebCompilerShaderClone;
	TExtBtn *ExtBtn4;
	TExtBtn *ebCShaderCreate;
	TBevel *Bevel3;
	TElTreeInplaceAdvancedEdit *InplaceEdit;
	TElTree *tvCompiler;
	TPanel *paShaderProperties;
	TLabel *Label6;
	TExtBtn *ExtBtn5;
	TBevel *Bevel6;
	TPanel *paShaderProps;
	TSplitter *Splitter1;
	TMenuItem *N4;
	TExtBtn *ebImageCommands;
	TMxPopupMenu *pmImages;
	TMenuItem *ImageEditor1;
	TMenuItem *N6;
	TMenuItem *Refresh1;
	TMenuItem *Checknewtextures1;
	TElTabSheet *tsMaterial;
	TElTabSheet *tsMaterialPair;
	TPanel *Panel2;
	TExtBtn *ebMaterialRemove;
	TExtBtn *ebMaterialClone;
	TExtBtn *ExtBtn3;
	TExtBtn *ebMaterialCreate;
	TBevel *Bevel2;
	TElTree *tvMtl;
	TElTree *tvMtlPair;
	TBevel *Bevel4;
	TPanel *Panel3;
	TExtBtn *ExtBtn1;
	TExtBtn *ebMaterialPairClone;
	TExtBtn *ExtBtn6;
	TExtBtn *ebMaterialPairCreate;
	TElTabSheet *tsSoundEnv;
	TBevel *Bevel5;
	TPanel *Panel5;
	TExtBtn *ExtBtn2;
	TExtBtn *ExtBtn7;
	TExtBtn *ExtBtn8;
	TExtBtn *ebCreateSoundEnv;
	TElTree *tvSoundEnv;
	TExtBtn *ebEngineShaderCreate;
	TMenuItem *N7;
	TMenuItem *miEnginePreviewObject;
	TMenuItem *Plane2;
	TMenuItem *Box2;
	TMenuItem *Ball2;
	TMenuItem *Teapot2;
	TMenuItem *N8;
	TMenuItem *Custom2;
	TMenuItem *N9;
	TMenuItem *Clear2;
	TMxPopupMenu *pmCustomFile;
	TMenuItem *MenuItem1;
	TMenuItem *MenuItem2;
	TMenuItem *MenuItem3;
	TMenuItem *MenuItem14;
	TMenuItem *MenuItem15;
	TPanel *paPreview;
	TLabel *Label2;
	TExtBtn *ExtBtn9;
	TBevel *Bevel7;
	TPanel *paPreviewProps;
	TSplitter *Splitter2;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall ebReloadClick(TObject *Sender);
    void __fastcall PanelMimimizeClick(TObject *Sender);
    void __fastcall PanelMaximizeClick(TObject *Sender);
    void __fastcall ebEditorPreferencesClick(TObject *Sender);
    void __fastcall ebRefreshTexturesClick(TObject *Sender);
	void __fastcall ebResetAnimationClick(TObject *Sender);
	void __fastcall ebCustomFileMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall tvEngineMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall CreateFolder1Click(TObject *Sender);
	void __fastcall ExpandAll1Click(TObject *Sender);
	void __fastcall CollapseAll1Click(TObject *Sender);
	void __fastcall tvItemFocused(TObject *Sender);
	void __fastcall tvEngineKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall InplaceEditValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall ebCreateItemClick(TObject *Sender);
	void __fastcall pcShadersChange(TObject *Sender);
	void __fastcall ebRemoveItemClick(TObject *Sender);
	void __fastcall ebCloneItemClick(TObject *Sender);
	void __fastcall OnDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall ebImageCommandsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ImageEditor1Click(TObject *Sender);
	void __fastcall Refresh1Click(TObject *Sender);
	void __fastcall Checknewtextures1Click(TObject *Sender);
	void __fastcall ebEngineFileMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall PreviewObjClick(TObject *Sender);
private:	// User declarations
	void __fastcall RenameItem			(LPCSTR p0, LPCSTR p1);
	BOOL __fastcall RemoveItem			(LPCSTR p0);
	void __fastcall AfterRemoveItem		();
    bool			bFocusedAffected;
public:		// User declarations
        __fastcall TfraLeftBar			(TComponent* Owner);
	void 			ChangeTarget		(int tgt);
    void 			UpdateBar			();
};
//---------------------------------------------------------------------------
extern PACKAGE TfraLeftBar *fraLeftBar;
//---------------------------------------------------------------------------
#endif
