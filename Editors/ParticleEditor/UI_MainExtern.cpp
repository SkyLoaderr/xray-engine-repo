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

bool TUI::CommandExt(int _Command, int p1, int p2)
{
	bool bRes = true;
	string256 filebuffer;
	switch (_Command){
    default:
		ELog.DlgMsg( mtError, "Warning: Undefined command: %04d", _Command );
        bRes = false;
    }
    return 	bRes;
}

char* TUI::GetCaption()
{
	return GetEditFileName()[0]?GetEditFileName():"noname";
}

bool __fastcall TUI::ApplyShortCutExt(WORD Key, TShiftState Shift)
{
/*
	bool bExec = false;
    if (Shift.Contains(ssCtrl)){
        if (Key==VK_F5)    				{Command(COMMAND_BUILD);                		bExec=true;}
        else if (Key==VK_F7)    		{Command(COMMAND_OPTIONS);                      bExec=true;}
        else if (Key=='A')    			{Command(COMMAND_SELECT_ALL);                   bExec=true;}
        else if (Key=='I')    			{Command(COMMAND_INVERT_SELECTION_ALL);         bExec=true;}
       	else if (Key=='1') 	 			{Command(COMMAND_CHANGE_TARGET, etGroup);       bExec=true;}
		else if (Key=='2')				{Command(COMMAND_CHANGE_TARGET, etPS);          bExec=true;}
        else if (Key=='3')				{Command(COMMAND_CHANGE_TARGET, etShape);       bExec=true;}
    }else{                                                                              
        if (Shift.Contains(ssAlt)){                                                     
        	if (Key=='F')   			{Command(COMMAND_FILE_MENU);                    bExec=true;}
        }else{                                                                          
            if (Key=='1')     			{Command(COMMAND_CHANGE_TARGET, etObject);      bExec=true;}
        	else if (Key=='2')  		{Command(COMMAND_CHANGE_TARGET, etLight);       bExec=true;}
        	else if (Key=='3')  		{Command(COMMAND_CHANGE_TARGET, etSound);       bExec=true;}
        	else if (Key=='4')  		{Command(COMMAND_CHANGE_TARGET, etEvent);       bExec=true;}
        	else if (Key=='5')  		{Command(COMMAND_CHANGE_TARGET, etGlow);        bExec=true;}
        	else if (Key=='6')  		{Command(COMMAND_CHANGE_TARGET, etDO);          bExec=true;}
        	else if (Key=='7')  		{Command(COMMAND_CHANGE_TARGET, etSpawnPoint);  bExec=true;}
        	else if (Key=='8')  		{Command(COMMAND_CHANGE_TARGET, etWay);         bExec=true;}
        	else if (Key=='9')  		{Command(COMMAND_CHANGE_TARGET, etSector);      bExec=true;}
        	else if (Key=='0')  		{Command(COMMAND_CHANGE_TARGET, etPortal);      bExec=true;}
            // simple press             
        	else if (Key=='W')			{Command(COMMAND_OBJECT_LIST);                  bExec=true;}
        	else if (Key==VK_DELETE)	{Command(COMMAND_DELETE_SELECTION);             bExec=true;}
        	else if (Key==VK_RETURN)	{Command(COMMAND_SHOW_PROPERTIES);              bExec=true;}
            else if (Key==VK_OEM_MINUS)	{Command(COMMAND_HIDE_SEL, FALSE);              bExec=true;}
            else if (Key==VK_OEM_PLUS)	{Command(COMMAND_HIDE_UNSEL, FALSE);            bExec=true;}
            else if (Key==VK_OEM_5)		{Command(COMMAND_HIDE_ALL, TRUE);               bExec=true;}
        	else if (Key=='N'){
            	switch (Tools.GetAction()){
            	case eaMove: 			{Command(COMMAND_SET_NUMERIC_POSITION); bExec=true;} 	break;
			    case eaRotate: 			{Command(COMMAND_SET_NUMERIC_ROTATION); bExec=true;} 	break;
            	case eaScale: 			{Command(COMMAND_SET_NUMERIC_SCALE);    bExec=true;}	break;
            	}
            }
        }
    }
    return bExec;
*/
}
//---------------------------------------------------------------------------

bool __fastcall TUI::ApplyGlobalShortCutExt(WORD Key, TShiftState Shift)
{
/*
	bool bExec = false;
    if (Shift.Contains(ssCtrl)){
        if (Key=='S'){
            if (Shift.Contains(ssAlt))  {Command(COMMAND_SAVEAS);               bExec=true;}
            else                        {Command(COMMAND_SAVE);                 bExec=true;}
        }
        else if (Key=='O')   			{Command(COMMAND_LOAD);                 bExec=true;}
        else if (Key=='N')   			{Command(COMMAND_CLEAR);                bExec=true;}
        else if (Key=='V')    			{Command(COMMAND_PASTE);                bExec=true;}
        else if (Key=='C')    			{Command(COMMAND_COPY);                 bExec=true;}
        else if (Key=='X')    			{Command(COMMAND_CUT);                  bExec=true;}
        else if (Key=='Z')    			{Command(COMMAND_UNDO);                 bExec=true;}
        else if (Key=='Y')    			{Command(COMMAND_REDO);                 bExec=true;}
		else if (Key=='R')				{Command(COMMAND_LOAD_FIRSTRECENT);     bExec=true;}
    }
    return bExec;
*/
}
//---------------------------------------------------------------------------
/*
//---------------------------------------------------------------------------
void TUI::Redraw(){
	VERIFY(m_bReady);               
    if (!psDeviceFlags.is(rsRenderRealTime)) m_Flags.set(flRedraw,FALSE);                                                                      
	if (m_Flags.is(flResize)) Device.Resize(m_D3DWindow->Width,m_D3DWindow->Height); m_Flags.set(flResize,FALSE);
// set render state
    Device.SetRS(D3DRS_TEXTUREFACTOR,	0xffffffff);
    // fog
	float fog_start	= ZFar();
	float fog_end	= ZFar();
	Device.SetRS( D3DRS_FOGCOLOR,		DEFAULT_CLEARCOLOR	);
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
        if( psDeviceFlags.is(rsFilterLinear)){
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
    if (psDeviceFlags.is(rsLighting)) 	Device.SetRS(D3DRS_AMBIENT,0x00000000);
    else                				Device.SetRS(D3DRS_AMBIENT,0xFFFFFFFF);

    try{
        Device.Begin();
        Device.UpdateView();
		Device.ResetMaterial();

        Device.SetRS(D3DRS_FILLMODE, Device.dwFillMode);
		Device.SetRS(D3DRS_SHADEMODE,Device.dwShadeMode);

    // draw grid
    	if (psDeviceFlags.is(rsDrawGrid)){
	        DU::DrawGrid();
    	    DU::DrawPivot(m_Pivot);
        }

        Tools.Render();

    // draw selection rect
		if(m_SelectionRect) DU::DrawSelectionRect(m_SelStart,m_SelEnd);

    // draw axis
        DU::DrawAxis(Device.m_Camera.GetTransform());

    // end draw
        Device.End();
    }catch(...){
		_clear87();
		FPU::m24r();
    	ELog.DlgMsg(mtError, "Critical error has occured in render routine.\nEditor may work incorrectly.");
        Device.End();
		Device.Resize(m_D3DWindow->Width,m_D3DWindow->Height);
    }
}
//---------------------------------------------------------------------------

*/
