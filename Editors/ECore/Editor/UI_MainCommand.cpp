//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ui_main.h"
#include "UI_ToolsCustom.h"

#include "ImageEditor.h"
#include "SoundEditor.h"
#include "d3dutils.h"

#include "PSLibrary.h"
#include "Library.h"
#include "LightAnimLibrary.h"

#include "ImageManager.h"
#include "SoundManager.h"
#include "ResourceManager.h"
#include "igame_persistent.h"

ECommandVec ECommands;
BOOL 		bAllowReceiveCommand	= FALSE;

ECommandVec&  GetEditorCommands()
{
	return 	ECommands;
}
void 	EnableReceiveCommands()
{
	bAllowReceiveCommand = TRUE;
}
u32 	ExecCommand		(u32 cmd, u32 p1, u32 p2)
{
	if (!bAllowReceiveCommand)	return 0;

	VERIFY		(cmd<ECommands.size());
	SECommand*	CMD = ECommands[cmd];
    VERIFY		(CMD&&!CMD->command.empty());
    u32 res		= TRUE;
    CMD->command(p1,p2,res);
//?	UI->RedrawScene();
    return		res;
}
void	RegisterCommand (u32 cmd, SECommand* cmd_impl)
{
	if (cmd>=ECommands.size()) 
    	ECommands.resize(cmd+1,0);
	SECommand*&	CMD = ECommands[cmd];
    if (CMD){
    	Msg			("RegisterCommand: command '%s' overridden by command '%s'.",*CMD->caption,*cmd_impl->caption);
    	xr_delete	(CMD);
    }
    CMD	   			= cmd_impl;
}
void	TUI::ClearCommands ()
{
	for (ECommandVecIt it=ECommands.begin(); it!=ECommands.end(); it++)
    	xr_delete	(*it);
	ECommands.clear	();
}

//------------------------------------------------------------------------------
// UI Commands
//------------------------------------------------------------------------------
void 	TUI::CommandRenderFocus(u32 p1, u32 p2, u32& res)
{
    if (((TForm*)m_D3DWindow->Owner)->Visible&&m_bReady)
        m_D3DWindow->SetFocus();
}
void 	TUI::CommandBreakLastOperation(u32 p1, u32 p2, u32& res)
{
    if (mrYes==ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure to break current action?")){
        NeedBreak	();
        ELog.Msg	(mtInformation,"Execution canceled.");
    }
}
void 	TUI::CommandRenderResize(u32 p1, u32 p2, u32& res)
{
    if (psDeviceFlags.is(rsDrawSafeRect)){
        int w=m_D3DPanel->Width,h=m_D3DPanel->Height,w_2=w/2,h_2=h/2;
        Irect rect;
        if ((0.75f*float(w))>float(h)) 	rect.set(w_2-1.33f*float(h_2),0,1.33f*h,h);
        else                   			rect.set(0,h_2-0.75f*float(w_2),w,0.75f*w);
        m_D3DWindow->Left  	= rect.x1;
        m_D3DWindow->Top  	= rect.y1;
        m_D3DWindow->Width 	= rect.x2;
        m_D3DWindow->Height	= rect.y2;
    }else{
        m_D3DWindow->Left  	= 0;
        m_D3DWindow->Top  	= 0;
        m_D3DWindow->Width 	= m_D3DPanel->Width;
        m_D3DWindow->Height	= m_D3DPanel->Height;
    }
    UI->RedrawScene		();
}

