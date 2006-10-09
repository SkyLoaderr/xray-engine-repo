//---------------------------------------------------------------------------

#ifndef single_paramH
#define single_paramH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Mask.hpp>
#include "float_constructor.h"
#include <xrCore.h>
#include "PostprocessAnimator.h"
#include "multi_edit.hpp"
#include "MXCtrls.hpp"
//---------------------------------------------------------------------------
class TSingleParam : public TForm
{
__published:	// IDE-managed Components
    TMxLabel *RxLabel1;
    TListBox *PointList;
    TGroupBox *GroupBox1;
    TMxLabel *RxLabel2;
    TMxLabel *RxLabel3;
    TMxLabel *RxLabel4;
    TMxLabel *RxLabel5;
    TMultiObjSpinEdit *Value;
    TMultiObjSpinEdit *Tension;
    TMultiObjSpinEdit *Contin;
    TMultiObjSpinEdit *Bias;
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
