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
#include "MXCtrls.hpp"

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
	TPanel *paParticles;
	TLabel *Label1;
	TMenuItem *Custom1;
	TMxPopupMenu *pmShaderList;
	TMenuItem *ExpandAll1;
	TMenuItem *CollapseAll1;
	TMenuItem *N1;
	TMenuItem *CreateFolder1;
	TMenuItem *Rename1;
	TMenuItem *N2;
	TMenuItem *Import1;
	TMenuItem *Export1;
	TElTreeInplaceAdvancedEdit *InplaceParticleEdit;
	TPanel *paPSList;
	TBevel *Bevel2;
	TElTree *tvParticles;
	TPanel *Panel1;
	TExtBtn *ebEngineShaderRemove;
	TExtBtn *ebEngineShaderClone;
	TExtBtn *ebEngineShaderFile;
	TExtBtn *ebCShaderCreate;
	TBevel *Bevel1;
	TPanel *paProperties;
	TLabel *Label2;
	TExtBtn *ExtBtn1;
	TPanel *paPSProps;
	TPanel *paAction;
	TPanel *paCurrentPS;
	TLabel *Label3;
	TExtBtn *ExtBtn4;
	TExtBtn *ebEngineApplyChanges;
	TExtBtn *ebCurrentPSPlay;
	TExtBtn *ebCurrentPSStop;
	TMxLabel *lbCurState;
	TMxLabel *lbParticleCount;
	TMxLabel *RxLabel2;
	TMxLabel *RxLabel1;
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
	void __fastcall tvParticlesMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall PreviewClick(TObject *Sender);
	void __fastcall ebEngineApplyChangesClick(TObject *Sender);
	void __fastcall CreateFolder1Click(TObject *Sender);
	void __fastcall ExpandAll1Click(TObject *Sender);
	void __fastcall CollapseAll1Click(TObject *Sender);
	void __fastcall ebParticleShaderRemoveClick(TObject *Sender);
	void __fastcall tvParticlesItemFocused(TObject *Sender);
	void __fastcall ebParticleCloneClick(TObject *Sender);
	void __fastcall tvParticlesKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall InplaceParticleEditValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall ebPSCreateClick(TObject *Sender);
	void __fastcall tvParticlesStartDrag(TObject *Sender,
          TDragObject *&DragObject);
	void __fastcall tvParticlesDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall tvParticlesDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall ebCurrentPSPlayClick(TObject *Sender);
	void __fastcall ebCurrentPSStopClick(TObject *Sender);
	void __fastcall ebImageCommandsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ImageEditor1Click(TObject *Sender);
	void __fastcall Refresh1Click(TObject *Sender);
	void __fastcall Checknewtextures1Click(TObject *Sender);
private:	// User declarations
	void __fastcall ShowPPMenu		(TMxPopupMenu* M, TObject* btn);
    TElTreeItem*	DragItem;
public:		// User declarations
        __fastcall TfraLeftBar		(TComponent* Owner);
    void 			UpdateBar		();
	void 			AddPS			(LPCSTR full_name, bool bLoadMode);
	void 			ClearParticleList();
};
//---------------------------------------------------------------------------
extern PACKAGE TfraLeftBar *fraLeftBar;
//---------------------------------------------------------------------------
#endif