//------------------------------------------------------------------------------
// Common Commands
//------------------------------------------------------------------------------
void 	CommandInitialize(u32 p1, u32 p2, u32& res)
{
    Engine.Initialize	();
    // make interface
    //----------------
    EPrefs.OnCreate		();
    if (UI->OnCreate((TD3DWindow*)p1,(TPanel*)p2)){
        ExecCommand		(COMMAND_CREATE_SOUND_LIB);	R_ASSERT(SndLib);
        SndLib->OnCreate();
        g_pGamePersistent= xr_new<IGame_Persistent>();
        Lib.OnCreate	();
        LALib.OnCreate	();
        if (Tools->OnCreate()){
            Device.seqAppStart.Process(rp_AppStart);
            ExecCommand	(COMMAND_RESTORE_UI_BAR);
            ExecCommand	(COMMAND_REFRESH_UI_BAR);
            ExecCommand	(COMMAND_CLEAR);
            ExecCommand	(COMMAND_RENDER_FOCUS);
            ExecCommand	(COMMAND_CHANGE_ACTION, etaSelect);
            ExecCommand	(COMMAND_RENDER_RESIZE);
        }else{
        	res			= FALSE;
        }
    }else{
        res 			= FALSE;
    }
}             
void 	CommandDestroy(u32 p1, u32 p2, u32& res)
{
    ExecCommand			(COMMAND_SAVE_UI_BAR);
    EPrefs.OnDestroy	();
    ExecCommand			(COMMAND_CLEAR);
    Device.seqAppEnd.Process(rp_AppEnd);
    xr_delete			(g_pGamePersistent);
    LALib.OnDestroy		();
    Tools->OnDestroy	();
    SndLib->OnDestroy	();
    xr_delete			(SndLib);
    Lib.OnDestroy		();
    UI->OnDestroy		();
    Engine.Destroy		();
}             
void 	CommandQuit(u32 p1, u32 p2, u32& res)
{
    UI->Quit			();
}             
void 	CommandEditorPrefs(u32 p1, u32 p2, u32& res)
{
    EPrefs.Edit			();
}             
void 	CommandChangeAction(u32 p1, u32 p2, u32& res)
{
    Tools->SetAction	(ETAction(p1));
}             
void 	CommandChangeAxis(u32 p1, u32 p2, u32& res)
{
    Tools->SetAxis	(ETAxis(p1));
}             
void 	CommandChangeSettings(u32 p1, u32 p2, u32& res)
{
	Tools->SetSettings(p1,p2);
}             
void 	CommandSoundEditor(u32 p1, u32 p2, u32& res)
{
    TfrmSoundLib::EditLib(AnsiString("Sound Editor"));
}
void 	CommandSyncSounds(u32 p1, u32 p2, u32& res)
{
    if (ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure to synchronize sounds?")==mrYes)
        SndLib->RefreshSounds(true);
}
void 	CommandImageEditor(u32 p1, u32 p2, u32& res)
{
    TfrmImageLib::EditLib(AnsiString("Image Editor"));
}
void 	CommandCheckTextures(u32 p1, u32 p2, u32& res)
{
    TfrmImageLib::ImportTextures();
}
void 	CommandRefreshTextures(u32 p1, u32 p2, u32& res)
{
    if (ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure to synchronize textures?")==mrYes)
        ImageLib.RefreshTextures(0);
}
void 	CommandReloadTextures(u32 p1, u32 p2, u32& res)
{
    Device.ReloadTextures();
    UI->RedrawScene		();
}
void 	CommandChangeSnap(u32 p1, u32 p2, u32& res)
{
    ((TExtBtn*)p1)->Down = !((TExtBtn*)p1)->Down;
}
void 	CommandUnloadTextures(u32 p1, u32 p2, u32& res)
{
    Device.UnloadTextures();
}
void 	CommandEvictObjects(u32 p1, u32 p2, u32& res)
{
    Lib.EvictObjects();
}
void 	CommandEvictTextures(u32 p1, u32 p2, u32& res)
{
    Device.Resources->Evict();
}
void 	CommandCheckModified(u32 p1, u32 p2, u32& res)
{
    res = Tools->IsModified();
}
void 	CommandExit(u32 p1, u32 p2, u32& res)
{
    res = Tools->IfModified();
}
void 	CommandShowProperties(u32 p1, u32 p2, u32& res)
{
    Tools->ShowProperties();
}
void 	CommandUpdateProperties(u32 p1, u32 p2, u32& res)
{
    Tools->UpdateProperties((bool)p1);
}
void 	CommandRefreshProperties(u32 p1, u32 p2, u32& res)
{
    Tools->RefreshProperties();
}
void 	CommandZoomExtents(u32 p1, u32 p2, u32& res)
{
    Tools->ZoomObject	(p1);
    UI->RedrawScene		();
}
void 	CommandRenderWire(u32 p1, u32 p2, u32& res)
{
    if (p1)	Device.dwFillMode 	= D3DFILL_WIREFRAME;
    else 	Device.dwFillMode 	= D3DFILL_SOLID;
    UI->RedrawScene		();
}
void 	CommandToggleSafeRect(u32 p1, u32 p2, u32& res)
{
    psDeviceFlags.set	(rsDrawSafeRect,!psDeviceFlags.is(rsDrawSafeRect));
    ExecCommand			(COMMAND_RENDER_RESIZE);
    UI->RedrawScene		();
}
void 	CommandToggleGrid(u32 p1, u32 p2, u32& res)
{
    psDeviceFlags.set(rsDrawGrid,!psDeviceFlags.is(rsDrawGrid));
    UI->RedrawScene		();
}
void 	CommandUpdateGrid(u32 p1, u32 p2, u32& res)
{
    DU.UpdateGrid		(EPrefs.grid_cell_count,EPrefs.grid_cell_size);
    UI->OutGridSize		();
    UI->RedrawScene		();
}
void 	CommandGridNumberOfSlots(u32 p1, u32 p2, u32& res)
{
    if (p1)	EPrefs.grid_cell_count += 2;
    else	EPrefs.grid_cell_count -= 2;
    ExecCommand			(COMMAND_UPDATE_GRID);
    UI->RedrawScene		();
}
void 	CommandGridSlotSize(u32 p1, u32 p2, u32& res)
{
    float step = 1.f;
    float val = EPrefs.grid_cell_size;
    if (p1){
        if (val<1) step/=10.f;
        EPrefs.grid_cell_size += step;
    }else{
        if (fsimilar(val,1.f)||(val<1)) step/=10.f;
        EPrefs.grid_cell_size -= step;
    }
    ExecCommand			(COMMAND_UPDATE_GRID);
    UI->RedrawScene		();
}
void 	CommandCreateSoundLib(u32 p1, u32 p2, u32& res)
{
    SndLib		= xr_new<CSoundManager>();
}
void 	CommandMuteSound(u32 p1, u32 p2, u32& res)
{
    SndLib->MuteSounds(p1);
}

void TUI::RegisterCommands()
{
	RegisterCommand(COMMAND_INITIALIZE,				xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandInitialize)));
	RegisterCommand(COMMAND_DESTROY,        		xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandDestroy)));
	RegisterCommand(COMMAND_EXIT,               	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandExit)));
	RegisterCommand(COMMAND_QUIT,           		xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandQuit)));
	RegisterCommand(COMMAND_EDITOR_PREF,    		xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandEditorPrefs)));
	RegisterCommand(COMMAND_CHANGE_ACTION,  		xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandChangeAction)));
	RegisterCommand(COMMAND_CHANGE_AXIS,    		xr_new<SECommand>("ChangeAxis","X,Y,Z,XZ",true,BIND_CMD_EVENT_S(CommandChangeAxis)));
	RegisterCommand(COMMAND_CHANGE_SETTINGS,		xr_new<SECommand>("ChangePreference","",true,BIND_CMD_EVENT_S(CommandChangeSettings)));
	RegisterCommand(COMMAND_SOUND_EDITOR,   		xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandSoundEditor)));
	RegisterCommand(COMMAND_SYNC_SOUNDS,    		xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandSyncSounds)));
    RegisterCommand(COMMAND_IMAGE_EDITOR,   		xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandImageEditor)));
	RegisterCommand(COMMAND_CHECK_TEXTURES,     	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandCheckTextures)));
	RegisterCommand(COMMAND_REFRESH_TEXTURES,   	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandRefreshTextures)));	
	RegisterCommand(COMMAND_RELOAD_TEXTURES,    	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandReloadTextures)));
	RegisterCommand(COMMAND_CHANGE_SNAP,        	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandChangeSnap)));
    RegisterCommand(COMMAND_UNLOAD_TEXTURES,    	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandUnloadTextures)));
    RegisterCommand(COMMAND_EVICT_OBJECTS,      	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandEvictObjects)));
    RegisterCommand(COMMAND_EVICT_TEXTURES,     	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandEvictTextures)));
    RegisterCommand(COMMAND_CHECK_MODIFIED,     	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandCheckModified)));
	RegisterCommand(COMMAND_SHOW_PROPERTIES,    	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandShowProperties)));
	RegisterCommand(COMMAND_UPDATE_PROPERTIES,  	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandUpdateProperties)));
	RegisterCommand(COMMAND_REFRESH_PROPERTIES, 	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandRefreshProperties)));
	RegisterCommand(COMMAND_ZOOM_EXTENTS,       	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandZoomExtents)));
    RegisterCommand(COMMAND_RENDER_WIRE,        	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandRenderWire)));
    RegisterCommand(COMMAND_RENDER_FOCUS,       	xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(this,TUI::CommandRenderFocus)));
	RegisterCommand(COMMAND_BREAK_LAST_OPERATION,	xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(this,TUI::CommandBreakLastOperation)));
    RegisterCommand(COMMAND_TOGGLE_SAFE_RECT,   	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandToggleSafeRect)));
	RegisterCommand(COMMAND_RENDER_RESIZE,      	xr_new<SECommand>("","",false,BIND_CMD_EVENT_C(this,TUI::CommandRenderResize)));
    RegisterCommand(COMMAND_TOGGLE_GRID,        	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandToggleGrid)));
	RegisterCommand(COMMAND_UPDATE_GRID,        	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandUpdateGrid)));
    RegisterCommand(COMMAND_GRID_NUMBER_OF_SLOTS,	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandGridNumberOfSlots)));
    RegisterCommand(COMMAND_GRID_SLOT_SIZE,     	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandGridSlotSize)));
    RegisterCommand(COMMAND_CREATE_SOUND_LIB,   	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandCreateSoundLib)));
    RegisterCommand(COMMAND_MUTE_SOUND,         	xr_new<SECommand>("","",false,BIND_CMD_EVENT_S(CommandMuteSound)));
}                                                                        

