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
	TExtBtn *ebSceneCommands;
	TExtBtn *ebPreferences;
	TMxPopupMenu *pmEngineShadersFile;
	TMenuItem *Save1;
	TMenuItem *Reload1;
	TMxPopupMenu *pmPreviewObject;
	TMenuItem *Plane1;
	TPanel *paEngineShaders;
	TLabel *Label1;
	TMenuItem *Box1;
	TMenuItem *Ball1;
	TMenuItem *Teapot1;
	TMenuItem *Custom1;
	TMenuItem *N3;
	TMxPopupMenu *pmListCommand;
	TMenuItem *ExpandAll1;
	TMenuItem *CollapseAll1;
	TMenuItem *N1;
	TMenuItem *CreateFolder1;
	TMxPopupMenu *pmTemplateList;
	TMenuItem *Rename1;
	TElTreeInplaceAdvancedEdit *InplaceEngineEdit;
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
	TExtBtn *ebEngineShaderCreate;
	TElTree *tvEngine;
	TElTabSheet *tsCompiler;
	TPanel *Panel4;
	TExtBtn *ebCompilerShaderRemove;
	TExtBtn *ebCompilerShaderClone;
	TExtBtn *ExtBtn4;
	TExtBtn *ebCShaderCreate;
	TBevel *Bevel3;
	TElTreeInplaceAdvancedEdit *InplaceCompilerEdit;
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
	TElTreeInplaceAdvancedEdit *InplaceMaterialEdit;
	TElTabSheet *tsMaterialPair;
	TPanel *Panel2;
	TExtBtn *ebMaterialRemove;
	TExtBtn *ebMaterialClone;
	TExtBtn *ExtBtn3;
	TExtBtn *ebMaterialCreate;
	TBevel *Bevel2;
	TElTree *tvMaterial;
	TElTree *tvMaterialPair;
	TBevel *Bevel4;
	TPanel *Panel3;
	TExtBtn *ExtBtn1;
	TExtBtn *ebMaterialPairClone;
	TExtBtn *ExtBtn6;
	TExtBtn *ebMaterialPairCreate;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall ebReloadClick(TObject *Sender);
    void __fastcall PanelMimimizeClick(TObject *Sender);
    void __fastcall PanelMaximizeClick(TObject *Sender);
    void __fastcall ebEditorPreferencesClick(TObject *Sender);
    void __fastcall ebRefreshTexturesClick(TObject *Sender);
	void __fastcall ebResetAnimationClick(TObject *Sender);
	void __fastcall ebEngineShaderFileMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ebSceneCommandsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall tvEngineMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall PreviewClick(TObject *Sender);
	void __fastcall ebEngineShaderCreateMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall CreateFolder1Click(TObject *Sender);
	void __fastcall ExpandAll1Click(TObject *Sender);
	void __fastcall CollapseAll1Click(TObject *Sender);
	void __fastcall tvItemFocused(TObject *Sender);
	void __fastcall ebEngineShaderCloneClick(TObject *Sender);
	void __fastcall tvEngineKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall InplaceEditValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall ebCShaderCreateClick(TObject *Sender);
	void __fastcall pcShadersChange(TObject *Sender);
	void __fastcall ebShaderRemoveClick(TObject *Sender);
	void __fastcall ebCompilerShaderCloneClick(TObject *Sender);
	void __fastcall OnDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall ebImageCommandsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ImageEditor1Click(TObject *Sender);
	void __fastcall Refresh1Click(TObject *Sender);
	void __fastcall Checknewtextures1Click(TObject *Sender);
	void __fastcall ebMaterialCreateClick(TObject *Sender);
	void __fastcall ebMaterialCloneClick(TObject *Sender);
	void __fastcall ebMaterialPairModeClick(TObject *Sender);
	void __fastcall ebMaterialPairCreateClick(TObject *Sender);
	void __fastcall ebMaterialPairCloneClick(TObject *Sender);
private:	// User declarations
	void __fastcall TemplateClick	(TObject *Sender);
    TElTree*		CurrentView		(){ if (pcShaders->ActivePage==tsEngine) 		return tvEngine;
									    else if (pcShaders->ActivePage==tsCompiler)	return tvCompiler;
                                        else if (pcShaders->ActivePage==tsMaterial) return tvMaterial;
                                        else if (pcShaders->ActivePage==tsMaterialPair) return tvMaterialPair;
                                        THROW;
    								  }
	void __fastcall RenameItem(LPCSTR p0, LPCSTR p1);
	BOOL __fastcall RemoveItem(LPCSTR p0);
	void __fastcall AfterRemoveItem();
    bool			bFocusedAffected;
public:		// User declarations
        __fastcall TfraLeftBar		(TComponent* Owner);
	void 			ChangeTarget	(int tgt);
    void 			UpdateBar		();
    void 			InitPalette		(TemplateVec& lst);
// blender
	void 			AddBlender		(LPCSTR full_name);
    void			SetCurrentBlender(LPCSTR full_name);
	void 			ClearEShaderList();
// cshader
	void 			AddCShader		(LPCSTR full_name);
    void			SetCurrentCShader(LPCSTR full_name);
    void			ClearCShaderList();
// material
	void 			AddMaterial		(LPCSTR full_name);
    void			SetCurrentMaterial(LPCSTR full_name);
    void			ClearMaterialList();
	void 			AddMaterialPair	(LPCSTR full_name);
    void			SetCurrentMaterialPair(LPCSTR full_name);
    void			ClearMaterialPairList();
};
//---------------------------------------------------------------------------
extern PACKAGE TfraLeftBar *fraLeftBar;
//---------------------------------------------------------------------------
#endif
