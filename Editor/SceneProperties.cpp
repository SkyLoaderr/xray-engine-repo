#include "stdafx.h"
#pragma hdrstop

#include "SceneProperties.h"
#include "OneEnvironment.h"
#include "xrLevel.h"
#ifdef _EDITOR
 #include "Scene.h"
 #include "ui_main.h"
 #include "library.h"
 #include "ChoseForm.h"
 #include "EditObject.h"
#else
 #include "Communicate.h"
 #include "FileSystem.h"
 #include "library.h"
 #include "ColorPicker.h"
#endif
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ToolEdit"
#pragma link "ElTree"
#pragma link "CloseBtn"
#pragma link "multi_color"
#pragma link "multi_edit"
#pragma resource "*.dfm"
TfrmSceneProperties *frmSceneProperties=0;
//---------------------------------------------------------------------------
void UpdateMinValue(TMultiObjSpinEdit* c, float min_val, float divers=0.f){
	c->MinValue=min_val+divers;
    if (c->Value<min_val) c->Value=min_val+divers;
}
//---------------------------------------------------------------------------

#ifdef _EDITOR
int frmScenePropertiesRun(b_params* params, bool bRunBuild){
#else
extern "C" __declspec(dllexport) __cdecl int frmScenePropertiesRun(b_params* params, DWORD version, bool bRunBuild){
	if (version!=XRCL_CURRENT_VERSION){
    	Log->DlgMsg(mtError,"Incorrect version of level options dll.");
    	return mrCancel;
    }
#endif
	frmSceneProperties = new TfrmSceneProperties(0,params);
    int res = frmSceneProperties->Run(params, bRunBuild);
    _DELETE(frmSceneProperties);
    return res;
}

//---------------------------------------------------------------------------
__fastcall TfrmSceneProperties::TfrmSceneProperties(TComponent* Owner, b_params* params)
    : TForm(Owner)
{
    VERIFY(params);
    m_BuildParams = params;
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsSceneProps->IniFileName = buf;
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
#ifdef _EDITOR
    tvOptions->Items->AddChild(root,"Level options");
    tvOptions->Items->AddChild(root,"Level environment");
    tvOptions->Items->AddChild(root,"Level script");
	edLevelName->Text		= Scene->m_LevelOp.m_LevelName;
	deLevelPath->Text		= Scene->m_LevelOp.m_FNLevelPath;
	seCurEnv->Value			= Scene->m_LevelOp.m_CurEnv;
	mmText->Text			= Scene->m_LevelOp.m_BOPText;
	edSkydomeObjectName->Text = Scene->m_LevelOp.m_SkydomeObjName;
	seDOClusterSize->Value	= Scene->m_LevelOp.m_DOClusterSize;
    
    tsLevelScript->Enabled = true;
    tsLevelOptions->Enabled = true;
    tsLevelEnvironment->Enabled = true;
#else
    tsLevelScript->Enabled = false;
    tsLevelOptions->Enabled = false;
    tsLevelEnvironment->Enabled = false;
    SetEditParams();
#endif
    tvOptions->Items->AddChild(root,"Optimizing");
    tvOptions->Items->AddChild(root,"Tesselation");
    tvOptions->Items->AddChild(root,"Lightmaps");
    tvOptions->Items->AddChild(root,"Progressive");
    tvOptions->Items->AddChild(root,"Strippifier");
    tvOptions->Items->AddChild(root,"Subdivision");
    tvOptions->Items->AddChild(root,"Vertex buffers");

    tvOptions->SetFocus();
    tvOptions->Items->GetFirstNode()->Selected=true;
    tvOptions->Items->GetFirstNode()->Focused=true;

	SetForegroundWindow(frmSceneProperties->Handle);
}
//---------------------------------------------------------------------------

static int JS[3]={1,4,9};

void __fastcall TfrmSceneProperties::SetSceneParams(){
// Build Options
    // Subdivision & PVS
    m_BuildParams->m_SS_maxsize        		= seSSMaxSize->Value;
    m_BuildParams->m_SS_merge_coeff      	= seSSMergeCoef->Value;
    m_BuildParams->m_SS_Low		         	= seSSLowLimit->Value;
    m_BuildParams->m_SS_High             	= seSSHighLimit->Value;
    m_BuildParams->m_SS_DedicateCached	 	= seSSDedicatedCashed->Value;

    // Normals & optimization
    m_BuildParams->m_sm_angle            	= seSMAngle->Value;
    m_BuildParams->m_weld_distance       	= seWeldDistance->Value;

    // Tesselation
    m_BuildParams->m_bTesselate          	= cbTesselation->Checked;
    m_BuildParams->m_maxedge             	= seMaxEdge->Value;

    // Vertex buffers
    m_BuildParams->m_VB_maxSize          	= seVBMaxSize->Value*1024;
    m_BuildParams->m_VB_maxVertices      	= seVBMaxVertices->Value;

    // Light maps
    m_BuildParams->m_bLightMaps          	= cbLMLightmaps->Checked;
    m_BuildParams->m_bRadiosity		     	= cbLMRadiosity->Checked;
    m_BuildParams->m_lm_split_angle 	 	= seLMDeflSplitAngle->Value;
    m_BuildParams->m_lm_pixels_per_meter 	= seLMPixelsPerMeter->Value;
    m_BuildParams->m_lm_dither           	= seLMDither->Value;
    m_BuildParams->m_lm_jitter           	= seLMJitter->Value;
    m_BuildParams->m_lm_jitter_samples   	= JS[rgLMJitterSamples->ItemIndex];
    m_BuildParams->m_lm_amb_color.set_windows(mcLMAmbient->Brush->Color);
    m_BuildParams->m_lm_amb_fogness      	= seLMAmbFogness->Value/100.f;
    m_BuildParams->m_lm_rms				 	= seLMRMS->Value;
    m_BuildParams->area_dispersion		 	= seLMAreaDispersion->Value;
    m_BuildParams->area_energy_summary	 	= seLMAreaSummaryEnergy->Value/100.f;
    m_BuildParams->areaDark.set_windows		(mcLMAreaDark->Brush->Color);
//    m_BuildParams->areaGround.set_windows	(mcLMAreaGround->Brush->Color);
    m_BuildParams->area_quality				= rgAreaQuality->ItemIndex;
    m_BuildParams->fuzzy_min				= seLMFuzzyMin->Value;
    m_BuildParams->fuzzy_max				= seLMFuzzyMax->Value;
    m_BuildParams->fuzzy_samples			= seLMFuzzySamples->Value;
    m_BuildParams->fuzzy_enable				= cbLMFuzzyEnabled->Checked;

    // Progressive
    m_BuildParams->m_bConvertProgressive 	= cbProgressive->Checked;
    m_BuildParams->m_pm_uv               	= sePMUV->Value;
    m_BuildParams->m_pm_pos              	= sePMPos->Value;
    m_BuildParams->m_pm_curv             	= sePMCurv->Value;
    m_BuildParams->m_pm_borderH_angle	 	= sePM_borderH_angle->Value;
    m_BuildParams->m_pm_borderH_distance 	= sePM_borderH_distance->Value;
    m_BuildParams->m_pm_heuristic		 	= sePM_heuristic->Value;

    // Strippify
    m_BuildParams->m_bStripify 				= cbStrippify->Checked;
    m_BuildParams->m_vCacheSize			 	= seStripCacheSize->Value;
#ifdef _EDITOR
	UI->Device.UpdateFog();
#endif
}

static int JS2[9]={0,0,1,1,1,1,1,1,2};
void __fastcall TfrmSceneProperties::SetEditParams(){
// Build Options
    // Subdivision & PVS
    seSSMaxSize->Value			   	= m_BuildParams->m_SS_maxsize;
    seSSMergeCoef->Value		   	= m_BuildParams->m_SS_merge_coeff;
    seSSLowLimit->Value            	= m_BuildParams->m_SS_Low;
    seSSHighLimit->Value           	= m_BuildParams->m_SS_High;
	seSSDedicatedCashed->Value	   	= m_BuildParams->m_SS_DedicateCached;

    // Normals & optimization
    seSMAngle->Value               	= m_BuildParams->m_sm_angle;
    seWeldDistance->Value          	= m_BuildParams->m_weld_distance;

    // Tesselation
    cbTesselation->Checked         	= m_BuildParams->m_bTesselate;
    seMaxEdge->Value               	= m_BuildParams->m_maxedge;

    // Vertex buffers
    seVBMaxSize->Value             	= m_BuildParams->m_VB_maxSize/1024;
    seVBMaxVertices->Value         	= m_BuildParams->m_VB_maxVertices;

    // Light maps
    cbLMLightmaps->Checked         	= m_BuildParams->m_bLightMaps;
    cbLMRadiosity->Checked         	= m_BuildParams->m_bRadiosity;
    seLMDeflSplitAngle->Value      	= m_BuildParams->m_lm_split_angle;
    seLMPixelsPerMeter->Value      	= m_BuildParams->m_lm_pixels_per_meter;
    seLMDither->Value              	= m_BuildParams->m_lm_dither;
    seLMJitter->Value              	= m_BuildParams->m_lm_jitter;
    rgLMJitterSamples->ItemIndex   	= JS2[m_BuildParams->m_lm_jitter_samples];
	mcLMAmbient->ObjFirstInit       ( m_BuildParams->m_lm_amb_color.get_windows() );
    seLMAmbFogness->Value          	= m_BuildParams->m_lm_amb_fogness*100.f;
    seLMRMS->Value				   	= m_BuildParams->m_lm_rms;
    seLMAreaDispersion->Value	   	= m_BuildParams->area_dispersion;
	mcLMAreaDark->ObjFirstInit     	( m_BuildParams->areaDark.get_windows() );
//	mcLMAreaGround->ObjFirstInit	( m_BuildParams->areaGround.get_windows() );
    seLMAreaSummaryEnergy->Value   	= m_BuildParams->area_energy_summary*100.f;
    rgAreaQuality->ItemIndex		= m_BuildParams->area_quality;
    seLMFuzzyMin->Value				= m_BuildParams->fuzzy_min;
    seLMFuzzyMax->Value				= m_BuildParams->fuzzy_max;
    seLMFuzzySamples->Value			= m_BuildParams->fuzzy_samples;
    cbLMFuzzyEnabled->Checked		= m_BuildParams->fuzzy_enable;

    // Progressive
    cbProgressive->Checked         	= m_BuildParams->m_bConvertProgressive;
    sePMUV->Value                  	= m_BuildParams->m_pm_uv;
    sePMPos->Value                 	= m_BuildParams->m_pm_pos;
    sePMCurv->Value                	= m_BuildParams->m_pm_curv;
    sePM_borderH_angle->Value      	= m_BuildParams->m_pm_borderH_angle;
    sePM_borderH_distance->Value   	= m_BuildParams->m_pm_borderH_distance;
    sePM_heuristic->Value          	= m_BuildParams->m_pm_heuristic;

    // Strippify
    cbStrippify->Checked		   	= m_BuildParams->m_bStripify;
    seStripCacheSize->Value		   	= m_BuildParams->m_vCacheSize;
}

void __fastcall TfrmSceneProperties::btContinueClick(TObject *Sender)
{
#ifdef _EDITOR
	Scene->m_LevelOp.m_LevelName 	= edLevelName->Text;  	// Level Name
	Scene->m_LevelOp.m_FNLevelPath 	= deLevelPath->Text;	// Path
	Scene->m_LevelOp.m_BOPText		= mmText->Text;			// Text
	Scene->m_LevelOp.m_CurEnv		= seCurEnv->Value;
//	Scene->m_LevelOp.m_ViewDist		= seViewDistance->Value;
//	Scene->m_LevelOp.m_Fogness		= seFogness->Value;
//	Scene->m_LevelOp.m_FogColor.set_windows(mcFogColor->Brush->Color);
//	Scene->m_LevelOp.m_AmbColor.set_windows(mcAmbientColor->Brush->Color);
	Scene->m_LevelOp.m_SkydomeObjName = edSkydomeObjectName->Text;
	Scene->m_LevelOp.m_DOClusterSize= seDOClusterSize->Value;
    Scene->UpdateSkydome();
#endif
    SetSceneParams();
    Close();
    ModalResult = mrOk;
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

void __fastcall TfrmSceneProperties::fsScenePropsRestorePlacement(
      TObject *Sender)
{
#ifdef _EDITOR
    SetSceneParams();
#endif
	UpdateMinValue(seLMFuzzyMax,seLMFuzzyMin->Value); // set min value
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
    if (Item) ShowPage(Item->Text);
}
//---------------------------------------------------------------------------


void __fastcall TfrmSceneProperties::mcLMAmbientMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	DWORD color = ((TMultiObjColor*)Sender)->Brush->Color;
	if (SelectColorWin(&color,&color))
		((TMultiObjColor*)Sender)->_Set(color);
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::ebChooseSkydomeClick(TObject *Sender)
{
#ifdef _EDITOR
	LPCSTR N = TfrmChoseItem::SelectObject(false,true,SKYDOME_FOLDER,0);
	if (!N) return;
	CLibObject* LO = Lib->SearchObject(N);
    if (!LO){
        Log->DlgMsg(mtError,"Object %s can't find in library.",N);
        return ;
    }
	CEditObject* O = LO->GetReference();
    if (!O->IsDynamic()){
    	Log->DlgMsg(mtError,"Non-dynamic models can't be used as Skydome.");
        return ;
    }
	edSkydomeObjectName->Text = LO->GetRefName();
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::ebClearSkydomeClick(TObject *Sender)
{
	edSkydomeObjectName->Text = "";
}
//---------------------------------------------------------------------------
static bool bUpdateMode = false;
void __fastcall TfrmSceneProperties::EnvsUpdate(){
#ifdef _EDITOR
	bUpdateMode = true;
//	SendMessage(sbEnvs->Handle,WM_SETREDRAW,0,0);
	// delete all created forms
    for (FrmEnvIt f_it=m_frmEnvs.begin(); f_it!=m_frmEnvs.end(); f_it++) _DELETE(*f_it);
	// create forms
	m_frmEnvs.resize(Scene->m_LevelOp.m_Envs.size());
    for (f_it=m_frmEnvs.begin(); f_it!=m_frmEnvs.end(); f_it++){
	    *f_it = new TfrmOneEnvironment(sbEnvs);
        (*f_it)->Parent = sbEnvs;
        AnsiString temp; temp.sprintf("Env #%d", f_it-m_frmEnvs.begin());
        (*f_it)->gbEnv->Caption = temp;
        (*f_it)->HideEnv();
    }
    seCurEnv->Value = Scene->m_LevelOp.m_CurEnv;

    EnvIt e_it=Scene->m_LevelOp.m_Envs.begin();
    for (f_it=m_frmEnvs.begin(); f_it!=m_frmEnvs.end(); f_it++, e_it++) 
    	(*f_it)->ShowEnv(e_it);
        
//	SendMessage(sbEnvs->Handle,WM_SETREDRAW,1,0);

	sbEnvs->Refresh();
    for (f_it=m_frmEnvs.begin(); f_it!=m_frmEnvs.end(); f_it++) (*f_it)->Refresh();

	bUpdateMode = false;
#endif
}

void __fastcall TfrmSceneProperties::tsLevelOptionsShow(TObject *Sender)
{
#ifdef _EDITOR
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::tsLevelEnvironmentShow(
      TObject *Sender)
{
#ifdef _EDITOR
	bUpdateMode = true;
	// delete all created forms
    for (FrmEnvIt f_it=m_frmEnvs.begin(); f_it!=m_frmEnvs.end(); f_it++) _DELETE(*f_it);
	// create forms
	m_frmEnvs.resize(Scene->m_LevelOp.m_Envs.size());
    for (f_it=m_frmEnvs.begin(); f_it!=m_frmEnvs.end(); f_it++){
	    *f_it = new TfrmOneEnvironment(sbEnvs);
        (*f_it)->Parent = sbEnvs;
        AnsiString temp; temp.sprintf("Env #%d", f_it-m_frmEnvs.begin());
        (*f_it)->gbEnv->Caption = temp;
        (*f_it)->HideEnv();
    }
    seEnvCount->Value = Scene->m_LevelOp.m_Envs.size();
    seCurEnv->Value = Scene->m_LevelOp.m_CurEnv;

    EnvIt e_it=Scene->m_LevelOp.m_Envs.begin();
    for (f_it=m_frmEnvs.begin(); f_it!=m_frmEnvs.end(); f_it++, e_it++) 
    	(*f_it)->ShowEnv(e_it);
	bUpdateMode = false;
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::seEnvCountChange(TObject *Sender)
{
#ifdef _EDITOR
    seCurEnv->MaxValue = seEnvCount->Value-1;

	if (bUpdateMode) return;
	Scene->m_LevelOp.m_Envs.resize(seEnvCount->Value);
    if (seCurEnv->Value>seCurEnv->MaxValue) seCurEnv->Value=seCurEnv->MaxValue;

    if (m_frmEnvs.size()==Scene->m_LevelOp.m_Envs.size()) return;

    if (m_frmEnvs.size()>Scene->m_LevelOp.m_Envs.size()){
    	// delete unused
        _DELETE(m_frmEnvs.back());
        m_frmEnvs.resize(Scene->m_LevelOp.m_Envs.size());
    }else{
    	// create new form
		m_frmEnvs.resize(Scene->m_LevelOp.m_Envs.size());
        m_frmEnvs.back() = new TfrmOneEnvironment(sbEnvs);;
        m_frmEnvs.back()->Parent = sbEnvs;
        AnsiString temp; temp.sprintf("Env #%d", m_frmEnvs.size()-1);
        m_frmEnvs.back()->gbEnv->Caption = temp;
        m_frmEnvs.back()->HideEnv();
    }
    EnvIt e_it=Scene->m_LevelOp.m_Envs.begin();
    for (FrmEnvIt f_it=m_frmEnvs.begin(); f_it!=m_frmEnvs.end(); f_it++, e_it++) 
    	(*f_it)->ShowEnv(e_it);
#endif
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::seSSLowLimitChange(TObject *Sender)
{
	if (seSSLowLimit->Text.IsEmpty()) return;
    if (seSSHighLimit->Value<seSSLowLimit->Value) seSSHighLimit->Value = seSSLowLimit->Value+24;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::seSSHighLimitChange(TObject *Sender)
{
	if (seSSHighLimit->Text.IsEmpty()) return;
    if (seSSLowLimit->Value>seSSHighLimit->Value) seSSLowLimit->Value = seSSHighLimit->Value-24;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::seLMFuzzyMinLWChange(
      TObject *Sender, int Val)
{
	UpdateMinValue(seLMFuzzyMax,seLMFuzzyMin->Value);
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::seLMFuzzyMinKeyDown(
      TObject *Sender, WORD &Key, TShiftState Shift)
{
	UpdateMinValue(seLMFuzzyMax,seLMFuzzyMin->Value);
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::seLMFuzzyMinExit(TObject *Sender)
{
	UpdateMinValue(seLMFuzzyMax,seLMFuzzyMin->Value);
}
//---------------------------------------------------------------------------


