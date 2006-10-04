//---------------------------------------------------------------------------

#ifndef ColorH
#define ColorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "RXCtrls.hpp"
#include "RXSpin.hpp"
#include <Mask.hpp>
#include <ExtCtrls.hpp>
#include "color_constructor.h"
#include <xrCore.h>
#include "PostprocessAnimator.h"
#include <Dialogs.hpp>
//---------------------------------------------------------------------------
class TColorForm : public TForm
{
__published:	// IDE-managed Components
    TListBox *PointList;
    TRxLabel *RxLabel1;
    TGroupBox *GroupBox1;
    TRxSpinEdit *RedValue;
    TRxSpinEdit *RedTension;
    TRxSpinEdit *RedContin;
    TRxSpinEdit *RedBias;
    TRxLabel *RxLabel2;
    TRxLabel *RxLabel3;
    TRxLabel *RxLabel4;
    TRxLabel *RxLabel5;
    TGroupBox *GroupBox2;
    TRxLabel *RxLabel6;
    TRxLabel *RxLabel7;
    TRxLabel *RxLabel8;
    TRxLabel *RxLabel9;
    TRxSpinEdit *GreenValue;
    TRxSpinEdit *GreenTension;
    TRxSpinEdit *GreenContin;
    TRxSpinEdit *GreenBias;
    TGroupBox *GroupBox3;
    TRxLabel *RxLabel10;
    TRxLabel *RxLabel11;
    TRxLabel *RxLabel12;
    TRxLabel *RxLabel13;
    TRxSpinEdit *BlueValue;
    TRxSpinEdit *BlueTension;
    TRxSpinEdit *BlueContin;
    TRxSpinEdit *BlueBias;
    TButton *Button1;
    TPanel *Color;
    TColorDialog *ColorDialog;
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall PointListClick(TObject *Sender);
    void __fastcall CnahgeRedParam(TObject *Sender);
    void __fastcall ChangeGreenParam(TObject *Sender);
    void __fastcall ChangeBlueParam(TObject *Sender);
    void __fastcall ColorClick(TObject *Sender);
private:	// User declarations
    TForm6                  *m_Constructor;
    CPostprocessAnimator    *m_Animator;
    pp_params                m_Param;
    bool                     m_bInternal;
    void    FillList        ();
    void    UpdateColor     ();
    void    UpdateConstructor ();
public:		// User declarations
    __fastcall TColorForm(TComponent* Owner, CPostprocessAnimator *pAnimator, pp_params param);
    void    UpdateAfterLoad ();
    void    Clear           ();
    CPostprocessAnimator*   GetAnimator () { return m_Animator; }
};
//---------------------------------------------------------------------------
extern PACKAGE TColorForm *ColorForm;
//---------------------------------------------------------------------------
#endif
