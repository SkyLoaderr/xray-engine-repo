//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "BottomBar.h"
#include "leftbar.h"
#include "LogForm.h"
#include "EditorPref.h"
#include "ui_main.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MxMenus"
#pragma link "mxPlacemnt"
#pragma resource "*.dfm"
TfraBottomBar *fraBottomBar=0;
//---------------------------------------------------------------------------
void TUI::SetStatus(LPSTR s){
	VERIFY(m_bReady);
    fraBottomBar->paStatus->Caption=s; fraBottomBar->paStatus->Repaint();
}
void TUI::ProgressInfo(LPCSTR text)
{
	if (text){
		fraBottomBar->paStatus->Caption=fraBottomBar->sProgressTitle+" ("+text+")";
    	fraBottomBar->paStatus->Repaint();
    }
}                                                           
void TUI::ProgressStart(float max_val, const char* text)
{
	VERIFY(m_bReady);
    fraBottomBar->sProgressTitle = text;
	fraBottomBar->paStatus->Caption=text;
    fraBottomBar->paStatus->Repaint();
	fraBottomBar->fMaxVal=max_val;
	fraBottomBar->fStatusProgress=0;
	fraBottomBar->cgProgress->Progress=0;
	fraBottomBar->cgProgress->Visible=true;
}
void TUI::ProgressEnd(){
	VERIFY(m_bReady);
    fraBottomBar->sProgressTitle = "";
	fraBottomBar->paStatus->Caption="";
    fraBottomBar->paStatus->Repaint();
	fraBottomBar->cgProgress->Visible=false;
}
void TUI::ProgressUpdate(float val){
	VERIFY(m_bReady);
	fraBottomBar->fStatusProgress=val;
    if (fraBottomBar->fMaxVal>=0){
    	int new_val = (int)((fraBottomBar->fStatusProgress/fraBottomBar->fMaxVal)*100);
        if (new_val!=fraBottomBar->cgProgress->Progress){
			fraBottomBar->cgProgress->Progress=(int)((fraBottomBar->fStatusProgress/fraBottomBar->fMaxVal)*100);
    	    fraBottomBar->cgProgress->Repaint();
        }
    }
}
void TUI::ProgressInc(const char* info){
	VERIFY(m_bReady);
    ProgressInfo(info);
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
	VERIFY(m_bReady);
    AnsiString s;
	const Fvector& c 	= Device.m_Camera.GetPosition();
	s.sprintf("Cam: %3.1f, %3.1f, %3.1f",c.x,c.y,c.z);
//	const Fvector& hpb 	= Device.m_Camera.GetHPB();
//	s.sprintf(" Cam: %3.1f°, %3.1f°, %3.1f°",rad2deg(hpb.y),rad2deg(hpb.x),rad2deg(hpb.z));
    fraBottomBar->paCamera->Caption=s; fraBottomBar->paCamera->Repaint();
}
//---------------------------------------------------------------------------
void TUI::OutUICursorPos(){
	VERIFY(m_bReady);
    AnsiString s; POINT pt;
    GetCursorPos(&pt);
    s.sprintf("Cur: %d, %d",pt.x,pt.y);
    fraBottomBar->paUICursor->Caption=s; fraBottomBar->paUICursor->Repaint();
}
//---------------------------------------------------------------------------
void TUI::OutGridSize(){
	VERIFY(m_bReady);
    AnsiString s;
    s.sprintf("Grid: %1.1f",float(frmEditorPreferences->seGridSquareSize->Value));
    fraBottomBar->paGridSquareSize->Caption=s; fraBottomBar->paGridSquareSize->Repaint();
}
//---------------------------------------------------------------------------
__fastcall TfraBottomBar::TfraBottomBar(TComponent* Owner)
    : TFrame(Owner)
{
    DEFINE_INI(fsStorage);
}
//---------------------------------------------------------------------------

