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
#include "ElXPThemedControl.hpp"
#include "ElTree.hpp"
#include "ElTreeAdvEdit.hpp"
#include "ElPgCtl.hpp"
#include "MXCtrls.hpp"

#include "UI_Tools.h"
#include <ImgList.hpp>

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
	TElTree *tvParticles___;
	TPanel *Panel1;
	TExtBtn *ebEngineShaderRemove;
	TExtBtn *ebEngineShaderClone;
	TExtBtn *ebFile;
	TBevel *Bevel1;
	TPanel *paProperties;
	TLabel *Label2;
	TExtBtn *ExtBtn1;
	TPanel *paItemProps;
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
	TExtBtn *ebResetState;
	TExtBtn *ebCreate;
	TMxPopupMenu *pmCreateMenu;
	TMenuItem *MenuItem1;
	TMenuItem *MenuItem2;
	TMenuItem *N3;
	TMenuItem *N4;
	TMenuItem *N5;
	TMenuItem *N7;
	TMenuItem *N8;
	TImageList *ilModeIcons;
	TMenuItem *ParticleGroup1;
	TPanel *paItemList;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall ebReloadClick(TObject *Sender);
    void __fastcall PanelMimimizeClick(TObject *Sender);
    void __fastcall PanelMaximizeClick(TObject *Sender);
    void __fastcall ebEditorPreferencesClick(TObject *Sender);
    void __fastcall ebRefreshTexturesClick(TObject *Sender);
	void __fastcall ebSceneCommandsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall tvParticles___MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall PreviewClick(TObject *Sender);
	void __fastcall ebEngineApplyChangesClick(TObject *Sender);
	void __fastcall CreateFolder1Click(TObject *Sender);
	void __fastcall ExpandAll1Click(TObject *Sender);
	void __fastcall CollapseAll1Click(TObject *Sender);
	void __fastcall ebParticleRemoveClick(TObject *Sender);
	void __fastcall ebParticleCloneClick(TObject *Sender);
	void __fastcall tvParticles___KeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall Rename1Click(TObject *Sender);
	void __fastcall InplaceParticleEditValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall ebPSCreateClick(TObject *Sender);
	void __fastcall ebCurrentPSPlayClick(TObject *Sender);
	void __fastcall ebCurrentPSStopClick(TObject *Sender);
	void __fastcall ebImageCommandsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ImageEditor1Click(TObject *Sender);
	void __fastcall Refresh1Click(TObject *Sender);
	void __fastcall Checknewtextures1Click(TObject *Sender);
	void __fastcall OnDragDrop(TObject *Sender, TObject *Source,
          int X, int Y);
	void __fastcall ExtBtn3Click(TObject *Sender);
	void __fastcall ebPECreateClick(TObject *Sender);
	void __fastcall tvParticles___DblClick(TObject *Sender);
	void __fastcall ebFileMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ebCreateMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall ebPGCreateClick(TObject *Sender);
private:	// User declarations
	void __fastcall ShowPPMenu		(TMxPopupMenu* M, TObject* btn);
	void __fastcall RenameItem(LPCSTR p0, LPCSTR p1);
	BOOL __fastcall RemoveItem(LPCSTR p0);
	void __fastcall AfterRemoveItem();
public:		// User declarations
        __fastcall TfraLeftBar		(TComponent* Owner);
    void 			UpdateBar		();
	void 			ClearParticleList();
};
//---------------------------------------------------------------------------
extern PACKAGE TfraLeftBar *fraLeftBar;
//---------------------------------------------------------------------------
#endif
