//---------------------------------------------------------------------------

#ifndef ShaderModificatorH
#define ShaderModificatorH
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
struct SH_StageDef;

class TfrmShaderModificator : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
	TLabel *Label4;
	TMultiObjSpinEdit *seScaleU;
	TLabel *Label1;
	TMultiObjSpinEdit *seRotFactor;
	TLabel *Label2;
	TMultiObjSpinEdit *seScrollU;
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
	TCheckBox *cbScale;
	TCheckBox *cbRotate;
	TCheckBox *cbScroll;
	TCheckBox *cbStretch;
	TMultiObjSpinEdit *seScaleV;
	TLabel *Label3;
	TLabel *Label5;
	TMultiObjSpinEdit *seScrollV;
	TExtBtn *ebStretchFunc;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
	void __fastcall seArgExit(TObject *Sender);
	void __fastcall CheckClick(TObject *Sender);
	void __fastcall ebStretchFuncClick(TObject *Sender);
	void __fastcall seScaleUKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall seScaleULWChange(TObject *Sender, int Val);
private:	// User declarations
    bool bLoadMode;

	SH_StageDef* m_CurStage;
	SH_StageDef* m_SaveStage;

    void GetModificatorData();
    void UpdateModificatorData();

public:		// User declarations
    __fastcall TfrmShaderModificator(TComponent* Owner);
    int __fastcall Run(SH_StageDef* stage);
};
//---------------------------------------------------------------------------
extern int frmShaderModificatorRun(SH_StageDef* stage);
//---------------------------------------------------------------------------
#endif
