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
	TMenuItem *miLoadMotions;
	TMenuItem *miSaveMotions;
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
	TExtBtn *ebMotionsRemove;
	TExtBtn *ebMotionsClear;
	TExtBtn *ebActorMotionsFile;
	TExtBtn *ebMotionsAppend;
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
	TExtBtn *ebCurrentPlay;
	TExtBtn *ebCurrentStop;
	TMxLabel *lbCurFrames;
	TMxLabel *lbCurFPS;
	TMxLabel *RxLabel2;
	TMxLabel *RxLabel1;
	TMenuItem *Load1;
	TMenuItem *Save2;
	TMenuItem *Export2;
	TMenuItem *SaevAs1;
	TMenuItem *N5;
	TBevel *Bevel4;
	TMenuItem *Import1;
	TSplitter *Splitter1;
	TBevel *Bevel3;
	TExtBtn *ebCurrentPause;
	TMenuItem *N2;
	TMenuItem *miRecentFiles;
	TExtBtn *ebBonePart;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall PanelMimimizeClick(TObject *Sender);
    void __fastcall PanelMaximizeClick(TObject *Sender);
    void __fastcall ebEditorPreferencesClick(TObject *Sender);
    void __fastcall ebRefreshTexturesClick(TObject *Sender);
	void __fastcall ebResetAnimationClick(TObject *Sender);
	void __fastcall ebActorMotionsFileMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ebSceneCommandsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall tvMotionsMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ebEngineApplyChangesClick(TObject *Sender);
	void __fastcall CreateFolder1Click(TObject *Sender);
	void __fastcall ExpandAll1Click(TObject *Sender);
	void __fastcall CollapseAll1Click(TObject *Sender);
	void __fastcall ebMotionsRemoveClick(TObject *Sender);
	void __fastcall tvMotionsItemFocused(TObject *Sender);
	void __fastcall tvMotionsKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall InplaceParticleEditValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall eMotionsAppendClick(TObject *Sender);
	void __fastcall tvMotionsStartDrag(TObject *Sender,
          TDragObject *&DragObject);
	void __fastcall tvMotionsDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall tvMotionsDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall ebCurrentPlayClick(TObject *Sender);
	void __fastcall ebCurrentStopClick(TObject *Sender);
	void __fastcall Import1Click(TObject *Sender);
	void __fastcall Load1Click(TObject *Sender);
	void __fastcall Save2Click(TObject *Sender);
	void __fastcall SaevAs1Click(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall ebCurrentPauseClick(TObject *Sender);
	void __fastcall miRecentFilesClick(TObject *Sender);
	void __fastcall ebBonePartClick(TObject *Sender);
	void __fastcall LoadClick(TObject *Sender);
	void __fastcall miSaveMotionsClick(TObject *Sender);
	void __fastcall ebMotionsClearClick(TObject *Sender);
	void __fastcall Export2Click(TObject *Sender);
private:	// User declarations
	void __fastcall ShowPPMenu		(TMxPopupMenu* M, TObject* btn);
    TElTreeItem*	DragItem;
public:		// User declarations
        __fastcall TfraLeftBar		(TComponent* Owner);
    void 			UpdateBar		();
	void 			AddMotion		(LPCSTR full_name, bool bLoadMode);
	void 			ClearMotionList	();
    void			UpdateMotionList();
    void			UpdateProperties();
    void			UpdateMotionProperties();
	void 			AppendRecentFile(LPCSTR name);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraLeftBar *fraLeftBar;
//---------------------------------------------------------------------------
#endif
