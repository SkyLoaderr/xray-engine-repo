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
#include "ElScrollBar.hpp"
#include "ElXPThemedControl.hpp"
#include "ElTree.hpp"
#include "ElTreeGrids.hpp"
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
	TStatusBar *sbStatus;
	TPanel *paMainDesc;
	TOpenDialog *od;
	TPanel *paMainMem;
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
	TPanel *paInfo;
	TFormStorage *fsStorage;
	TPanel *paCaptionDesc;
	TPanel *paCaptionMem;
	TScrollBar *sbMem;
	TBevel *Bevel2;
	TBevel *Bevel4;
	TBevel *Bevel1;
	TPanel *paMainDet;
	TBevel *Bevel6;
	TBevel *Bevel7;
	TPanel *paCaptionDet;
	TMxPanel *paDetMem;
	TBevel *Bevel3;
	TMenuItem *N2;
	TMenuItem *ShowLog1;
	TMenuItem *Bytype1;
	TMenuItem *Bypool1;
	TMenuItem *N3;
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
	void __fastcall paDetMemMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall ShowLog1Click(TObject *Sender);
	void __fastcall Bytype1Click(TObject *Sender);
	void __fastcall Bypool1Click(TObject *Sender);
private:	// User declarations
	void __fastcall IdleHandler(TObject *Sender, bool &Done);
    void __stdcall OnRefreshBuffer	(PropValue*);
    void __stdcall OnCellSizeChanged(PropValue*);
	void __stdcall OnCategoryCommandClick(ButtonValue*, bool&, bool&);
	void __stdcall OnCommonCommandClick(ButtonValue*, bool&, bool&);
    
    enum{
    	flUpdateProps			= (1<<0),
    	flResizeBuffer			= (1<<1),
    	flRedrawBuffer			= (1<<2),
    	flRedrawDetailed		= (1<<3),
    	flUpdateCatProps		= (1<<4),
    };
    Flags32						m_Flags;
    
	void ResizeBuffer			(){m_Flags.set(flResizeBuffer,TRUE);}
	void RealResizeBuffer		();

	void RedrawBuffer			(bool forced=false){m_Flags.set(flRedrawBuffer,TRUE);if (forced) RealRedrawBuffer();}
	void RealRedrawBuffer		();

	void RedrawDetailed			(){m_Flags.set(flRedrawDetailed,TRUE);}
	void RealRedrawDetailed		();

    void UpdateProperties		(){m_Flags.set(flUpdateProps,TRUE);}
    void RealUpdateProperties	();

    void UpdateCatProperties	(){m_Flags.set(flUpdateCatProps,TRUE);}
    void RealUpdateCatProperties();
    
    void OnFrame				();

    void ShowLog				(bool val);
public:		// User declarations
	__fastcall TfrmMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
