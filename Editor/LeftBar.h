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
#include "CloseBtn.hpp"
#include "Placemnt.hpp"
#include "RxMenus.hpp"
#include <Menus.hpp>
//---------------------------------------------------------------------------
class TfraLeftBar : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paLeftBar;
    TPanel *paScene;
    TPanel *paEdit;
    TPanel *paTarget;
	TExtBtn *ebTargetObject;
	TExtBtn *ebTargetLight;
	TExtBtn *ebTargetSound;
    TExtBtn *ebTargetOccluder;
    TPanel *paFrames;
	TFormStorage *fsStorage;
    TExtBtn *ebTargetGlow;
	TExtBtn *ebTargetDPatch;
    TExtBtn *ebTargetRPoint;
	TExtBtn *ebTargetAITraffic;
	TExtBtn *ebTargetSector;
	TExtBtn *ebTargetPortal;
	TLabel *APHeadLabel2;
	TExtBtn *sbSceneMin;
	TLabel *lbTools;
	TLabel *lbEditMode;
	TExtBtn *sbEditMin;
	TExtBtn *sbTargetMin;
	TExtBtn *ebTargetEvent;
	TExtBtn *ebSceneFile;
	TExtBtn *ebSceneCompile;
	TExtBtn *ebLibraryEditor;
	TExtBtn *ebShaderEditor;
	TExtBtn *ebObjectList;
	TExtBtn *ebSceneCommands;
	TExtBtn *ebPreferences;
	TExtBtn *ExtBtn7;
	TExtBtn *ExtBtn8;
	TExtBtn *ExtBtn12;
	TExtBtn *ebIgnoreTarget;
	TExtBtn *ExtBtn9;
	TExtBtn *ExtBtn10;
	TExtBtn *ebRandomAdd;
	TExtBtn *ebIgnoreGroup;
	TRxPopupMenu *pmSceneFile;
	TMenuItem *Clear1;
	TMenuItem *Load1;
	TMenuItem *Save1;
	TMenuItem *SaveAs1;
	TRxPopupMenu *pmSceneCompile;
	TMenuItem *Options1;
	TMenuItem *Build1;
	TMenuItem *MakeLTX1;
	TMenuItem *Validate1;
	TRxPopupMenu *pmSceneCommands;
	TMenuItem *Refresh1;
	TMenuItem *ResetAniamation1;
	TMenuItem *CleanLibrary1;
	TRxPopupMenu *pmToolsEdit;
	TRxPopupMenu *pmToolsSelection;
	TRxPopupMenu *pmToolsVisibility;
	TRxPopupMenu *pmToolsLocking;
	TRxPopupMenu *pmToolsGrouping;
	TMenuItem *Cut1;
	TMenuItem *Copy1;
	TMenuItem *miPaste;
	TMenuItem *Undo1;
	TMenuItem *Redo1;
	TMenuItem *Delete1;
	TMenuItem *Invert1;
	TMenuItem *SelectAll1;
	TMenuItem *UnselectAll1;
	TMenuItem *HideAll1;
	TMenuItem *HideSelected1;
	TMenuItem *N1;
	TMenuItem *HideUnselected1;
	TMenuItem *UnhideAll1;
	TMenuItem *LockAll1;
	TMenuItem *LockSelection1;
	TMenuItem *LockUnselected1;
	TMenuItem *N2;
	TMenuItem *UnlockAll1;
	TMenuItem *UnlockSelected1;
	TMenuItem *UnlockUnselected1;
	TMenuItem *Create1;
	TMenuItem *Save2;
	TMenuItem *Destroy1;
	TMenuItem *DestroyAll1;
	TExtBtn *ebParticleEditor;
	TExtBtn *ebTargetPS;
	TExtBtn *ebImageEditor;
	TPanel *paSnapList;
	TLabel *Label1;
	TExtBtn *ExtBtn16;
	TExtBtn *ebEnableSnapList;
	TExtBtn *ExtBtn1;
	TRxPopupMenu *pmSnapListCommand;
	TMenuItem *MenuItem3;
	TMenuItem *MenuItem4;
	TListBox *lbSnapList;
	TBevel *Bevel1;
	TMenuItem *miAddSelectedToList;
	TExtBtn *ebTargetDO;
	TMenuItem *Checknewtextures1;
	TMenuItem *RefreshObjects1;
	TMenuItem *MakeDetails;
    void __fastcall ebClearClick(TObject *Sender);
    void __fastcall ebLoadClick(TObject *Sender);
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall ebSaveAsClick(TObject *Sender);
    void __fastcall ebBuildClick(TObject *Sender);
    void __fastcall ebOptionsClick(TObject *Sender);
    void __fastcall ebCutClick(TObject *Sender);
    void __fastcall ebCopyClick(TObject *Sender);
    void __fastcall ebPasteClick(TObject *Sender);
    void __fastcall ebUndoClick(TObject *Sender);
    void __fastcall ebRedoClick(TObject *Sender);
    void __fastcall ebValidateSceneClick(TObject *Sender);
    void __fastcall ebObjectListClick(TObject *Sender);
    void __fastcall ebEditLibClick(TObject *Sender);
    void __fastcall TargetClick(TObject *Sender);
    void __fastcall PanelMimimizeClickClick(TObject *Sender);
    void __fastcall PanelMaximizeClick(TObject *Sender);
    void __fastcall ebEditorPreferencesClick(TObject *Sender);
    void __fastcall ebRefreshEditorClick(TObject *Sender);
	void __fastcall ebInvertClick(TObject *Sender);
	void __fastcall ebSelectAllClick(TObject *Sender);
	void __fastcall ebUnselectAllClick(TObject *Sender);
	void __fastcall ebDeleteClick(TObject *Sender);
	void __fastcall sbPropertiesClick(TObject *Sender);
	void __fastcall ebHideAllClick(TObject *Sender);
	void __fastcall ebHideSelectedClick(TObject *Sender);
	void __fastcall ebUnhideAllClick(TObject *Sender);
	void __fastcall ebHideUnselectedClick(TObject *Sender);
	void __fastcall ebRandomAddClick(TObject *Sender);
	void __fastcall ebShaderEditorClick(TObject *Sender);
	void __fastcall ebLockAllClick(TObject *Sender);
	void __fastcall ebLockSelClick(TObject *Sender);
	void __fastcall ebLockUnselClick(TObject *Sender);
	void __fastcall ebUnlockAllClick(TObject *Sender);
	void __fastcall ebUnlockSelClick(TObject *Sender);
	void __fastcall ebUnlockUnselClick(TObject *Sender);
	void __fastcall ebGroupCreateClick(TObject *Sender);
	void __fastcall ebGroupDestroyClick(TObject *Sender);
	void __fastcall ebGroupSaveClick(TObject *Sender);
	void __fastcall ebResetAnimationClick(TObject *Sender);
	void __fastcall ebCleanLibraryClick(TObject *Sender);
	void __fastcall ebGroupDestroyAllClick(TObject *Sender);
	void __fastcall ebMakeLTXClick(TObject *Sender);
	void __fastcall ebSceneFileMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ExtBtn12MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ebSceneCompileMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ebSceneCommandsMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ExtBtn7MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ExtBtn8MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ExtBtn10MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ExtBtn9MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ebParticleEditorClick(TObject *Sender);
	void __fastcall ebImageEditorClick(TObject *Sender);
	void __fastcall ebClearSnapClick(TObject *Sender);
	void __fastcall ebSetSnapClick(TObject *Sender);
	void __fastcall ExtBtn1MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall miAddSelectedToListClick(TObject *Sender);
	void __fastcall ebEnableSnapListClick(TObject *Sender);
	void __fastcall RefreshObjects1Click(TObject *Sender);
	void __fastcall Checknewtextures1Click(TObject *Sender);
	void __fastcall MakeDetailsClick(TObject *Sender);
private:	// User declarations
	void __fastcall ShowPPMenu(TRxPopupMenu* M, TObject* btn);
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
