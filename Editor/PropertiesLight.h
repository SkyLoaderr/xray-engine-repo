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

#include "CustomObject.h"
#include "ELight.h"
#include "multi_check.hpp"
#include "multi_color.hpp"
#include "multi_edit.hpp"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"

//refs
struct SAnimLightItem;
class CLight;

//---------------------------------------------------------------------------
class TfrmPropertiesLight : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *gbColor;
	TGroupBox *gbAddition;
    TGroupBox *gbType;
    TMultiObjColor *mcDiffuse;
    TMultiObjCheck *cbCastShadows;
    TMxLabel *RxLabel6;
    TMxLabel *RxLabel1;
    TPageControl *pcType;
    TTabSheet *tsSun;
    TTabSheet *tsPoint;
    TBevel *Bevel2;
    TMxLabel *RxLabel13;
    TMxLabel *RxLabel14;
    TImage *LG;
    TMxLabel *RxLabel15;
    TMxLabel *RxLabel16;
    TMxLabel *RxLabel17;
    TTrackBar *tbA0;
    TTrackBar *tbA1;
    TTrackBar *tbA2;
    TLabel *lbRange;
    TExtBtn *ebALauto;
    TExtBtn *ebQLauto;
    TMxLabel *RxLabel3;
    TMultiObjSpinEdit *seAutoBMax;
    TMxLabel *RxLabel4;
    TTrackBar *tbBrightness;
    TMxLabel *RxLabel18;
    TMultiObjCheck *cbFlares;
    TMemo *mmFlares;
	TGroupBox *gbLightType;
	TMultiObjCheck *cbTargetLM;
	TMultiObjCheck *cbTargetDynamic;
	TMultiObjCheck *cbTargetAnimated;
	TMultiObjSpinEdit *sePointRange;
	TMultiObjSpinEdit *seA0;
	TMultiObjSpinEdit *seA1;
	TMultiObjSpinEdit *seA2;
	TMultiObjSpinEdit *seBrightness;
	TTimer *tmAnimation;
	TMultiObjCheck *cbUseInD3D;
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
	TExtBtn *ExtBtn1;
	TExtBtn *ebAdjustScene;
    void __fastcall mcColorMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btApplyClick(TObject *Sender);
    void __fastcall btOkClick(TObject *Sender);
    void __fastcall tbA0Change(TObject *Sender);
    void __fastcall tbA1Change(TObject *Sender);
    void __fastcall tbA2Change(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall tbBrightnessChange(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall seA0Change(TObject *Sender);
    void __fastcall seA1Change(TObject *Sender);
    void __fastcall seA2Change(TObject *Sender);
    void __fastcall seBrightnessChange(TObject *Sender);
    void __fastcall sePointRangeChange(TObject *Sender);
    void __fastcall ebALautoClick(TObject *Sender);
    void __fastcall ebQLautoClick(TObject *Sender);
    void __fastcall cbFlaresClick(TObject *Sender);
	void __fastcall cbTargetAnimatedClick(TObject *Sender);
	void __fastcall btCancelClick(TObject *Sender);
	void __fastcall cbTargetLMClick(TObject *Sender);
private:	// User declarations
    list<CCustomObject*>* m_Objects;
    void 			GetObjectsInfo();
    bool 			ApplyObjectsInfo();
    void __fastcall DrawGraph();

    CLight* 		m_SaveLight;
    CLight* 		m_CurLight;
public:		// User declarations
    __fastcall TfrmPropertiesLight(TComponent* Owner);
    void __fastcall Run(ObjectList* pObjects, bool& bChange);
};
//---------------------------------------------------------------------------
extern void frmPropertiesLightRun(ObjectList* pObjects, bool& bChange);
//---------------------------------------------------------------------------
#endif
