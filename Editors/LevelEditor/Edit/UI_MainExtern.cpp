//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "UI_Tools.h"
#include "EditLibrary.h"
#include "EditLightAnim.h"
#include "ImageEditor.h"
#include "topbar.h"
#include "leftbar.h"
#include "scene.h"
#include "sceneobject.h"
#include "EditorPref.h"
#include "Cursor3D.h"
#include "bottombar.h"
#include "xr_trims.h"
#include "main.h"
#include "xr_input.h"
#include "ui_main.h"
#include "d3dutils.h"
#include "frustum.h"
#include "render.h"


bool TUI::PickGround(Fvector& hitpoint, const Fvector& start, const Fvector& direction, int bSnap, Fvector* hitnormal){
	VERIFY(m_bReady);
    // pick object geometry
    if ((bSnap==-1)||(fraTopBar->ebOSnap->Down&&(bSnap==1))){
        bool bPickObject;
        SRayPickInfo pinf;
	    EEditorState est = GetEState();
        switch(est){
        case esEditLibrary:		bPickObject = !!TfrmEditLibrary::RayPick(start,direction,&pinf); break;
        case esEditScene:		bPickObject = !!Scene.RayPick(start,direction,OBJCLASS_SCENEOBJECT,&pinf,false,Scene.GetSnapList()); break;
        default: return false;
        }
        if (bPickObject){
		    if (fraTopBar->ebVSnap->Down&&bSnap){
                Fvector pn;
                float u = pinf.inf.u;
                float v = pinf.inf.v;
                float w = 1-(u+v);
				Fvector verts[3];
                pinf.s_obj->GetFaceWorld(pinf.e_mesh,pinf.inf.id,verts);
                if ((w>u) && (w>v)) pn.set(verts[0]);
                else if ((u>w) && (u>v)) pn.set(verts[1]);
                else pn.set(verts[2]);
                if (pn.distance_to(pinf.pt) < movesnap()) hitpoint.set(pn);
                else hitpoint.set(pinf.pt);
            }else{
            	hitpoint.set(pinf.pt);
            }
            if (hitnormal){
	            Fvector verts[3];
    	        pinf.s_obj->GetFaceWorld(pinf.e_mesh,pinf.inf.id,verts);
        	    hitnormal->mknormal(verts[0],verts[1],verts[2]);
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
	if (hitnormal) hitnormal->set(0,1,0);
	return true;
}
//----------------------------------------------------

bool TUI::SelectionFrustum(CFrustum& frustum){
	VERIFY(m_bReady);
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
			pinf.inf.range = Device.m_Camera.m_Zfar; // max pick range
            if (Scene.RayPick(st, d, OBJCLASS_SCENEOBJECT, &pinf, false, 0))
	            if (pinf.inf.range > depth) depth = pinf.inf.range;
        }
    }
    if (depth<Device.m_Camera.m_Znear) depth = Device.m_Camera.m_Zfar;
    else depth += frmEditorPreferences->seBoxPickDepthTolerance->Value;

    for (i=0; i<4; i++){
	    Device.m_Camera.MouseRayFromPoint(st, d, pt[i]);
        p[i].mad(st,d,depth);
    }

    frustum.CreateFromPoints(p,4,Device.m_Camera.m_Position);

    Fplane P; P.build(p[0],p[1],p[2]);
    if (P.classify(st)>0) P.build(p[2],p[1],p[0]);
	frustum._add(P);

	return true;
}
//----------------------------------------------------
void TUI::Redraw(){
	VERIFY(m_bReady);
    if (!(psDeviceFlags&rsRenderRealTime)) bRedraw = false;
	if (bResize){ Device.Resize(m_D3DWindow->Width,m_D3DWindow->Height); bResize=false; }
// set render state
    Device.SetRS(D3DRS_TEXTUREFACTOR,	0xffffffff);
    // fog
	st_Environment& E	= Scene.m_LevelOp.m_Envs[Scene.m_LevelOp.m_CurEnv];
	float fog_start	= (psDeviceFlags&rsFog)?(1.0f - E.m_Fogness)* 0.85f * E.m_ViewDist:ZFar();
	float fog_end	= (psDeviceFlags&rsFog)?0.91f * E.m_ViewDist:ZFar();
	Device.SetRS( D3DRS_FOGCOLOR,	E.m_FogColor.get());
	Device.SetRS( D3DRS_RANGEFOGENABLE,	FALSE				);
	if (HW.Caps.bTableFog)	{
		Device.SetRS( D3DRS_FOGTABLEMODE,	D3DFOG_LINEAR 	);
		Device.SetRS( D3DRS_FOGVERTEXMODE,	D3DFOG_NONE	 	);
	} else {
		Device.SetRS( D3DRS_FOGTABLEMODE,	D3DFOG_NONE	 	);
		Device.SetRS( D3DRS_FOGVERTEXMODE,	D3DFOG_LINEAR	);
	}
	Device.SetRS( D3DRS_FOGSTART,	*(DWORD *)(&fog_start)	);
	Device.SetRS( D3DRS_FOGEND,		*(DWORD *)(&fog_end)	);
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
    	Device.Statistic.RenderDUMP_RT.Begin();
        Device.Begin();
        Device.UpdateView();
		Device.ResetMaterial();

        Device.SetRS(D3DRS_FILLMODE, Device.dwFillMode);
		Device.SetRS(D3DRS_SHADEMODE,Device.dwShadeMode);

        // draw sky
	    EEditorState est = GetEState();
        switch(est){
        case esEditLightAnim:
        case esEditScene:		Scene.RenderSky(Device.m_Camera.GetTransform()); break;
        }

    // draw grid
    	if (psDeviceFlags&rsDrawGrid){
	        DU::DrawGrid();
    	    DU::DrawPivot(m_Pivot);
        }

        switch(est){
        case esEditLibrary: 	TfrmEditLibrary::OnRender(); break;
        case esEditLightAnim:
        case esEditScene:		Scene.Render(Device.m_Camera.GetTransform()); break;
        }

    // draw selection rect
		if(m_SelectionRect) DU::DrawSelectionRect(m_SelStart,m_SelEnd);

    // draw cursor
        m_Cursor->Render();

    // draw axis
        DU::DrawAxis(Device.m_Camera.GetTransform());

    // end draw
        Device.End();
    	Device.Statistic.RenderDUMP_RT.End();
    }
    catch(...)
    {
		_clear87();
		FPU::m24r();
    	ELog.DlgMsg(mtError, "Critical error has occured in render routine.\nEditor may work incorrectly.");
        Device.End();
		Device.Resize(m_D3DWindow->Width,m_D3DWindow->Height);
    }

	fraBottomBar->paSel->Caption = AnsiString(AnsiString(" Sel: ")+AnsiString(Scene.SelectionCount(true,Tools.CurrentClassID())));
}
//---------------------------------------------------------------------------
void TUI::Idle()
{
	VERIFY(m_bReady);
	// reset fpu
	// input
    pInput->OnFrame();
    if (g_ErrorMode) return;
    Sleep(1);
	Device.UpdateTimer();
    EEditorState est = GetEState();
    if ((est==esEditScene)||(est==esEditLibrary)||(est==esEditLightAnim)){
	    if (bUpdateScene) RealUpdateScene();
    	if (bRedraw){
            Scene.Update(Device.fTimeDelta);
            Render->Calculate	();
            Render->Render		();
        	Redraw();
        }
		Tools.Update();
        if (est==esEditLightAnim) TfrmEditLightAnim::OnIdle();
    }
        // show hint
    ShowObjectHint	();

	ResetBreak		();
	// check mail    
    CheckMailslot	();
    if (bNeedQuit) 	frmMain->Close();
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


void TUI::ShowContextMenu(int cls)
{
	VERIFY(m_bReady);
    POINT pt;
    GetCursorPos(&pt);
    fraLeftBar->miProperties->Enabled = false;
    if (Scene.SelectionCount( true, (EObjClass)cls )) fraLeftBar->miProperties->Enabled = true;
    RedrawScene(true);
    fraLeftBar->pmObjectContext->TrackButton = tbRightButton;
    fraLeftBar->pmObjectContext->Popup(pt.x,pt.y);
}

void ResetActionToSelect()
{
    UI.Command(COMMAND_CHANGE_ACTION, eaSelect);
}
//---------------------------------------------------------------------------

