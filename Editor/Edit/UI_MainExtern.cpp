//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "UI_Main.h"
#include "UI_Tools.h"
#include "EditLibrary.h"
#include "ImageEditor.h"
#include "topbar.h"
#include "leftbar.h"
#include "scene.h"
#include "EditorPref.h"
#include "D3DUtils.h"
#include "Cursor3D.h"
#include "bottombar.h"
#include "xr_trims.h"
#include "main.h"
#include "xr_input.h"

bool g_bEditorValid = false;

bool TUI::PickGround(Fvector& hitpoint, const Fvector& start, const Fvector& direction, int bSnap){
    // pick object geometry
    if ((bSnap==-1)||(fraTopBar->ebOSnap->Down&&(bSnap==1))){
        bool bPickObject;
        SRayPickInfo pinf;
	    EEditorState est = GetEState();
        switch(est){
        case esEditLibrary:		bPickObject = !!TfrmEditLibrary::RayPick(start,direction,&pinf); break;
        case esEditScene:		bPickObject = !!Scene.RayPick(start,direction,OBJCLASS_SCENEOBJECT,&pinf,false,true); break;
        default: THROW;
        }
        if (bPickObject){
		    if (fraTopBar->ebVSnap->Down&&bSnap){
                Fvector pn;
                float u = pinf.rp_inf.u;
                float v = pinf.rp_inf.v;
                float w = 1-(u+v);
                if ((w>u) && (w>v)) pn.set(pinf.rp_inf.p[0]);
                else if ((u>w) && (u>v)) pn.set(pinf.rp_inf.p[1]);
                else pn.set(pinf.rp_inf.p[2]);
                if (pn.distance_to(pinf.pt) < movesnap()) hitpoint.set(pn);
                else hitpoint.set(pinf.pt);
            }else{
            	hitpoint.set(pinf.pt);
            }
            return true;
        }
    }

    // pick grid
	Fvector normal;
	normal.set( 0, 1, 0 );
	float clcheck = direction.dotproduct( normal );
	if( fis_zero( clcheck ) ) return false;
	float alpha = - start.dotproduct(normal) / clcheck;
	if( alpha <= 0 ) return false;

	hitpoint.x = start.x + direction.x * alpha;
	hitpoint.y = start.y + direction.y * alpha;
	hitpoint.z = start.z + direction.z * alpha;

    if (fraTopBar->ebGSnap->Down && bSnap){
        hitpoint.x = snapto( hitpoint.x, movesnap() );
        hitpoint.z = snapto( hitpoint.z, movesnap() );
        hitpoint.y = 0.f;
    }
	return true;
}
//----------------------------------------------------

bool TUI::SelectionFrustum(CFrustum& frustum){
    Fvector st,d,p[4];
    Ipoint pt[4];

    float depth = 0;

    float x1=m_StartCp.x, x2=m_CurrentCp.x;
    float y1=m_StartCp.y, y2=m_CurrentCp.y;

	if(!(x1!=x2&&y1!=y2)) return false;

	pt[0].set(min(x1,x2),min(y1,y2));
	pt[1].set(max(x1,x2),min(y1,y2));
	pt[2].set(max(x1,x2),max(y1,y2));
	pt[3].set(min(x1,x2),max(y1,y2));

    SRayPickInfo pinf;
    for (int i=0; i<4; i++){
	    Device.m_Camera.MouseRayFromPoint(st, d, pt[i]);
        if (frmEditorPreferences->cbBoxPickLimitedDepth->Checked){
			pinf.rp_inf.range = Device.m_Camera.m_Zfar; // max pick range
            if (Scene.RayPick(st, d, OBJCLASS_SCENEOBJECT, &pinf, false, false))
	            if (pinf.rp_inf.range > depth) depth = pinf.rp_inf.range;
        }
    }
    if (depth<Device.m_Camera.m_Znear) depth = Device.m_Camera.m_Zfar;
    else depth += frmEditorPreferences->seBoxPickDepthTolerance->Value;

    for (i=0; i<4; i++){
	    Device.m_Camera.MouseRayFromPoint(st, d, pt[i]);
        p[i].direct(st,d,depth);
    }

    frustum.CreateFromPoints(p,4);

    Fplane P; P.build(p[0],p[1],p[2]);
    if (P.classify(st)>0) P.build(p[2],p[1],p[0]);
	frustum._add(P);

	return true;
}
//----------------------------------------------------

