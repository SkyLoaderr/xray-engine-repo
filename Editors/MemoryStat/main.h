//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtBtn.hpp>
#include <ElFolderDlg.hpp>
#include <Dialogs.hpp>
#include "MXCtrls.hpp"
#include <ExtCtrls.hpp>
#include "multi_edit.hpp"
#include <ComCtrls.hpp>
#include <Menus.hpp>
#include "mxPlacemnt.hpp"
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TStatusBar *sbStatus;
	TPanel *paDesc;
	TOpenDialog *od;
	TPanel *Panel6;
	TPanel *paMemBase;
	TMxPanel *paMem;
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *Tools1;
	TMenuItem *Help1;
	TMenuItem *N1;
	TMenuItem *Exit1;
	TMenuItem *Open1;
	TMenuItem *Close1;
	TMxPanel *paDetMem;
	TPanel *paInfo;
	TFormStorage *fsStorage;
	TScrollBar *sbMem;
	TPanel *Panel2;
	TPanel *Panel3;
	TPanel *Panel9;
	TPanel *Panel4;
	void __fastcall OpenClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall paMemPaint(TObject *Sender);
	void __fastcall paMemMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall sbMemChange(TObject *Sender);
	void __fastcall paMemBaseResize(TObject *Sender);
	void __fastcall paMemMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall paDetMemPaint(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
private:	// User declarations
	void __fastcall IdleHandler(TObject *Sender, bool &Done);
    void __stdcall OnRefreshBuffer	(PropValue*);
    void __stdcall OnCellChanged	(PropValue*);
    void __stdcall OnResizeBuffer	(PropValue*);
    void __stdcall OnRedrawBuffer	(PropValue*);

    enum{
    	flUpdateProps			= (1<<0),
    	flResizeBuffer			= (1<<1),
    	flRedrawBuffer			= (1<<2),
    	flRedrawDetailed		= (1<<3),
    };
    Flags32						m_Flags;
    
	void ResizeBuffer			(){m_Flags.set(flResizeBuffer,TRUE);}
	void RealResizeBuffer		();

	void RedrawBuffer			(){m_Flags.set(flRedrawBuffer,TRUE);}
	void RealRedrawBuffer		();

	void RedrawDetailed			(){m_Flags.set(flRedrawDetailed,TRUE);}
	void RealRedrawDetailed		();

    void UpdateProperties		(){m_Flags.set(flUpdateProps,TRUE);}
    void RealUpdateProperties	();
    
    void OnFrame				();
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
