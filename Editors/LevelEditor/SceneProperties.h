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

//---------------------------------------------------------------------------
class TfrmSceneProperties : public TForm
{
__published:	// IDE-managed Components
    TPageControl *pcBuildOptions;
    TTabSheet *tsLevelOptions;
    TTabSheet *tsOptimizing;
    TTabSheet *tsLightmaps;
    TTabSheet *tsProgressive;
	TMultiObjSpinEdit *seLMPixelsPerMeter;
	TLabel *RxLabel5;
    TFormStorage *fsSceneProps;
	TLabel *RxLabel11;
    TMultiObjSpinEdit *seSMAngle;
	TLabel *RxLabel15;
    TMultiObjSpinEdit *sePMUV;
	TLabel *RxLabel16;
    TMultiObjSpinEdit *sePMPos;
	TLabel *RxLabel17;
    TMultiObjSpinEdit *sePMCurv;
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
	TLabel *RxLabel2;
	TLabel *RxLabel32;
	TMultiObjSpinEdit *seLMRMS;
	TEdit *edLevelPath;
	TLabel *MxLabel1;
	TMultiObjSpinEdit *seLMRMSZero;
    void __fastcall FormShow(TObject *Sender);
    void __fastcall btContinueClick(TObject *Sender);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall tvOptionsItemSelectedChange(TObject *Sender,
          TElTreeItem *Item);
    void __fastcall mcLMAmbientMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ebCancelClick(TObject *Sender);
private:	// User declarations
    void __fastcall SetSceneParams();
    void __fastcall SetEditParams();
    void __fastcall ShowPage(AnsiString& s);
    b_params* m_BuildParams;

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