/*
bool TUI::Command( int _Command, int p1, int p2 )
{
	if ((_Command!=COMMAND_INITIALIZE)&&!m_bReady) return false;

    bool bRes 		= true;

    // execute main command
	switch( _Command ){
	case COMMAND_INITIALIZE:{
		Engine.Initialize	();
        // make interface
		//----------------
        EPrefs.OnCreate		();
        if (UI->OnCreate((TD3DWindow*)p1,(TPanel*)p2)){
            Command			(COMMAND_CREATE_SOUND_LIB);	R_ASSERT(SndLib);
            SndLib->OnCreate();
			g_pGamePersistent= xr_new<IGame_Persistent>();
            Lib.OnCreate	();
            LALib.OnCreate	();
			if (!Tools->OnCreate()){
                bRes=false;
            	break;
            }
			Device.seqAppCycleStart.Process(rp_AppCycleStart);
            Command			(COMMAND_RESTORE_UI_BAR);
            Command			(COMMAND_REFRESH_UI_BAR);
		    Command			(COMMAND_CLEAR);
			Command			(COMMAND_RENDER_FOCUS);
			Command			(COMMAND_CHANGE_ACTION, etaSelect);
			Command			(COMMAND_RENDER_RESIZE);
        }else{
        	bRes = false;
        }
    	}break;
	case COMMAND_DESTROY:
        Command				(COMMAND_SAVE_UI_BAR);
        EPrefs.OnDestroy	();
		Command				(COMMAND_CLEAR);
		Device.seqAppCycleEnd.Process(rp_AppCycleEnd);
        xr_delete			(g_pGamePersistent);
        LALib.OnDestroy		();
		Tools->OnDestroy	();
		SndLib->OnDestroy	();
        xr_delete			(SndLib);
		Lib.OnDestroy		();
        UI->OnDestroy		();
		Engine.Destroy		();
		//----------------
    	break;
	case COMMAND_QUIT:
    	Quit();
    	break;
	case COMMAND_EDITOR_PREF:
    	EPrefs.Edit			();
        break;
	case COMMAND_CHANGE_ACTION:
		Tools->SetAction	(ETAction(p1));
        break;
	case COMMAND_CHANGE_AXIS:
    	Tools->SetAxis	(ETAxis(p1));
        break;
	case COMMAND_CHANGE_SETTINGS:
    	Tools->SetSettings(p1,p2);
        break;
	case COMMAND_SOUND_EDITOR:
    	TfrmSoundLib::EditLib(AnsiString("Sound Editor"));
        break;
	case COMMAND_SYNC_SOUNDS:
    	if (ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure to synchronize sounds?")==mrYes)
			SndLib->RefreshSounds(true);
        break;
    case COMMAND_IMAGE_EDITOR:
    	TfrmImageLib::EditLib(AnsiString("Image Editor"));
    	break;
	case COMMAND_CHECK_TEXTURES:
    	TfrmImageLib::ImportTextures();
    	break;
	case COMMAND_REFRESH_TEXTURES:
    	if (ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure to synchronize textures?")==mrYes)
			ImageLib.RefreshTextures(0);
		break;
	case COMMAND_RELOAD_TEXTURES:
    	Device.ReloadTextures();
    	break;
	case COMMAND_CHANGE_SNAP:
        ((TExtBtn*)p1)->Down = !((TExtBtn*)p1)->Down;
        break;
    case COMMAND_UNLOAD_TEXTURES:
		Device.UnloadTextures();
    	break;
    case COMMAND_EVICT_OBJECTS:
    	Lib.EvictObjects();
    	break;
    case COMMAND_EVICT_TEXTURES:
    	Device.Resources->Evict();
    	break;
    case COMMAND_CHECK_MODIFIED:
    	bRes = Tools->IsModified();
		break;
	case COMMAND_EXIT:
    	bRes = Tools->IfModified();
		break;
	case COMMAND_SHOW_PROPERTIES:
        Tools->ShowProperties();
        break;
	case COMMAND_UPDATE_PROPERTIES:
        Tools->UpdateProperties((bool)p1);
        break;
	case COMMAND_REFRESH_PROPERTIES:
        Tools->RefreshProperties();
        break;
	case COMMAND_ZOOM_EXTENTS:
		Tools->ZoomObject(p1);
    	break;
    case COMMAND_RENDER_WIRE:
    	if (p1)	Device.dwFillMode 	= D3DFILL_WIREFRAME;
        else 	Device.dwFillMode 	= D3DFILL_SOLID;
	    break;
    case COMMAND_RENDER_FOCUS:
		if (((TForm*)m_D3DWindow->Owner)->Visible&&m_bReady)
        {
        	m_D3DWindow->SetFocus();
        }
    	break;
	case COMMAND_BREAK_LAST_OPERATION:
        if (mrYes==ELog.DlgMsg(mtConfirmation,TMsgDlgButtons() << mbYes << mbNo,"Are you sure to break current action?")){
            NeedBreak	();
            ELog.Msg	(mtInformation,"Execution canceled.");
        }
    	break;
    case COMMAND_TOGGLE_SAFE_RECT:
    	psDeviceFlags.set	(rsDrawSafeRect,!psDeviceFlags.is(rsDrawSafeRect));
        Command				(COMMAND_RENDER_RESIZE);
    	break;
	case COMMAND_RENDER_RESIZE:
        if (psDeviceFlags.is(rsDrawSafeRect)){
            int w=m_D3DPanel->Width,h=m_D3DPanel->Height,w_2=w/2,h_2=h/2;
            Irect rect;
            if ((0.75f*float(w))>float(h)) 	rect.set(w_2-1.33f*float(h_2),0,1.33f*h,h);
            else                   			rect.set(0,h_2-0.75f*float(w_2),w,0.75f*w);
            m_D3DWindow->Left  	= rect.x1;
            m_D3DWindow->Top  	= rect.y1;
            m_D3DWindow->Width 	= rect.x2;
            m_D3DWindow->Height	= rect.y2;
        }else{
            m_D3DWindow->Left  	= 0;
            m_D3DWindow->Top  	= 0;
            m_D3DWindow->Width 	= m_D3DPanel->Width;
            m_D3DWindow->Height	= m_D3DPanel->Height;
        }
    	break;
    case COMMAND_TOGGLE_GRID:
    	psDeviceFlags.set(rsDrawGrid,!psDeviceFlags.is(rsDrawGrid));
    	break;
	case COMMAND_UPDATE_GRID:
    	DU.UpdateGrid(EPrefs.grid_cell_count,EPrefs.grid_cell_size);
	    OutGridSize();
    	break;
    case COMMAND_GRID_NUMBER_OF_SLOTS:
    	if (p1)	EPrefs.grid_cell_count += 2;
        else	EPrefs.grid_cell_count -= 2;
        Command(COMMAND_UPDATE_GRID);
    	break;
    case COMMAND_GRID_SLOT_SIZE:{
    	float step = 1.f;
        float val = EPrefs.grid_cell_size;
    	if (p1){
	    	if (val<1) step/=10.f;
        	EPrefs.grid_cell_size += step;
        }else{
	    	if (fsimilar(val,1.f)||(val<1)) step/=10.f;
        	EPrefs.grid_cell_size -= step;
        }
        Command(COMMAND_UPDATE_GRID);
    	}break;
    case COMMAND_CREATE_SOUND_LIB:
    	SndLib		= xr_new<CSoundManager>();
    	break;
    case COMMAND_MUTE_SOUND:
    	SndLib->MuteSounds(p1);
    	break;
 	default:
	    Debug.fatal	("ERROR: Undefined command: %04d", _Command);
	}

    RedrawScene();
    return bRes;
}
*/