#define SET_FLAG(F,V){if (V) psDeviceFlags|=(F); else psDeviceFlags&=~(F);}

void __fastcall TfraBottomBar::ClickOptionsMenuItem(TObject *Sender)
{
    TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    if (mi){
        mi->Checked = !mi->Checked;
        if (mi==miDrawGrid)     			SET_FLAG(rsDrawGrid,mi->Checked)
        else if (mi==miRenderFillPoint)		Device.dwFillMode 	= D3DFILL_POINT;
        else if (mi==miRenderFillWireframe)	Device.dwFillMode 	= D3DFILL_WIREFRAME;
        else if (mi==miRenderFillSolid)		Device.dwFillMode 	= D3DFILL_SOLID;
        else if (mi==miRenderShadeFlat)		Device.dwShadeMode	= D3DSHADE_FLAT;
        else if (mi==miRenderShadeGouraud)	Device.dwShadeMode	=D3DSHADE_GOURAUD;
        else if (mi==miRenderWithTextures)	SET_FLAG(rsRenderTextures,mi->Checked)
        else if (mi==miLightScene)  		SET_FLAG(rsLighting,mi->Checked)
        else if (mi==miRenderLinearFilter)	SET_FLAG(rsFilterLinear,mi->Checked)
        else if (mi==miRenderEdgedFaces)	SET_FLAG(rsEdgedFaces,mi->Checked)
        else if (mi==miFog){				SET_FLAG(rsFog,mi->Checked); Device.UpdateFog();}
        else if (mi==miRenderHWTransform){	SET_FLAG(rsForceHWTransform,mi->Checked); UI.Resize(); }
        else if (mi==miRealTime)			SET_FLAG(rsRenderRealTime,mi->Checked)
    }
    UI.RedrawScene();
    UI.Command(COMMAND_UPDATE_TOOLBAR);
}
//---------------------------------------------------------------------------
void __fastcall TfraBottomBar::QualityClick(TObject *Sender)
{
    UI.SetRenderQuality((float)(((TMenuItem*)Sender)->Tag)/100);
    ((TMenuItem*)Sender)->Checked = true;
    UI.Resize();
}
//---------------------------------------------------------------------------
void __fastcall TfraBottomBar::fsStorageRestorePlacement(TObject *Sender)
{
    // fill mode
    if (miRenderFillPoint->Checked) 		Device.dwFillMode=D3DFILL_POINT;
    else if (miRenderFillWireframe->Checked)Device.dwFillMode=D3DFILL_WIREFRAME;
	else if (miRenderFillSolid->Checked)	Device.dwFillMode=D3DFILL_SOLID;
    // shade mode
	if (miRenderShadeFlat->Checked)			Device.dwShadeMode=D3DSHADE_FLAT;
    else if (miRenderShadeGouraud->Checked)	Device.dwShadeMode=D3DSHADE_GOURAUD;
    // hw transform
    SET_FLAG(rsForceHWTransform,miRenderHWTransform->Checked);
    // other render
    SET_FLAG(rsRenderTextures,	miRenderWithTextures->Checked);
    SET_FLAG(rsLighting,		miLightScene->Checked);
    SET_FLAG(rsFilterLinear,	miRenderLinearFilter->Checked);
    SET_FLAG(rsEdgedFaces,		miRenderEdgedFaces->Checked);
    SET_FLAG(rsRenderRealTime,	miRealTime->Checked);
    SET_FLAG(rsFog,				miFog->Checked);
    SET_FLAG(rsDrawGrid,		miDrawGrid->Checked);

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
	UI.Command(COMMAND_BREAK_LAST_OPERATION);
}
//---------------------------------------------------------------------------

void __fastcall TfraBottomBar::ebStatClick(TObject *Sender)
{
	if (ebStat->Down) 	psDeviceFlags |= rsStatistic;
    else				psDeviceFlags &=~rsStatistic;
    UI.RedrawScene();
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

