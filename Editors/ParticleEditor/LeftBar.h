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
	TMenuItem *N2;
	TMenuItem *Import1;
	TMenuItem *Export1;
	TPanel *paPSList;
	TBevel *Bevel2;
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
	TExtBtn *ebEngineApplyChanges;
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
	TImageList *ilModeIcons;
	TMenuItem *ParticleGroup1;
	TPanel *paItemList;
	TSplitter *Splitter1;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall ebReloadClick(TObject *Sender);
    void __fastcall PanelMimimizeClick(TObject *Sender);
    void __fastcall PanelMaximizeClick(TObject *Sender);
    void __fastcall ebEditorPreferencesClick(TObject *Sender);
    void __fastcall ebRefreshTexturesClick(TObject *Sender);
	void __fastcall ebSceneCommandsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall PreviewClick(TObject *Sender);
	void __fastcall ebEngineApplyChangesClick(TObject *Sender);
	void __fastcall ebParticleCloneClick(TObject *Sender);
	void __fastcall ebPSCreateClick(TObject *Sender);
	void __fastcall ebImageCommandsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ImageEditor1Click(TObject *Sender);
	void __fastcall Refresh1Click(TObject *Sender);
	void __fastcall Checknewtextures1Click(TObject *Sender);
	void __fastcall ExtBtn3Click(TObject *Sender);
	void __fastcall ebPECreateClick(TObject *Sender);
	void __fastcall ebFileMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ebCreateMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall ebPGCreateClick(TObject *Sender);
	void __fastcall ebEngineShaderRemoveClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraLeftBar		(TComponent* Owner);
    void 			UpdateBar		();
	void 			ClearParticleList();
};
//---------------------------------------------------------------------------
extern PACKAGE TfraLeftBar *fraLeftBar;
//---------------------------------------------------------------------------
#endif
