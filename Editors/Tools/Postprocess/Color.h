//---------------------------------------------------------------------------

#ifndef ColorH
#define ColorH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Mask.hpp>
#include <ExtCtrls.hpp>
#include "color_constructor.h"
#include <xrCore.h>
#include "PostprocessAnimator.h"
#include <Dialogs.hpp>
#include "multi_edit.hpp"
#include "MXCtrls.hpp"
//---------------------------------------------------------------------------
class TColorForm : public TForm
{
__published:	// IDE-managed Components
    TListBox *PointList;
    TMxLabel *RxLabel1;
    TGroupBox *GroupBox1;
    TMultiObjSpinEdit *RedValue;
    TMultiObjSpinEdit *RedTension;
    TMultiObjSpinEdit *RedContin;
    TMultiObjSpinEdit *RedBias;
    TMxLabel *RxLabel2;
    TMxLabel *RxLabel3;
    TMxLabel *RxLabel4;
    TMxLabel *RxLabel5;
    TGroupBox *GroupBox2;
    TMxLabel *RxLabel6;
    TMxLabel *RxLabel7;
    TMxLabel *RxLabel8;
    TMxLabel *RxLabel9;
    TMultiObjSpinEdit *GreenValue;
    TMultiObjSpinEdit *GreenTension;
    TMultiObjSpinEdit *GreenContin;
    TMultiObjSpinEdit *GreenBias;
    TGroupBox *GroupBox3;
    TMxLabel *RxLabel10;
    TMxLabel *RxLabel11;
    TMxLabel *RxLabel12;
    TMxLabel *RxLabel13;
    TMultiObjSpinEdit *BlueValue;
    TMultiObjSpinEdit *BlueTension;
    TMultiObjSpinEdit *BlueContin;
    TMultiObjSpinEdit *BlueBias;
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
