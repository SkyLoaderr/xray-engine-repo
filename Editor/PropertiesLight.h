//---------------------------------------------------------------------------

#ifndef PropertiesLightH
#define PropertiesLightH
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
#include "multiobj.hpp"
#include "SceneObject.h"

//---------------------------------------------------------------------------
class TfrmPropertiesLight : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TColorDialog *cdColor;
    TBevel *Bevel1;
    TGroupBox *GroupBox2;
    TGroupBox *gbType;
    TButton *btApply;
    TButton *btOk;
    TButton *btCancel;
    TMultiObjColor *mcAmbient;
    TMultiObjColor *mcDiffuse;
    TMultiObjColor *mcSpecular;
    TMultiObjCheck *cbCastShadows;
    TRxLabel *RxLabel5;
    TRxLabel *RxLabel6;
    TRxLabel *RxLabel7;
    TRxLabel *RxLabel1;
    TPageControl *pcType;
    TTabSheet *tsSun;
    TRxLabel *RxLabel9;
    TRxLabel *RxLabel10;
    TRxLabel *RxLabel11;
    TRxLabel *RxLabel12;
    TRxLabel *RxLabel2;
    TRxLabel *RxLabel3;
    TRxLabel *RxLabel4;
    TRxLabel *RxLabel8;
    TRxSpinEdit *seDirectionalDirX;
    TRxSpinEdit *seDirectionalDirY;
    TRxSpinEdit *seDirectionalDirZ;
    TCheckBox *cbFlares;
    TCheckBox *cbGradient;
    TRxSpinEdit *seSunSize;
    TRxSpinEdit *sePower;
    TTabSheet *tsPoint;
    TBevel *Bevel2;
    TRxLabel *RxLabel13;
    TRxLabel *RxLabel14;
    TImage *LG;
    TRxLabel *RxLabel15;
    TRxLabel *RxLabel16;
    TRxLabel *RxLabel17;
    TRxSpinEdit *sePointRange;
    TTrackBar *tbBrightness;
    TTrackBar *tbA0;
    TTrackBar *tbA1;
    TTrackBar *tbA2;
    TMultiObjSpinEdit *seA0;
    TMultiObjSpinEdit *seA1;
    TMultiObjSpinEdit *seA2;
    TCheckBox *cbAutoApply;
    TRxLabel *RxLabel18;
    TMultiObjSpinEdit *seBrightness;
    TLabel *lbMaxA;
    TLabel *lbRange;
    void __fastcall mcColorMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btApplyClick(TObject *Sender);
    void __fastcall btOkClick(TObject *Sender);
    void __fastcall seA0Change(TObject *Sender);
    void __fastcall seA1Change(TObject *Sender);
    void __fastcall seA2Change(TObject *Sender);
    void __fastcall tbA0Change(TObject *Sender);
    void __fastcall tbA1Change(TObject *Sender);
    void __fastcall tbA2Change(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall seBrightnessChange(TObject *Sender);
    void __fastcall tbBrightnessChange(TObject *Sender);
    void __fastcall sePointRangeChange(TObject *Sender);
    void __fastcall cbAutoApplyClick(TObject *Sender);
    void __fastcall pcTypeChange(TObject *Sender);
    void __fastcall seDirectionalDirChange(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
private:	// User declarations
    list<SceneObject*>* m_Objects;
    void GetObjectsInfo();
    void ApplyObjectsInfo();
    void __fastcall DrawGraph();
public:		// User declarations
    __fastcall TfrmPropertiesLight(TComponent* Owner);
    void __fastcall Run(ObjectList* pObjects);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmPropertiesLight *frmPropertiesLight;
//---------------------------------------------------------------------------
#endif
