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
#include "FrameProperties.h"
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
//---------------------------------------------------------------------------
class TfraLeftBar : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paLeftBar;
    TPanel *paScene;
    TPanel *paFrames;
	TFormStorage *fsStorage;
	TLabel *APHeadLabel2;
	TExtBtn *ebSceneMin;
	TExtBtn *ebSceneFile;
	TExtBtn *ebSceneCommands;
	TExtBtn *ebPreferences;
	TMxPopupMenu *pmSceneFile;
	TMenuItem *Clear1;
	TMenuItem *Load1;
	TMenuItem *Save1;
	TMenuItem *SaveAs1;
	TMxPopupMenu *pmSceneCommands;
	TMenuItem *Refresh1;
	TMenuItem *ResetAniamation1;
	TPanel *paShaders;
	TLabel *Label1;
	TExtBtn *ebShaderList;
	TPanel *paShaderList;
	TElTree *ElTree1;
	TPanel *Panel1;
	TExtBtn *ExtBtn1;
	TExtBtn *ExtBtn2;
	TPanel *Panel2;
	TExtBtn *ExtBtn3;
	TExtBtn *ExtBtn4;
    void __fastcall ebClearClick(TObject *Sender);
    void __fastcall ebLoadClick(TObject *Sender);
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall ebSaveAsClick(TObject *Sender);
    void __fastcall PanelMimimizeClickClick(TObject *Sender);
    void __fastcall PanelMaximizeClick(TObject *Sender);
    void __fastcall ebEditorPreferencesClick(TObject *Sender);
    void __fastcall ebRefreshEditorClick(TObject *Sender);
	void __fastcall sbPropertiesClick(TObject *Sender);
	void __fastcall ebResetAnimationClick(TObject *Sender);
	void __fastcall ebSceneFileMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ebSceneCommandsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
private:	// User declarations
	void __fastcall ShowPPMenu(TMxPopupMenu* M, TObject* btn);
public:		// User declarations
        __fastcall TfraLeftBar(TComponent* Owner);
	void ChangeTarget(int tgt);
    void UpdateBar();
    void UpdateSnapList();
};
//---------------------------------------------------------------------------
extern PACKAGE TfraLeftBar *fraLeftBar;
//---------------------------------------------------------------------------
#endif
