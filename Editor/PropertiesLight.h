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
#include "SceneObject.h"
#include "CloseBtn.hpp"

#include "ELight.h"
#include "multi_check.hpp"
#include "multi_color.hpp"
#include "multi_edit.hpp"

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
    TRxLabel *RxLabel6;
    TRxLabel *RxLabel1;
    TPageControl *pcType;
    TTabSheet *tsSun;
    TTabSheet *tsPoint;
    TBevel *Bevel2;
    TRxLabel *RxLabel13;
    TRxLabel *RxLabel14;
    TImage *LG;
    TRxLabel *RxLabel15;
    TRxLabel *RxLabel16;
    TRxLabel *RxLabel17;
    TTrackBar *tbA0;
    TTrackBar *tbA1;
    TTrackBar *tbA2;
    TLabel *lbRange;
    TRxLabel *RxLabel2;
    TMultiObjSpinEdit *seShadowedScale;
    TExtBtn *ebALauto;
    TExtBtn *ebQLauto;
    TRxLabel *RxLabel3;
    TMultiObjSpinEdit *seAutoBMax;
    TRxLabel *RxLabel4;
    TTrackBar *tbBrightness;
    TRxLabel *RxLabel18;
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
	TPanel *paHideAnimKeys;
	TGroupBox *gbProceduralKeys;
	TRxLabel *RxLabel8;
	TRxLabel *lbAnimKeyNum;
	TRxLabel *lbMinAnimKey;
	TRxLabel *lbMaxAnimKey;
	TGroupBox *gbKeyActions;
	TExtBtn *ebAppendKey;
	TExtBtn *ebInsertKey;
	TExtBtn *ebRemoveKey;
	TExtBtn *ebRemoveAllKeys;
	TTrackBar *tbAnimKeys;
	TTimer *tmAnimation;
	TExtBtn *ebAnimPlay;
	TExtBtn *ebAnimStop;
	TRxLabel *RxLabel20;
	TMultiObjSpinEdit *seAnimSpeed;
	TExtBtn *ebAnimStart;
	TExtBtn *ebAnimEnd;
	TExtBtn *ebAnimForward;
	TExtBtn *ebAnimRewind;
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
	void __fastcall tbAnimKeysChange(TObject *Sender);
	void __fastcall ebAppendKeyClick(TObject *Sender);
	void __fastcall ebInsertKeyClick(TObject *Sender);
	void __fastcall ebRemoveAllKeysClick(TObject *Sender);
	void __fastcall ebRemoveKeyClick(TObject *Sender);
	void __fastcall ebAnimPlayClick(TObject *Sender);
	void __fastcall ebAnimStopClick(TObject *Sender);
	void __fastcall tmAnimationTimer(TObject *Sender);
	void __fastcall btCancelClick(TObject *Sender);
	void __fastcall cbTargetLMClick(TObject *Sender);
	void __fastcall ebAnimStartClick(TObject *Sender);
	void __fastcall ebAnimEndClick(TObject *Sender);
	void __fastcall ebAnimRewindClick(TObject *Sender);
	void __fastcall ebAnimForwardClick(TObject *Sender);
	void __fastcall btAdjustSceneClick(TObject *Sender);
private:	// User declarations
    list<SceneObject*>* m_Objects;
    void GetObjectsInfo();
    bool ApplyObjectsInfo();
    void __fastcall DrawGraph();

    bool m_bAllowEditProcedural;
    int m_CurrentAnimKey;
    CLight* 		m_SaveLight;
    CLight* 		m_CurLight;
    void AllowEditProcedural(bool bAllowEditProcedural);
    void InitCurrentKey();
    void SaveCurrentKey();
    bool SetCurrentKey(DWORD key);
    void UpdateKeys();
    void UpdateCurAnimLight();
    void SetEnabledAnimControlsForPlay(bool bFlag);
public:		// User declarations
    __fastcall TfrmPropertiesLight(TComponent* Owner);
    void __fastcall Run(ObjectList* pObjects, bool& bChange);
};
//---------------------------------------------------------------------------
extern void frmPropertiesLightRun(ObjectList* pObjects, bool& bChange);
//---------------------------------------------------------------------------
#endif
