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
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TStatusBar *sbStatus;
	TPanel *Panel1;
	TOpenDialog *od;
	TScrollBar *sbMem;
	TPanel *Panel2;
	TPanel *Panel3;
	TPanel *Panel4;
	TMxLabel *MxLabel2;
	TMxLabel *lbEndPtr;
	TMxLabel *MxLabel4;
	TMxLabel *lbBeginPtr;
	TMxLabel *MxLabel3;
	TMxLabel *lbMemSize;
	TRadioGroup *rgCellSize;
	TMxLabel *MxLabel5;
	TMxLabel *lbMaxBlock;
	TMxLabel *MxLabel7;
	TMxLabel *lbMinBlock;
	TPanel *Panel6;
	TPanel *paMemBase;
	TMxPanel *paMem;
	TPanel *Panel7;
	TListBox *lbDetDesc;
	TPanel *Panel8;
	TMxLabel *MxLabel1;
	TMxLabel *lbCurrentCell;
	TMainMenu *MainMenu1;
	TMenuItem *File1;
	TMenuItem *Tools1;
	TMenuItem *Help1;
	TMenuItem *N1;
	TMenuItem *Exit1;
	TMenuItem *Open1;
	TMenuItem *Close1;
	TMxPanel *paDetMem;
	void __fastcall OpenClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall paMemPaint(TObject *Sender);
	void __fastcall paMemMouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
	void __fastcall sbMemChange(TObject *Sender);
	void __fastcall rgCellSizeClick(TObject *Sender);
	void __fastcall paMemBaseResize(TObject *Sender);
	void __fastcall paMemMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall paDetMemPaint(TObject *Sender);
private:	// User declarations
	void ResizeImage	();
	void DrawImage		();
	void DrawDetImage	();
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
