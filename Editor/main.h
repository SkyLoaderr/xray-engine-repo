//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include "RXCtrls.hpp"
#include "CloseBtn.hpp"
#include "RxMenus.hpp"
#include "Placemnt.hpp"
#include <ImgList.hpp>
#include <ToolWin.hpp>
#include "SceneClassList.h"
#include "RenderWindow.hpp"
//---------------------------------------------------------------------------
//class TUI;
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
        TPanel *paLeftBar;
        TPanel *paBottomBar;
    TPanel *paTools;
    TTimer *tmRefresh;
    TRxPopupMenu *pmObjectContext;
    TMenuItem *miProperties;
    TMenuItem *miCopy;
    TMenuItem *miPaste;
    TMenuItem *miCut;
    TFormStorage *fsMainForm;
    TPanel *paMain;
    TPanel *paTopBar;
	TMenuItem *miVisibility;
	TPanel *paWindow;
	TD3DWindow *D3DWindow;
	TLabel *APHeadLabel2;
	TExtBtn *sbToolsMin;
	TMenuItem *Numeric1;
	TMenuItem *Position1;
	TMenuItem *Rotation1;
	TMenuItem *Scale1;
	TMenuItem *N4;
	TMenuItem *HideAll2;
	TMenuItem *HideSelected2;
	TMenuItem *HideUnselected2;
	TMenuItem *N5;
	TMenuItem *UnhideAll2;
	TMenuItem *Locking1;
	TMenuItem *LockAll1;
	TMenuItem *LockSelected1;
	TMenuItem *LockUnselected1;
	TMenuItem *N6;
	TMenuItem *UnlockAll1;
	TMenuItem *UnlockSelected1;
	TMenuItem *UnlockUnselected1;
	TMenuItem *Edit1;
        void __fastcall FormCreate(TObject *Sender);
    void __fastcall D3DWindowResize(TObject *Sender);
    void __fastcall D3DWindowMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
    void __fastcall D3DWindowMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall D3DWindowKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall D3DWindowMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall D3DWindowKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall sbToolsMinClick(TObject *Sender);
    void __fastcall TopClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall tmRefreshTimer(TObject *Sender);
    void __fastcall D3DWindowPaint(TObject *Sender);
    void __fastcall D3DWindowKeyPress(TObject *Sender, char &Key);
    void __fastcall miPropertiesClick(TObject *Sender);
    void __fastcall miHideSelectedClick(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall fsMainFormRestorePlacement(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall miUnhideAllClick(TObject *Sender);
	void __fastcall miHideAllClick(TObject *Sender);
	void __fastcall paWindowResize(TObject *Sender);
	void __fastcall D3DWindowChangeFocus(TObject *Sender);
	void __fastcall miCopyClick(TObject *Sender);
	void __fastcall miPasteClick(TObject *Sender);
	void __fastcall miCutClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall Position1Click(TObject *Sender);
	void __fastcall Rotation1Click(TObject *Sender);
	void __fastcall Scale1Click(TObject *Sender);
	void __fastcall HideUnselected2Click(TObject *Sender);
	void __fastcall LockAll1Click(TObject *Sender);
	void __fastcall LockUnselected1Click(TObject *Sender);
	void __fastcall UnlockAll1Click(TObject *Sender);
	void __fastcall UnlockUnselected1Click(TObject *Sender);
	void __fastcall UnlockSelected1Click(TObject *Sender);
	void __fastcall LockSelected1Click(TObject *Sender);
private:	// User declarations
    void __fastcall IdleHandler(TObject *Sender, bool &Done);

    TShiftState ShiftMouse;
    TShiftState ShiftKey;
    HINSTANCE m_HInstance;

    void __fastcall ApplyShortCut(WORD Key, TShiftState Shift);
public:		// User declarations
    __fastcall TfrmMain(TComponent* Owner);
    void __fastcall UpdateCaption();
	void __fastcall ShowContextMenu(const EObjClass cls);
    __inline void SetHInst(HINSTANCE inst){ m_HInstance=inst; }
    void __fastcall ApplyGlobalShortCut(WORD Key, TShiftState Shift);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;

void ResetActionToSelect();
//---------------------------------------------------------------------------
#endif
