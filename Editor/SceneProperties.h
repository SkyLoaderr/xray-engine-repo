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
#include "RXCtrls.hpp"
#include "RXSpin.hpp"

//#include "SceneObject.h"
#include "ToolEdit.hpp"
#include <Mask.hpp>
#include "ElTree.hpp"
#include "Placemnt.hpp"
#include "CloseBtn.hpp"
#include "multi_color.hpp"
#include "multi_edit.hpp"

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
    TRxLabel *RxLabel4;
	TMultiObjSpinEdit *seLMPixelsPerMeter;
    TRxLabel *RxLabel5;
    TMultiObjSpinEdit *seVBMaxSize;
    TRxLabel *RxLabel6;
    TMultiObjSpinEdit *seVBMaxVertices;
    TRxLabel *RxLabel7;
	TMultiObjSpinEdit *seSSMaxSize;
    TRxLabel *RxLabel8;
    TFormStorage *fsSceneProps;
    TRxLabel *RxLabel11;
    TMultiObjSpinEdit *seSMAngle;
    TRxLabel *RxLabel12;
	TMultiObjSpinEdit *seLMDeflSplitAngle;
    TRxLabel *RxLabel15;
    TMultiObjSpinEdit *sePMUV;
    TRxLabel *RxLabel16;
    TMultiObjSpinEdit *sePMPos;
    TRxLabel *RxLabel17;
    TMultiObjSpinEdit *sePMCurv;
	TCheckBox *cbStrippify;
    TRxLabel *RxLabel18;
	TMultiObjSpinEdit *seStripCacheSize;
    TCheckBox *cbTesselation;
	TCheckBox *cbLMLightmaps;
    TCheckBox *cbProgressive;
    TElTree *tvOptions;
    TRxLabel *RxLabel20;
    TMultiObjSpinEdit *seWeldDistance;
    TPanel *Panel1;
    TTabSheet *tsBuildOptions;
    TRxLabel *RxLabel25;
	TCheckBox *cbLMRadiosity;
	TRxLabel *RxLabel26;
	TRxLabel *RxLabel27;
	TRxLabel *RxLabel28;
	TMultiObjSpinEdit *sePM_borderH_angle;
	TMultiObjSpinEdit *sePM_borderH_distance;
	TMultiObjSpinEdit *sePM_heuristic;
	TTabSheet *tsLevelScript;
	TRichEdit *mmText;
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
	TPanel *Panel2;
	TRxLabel *RxLabel1;
	TRxLabel *RxLabel2;
	TEdit *edLevelName;
	TDirectoryEdit *deLevelPath;
	TMultiObjSpinEdit *seSSMergeCoef;
	TRxLabel *RxLabel19;
	TRxLabel *RxLabel21;
	TMultiObjSpinEdit *seSSLowLimit;
	TRxLabel *RxLabel30;
	TMultiObjSpinEdit *seSSHighLimit;
	TRxLabel *RxLabel31;
	TMultiObjSpinEdit *seSSDedicatedCashed;
	TRxLabel *RxLabel32;
	TMultiObjSpinEdit *seLMRMS;
	TTabSheet *tsLevelEnvironment;
	TPanel *Panel3;
	TRxLabel *RxLabel3;
	TExtBtn *ebChooseSkydome;
	TExtBtn *ebClearSkydome;
	TRxLabel *RxLabel10;
	TRxLabel *RxLabel29;
	TEdit *edSkydomeObjectName;
	TMultiObjSpinEdit *seEnvCount;
	TMultiObjSpinEdit *seCurEnv;
	TScrollBox *sbEnvs;
	TGroupBox *GroupBox1;
	TRxLabel *RxLabel33;
	TMultiObjSpinEdit *seDOClusterSize;
	TGroupBox *GroupBox2;
	TRxLabel *RxLabel34;
	TRxLabel *RxLabel36;
	TMultiObjSpinEdit *seLMAreaDispersion;
	TMultiObjSpinEdit *seLMAreaSummaryEnergy;
	TGroupBox *GroupBox3;
	TRxLabel *RxLabel9;
	TRadioGroup *rgLMJitterSamples;
	TRxLabel *RxLabel13;
	TRxLabel *RxLabel14;
	TMultiObjSpinEdit *seLMDither;
	TMultiObjSpinEdit *seLMJitter;
	TGroupBox *GroupBox4;
	TBevel *Bevel1;
	TMultiObjColor *mcLMAmbient;
	TRxLabel *RxLabel22;
	TRxLabel *RxLabel23;
	TMultiObjSpinEdit *seLMAmbFogness;
	TRxLabel *RxLabel37;
	TBevel *Bevel2;
	TMultiObjColor *mcLMAreaDark;
	TRxLabel *RxLabel24;
	TRxLabel *RxLabel39;
	TRadioGroup *rgAreaQuality;
	TGroupBox *GroupBox5;
	TCheckBox *cbLMFuzzyEnabled;
	TRxLabel *RxLabel35;
	TRxLabel *RxLabel40;
	TMultiObjSpinEdit *seLMFuzzySamples;
	TMultiObjSpinEdit *seLMFuzzyMin;
	TRxLabel *RxLabel41;
	TMultiObjSpinEdit *seLMFuzzyMax;
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
