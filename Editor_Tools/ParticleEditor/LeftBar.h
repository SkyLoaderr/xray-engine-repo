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
	TExtBtn *ExtBtn3;
	TMenuItem *Box1;
	TMenuItem *Ball1;
	TMenuItem *Teapot1;
	TMenuItem *Custom1;
	TMenuItem *N3;
	TMxPopupMenu *pmShaderList;
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
	TBevel *Bevel2;
	TPanel *Panel1;
	TExtBtn *ebEngineShaderRemove;
	TExtBtn *ebEngineShaderClone;
	TExtBtn *ebEngineShaderFile;
	TExtBtn *ebEngineShaderCreate;
	TElTree *tvEngine;
	TPanel *paAction;
	TExtBtn *ebEngineShaderProperties;
	TExtBtn *ebEngineApplyChanges;
	TElTabSheet *tsCompiler;
	TPanel *Panel4;
	TExtBtn *ebCompilerShaderRemove;
	TExtBtn *ebCompilerShaderClone;
	TExtBtn *ExtBtn4;
	TExtBtn *ebCShaderCreate;
	TPanel *Panel5;
	TExtBtn *ExtBtn6;
	TExtBtn *ExtBtn7;
	TBevel *Bevel4;
	TBevel *Bevel3;
	TElTree *tvCompiler;
	TElTreeInplaceAdvancedEdit *InplaceCompilerEdit;
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
	void __fastcall ebEngineShaderPropertiesClick(TObject *Sender);
	void __fastcall PreviewClick(TObject *Sender);
	void __fastcall ebEngineApplyChangesClick(TObject *Sender);
	void __fastcall ebEngineShaderCreateMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall CreateFolder1Click(TObject *Sender);
	void __fastcall ExpandAll1Click(TObject *Sender);
	void __fastcall CollapseAll1Click(TObject *Sender);
	void __fastcall tvViewDblClick(TObject *Sender);
	void __fastcall ebEngineShaderRemoveClick(TObject *Sender);
	void __fastcall tvEngineItemFocused(TObject *Sender);
	void __fastcall ebEngineShaderCloneClick(TObject *Sender);
	void __fastcall tvEngineKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall InplaceEngineEditValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall ebCShaderCreateClick(TObject *Sender);
	void __fastcall pcShadersChange(TObject *Sender);
	void __fastcall ebCompilerShaderRemoveClick(TObject *Sender);
	void __fastcall ebCompilerShaderCloneClick(TObject *Sender);
	void __fastcall tvEngineStartDrag(TObject *Sender,
          TDragObject *&DragObject);
	void __fastcall tvEngineDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall tvEngineDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
private:	// User declarations
	void __fastcall TemplateClick	(TObject *Sender);
	void __fastcall ShowPPMenu		(TMxPopupMenu* M, TObject* btn);
    TElTree*		CurrentView		(){ if (pcShaders->ActivePage==tsEngine) 		return tvEngine;
									    else if (pcShaders->ActivePage==tsCompiler)	return tvCompiler;
                                        THROW;
                                        return 0;
    								  }
    TElTreeItem*	DragItem;
public:		// User declarations
        __fastcall TfraLeftBar		(TComponent* Owner);
	void 			ChangeTarget	(int tgt);
    void 			UpdateBar		();
    void 			InitPalette		(TemplateVec& lst);
	void 			AddBlender		(LPCSTR full_name, bool bLoadMode);
	void 			AddCShader		(LPCSTR full_name, bool bLoadMode);
	void 			ClearEShaderList();
    void			ClearCShaderList();
};
//---------------------------------------------------------------------------
extern PACKAGE TfraLeftBar *fraLeftBar;
//---------------------------------------------------------------------------
#endif
