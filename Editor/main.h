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
#include "RenderWindow.hpp"
#include "ExtBtn.hpp"
#include "MxMenus.hpp"
#include "mxPlacemnt.hpp"
//---------------------------------------------------------------------------
// refs

class TfrmMain : public TForm
{
__published:	// IDE-managed Components
        TPanel *paLeftBar;
        TPanel *paBottomBar;
    TPanel *paTools;
    TTimer *tmRefresh;
    TFormStorage *fsMainForm;
    TPanel *paMain;
    TPanel *paTopBar;
	TPanel *paWindow;
	TD3DWindow *D3DWindow;
	TLabel *APHeadLabel2;
	TExtBtn *sbToolsMin;
        void __fastcall FormCreate(TObject *Sender);
    void __fastcall D3DWindowResize(TObject *Sender);
    void __fastcall D3DWindowKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall D3DWindowKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall sbToolsMinClick(TObject *Sender);
    void __fastcall TopClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall tmRefreshTimer(TObject *Sender);
    void __fastcall D3DWindowPaint(TObject *Sender);
    void __fastcall D3DWindowKeyPress(TObject *Sender, char &Key);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall fsMainFormRestorePlacement(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall paWindowResize(TObject *Sender);
	void __fastcall D3DWindowChangeFocus(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormDeactivate(TObject *Sender);
private:	// User declarations
    void __fastcall IdleHandler(TObject *Sender, bool &Done);

	TShiftState	ShiftKey;
    HINSTANCE 	m_HInstance;

public:		// User declarations
    __fastcall TfrmMain(TComponent* Owner);
    void __fastcall UpdateCaption();
    __inline void SetHInst(HINSTANCE inst){ m_HInstance=inst; }
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
