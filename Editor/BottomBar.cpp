//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "BottomBar.h"
#include "UI_main.h"
#include "leftbar.h"
#include "LogForm.h"
#include "d3dx.h"
#include "statisticform.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma link "Placemnt"
#pragma link "RxMenus"
#pragma link "CGAUGES"
#pragma resource "*.dfm"
TfraBottomBar *fraBottomBar=0;
//---------------------------------------------------------------------------
void TUI::SetStatus(LPSTR s){
    if (g_bEditorValid){
        fraBottomBar->paStatus->Caption=s; fraBottomBar->paStatus->Repaint();
    }
}
void TUI::ProgressStart(float max_val, const char* text){
	fraBottomBar->paStatus->Caption=text;
    fraBottomBar->paStatus->Repaint();
	fraBottomBar->fMaxVal=max_val;
	fraBottomBar->fStatusProgress=0;
	fraBottomBar->cgProgress->Progress=0;
	fraBottomBar->cgProgress->Visible=true;
}
void TUI::ProgressEnd(){
	fraBottomBar->paStatus->Caption="";
    fraBottomBar->paStatus->Repaint();
	fraBottomBar->cgProgress->Visible=false;
}
void TUI::ProgressUpdate(float val){
	fraBottomBar->fStatusProgress=val;
    if (fraBottomBar->fMaxVal>=0){
    	int new_val = (int)((fraBottomBar->fStatusProgress/fraBottomBar->fMaxVal)*100);
        if (new_val!=fraBottomBar->cgProgress->Progress){
			fraBottomBar->cgProgress->Progress=(int)((fraBottomBar->fStatusProgress/fraBottomBar->fMaxVal)*100);
    	    fraBottomBar->cgProgress->Repaint();
        }
    }
}
void TUI::ProgressInc(){
	fraBottomBar->fStatusProgress++;
    if (fraBottomBar->fMaxVal>=0){
    	int val = (int)((fraBottomBar->fStatusProgress/fraBottomBar->fMaxVal)*100);
        if (val!=fraBottomBar->cgProgress->Progress){
			fraBottomBar->cgProgress->Progress=val;
	        fraBottomBar->cgProgress->Repaint();
        }
    }
}
//---------------------------------------------------------------------------
void TUI::OutCameraPos(){
    if (g_bEditorValid){
    	AnsiString s;
        const Fvector& c = Device.m_Camera.GetPosition();
        s.sprintf(" Cam: %3.1f, %3.1f, %3.1f",c.x,c.y,c.z);
        fraBottomBar->paCameraPos->Caption=s; fraBottomBar->paCameraPos->Repaint();
    }
}
//---------------------------------------------------------------------------
void TUI::OutUICursorPos(){
    if (g_bEditorValid){
        AnsiString s; POINT pt;
        GetCursorPos(&pt);
        s.sprintf("Cursor: %d, %d",pt.x,pt.y);
        fraBottomBar->paUICursor->Caption=s; fraBottomBar->paUICursor->Repaint();
    }
}
//---------------------------------------------------------------------------
__fastcall TfraBottomBar::TfraBottomBar(TComponent* Owner)
    : TFrame(Owner)
{
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------
void __fastcall TfraBottomBar::ClickOptionsMenuItem(TObject *Sender)
{
    TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    if (mi){
        mi->Checked = !mi->Checked;
        if (mi==miDrawGrid)     			UI->bDrawGrid    	= mi->Checked;
        else if (mi==miRenderFillPoint)		UI->dwRenderFillMode=D3DFILL_POINT;
        else if (mi==miRenderFillWireframe)	UI->dwRenderFillMode=D3DFILL_WIREFRAME;
        else if (mi==miRenderFillSolid)		UI->dwRenderFillMode=D3DFILL_SOLID;
        else if (mi==miRenderShadeFlat)		UI->dwRenderShadeMode=D3DSHADE_FLAT;
        else if (mi==miRenderShadeGouraud)	UI->dwRenderShadeMode=D3DSHADE_GOURAUD;
        else if (mi==miRenderWithTextures)	UI->bRenderTextures = mi->Checked;
        else if (mi==miLightScene)  		UI->bRenderLights   = mi->Checked;
        else if (mi==miRenderLinearFilter)	UI->bRenderFilter   = mi->Checked;
        else if (mi==miRenderEdgedFaces)	UI->bRenderEdgedFaces =mi->Checked;
        else if (mi==miFog){				UI->bRenderFog = mi->Checked; UI->Device.UpdateFog();}
        else if (mi==miRenderHWTransform){	UI->dwRenderHWTransform = mi->Checked?D3DX_DEFAULT:D3DX_HWLEVEL_RASTER;}
        else if (mi==miRealTime)			UI->bRenderRealTime = mi->Checked;
    }
    UI->RedrawScene();
    UI->Command(COMMAND_UPDATE_TOOLBAR);
}
//---------------------------------------------------------------------------
void __fastcall TfraBottomBar::QualityClick(TObject *Sender)
{
    UI->SetRenderQuality((float)(((TMenuItem*)Sender)->Tag)/100);
    ((TMenuItem*)Sender)->Checked = true;
    UI->Resize();
}
//---------------------------------------------------------------------------
void __fastcall TfraBottomBar::fsStorageRestorePlacement(TObject *Sender)
{
    // fill mode
    if (miRenderFillPoint->Checked) 		UI->dwRenderFillMode=D3DFILL_POINT;
    else if (miRenderFillWireframe->Checked)UI->dwRenderFillMode=D3DFILL_WIREFRAME;
	else if (miRenderFillSolid->Checked)	UI->dwRenderFillMode=D3DFILL_SOLID;
    // shade mode
	if (miRenderShadeFlat->Checked)			UI->dwRenderShadeMode=D3DSHADE_FLAT;
    else if (miRenderShadeGouraud->Checked)	UI->dwRenderShadeMode=D3DSHADE_GOURAUD;
    // hw transform
	if (miRenderHWTransform->Checked)		UI->dwRenderHWTransform=D3DX_DEFAULT;
    else 									UI->dwRenderHWTransform=D3DX_HWLEVEL_RASTER;
    // other render
    UI->bRenderTextures 	= miRenderWithTextures->Checked;
    UI->bRenderLights   	= miLightScene->Checked;
    UI->bRenderFilter   	= miRenderLinearFilter->Checked;
    UI->bRenderEdgedFaces	= miRenderEdgedFaces->Checked;
    UI->bRenderRealTime 	= miRealTime->Checked;
    UI->bRenderFog			= miFog->Checked;
    // other draw
    UI->bDrawGrid       	= miDrawGrid->Checked;
    // quality
    if 		(N200->Checked)	QualityClick(N200);
    else if (N150->Checked)	QualityClick(N150);
    else if (N125->Checked)	QualityClick(N125);
    else if (N100->Checked)	QualityClick(N100);
    else if (N75->Checked)	QualityClick(N75);
    else if (N50->Checked)	QualityClick(N50);
    else if (N25->Checked)	QualityClick(N25);
    // statistic
    ebStatClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraBottomBar::ebLogClick(TObject *Sender)
{
	TfrmLog::ChangeVisible();
}
//---------------------------------------------------------------------------

void __fastcall TfraBottomBar::ebStopClick(TObject *Sender)
{
	UI->Command(COMMAND_BREAK_LAST_OPERATION);	
}
//---------------------------------------------------------------------------

void __fastcall TfraBottomBar::ebStatClick(TObject *Sender)
{
	if (ebStat->Down) 	TfrmStatistic::ShowStatistic();
    else				TfrmStatistic::HideStatistic();
}
//---------------------------------------------------------------------------

void __fastcall TfraBottomBar::ebOptionsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    POINT pt;
    GetCursorPos(&pt);
    pmOptions->Popup(pt.x,pt.y);
    TExtBtn* btn = dynamic_cast<TExtBtn*>(Sender); VERIFY(btn); btn->MouseManualUp();
}
//---------------------------------------------------------------------------

