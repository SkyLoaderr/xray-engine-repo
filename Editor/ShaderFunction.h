//---------------------------------------------------------------------------

#ifndef ShaderFunctionH
#define ShaderFunctionH
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
#include "multi_edit.hpp"

// refs
struct xr_token;
struct SH_Function;

class TfrmShaderFunction : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
	TStaticText *stFunction;
	TRxPopupMenu *pmFunction;
	TRxLabel *RxLabel1;
	TLabel *Label4;
	TMultiObjSpinEdit *seArg1;
	TLabel *Label1;
	TMultiObjSpinEdit *seArg2;
	TLabel *Label2;
	TMultiObjSpinEdit *seArg3;
	TLabel *Label3;
	TMultiObjSpinEdit *seArg4;
	TBevel *Bevel2;
	TImage *imDraw;
	TLabel *Label5;
	TLabel *lbMax;
	TLabel *lbCenter;
	TLabel *lbMin;
	TLabel *Label6;
	TLabel *Label7;
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
	void __fastcall stFunctionMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall stFunctionClick(TObject *Sender);
	void __fastcall seArgExit(TObject *Sender);
	void __fastcall seArgLWChange(TObject *Sender, int Val);
	void __fastcall seArgKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
    bool bLoadMode;

	SH_Function* m_CurFunc;
    SH_Function* m_SaveFunc;

    void GetFuncData();
    void UpdateFuncData();

	void __fastcall DrawGraph();

	AnsiString& GetTokenNameFromVal_EQ(DWORD val, AnsiString& res, const xr_token *token_list);
	DWORD GetTokenValFromName(AnsiString& res, const xr_token *token_list);
	void FillMenuFromToken(TRxPopupMenu* menu, const xr_token *token_list );
public:		// User declarations
    __fastcall TfrmShaderFunction(TComponent* Owner);
    int __fastcall Run(SH_Function* func);
};
//---------------------------------------------------------------------------
extern int frmShaderFunctionRun(SH_Function* func);
//---------------------------------------------------------------------------
#endif
