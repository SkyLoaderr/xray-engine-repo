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
	TExtBtn *ExtBtn3;
	TMxPopupMenu *pmShaderList;
	TMenuItem *ExpandAll1;
	TMenuItem *CollapseAll1;
	TMenuItem *N1;
	TMenuItem *CreateFolder1;
	TMenuItem *Rename1;
	TElTreeInplaceAdvancedEdit *InplaceParticleEdit;
	TPanel *paPSList;
	TBevel *Bevel2;
	TElTree *tvMotions;
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
	TMxLabel *lbCurFrames;
	TMxLabel *lbCurFPS;
	TMxLabel *RxLabel2;
	TMxLabel *RxLabel1;
	TMenuItem *Load1;
	TMenuItem *Save2;
	TMenuItem *Reload2;
	TMenuItem *Export2;
	TMenuItem *SaevAs1;
	TMenuItem *N5;
	TBevel *Bevel4;
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
	void __fastcall tvMotionsMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ebEngineApplyChangesClick(TObject *Sender);
	void __fastcall CreateFolder1Click(TObject *Sender);
	void __fastcall ExpandAll1Click(TObject *Sender);
	void __fastcall CollapseAll1Click(TObject *Sender);
	void __fastcall ebParticleShaderRemoveClick(TObject *Sender);
	void __fastcall tvMotionsItemFocused(TObject *Sender);
	void __fastcall ebParticleCloneClick(TObject *Sender);
	void __fastcall tvMotionsKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall InplaceParticleEditValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall ebPSCreateClick(TObject *Sender);
	void __fastcall tvMotionsStartDrag(TObject *Sender,
          TDragObject *&DragObject);
	void __fastcall tvMotionsDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall tvMotionsDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall ebCurrentPSPlayClick(TObject *Sender);
	void __fastcall ebCurrentPSStopClick(TObject *Sender);
private:	// User declarations
	void __fastcall ShowPPMenu		(TMxPopupMenu* M, TObject* btn);
    TElTreeItem*	DragItem;
public:		// User declarations
        __fastcall TfraLeftBar		(TComponent* Owner);
    void 			UpdateBar		();
	void 			AddMotion		(LPCSTR full_name, bool bLoadMode);
	void 			ClearMotionList	();
};
//---------------------------------------------------------------------------
extern PACKAGE TfraLeftBar *fraLeftBar;
//---------------------------------------------------------------------------
#endif
