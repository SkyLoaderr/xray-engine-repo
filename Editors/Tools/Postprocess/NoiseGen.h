//---------------------------------------------------------------------------

#ifndef NoiseGenH
#define NoiseGenH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "RXCtrls.hpp"
#include "RXSpin.hpp"
#include <ExtCtrls.hpp>
#include <Mask.hpp>
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TNGen : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TRxSpinEdit *FR;
    TRxSpinEdit *FG;
    TRxSpinEdit *FB;
    TRxLabel *RxLabel1;
    TRxLabel *RxLabel2;
    TRxLabel *RxLabel3;
    TPanel *FColor;
    TGroupBox *GroupBox2;
    TRxLabel *RxLabel4;
    TRxLabel *RxLabel5;
    TRxLabel *RxLabel6;
    TRxSpinEdit *SR;
    TRxSpinEdit *SG;
    TRxSpinEdit *SB;
    TPanel *SColor;
    TRxLabel *RxLabel7;
    TRxLabel *RxLabel8;
    TRxSpinEdit *Period;
    TRxSpinEdit *Time;
    TButton *Button1;
    TButton *Button2;
    TBevel *Bevel1;
    TColorDialog *ColorDialog;
    void __fastcall FColorClick(TObject *Sender);
    void __fastcall SColorClick(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
    __fastcall TNGen(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TNGen *NGen;
//---------------------------------------------------------------------------
#endif
