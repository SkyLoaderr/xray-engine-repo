//---------------------------------------------------------------------------

#ifndef NumericVectorH
#define NumericVectorH
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
#include "multi_edit.hpp"
#include "MXCtrls.hpp"

class TfrmNumericVector : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TGroupBox *gbTitle;
	TMxLabel *RxLabel3;
	TMultiObjSpinEdit *seX;
	TMultiObjSpinEdit *seY;
	TMultiObjSpinEdit *seZ;
	TMxLabel *RxLabel1;
	TMxLabel *RxLabel2;
	TExtBtn *ebReset;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall ebResetClick(TObject *Sender);
private:	// User declarations
	Fvector* edit_data;
	Fvector* reset_value;
public:		// User declarations
    __fastcall TfrmNumericVector(TComponent* Owner);
    int __fastcall Run(const char* title, Fvector* data, int decimal=3, Fvector* reset_value=0, Fvector* min=0, Fvector* max=0);
};
//---------------------------------------------------------------------------
int NumericVectorRun(const char* title, Fvector* data, int decimal=3, Fvector* reset_value=0, Fvector* min=0, Fvector* max=0);
//---------------------------------------------------------------------------
#endif
