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
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall ebImageCommandsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ImageEditor1Click(TObject *Sender);
	void __fastcall Refresh1Click(TObject *Sender);
	void __fastcall Checknewtextures1Click(TObject *Sender);
private:	// User declarations
	void __fastcall TemplateClick	(TObject *Sender);
    TElTree*		CurrentView		(){ if (pcShaders->ActivePage==tsEngine) 		return tvEngine;
									    else if (pcShaders->ActivePage==tsCompiler)	return tvCompiler;
                                        THROW;
                                        return 0;
    								  }
	void __fastcall RenameItem(LPCSTR p0, LPCSTR p1);
    bool			bFocusedAffected;
public:		// User declarations
        __fastcall TfraLeftBar		(TComponent* Owner);
	void 			ChangeTarget	(int tgt);
    void 			UpdateBar		();
    void 			InitPalette		(TemplateVec& lst);
	void 			AddBlender		(LPCSTR full_name);
	void 			AddCShader		(LPCSTR full_name);
    void			SetCurrentBlender(LPCSTR full_name);
    void			SetCurrentCShader(LPCSTR full_name);
	void 			ClearEShaderList();
    void			ClearCShaderList();
};
//---------------------------------------------------------------------------
extern PACKAGE TfraLeftBar *fraLeftBar;
//---------------------------------------------------------------------------
#endif
