//---------------------------------------------------------------------------

#ifndef ScenePropertiesH
#define ScenePropertiesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "CSPIN.h"
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include "multi_color.hpp"
#include "multi_edit.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"

//refs
struct b_params;
class TfrmOneEnvironment;

DEFINE_VECTOR(TfrmOneEnvironment*, FrmEnvList, FrmEnvIt);
//---------------------------------------------------------------------------
class TfrmSceneProperties : public TForm
{
__published:	// IDE-managed Components
    TPageControl *pcBuildOptions;
    TTabSheet *tsLevelOptions;
    TTabSheet *tsOptimizing;
    TTabSheet *tsTesselation;
    TTabSheet *tsLightmaps;
    TTabSheet *tsProgressive;
    TTabSheet *tsVertexBuffers;
    TTabSheet *tsSubdivision;
	TTabSheet *tsStrippifier;
    TMultiObjSpinEdit *seMaxEdge;
    TMxLabel *RxLabel4;
	TMultiObjSpinEdit *seLMPixelsPerMeter;
    TMxLabel *RxLabel5;
    TMultiObjSpinEdit *seVBMaxSize;
    TMxLabel *RxLabel6;
    TMultiObjSpinEdit *seVBMaxVertices;
    TMxLabel *RxLabel7;
	TMultiObjSpinEdit *seSSMaxSize;
    TMxLabel *RxLabel8;
    TFormStorage *fsSceneProps;
    TMxLabel *RxLabel11;
    TMultiObjSpinEdit *seSMAngle;
    TMxLabel *RxLabel12;
	TMultiObjSpinEdit *seLMDeflSplitAngle;
    TMxLabel *RxLabel15;
    TMultiObjSpinEdit *sePMUV;
    TMxLabel *RxLabel16;
    TMultiObjSpinEdit *sePMPos;
    TMxLabel *RxLabel17;
    TMultiObjSpinEdit *sePMCurv;
	TCheckBox *cbStrippify;
    TMxLabel *RxLabel18;
	TMultiObjSpinEdit *seStripCacheSize;
    TCheckBox *cbTesselation;
	TCheckBox *cbLMLightmaps;
    TCheckBox *cbProgressive;
    TElTree *tvOptions;
    TMxLabel *RxLabel20;
    TMultiObjSpinEdit *seWeldDistance;
    TPanel *Panel1;
    TTabSheet *tsBuildOptions;
    TMxLabel *RxLabel25;
	TMxLabel *RxLabel26;
	TMxLabel *RxLabel27;
	TMxLabel *RxLabel28;
	TMultiObjSpinEdit *sePM_borderH_angle;
	TMultiObjSpinEdit *sePM_borderH_distance;
	TMultiObjSpinEdit *sePM_heuristic;
	TTabSheet *tsLevelScript;
	TRichEdit *mmText;
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
	TPanel *Panel2;
	TMxLabel *RxLabel1;
	TMxLabel *RxLabel2;
	TEdit *edLevelName;
	TMultiObjSpinEdit *seSSMergeCoef;
	TMxLabel *RxLabel19;
	TMxLabel *RxLabel21;
	TMultiObjSpinEdit *seSSLowLimit;
	TMxLabel *RxLabel30;
	TMultiObjSpinEdit *seSSHighLimit;
	TMxLabel *RxLabel31;
	TMultiObjSpinEdit *seSSDedicatedCashed;
	TMxLabel *RxLabel32;
	TMultiObjSpinEdit *seLMRMS;
	TTabSheet *tsLevelEnvironment;
	TPanel *Panel3;
	TMxLabel *RxLabel3;
	TExtBtn *ebChooseSkydome;
	TExtBtn *ebClearSkydome;
	TMxLabel *RxLabel10;
	TMxLabel *RxLabel29;
	TEdit *edSkydomeObjectName;
	TMultiObjSpinEdit *seEnvCount;
	TMultiObjSpinEdit *seCurEnv;
	TScrollBox *sbEnvs;
	TGroupBox *GroupBox2;
	TMxLabel *RxLabel34;
	TMxLabel *RxLabel36;
	TMultiObjSpinEdit *seLMAreaDispersion;
	TMultiObjSpinEdit *seLMAreaSummaryEnergy;
	TGroupBox *GroupBox3;
	TMxLabel *RxLabel9;
	TRadioGroup *rgLMJitterSamples;
	TMxLabel *RxLabel13;
	TMxLabel *RxLabel14;
	TMultiObjSpinEdit *seLMDither;
	TMultiObjSpinEdit *seLMJitter;
	TGroupBox *GroupBox4;
	TBevel *Bevel1;
	TMultiObjColor *mcLMAmbient;
	TMxLabel *RxLabel22;
	TMxLabel *RxLabel23;
	TMultiObjSpinEdit *seLMAmbFogness;
	TMxLabel *RxLabel37;
	TBevel *Bevel2;
	TMultiObjColor *mcLMAreaColor;
	TMxLabel *RxLabel24;
	TMxLabel *RxLabel39;
	TRadioGroup *rgAreaQuality;
	TGroupBox *GroupBox5;
	TCheckBox *cbLMFuzzyEnabled;
	TMxLabel *RxLabel35;
	TMxLabel *RxLabel40;
	TMultiObjSpinEdit *seLMFuzzySamples;
	TMultiObjSpinEdit *seLMFuzzyMin;
	TMxLabel *RxLabel41;
	TMultiObjSpinEdit *seLMFuzzyMax;
	TEdit *edLevelPath;
	TMxLabel *MxLabel1;
	TMultiObjSpinEdit *seLMRMSZero;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall btContinueClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall fsScenePropsRestorePlacement(TObject *Sender);
    void __fastcall tvOptionsItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
    void __fastcall mcLMAmbientMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ebCancelClick(TObject *Sender);
	void __fastcall ebChooseSkydomeClick(TObject *Sender);
	void __fastcall ebClearSkydomeClick(TObject *Sender);
	void __fastcall tsLevelOptionsShow(TObject *Sender);
	void __fastcall seEnvCountChange(TObject *Sender);
	void __fastcall seSSLowLimitChange(TObject *Sender);
	void __fastcall seSSHighLimitChange(TObject *Sender);
	void __fastcall tsLevelEnvironmentShow(TObject *Sender);
	void __fastcall seLMFuzzyMinLWChange(TObject *Sender, int Val);
	void __fastcall seLMFuzzyMinKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall seLMFuzzyMinExit(TObject *Sender);
private:	// User declarations
    void __fastcall SetSceneParams();
    void __fastcall SetEditParams();
    void __fastcall ShowPage(AnsiString& s);
    b_params* m_BuildParams;

    // environment
    FrmEnvList m_frmEnvs;
    void __fastcall EnvsUpdate();
public:		// User declarations
    __fastcall TfrmSceneProperties(TComponent* Owner, b_params* params);
    int __fastcall Run(b_params* params, bool bRunBuild=false);
};
//---------------------------------------------------------------------------
#ifdef _EDITOR
	extern int frmScenePropertiesRun(b_params* params, bool bRunBuild=false);
#else
	extern "C" __declspec(dllexport) __cdecl int frmScenePropertiesRun(b_params* params, DWORD version, bool bRunBuild=false);
#endif
//---------------------------------------------------------------------------
#endif