//---------------------------------------------------------------------------
bool TUI::ApplyShortCut(WORD Key, TShiftState Shift)
{
	VERIFY(m_bReady);

    if (ApplyGlobalShortCut(Key,Shift))	return true;

	bool bExec = false;

    if (Key==VK_ESCAPE)   			COMMAND1(COMMAND_CHANGE_ACTION, etaSelect)
    if (Shift.Contains(ssCtrl)){
    	if (Key==VK_CANCEL)			COMMAND0(COMMAND_BREAK_LAST_OPERATION);
    }else{
        if (Shift.Contains(ssAlt)){
        }else{
            // simple press
        	if (Key=='S')			COMMAND1(COMMAND_CHANGE_ACTION, etaSelect)
        	else if (Key=='A')		COMMAND1(COMMAND_CHANGE_ACTION, etaAdd)
        	else if (Key=='T')		COMMAND1(COMMAND_CHANGE_ACTION, etaMove)
        	else if (Key=='Y')		COMMAND1(COMMAND_CHANGE_ACTION, etaRotate)
        	else if (Key=='H')		COMMAND1(COMMAND_CHANGE_ACTION, etaScale)
        	else if (Key=='Z')		COMMAND1(COMMAND_CHANGE_AXIS,   etAxisX)
        	else if (Key=='X')		COMMAND1(COMMAND_CHANGE_AXIS,   etAxisY)
        	else if (Key=='C')		COMMAND1(COMMAND_CHANGE_AXIS,   etAxisZ)
        	else if (Key=='V')		COMMAND1(COMMAND_CHANGE_AXIS,   etAxisZX)
        	else if (Key=='P')		COMMAND0(COMMAND_EDITOR_PREF)
            else if (Key==VK_OEM_4)	COMMAND1(COMMAND_GRID_SLOT_SIZE,false)
            else if (Key==VK_OEM_6)	COMMAND1(COMMAND_GRID_SLOT_SIZE,true)
        	else if (Key=='W')		COMMAND1(COMMAND_RENDER_WIRE, !(Device.dwFillMode==D3DFILL_WIREFRAME))
        }
    }
    return bExec;
}
//---------------------------------------------------------------------------

bool TUI::ApplyGlobalShortCut(WORD Key, TShiftState Shift)
{
	VERIFY(m_bReady);
	bool bExec = false;
    if (Shift.Contains(ssCtrl)){
        if (Key=='S'){
            if (Shift.Contains(ssAlt))  COMMAND0(COMMAND_SAVEAS)
            else                        COMMAND0(COMMAND_SAVE)
        }
        else if (Key=='O')   			COMMAND0(COMMAND_LOAD)
        else if (Key=='N')   			COMMAND0(COMMAND_CLEAR)
        else if (Key=='G')   			COMMAND0(COMMAND_TOGGLE_GRID)
        else if (Key=='F')				COMMAND0(COMMAND_TOGGLE_SAFE_RECT)
    }
    if (Key==VK_OEM_3)					COMMAND0(COMMAND_RENDER_FOCUS)
    return bExec;
}
//---------------------------------------------------------------------------