void TUI::Redraw(){
	if (bResize){ Device.Resize(m_D3DWindow->Width,m_D3DWindow->Height); bResize=false; }
// set render state
    Device.SetRS(D3DRS_TEXTUREFACTOR,	0xffffffff);
    // filter
    for (DWORD k=0; k<HW.Caps.pixel.dwStages; k++){
        if( psDeviceFlags&rsFilterLinear){
            Device.SetTSS(k,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
            Device.SetTSS(k,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
            Device.SetTSS(k,D3DTSS_MIPFILTER,D3DTEXF_LINEAR);
        } else {
            Device.SetTSS(k,D3DTSS_MAGFILTER,D3DTEXF_POINT);
            Device.SetTSS(k,D3DTSS_MINFILTER,D3DTEXF_POINT);
            Device.SetTSS(k,D3DTSS_MIPFILTER,D3DTEXF_POINT);
        }
    }
	// ligthing
    if (psDeviceFlags&rsLighting) 	Device.SetRS(D3DRS_AMBIENT,0x00000000);
    else                			Device.SetRS(D3DRS_AMBIENT,0xFFFFFFFF);

    try{
        Device.Begin();
        Device.UpdateView();
		Device.ResetMaterial();

        Device.SetRS(D3DRS_FILLMODE, Device.dwFillMode);
		Device.SetRS(D3DRS_SHADEMODE,Device.dwShadeMode);

    // draw grid
    	if (psDeviceFlags&rsDrawGrid){
	        DU::DrawGrid();
    	    DU::DrawPivot(m_Pivot);
        }

	    EEditorState est = GetEState();
        switch(est){
        case esEditLibrary: 	TfrmEditLibrary::OnRender(); break;
        case esEditScene:		Scene.Render(&precalc_identity); break;
        }

    // draw selection rect
		if(m_SelectionRect) DU::DrawSelectionRect(m_SelStart,m_SelEnd);

    // draw cursor
        m_Cursor->Render();

    // draw axis
        DU::DrawAxis();

    // end draw
        Device.End();
    }catch(...){
        Device.End();
		Device.Resize(m_D3DWindow->Width,m_D3DWindow->Height);
    }
    if (!(psDeviceFlags&rsRenderRealTime)) bRedraw = false;
	fraBottomBar->paSel->Caption = AnsiString(AnsiString(" Sel: ")+AnsiString(Scene.SelectionCount(true,Tools.CurrentClassID())));
}
//---------------------------------------------------------------------------
void TUI::Idle()
{
    pInput->OnFrame();
    if (g_ErrorMode) return;
//    ELog.Msg(mtInformation,"%f",Device.m_FrameDTime);
    Sleep(5);
	Device.UpdateTimer();
    EEditorState est = GetEState();
    if ((est==esEditScene)||(est==esEditLibrary)||(est==esEditImages)){
	    if (bUpdateScene) RealUpdateScene();
    	if (bRedraw){
            Scene.Update(Device.fTimeDelta);
        	Redraw();
        }
		Tools.Update();
    }
        // show hint
    ShowObjectHint();

	ResetBreak();
}
//---------------------------------------------------------------------------
void TUI::RealUpdateScene(){
	if (GetEState()==esEditScene){
	    Scene.OnObjectsUpdate();
    	Tools.OnObjectsUpdate(); // обновить все что как-то связано с объектами
	    RedrawScene();
    }
    bUpdateScene = false;
}
//---------------------------------------------------------------------------

void ResetActionToSelect()
{
    UI.Command(COMMAND_CHANGE_ACTION, eaSelect);
}
//---------------------------------------------------------------------------



static int iLastMouseX=0;
static int iLastMouseY=0;
static int iDeltaMouseX=0;
static int iDeltaMouseY=0;
static POINT pt;
static THintWindow* pHintWindow=0;
static AnsiString LastHint="";

bool TUI::ShowHint(const AStringVec& SS){
    if (SS.size()){
        AnsiString S=ListToSequence2(SS);
        if (S==LastHint) return false;
        LastHint = S;
        bHintShowing = true;
        if (!pHintWindow){
            pHintWindow = new THintWindow(frmMain);
            pHintWindow->Brush->Color = 0x0d9F2FF;
        }
        TRect rect = pHintWindow->CalcHintRect(320,S,0);
        rect.Left+=pt.x;    rect.Top+=pt.y;
        rect.Right+=pt.x;   rect.Bottom+=pt.y;
        pHintWindow->ActivateHint(rect,S);
    }else bHintShowing = false;
    return bHintShowing;
}
//---------------------------------------------------------------------------

void TUI::HideHint(){
    bHintShowing = false;
    _DELETE(pHintWindow);
}
//---------------------------------------------------------------------------

void TUI::ShowObjectHint(){
    if (!fraBottomBar->miShowHint->Checked) return;

    GetCursorPos(&pt);
    TWinControl* ctr = FindVCLWindow(pt);
    if (ctr!=frmMain->D3DWindow) return;

    iDeltaMouseX=pt.x-iLastMouseX;
    iDeltaMouseY=pt.y-iLastMouseY;
    iLastMouseX=pt.x; iLastMouseY=pt.y;

    if (iDeltaMouseX||iDeltaMouseY){
        if(bHintShowing){
            fHintHideTime = 0.f;
            HideHint();
        }
        fHintPauseTime = fHintPause+1.f;
        return;
    }
    if (!iDeltaMouseX&&!iDeltaMouseY&&(fHintPauseTime>fHintPause)){
        fHintPauseTime = fHintPause;
    }
    if (bHintShowing){
        fHintHideTime -= Device.fTimeDelta;
        if (fHintHideTime<0) HideHint();
        return;
    }
    if (fHintPauseTime>fHintPause) return;
    if (fHintPauseTime<0){
        fHintPauseTime = fHintPause+1;
        AStringVec SS;
        Scene.OnShowHint(SS);
        if (ShowHint(SS)) fHintHideTime = fHintHide;
    }else{
        fHintPauseTime -= Device.fTimeDelta;
    }
}
//---------------------------------------------------------------------------

