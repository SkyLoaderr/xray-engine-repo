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
	TLabel *RxLabel13;
	TLabel *RxLabel14;
    TImage *LG;
	TLabel *RxLabel15;
	TLabel *RxLabel16;
	TLabel *RxLabel17;
    TTrackBar *tbA0;
    TTrackBar *tbA1;
    TTrackBar *tbA2;
    TLabel *lbRange;
    TExtBtn *ebALauto;
    TExtBtn *ebQLauto;
	TLabel *RxLabel3;
    TMultiObjSpinEdit *seAutoBMax;
	TLabel *RxLabel4;
	TMultiObjSpinEdit *sePointRange;
	TMultiObjSpinEdit *seA0;
	TMultiObjSpinEdit *seA1;
	TMultiObjSpinEdit *seA2;
	TTimer *tmAnimation;
	TPanel *Panel1;
	TExtBtn *ExtBtn1;
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
	TFormStorage *fsStorage;
	TPanel *paProps;
    void __fastcall mcColorMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btApplyClick(TObject *Sender);
    void __fastcall btOkClick(TObject *Sender);
    void __fastcall tbA0Change(TObject *Sender);
    void __fastcall tbA1Change(TObject *Sender);
    void __fastcall tbA2Change(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall seA0Change(TObject *Sender);
    void __fastcall seA1Change(TObject *Sender);
    void __fastcall seA2Change(TObject *Sender);
    void __fastcall sePointRangeChange(TObject *Sender);
    void __fastcall ebALautoClick(TObject *Sender);
    void __fastcall ebQLautoClick(TObject *Sender);
	void __fastcall btCancelClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
private:	// User declarations
    list<CCustomObject*>* m_Objects;
    void 			GetObjectsInfo();
    bool 			ApplyObjectsInfo();
    void 			CancelObjectsInfo();
    void __fastcall DrawGraph();

    TProperties*	m_SunProps;
    TProperties*	m_Props;

    TextValue*		txName;
    ColorValue*		clDiffuse;
    FloatValue*		flBrightness;
    BOOLValue* 		blUseInD3D;
    FlagValue* 		fgTargetLM;
    FlagValue* 		fgTargetDyn;
    FlagValue* 		fgTargetAnm;
	void __fastcall OnBrightnessAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
public:		// User declarations
    __fastcall TfrmPropertiesLight(TComponent* Owner);
    void __fastcall Run(ObjectList* pObjects, bool& bChange);
};
//---------------------------------------------------------------------------
extern void frmPropertiesLightRun(ObjectList* pObjects, bool& bChange);
//---------------------------------------------------------------------------
#endif
