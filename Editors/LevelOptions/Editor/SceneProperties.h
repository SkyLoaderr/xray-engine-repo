//---------------------------------------------------------------------------

#ifndef ScenePropertiesH
#define ScenePropertiesH
//---------------------------------------------------------------------------

#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include "multi_color.hpp"
#include "multi_edit.hpp"
#include "mxPlacemnt.hpp"
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include <StdCtrls.hpp>

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
	TLabel *RxLabel4;
	TMultiObjSpinEdit *seLMPixelsPerMeter;
	TLabel *RxLabel5;
    TMultiObjSpinEdit *seVBMaxSize;
	TLabel *RxLabel6;
    TMultiObjSpinEdit *seVBMaxVertices;
	TLabel *RxLabel7;
	TMultiObjSpinEdit *seSSMaxSize;
	TLabel *RxLabel8;
    TFormStorage *fsSceneProps;
	TLabel *RxLabel11;
    TMultiObjSpinEdit *seSMAngle;
	TLabel *RxLabel12;
	TMultiObjSpinEdit *seLMDeflSplitAngle;
	TLabel *RxLabel15;
    TMultiObjSpinEdit *sePMUV;
	TLabel *RxLabel16;
    TMultiObjSpinEdit *sePMPos;
	TLabel *RxLabel17;
    TMultiObjSpinEdit *sePMCurv;
	TCheckBox *cbStrippify;
	TLabel *RxLabel18;
	TMultiObjSpinEdit *seStripCacheSize;
    TCheckBox *cbTesselation;
	TCheckBox *cbLMLightmaps;
    TCheckBox *cbProgressive;
    TElTree *tvOptions;
	TLabel *RxLabel20;
    TMultiObjSpinEdit *seWeldDistance;
    TPanel *Panel1;
    TTabSheet *tsBuildOptions;
	TLabel *RxLabel25;
	TLabel *RxLabel26;
	TLabel *RxLabel27;
	TLabel *RxLabel28;
	TMultiObjSpinEdit *sePM_borderH_angle;
	TMultiObjSpinEdit *sePM_borderH_distance;
	TMultiObjSpinEdit *sePM_heuristic;
	TTabSheet *tsLevelScript;
	TRichEdit *mmText;
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
	TPanel *Panel2;
	TLabel *RxLabel1;
	TLabel *RxLabel2;
	TEdit *edLevelName;
	TMultiObjSpinEdit *seSSMergeCoef;
	TLabel *RxLabel19;
	TLabel *RxLabel21;
	TMultiObjSpinEdit *seSSLowLimit;
	TLabel *RxLabel30;
	TMultiObjSpinEdit *seSSHighLimit;
	TLabel *RxLabel31;
	TMultiObjSpinEdit *seSSDedicatedCashed;
	TLabel *RxLabel32;
	TMultiObjSpinEdit *seLMRMS;
	TTabSheet *tsLevelEnvironment;
	TPanel *Panel3;
	TLabel *RxLabel3;
	TExtBtn *ebChooseSkydome;
	TExtBtn *ebClearSkydome;
	TLabel *RxLabel10;
	TLabel *RxLabel29;
	TEdit *edSkydomeObjectName;
	TMultiObjSpinEdit *seEnvCount;
	TMultiObjSpinEdit *seCurEnv;
	TScrollBox *sbEnvs;
	TGroupBox *GroupBox2;
	TLabel *RxLabel34;
	TLabel *RxLabel36;
	TMultiObjSpinEdit *seLMAreaDispersion;
	TMultiObjSpinEdit *seLMAreaSummaryEnergy;
	TGroupBox *GroupBox3;
	TLabel *RxLabel9;
	TRadioGroup *rgLMJitterSamples;
	TLabel *RxLabel13;
	TLabel *RxLabel14;
	TMultiObjSpinEdit *seLMDither;
	TMultiObjSpinEdit *seLMJitter;
	TGroupBox *GroupBox4;
	TBevel *Bevel1;
	TMultiObjColor *mcLMAmbient;
	TLabel *RxLabel22;
	TLabel *RxLabel23;
	TMultiObjSpinEdit *seLMAmbFogness;
	TLabel *RxLabel37;
	TBevel *Bevel2;
	TMultiObjColor *mcLMAreaColor;
	TLabel *RxLabel24;
	TLabel *RxLabel39;
	TRadioGroup *rgAreaQuality;
	TGroupBox *GroupBox5;
	TCheckBox *cbLMFuzzyEnabled;
	TLabel *RxLabel35;
	TLabel *RxLabel40;
	TMultiObjSpinEdit *seLMFuzzySamples;
	TMultiObjSpinEdit *seLMFuzzyMin;
	TLabel *RxLabel41;
	TMultiObjSpinEdit *seLMFuzzyMax;
	TEdit *edLevelPath;
	TLabel *MxLabel1;
	TMultiObjSpinEdit *seLMRMSZero;
	TLabel *Label1;
	TExtBtn *ebChooseHOM;
	TExtBtn *ebClearHOM;
	TEdit *edHOMObjectName;
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
	void __fastcall ebChooseHOMClick(TObject *Sender);
	void __fastcall ebClearHOMClick(TObject *Sender);
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
