//---------------------------------------------------------------------------

#ifndef OneStageH
#define OneStageH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "CSPIN.h"
#include "RXCtrls.hpp"
#include "RXSpin.hpp"

#include "SceneObject.h"
#include "CloseBtn.hpp"
#include "ElHeader.hpp"
#include <Grids.hpp>
#include "ElTree.hpp"
#include "RxMenus.hpp"
#include <Menus.hpp>

//refs
struct xr_token;
struct SH_StageDef;

class TfrmOneStage : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *gbStage;
	TLabel *Label5;
	TLabel *Label6;
	TGroupBox *GroupBox12;
	TExtBtn *ebTCM;
	TEdit *edVector;
	TRadioGroup *rbTCSource;
	TRadioGroup *rbTCAddressing;
	TCheckBox *cbCComplArg1;
	TCheckBox *cbCAReplArg1;
	TCheckBox *cbCComplArg2;
	TCheckBox *cbCAReplArg2;
	TCheckBox *cbAComplArg1;
	TCheckBox *cbAComplArg2;
	TStaticText *stAArg1;
	TStaticText *stAOp;
	TStaticText *stAArg2;
	TStaticText *stCArg1;
	TStaticText *stCOp;
	TStaticText *stCArg2;
	TStaticText *stTname;
	TStaticText *stTCSrcGeom;
	TRxPopupMenu *pmArgument;
	TRxPopupMenu *pmOperation;
	TRxPopupMenu *pmTname;
	TMenuItem *File1;
	TMenuItem *N1;
	TMenuItem *null1;
	TMenuItem *lmap1;
	TMenuItem *base01;
	TMenuItem *base02;
	TMenuItem *base03;
	TMenuItem *base04;
	TMenuItem *base05;
	TMenuItem *base06;
	TMenuItem *base07;
	TMenuItem *base08;
	TRxPopupMenu *pmTCSourceGeom;
	TMenuItem *MenuItem4;
	TMenuItem *MenuItem5;
	TMenuItem *MenuItem6;
	TMenuItem *MenuItem7;
	TMenuItem *MenuItem8;
	TMenuItem *MenuItem9;
	TMenuItem *MenuItem10;
	TMenuItem *MenuItem11;
	TMenuItem *MenuItem12;
	TLabel *Label1;
	void __fastcall ArgumentMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall OperationMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall stTCSrcGeomMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall stTnameMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall pmSetTextClick(TObject *Sender);
	void __fastcall PictureClick(TObject *Sender);
	void __fastcall UpdateClick(TObject *Sender);
	void __fastcall rbTCSourceClick(TObject *Sender);
	void __fastcall ebTCMClick(TObject *Sender);
private:	// User declarations
	void FillMenuFromToken(TRxPopupMenu* menu, const xr_token *token_list );
	void ShowMenu(TRxPopupMenu);
	void CheckTCM();
public:
	SH_StageDef* m_CurStage;
    bool bLoadMode;
    void UpdateStageData();
public:		// User declarations
    __fastcall TfrmOneStage(TComponent* Owner);
    void ShowStage(SH_StageDef* stage);
    void HideStage();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmOneStage *frmOneStage;
//---------------------------------------------------------------------------
extern xr_token argument_token[];
extern xr_token operation_token[];
extern xr_token	blend_token[];
extern AnsiString&  GetTokenNameFromVal_ARG (DWORD val, AnsiString& res, const xr_token *token_list);
extern AnsiString&  GetTokenNameFromVal_AND(DWORD val, AnsiString& res, const xr_token *token_list);
extern AnsiString&  GetTokenNameFromVal_EQ(DWORD val, AnsiString& res, const xr_token *token_list);
extern DWORD GetTokenValFromName(AnsiString& res, const xr_token *token_list);
//---------------------------------------------------------------------------
#endif
