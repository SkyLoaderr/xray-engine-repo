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

#include "ELight.h"
#include "PropertiesListTypes.h"
#include "multi_check.hpp"
#include "multi_color.hpp"
#include "multi_edit.hpp"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"

//refs
struct SAnimLightItem;
class CLight;
class TProperties;

//---------------------------------------------------------------------------
class TfrmPropertiesLight : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *gbType;
	TLabel *RxLabel1;
    TPageControl *pcType;
    TTabSheet *tsSun;
    TTabSheet *tsPoint;
    TBevel *Bevel2;
    TImage *LG;
    TLabel *lbRange;
    TExtBtn *ebALauto;
    TExtBtn *ebQLauto;
	TLabel *RxLabel3;
    TMultiObjSpinEdit *seAutoBMax;
	TLabel *RxLabel4;
	TPanel *Panel1;
	TExtBtn *ExtBtn1;
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
	TFormStorage *fsStorage;
	TPanel *paProps;
	TPanel *paPointProps;
	TTabSheet *tsSpot;
	TPanel *paSpotProps;
    void __fastcall mcColorMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btApplyClick(TObject *Sender);
    void __fastcall btOkClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebALautoClick(TObject *Sender);
    void __fastcall ebQLautoClick(TObject *Sender);
	void __fastcall btCancelClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
private:	// User declarations
    ObjectList* 	m_Objects;
    void 			GetObjectsInfo();
    bool 			ApplyObjectsInfo();
    void 			CancelObjectsInfo();
    void __fastcall	DrawGraph();
    void __fastcall DrawGraph(float r, float b, float b_max, float a0, float a1, float a2);

    TProperties*	m_SpotProps;
    TProperties*	m_PointProps;
    TProperties*	m_SunProps;
    TProperties*	m_Props;

    FloatValue*		flBrightness;
    FloatValue*		flPointRange;
    FloatValue*		flPointA0;
    FloatValue*		flPointA1;
    FloatValue*		flPointA2;

	void __fastcall OnBrightnessChange(PropValue* sender);
    void __fastcall OnModified();
public:		// User declarations
    __fastcall TfrmPropertiesLight(TComponent* Owner);
    void __fastcall Run(ObjectList* pObjects, bool& bChange);
};
//---------------------------------------------------------------------------
extern void frmPropertiesLightRun(ObjectList* pObjects, bool& bChange);
//---------------------------------------------------------------------------
#endif
