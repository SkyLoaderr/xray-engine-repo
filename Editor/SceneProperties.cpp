//---------------------------------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "SceneProperties.h"
#include "Scene.h"
#include "FileSystem.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ToolEdit"
#pragma link "ElTree"
#pragma link "multiobj"
#pragma link "Placemnt"
#pragma resource "*.dfm"
TfrmSceneProperties *frmSceneProperties;
//---------------------------------------------------------------------------
__fastcall TfrmSceneProperties::TfrmSceneProperties(TComponent* Owner)
    : TForm(Owner)
{
    pcBuildOptions->ActivePage = tsBuildStages;
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsSceneProps->IniFileName = buf;
}
//---------------------------------------------------------------------------

int __fastcall TfrmSceneProperties::Run()
{
    int mr = 0;
    btOk->Caption = "Continue Build";
    mr = frmSceneProperties->ShowModal();
    btOk->Caption = "Ok";
    return mr;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::FormShow(TObject *Sender)
{
    pcBuildOptions->ActivePage = tsBuildStages;
// Level Name
 	edLevelName->Text       = Scene.m_LevelName;
// Path
    deLevelPath->Text       = Scene.m_FNLevelPath;
// Text
    mmText->Text            = Scene.m_BOPText;

// Build Options
//    cbTesselation->Checked  = !!Scene.m_bTesselate;
//    cbProgressive->Checked  = !!Scene.m_bConvertProgressive;
//    cbLightmaps->Checked    = !!Scene.m_bLightMaps;

//    seMaxEdge->Value        = Scene.m_maxedge;
//    seVBMaxSize->Value      = Scene.m_VB_maxSize;
//    seVBMaxVertices->Value  = Scene.m_VB_maxVertices;
//    sePixelsPerMeter->Value = Scene.m_pixels_per_meter;
//    seMaxSize->Value        = Scene.m_maxsize;
//    seRelevance->Value      = Scene.m_relevance;
//    seViewDist->Value       = Scene.m_viewdist;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::btContinueClick(TObject *Sender)
{
// Level Name
	strcpy(Scene.m_LevelName, edLevelName->Text.c_str());
// Path
	strcpy(Scene.m_FNLevelPath, deLevelPath->Text.c_str());
// Text
	strcpy(Scene.m_BOPText,mmText->Text.c_str());
// Build Options
   Scene.m_bTesselate           = cbTesselation->Checked;
   Scene.m_bConvertProgressive  = cbProgressive->Checked;
   Scene.m_bLightMaps           = cbLightmaps->Checked;

   Scene.m_maxedge              = seMaxEdge->Value;
   Scene.m_VB_maxSize           = seVBMaxSize->Value;
   Scene.m_VB_maxVertices       = seVBMaxVertices->Value;
   Scene.m_pixels_per_meter     = sePixelsPerMeter->Value;
   Scene.m_maxsize              = seMaxSize->Value;
   Scene.m_relevance            = seRelevance->Value;
   Scene.m_viewdist             = seViewDist->Value;
}
//---------------------------------------------------------------------------


void __fastcall TfrmSceneProperties::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) btCancel->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::cbTesselationClick(TObject *Sender)
{
    tsTesselation->TabVisible = cbTesselation->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::cbProgressiveClick(TObject *Sender)
{
    tsProgressive->TabVisible = cbProgressive->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfrmSceneProperties::cbLightmapsClick(TObject *Sender)
{
    tsLightmaps->TabVisible = cbLightmaps->Checked;
}
//---------------------------------------------------------------------------

