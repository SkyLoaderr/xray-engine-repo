//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "UI_Main.h"
#include "UI_Tools.h"
#include "topbar.h"
#include "leftbar.h"
#include "EditorPref.h"
#include "D3DUtils.h"
#include "bottombar.h"
#include "xr_trims.h"
#include "main.h"
#include "xr_input.h"

//---------------------------------------------------------------------------
void TUI::Redraw(){
	VERIFY(m_bReady);
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

        Tools.Render();

    // draw selection rect
		if(m_SelectionRect) DU::DrawSelectionRect(m_SelStart,m_SelEnd);

    // draw axis
        DU::DrawAxis();

    // end draw
        Device.End();
    }catch(...){
        Device.End();
		Device.Resize(m_D3DWindow->Width,m_D3DWindow->Height);
    }
    if (!(psDeviceFlags&rsRenderRealTime)) bRedraw = false;
}
//---------------------------------------------------------------------------

void TUI::Idle()
{
	VERIFY(m_bReady);
    pInput->OnFrame();
    if (g_ErrorMode) return;
//    ELog.Msg(mtInformation,"%f",Device.m_FrameDTime);
    Sleep(5);
	Device.UpdateTimer();
    EEditorState est = GetEState();
    if ((est==esEditScene)||(est==esEditLibrary)||(est==esEditImages)){
    	if (bRedraw){
            Tools.Update();
        	Redraw();
        }
    }

	ResetBreak();
}
//---------------------------------------------------------------------------

