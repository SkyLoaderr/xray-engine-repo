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
#include "MxMenus.hpp"
#include "mxPlacemnt.hpp"
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
	TExtBtn *ebTargetEvent;
    TPanel *paFrames;
	TFormStorage *fsStorage;
    TExtBtn *ebTargetGlow;
    TExtBtn *ebTargetRPoint;
	TExtBtn *ebTargetWay;
	TExtBtn *ebTargetSector;
	TExtBtn *ebTargetPortal;
	TLabel *APHeadLabel2;
	TExtBtn *sbSceneMin;
	TLabel *lbTools;
	TLabel *lbEditMode;
	TExtBtn *sbEditMin;
	TExtBtn *sbTargetMin;
	TExtBtn *ebTargetGroup;
	TExtBtn *ebSceneFile;
	TExtBtn *ebSceneCompile;
	TExtBtn *ebLibraryEditor;
	TExtBtn *ebObjectList;
	TExtBtn *ebSceneCommands;
	TExtBtn *ebPreferences;
	TExtBtn *ExtBtn7;
	TExtBtn *ExtBtn8;
	TExtBtn *ExtBtn9;
	TExtBtn *ExtBtn10;
	TExtBtn *ebRandomAdd;
	TMxPopupMenu *pmSceneFile;
	TMenuItem *Clear1;
	TMenuItem *miOpen;
	TMenuItem *Save1;
	TMenuItem *SaveAs1;
	TMxPopupMenu *pmSceneCompile;
	TMenuItem *Options1;
	TMenuItem *Build1;
	TMenuItem *MakeLTX1;
	TMenuItem *Validate1;
	TMxPopupMenu *pmSceneCommands;
	TMenuItem *ResetAniamation1;
	TMxPopupMenu *pmToolsEdit;
	TMxPopupMenu *pmToolsSelection;
	TMxPopupMenu *pmToolsVisibility;
	TMxPopupMenu *pmToolsLocking;
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
	TExtBtn *ebTargetPS;
	TPanel *paSnapList;
	TLabel *Label1;
	TExtBtn *ExtBtn16;
	TExtBtn *ebEnableSnapList;
	TExtBtn *ExtBtn1;
	TMxPopupMenu *pmSnapListCommand;
	TMenuItem *MenuItem3;
	TMenuItem *MenuItem4;
	TListBox *lbSnapList;
	TBevel *Bevel1;
	TMenuItem *miAddSelectedToList;
	TExtBtn *ebTargetDO;
	TMenuItem *RefreshObjects1;
	TMenuItem *MakeDetails;
	TMxPopupMenu *pmObjectContext;
	TMenuItem *miVisibility;
	TMenuItem *HideSelected2;
	TMenuItem *HideUnselected2;
	TMenuItem *HideAll2;
	TMenuItem *N5;
	TMenuItem *UnhideAll2;
	TMenuItem *Locking1;
	TMenuItem *LockSelected1;
	TMenuItem *MenuItem1;
	TMenuItem *MenuItem2;
	TMenuItem *N6;
	TMenuItem *MenuItem5;
	TMenuItem *MenuItem6;
	TMenuItem *MenuItem7;
	TMenuItem *Edit1;
	TMenuItem *miCopy;
	TMenuItem *miPaste2;
	TMenuItem *miCut;
	TMenuItem *Numeric1;
	TMenuItem *Position1;
	TMenuItem *Rotation1;
	TMenuItem *Scale1;
	TMenuItem *N4;
	TMenuItem *miProperties;
	TMenuItem *RefreshLibrary1;
	TMenuItem *N3;
	TMenuItem *miRecentFiles;
	TMenuItem *N7;
	TMenuItem *Quit1;
	TExtBtn *ebProperties;
	TExtBtn *ebLightAnimationEditor;
	TExtBtn *ebIgnoreMode;
	TExtBtn *ebImages;
	TMxPopupMenu *pmImages;
	TMenuItem *ImageEditor1;
	TMenuItem *MenuItem8;
	TMenuItem *MenuItem9;
	TMenuItem *MenuItem10;
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
	void __fastcall ebLockAllClick(TObject *Sender);
	void __fastcall ebLockSelClick(TObject *Sender);
	void __fastcall ebLockUnselClick(TObject *Sender);
	void __fastcall ebUnlockAllClick(TObject *Sender);
	void __fastcall ebUnlockSelClick(TObject *Sender);
	void __fastcall ebUnlockUnselClick(TObject *Sender);
	void __fastcall ebResetAnimationClick(TObject *Sender);
	void __fastcall ebMakeLTXClick(TObject *Sender);
	void __fastcall ebSceneFileMouseDown(TObject *Sender, TMouseButton Button,
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
	void __fastcall ebClearSnapClick(TObject *Sender);
	void __fastcall ebSetSnapClick(TObject *Sender);
	void __fastcall ExtBtn1MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall miAddSelectedToListClick(TObject *Sender);
	void __fastcall ebEnableSnapListClick(TObject *Sender);
	void __fastcall RefreshObjects1Click(TObject *Sender);
	void __fastcall Checknewtextures1Click(TObject *Sender);
	void __fastcall MakeDetailsClick(TObject *Sender);
	void __fastcall Position1Click(TObject *Sender);
	void __fastcall Rotation1Click(TObject *Sender);
	void __fastcall Scale1Click(TObject *Sender);
	void __fastcall miPropertiesClick(TObject *Sender);
	void __fastcall RefreshLibrary1Click(TObject *Sender);
	void __fastcall miRecentFilesClick(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall Quit1Click(TObject *Sender);
	void __fastcall ebPropertiesClick(TObject *Sender);
	void __fastcall ebLightAnimationEditorClick(TObject *Sender);
	void __fastcall ebImagesMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ImageEditor1Click(TObject *Sender);
	void __fastcall MenuItem9Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraLeftBar(TComponent* Owner);
	void ChangeTarget(int tgt);
    void UpdateBar();
    void UpdateSnapList();
    void AppendRecentFile(LPCSTR name);
    LPCSTR FirstRecentFile();
};
//---------------------------------------------------------------------------
extern PACKAGE TfraLeftBar *fraLeftBar;
//---------------------------------------------------------------------------
#endif
