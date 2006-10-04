//---------------------------------------------------------------------------

#ifndef single_paramH
#define single_paramH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "RXCtrls.hpp"
#include "RXSpin.hpp"
#include <Mask.hpp>
#include "float_constructor.h"
#include <xrCore.h>
#include "PostprocessAnimator.h"
//---------------------------------------------------------------------------
class TSingleParam : public TForm
{
__published:	// IDE-managed Components
    TRxLabel *RxLabel1;
    TListBox *PointList;
    TGroupBox *GroupBox1;
    TRxLabel *RxLabel2;
    TRxLabel *RxLabel3;
    TRxLabel *RxLabel4;
    TRxLabel *RxLabel5;
    TRxSpinEdit *Value;
    TRxSpinEdit *Tension;
    TRxSpinEdit *Contin;
    TRxSpinEdit *Bias;
    TButton *Button1;
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall ChangeParam(TObject *Sender);
    void __fastcall PointListClick(TObject *Sender);
private:	// User declarations
    TForm8                  *m_Constructor;
    CPostprocessAnimator    *m_Animator;
    pp_params                m_Param;
    bool                     m_bInternal;
    void    FillList        ();
    void    UpdateConstructor ();
public:		// User declarations
    __fastcall TSingleParam(TComponent* Owner, CPostprocessAnimator *pAnimator, pp_params param);
    void    UpdateAfterLoad ();
    void    Clear           ();
};
//---------------------------------------------------------------------------
extern PACKAGE TSingleParam *SingleParam;
//---------------------------------------------------------------------------
#endif
