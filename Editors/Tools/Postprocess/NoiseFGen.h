//---------------------------------------------------------------------------

#ifndef NoiseFGenH
#define NoiseFGenH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "RXCtrls.hpp"
#include "RXSpin.hpp"
#include <ExtCtrls.hpp>
#include <Mask.hpp>
//---------------------------------------------------------------------------
class TNFGen : public TForm
{
__published:	// IDE-managed Components
    TRxLabel *RxLabel1;
    TRxLabel *RxLabel2;
    TRxLabel *RxLabel3;
    TRxLabel *RxLabel4;
    TRxSpinEdit *First;
    TRxSpinEdit *Period;
    TRxSpinEdit *Second;
    TRxSpinEdit *Time;
    TBevel *Bevel1;
    TButton *Button1;
    TButton *Button2;
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TNFGen(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TNFGen *NFGen;
//---------------------------------------------------------------------------
#endif
