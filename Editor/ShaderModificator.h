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
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "MxMenus.hpp"
#include "mxPlacemnt.hpp"
#include <Menus.hpp>

class TfrmShaderMatrix : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
	TMxLabel *RxLabel1;
	TStaticText *stMode;
	TMxPopupMenu *pmMode;
	TFormStorage *fsStorage;
	TPanel *paTCM;
	TLabel *Label4;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Label5;
	TExtBtn *ebScaleU;
	TExtBtn *ebRotate;
	TExtBtn *ebScrollU;
	TExtBtn *ebScaleV;
	TExtBtn *ebScrollV;
	TCheckBox *cbScale;
	TCheckBox *cbRotate;
	TCheckBox *cbScroll;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
	void __fastcall seArgExit(TObject *Sender);
	void __fastcall CheckClick(TObject *Sender);
	void __fastcall ebScaleUClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall stModeMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ebScaleVClick(TObject *Sender);
	void __fastcall ebRotateClick(TObject *Sender);
	void __fastcall ebScrollUClick(TObject *Sender);
	void __fastcall ebScrollVClick(TObject *Sender);
private:	// User declarations
    bool bLoadMode;
	void __fastcall stModeClick(TObject *Sender);

	CMatrix* m_CurMatrix;
	CMatrix* m_SaveMatrix;

    void GetMatrixData();
    void UpdateMatrixData();

	static TfrmShaderMatrix *form;
public:		// User declarations
    __fastcall TfrmShaderMatrix(TComponent* Owner);
    static int __fastcall Run(CMatrix* matrix);
};
//---------------------------------------------------------------------------
#endif
