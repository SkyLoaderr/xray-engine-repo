#include "stdafx.h"
#pragma hdrstop

#include "SceneProperties.h"
#include "xrLevel.h"
#include "ColorPicker.h"
#ifdef _LEVEL_EDITOR
 #include "Scene.h"
 #include "library.h"
 #include "ChoseForm.h"
 #include "EditObject.h"
 #include "ui_main.h"
#else
 #include "Communicate.h"
#endif
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ElTree"
#pragma link "ElXPThemedControl"
#pragma link "ExtBtn"
#pragma link "multi_color"
#pragma link "multi_edit"
#pragma link "mxPlacemnt"
#pragma resource "*.dfm"
TfrmSceneProperties *frmSceneProperties=0;
//---------------------------------------------------------------------------
void UpdateMinValue(TMultiObjSpinEdit* c, float min_val, float divers=0.f){
	c->MinValue=min_val+divers;
    if (c->Value<min_val) c->Value=min_val+divers;
}
//---------------------------------------------------------------------------

#ifdef _LEVEL_EDITOR
int frmScenePropertiesRun(b_params* params, bool bRunBuild){
#else
extern "C" __declspec(dllexport) __cdecl int frmScenePropertiesRun(b_params* params, DWORD version, bool bRunBuild){
	if (version!=XRCL_CURRENT_VERSION){
    	ELog.DlgMsg(mtError,"Incorrect version of level options dll.");
    	return mrCancel;
    }
#endif
	frmSceneProperties = xr_new<TfrmSceneProperties>((TComponent*)0,params);
    int res = frmSceneProperties->Run(params, bRunBuild);
    xr_delete(frmSceneProperties);
    return res;
}

//---------------------------------------------------------------------------
__fastcall TfrmSceneProperties::TfrmSceneProperties(TComponent* Owner, b_params* params)
    : TForm(Owner)
{
    VERIFY(params);
    m_BuildParams = params;
    DEFINE_INI(fsSceneProps);
    SetEditParams();
    ShowPage(AnsiString(""));
	fsSceneProps->RestoreFormPlacement();
}
//---------------------------------------------------------------------------

int __fastcall TfrmSceneProperties::Run(b_params* params, bool bRunBuild)
{
    if (bRunBuild) ebOk->Caption = "Continue Build";
    m_BuildParams = params;
    int mr = frmSceneProperties->ShowModal();
    if (bRunBuild) ebOk->Caption = "Ok";
    return mr;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::FormShow(TObject *Sender)
{
	tvOptions->Items->Clear();
    TElTreeItem* root = tvOptions->Items->Add(0,"Build Options");
    root->ParentStyle = false;
    root->Bold = true;
#ifdef _LEVEL_EDITOR
    tvOptions->Items->AddChild(root,"Level options");
    tvOptions->Items->AddChild(root,"Level script");
	edLevelPath->Text		= Scene.m_LevelOp.m_FNLevelPath;
	mmText->Text			= Scene.m_LevelOp.m_BOPText;

    tsLevelScript->Enabled 	= true;
    tsLevelOptions->Enabled = true;

#else
    tsLevelScript->Enabled = false;
    tsLevelOptions->Enabled = false;
    tsLevelEnvironment->Enabled = false;
    SetEditParams();
#endif
    tvOptions->Items->AddChild(root,"Optimizing");
    tvOptions->Items->AddChild(root,"Lightmaps");
    tvOptions->Items->AddChild(root,"Progressive");

    tvOptions->SetFocus();
    tvOptions->Items->GetFirstNode()->Selected=true;
    tvOptions->Items->GetFirstNode()->Focused=true;

	SetForegroundWindow(frmSceneProperties->Handle);

#ifdef _LEVEL_EDITOR
	// check window position
	UI.CheckWindowPos(this);
#endif
}
//---------------------------------------------------------------------------

static int JS[3]={1,4,9};

void __fastcall TfrmSceneProperties::SetSceneParams(){
// Build Options
    // Normals & optimization
    m_BuildParams->m_sm_angle            	= seSMAngle->Value;
    m_BuildParams->m_weld_distance       	= seWeldDistance->Value;

    // Light maps
    m_BuildParams->m_lm_pixels_per_meter 	= seLMPixelsPerMeter->Value;
 	m_BuildParams->m_lm_jitter_samples   	= JS[rgLMJitterSamples->ItemIndex];
    m_BuildParams->m_lm_rms				 	= seLMRMS->Value;
    m_BuildParams->m_lm_rms_zero			= seLMRMSZero->Value;

    // Progressive
    m_BuildParams->m_bConvertProgressive 	= cbProgressive->Checked;
    m_BuildParams->m_pm_uv               	= sePMUV->Value;
    m_BuildParams->m_pm_pos              	= sePMPos->Value;
    m_BuildParams->m_pm_curv             	= sePMCurv->Value;
    m_BuildParams->m_pm_borderH_angle	 	= sePM_borderH_angle->Value;
    m_BuildParams->m_pm_borderH_distance 	= sePM_borderH_distance->Value;
    m_BuildParams->m_pm_heuristic		 	= sePM_heuristic->Value;
}

static int JS2[10]={-1,0,-1,-1,1,-1,-1,-1,-1,2};
void __fastcall TfrmSceneProperties::SetEditParams(){
// Build Options
    // Normals & optimization
    seSMAngle->Value               	= m_BuildParams->m_sm_angle;
    seWeldDistance->Value          	= m_BuildParams->m_weld_distance;

    // Light maps
    seLMPixelsPerMeter->Value      	= m_BuildParams->m_lm_pixels_per_meter;
    rgLMJitterSamples->ItemIndex   	= JS2[m_BuildParams->m_lm_jitter_samples];
    seLMRMS->Value				   	= m_BuildParams->m_lm_rms;
	seLMRMSZero->Value				= m_BuildParams->m_lm_rms_zero;

    // Progressive
    cbProgressive->Checked         	= m_BuildParams->m_bConvertProgressive;
    sePMUV->Value                  	= m_BuildParams->m_pm_uv;
    sePMPos->Value                 	= m_BuildParams->m_pm_pos;
    sePMCurv->Value                	= m_BuildParams->m_pm_curv;
    sePM_borderH_angle->Value      	= m_BuildParams->m_pm_borderH_angle;
    sePM_borderH_distance->Value   	= m_BuildParams->m_pm_borderH_distance;
    sePM_heuristic->Value          	= m_BuildParams->m_pm_heuristic;
}

void __fastcall TfrmSceneProperties::btContinueClick(TObject *Sender)
{
#ifdef _LEVEL_EDITOR
	Scene.m_LevelOp.m_FNLevelPath 	= edLevelPath->Text;	// Path
	Scene.m_LevelOp.m_BOPText		= mmText->Text;			// Text
#endif
    SetSceneParams					();
    Close							();
    ModalResult 					= mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::ebCancelClick(TObject *Sender)
{
	SetEditParams();
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::ShowPage(AnsiString& s){
    for(int i=0;i<pcBuildOptions->PageCount;i++){
        if (s==pcBuildOptions->Pages[i]->Caption)
            pcBuildOptions->Pages[i]->TabVisible = true;
        else
            pcBuildOptions->Pages[i]->TabVisible = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::tvOptionsItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
    if (Item) ShowPage(AnsiString(Item->Text));
}
//---------------------------------------------------------------------------


void __fastcall TfrmSceneProperties::mcLMAmbientMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	u32 color = ((TMultiObjColor*)Sender)->Brush->Color;
#ifdef _LEVEL_EDITOR
	if (SelectColorWin(&color))
#else
	if (SelectColorWin(&color,true))
#endif
		((TMultiObjColor*)Sender)->_Set(color);
}
//---------------------------------------------------------------------------










