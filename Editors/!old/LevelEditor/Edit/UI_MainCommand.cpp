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
SECommand::SESubCommand* FindCommandByShortcut(const xr_shortcut& val)
{
    ECommandVec& cmds		= GetEditorCommands();
    for (u32 cmd_idx=0; cmd_idx<cmds.size(); cmd_idx++){
    	SECommand*& CMD		= cmds[cmd_idx];
        if (CMD&&CMD->editable){
        	VERIFY(!CMD->sub_commands.empty());
		    for (u32 sub_cmd_idx=0; sub_cmd_idx<CMD->sub_commands.size(); sub_cmd_idx++){
            	SECommand::SESubCommand*& SUB_CMD = CMD->sub_commands[sub_cmd_idx];
                if (SUB_CMD->shortcut.similar(val)) return SUB_CMD;
            }
        }
    }
    return 0;
}
u32 	ExecCommand		(const xr_shortcut& val)
{
	SECommand::SESubCommand* CMD = FindCommandByShortcut(val);
    u32 res 			= false;
    if (CMD){
	    CMD->parent->command(CMD->index,0,res);
    	res				= true;
    }
    return res;
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
    	Msg			("RegisterCommand: command '%s' overridden by command '%s'.",*CMD->desc,*cmd_impl->desc);
    	xr_delete	(CMD);
    }
    CMD	   			= cmd_impl;
}
BOOL	LoadShortcuts(CInifile* ini)
{
    for (u32 cmd_idx=0; cmd_idx<ECommands.size(); cmd_idx++){
    	SECommand*& CMD		= ECommands[cmd_idx];
        if (CMD&&CMD->editable){
		    for (u32 sub_cmd_idx=0; sub_cmd_idx<CMD->sub_commands.size(); sub_cmd_idx++){
            	SECommand::SESubCommand*& SUB_CMD = CMD->sub_commands[sub_cmd_idx];
                string128 nm; 	sprintf(nm,"%s.%s",CMD->Name(),SUB_CMD->Desc());
                if (ini->line_exist("shortcuts",nm))
                	SUB_CMD->shortcut.hotkey=ini->r_u16("shortcuts",nm);
            }
        }
    }
	return TRUE;
}
BOOL	SaveShortcuts(CInifile* ini)
{
    for (u32 cmd_idx=0; cmd_idx<ECommands.size(); cmd_idx++){
    	SECommand*& CMD		= ECommands[cmd_idx];
        if (CMD&&CMD->editable){
		    for (u32 sub_cmd_idx=0; sub_cmd_idx<CMD->sub_commands.size(); sub_cmd_idx++){
            	SECommand::SESubCommand*& SUB_CMD = CMD->sub_commands[sub_cmd_idx];
                string128 nm; 	sprintf(nm,"%s.%s",CMD->Name(),SUB_CMD->Desc());
                ini->w_u16		("shortcuts",nm,SUB_CMD->shortcut.hotkey);
            }
        }
    }
	return TRUE;
}
void	ClearCommands()
{
	for (ECommandVecIt it=ECommands.begin(); it!=ECommands.end(); it++)
    	xr_delete	(*it);
	ECommands.clear	();
}

void	TUI::ClearCommands ()
{
	::ClearCommands	();
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
void 	CommandSetSettings(u32 p1, u32 p2, u32& res)
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
	REGISTER_CMD_S	(COMMAND_INITIALIZE,			CommandInitialize);
	REGISTER_CMD_S	(COMMAND_DESTROY,        		CommandDestroy);
	REGISTER_CMD_SE	(COMMAND_EXIT,               	"Exit",					CommandExit);
	REGISTER_CMD_S	(COMMAND_QUIT,           		CommandQuit);
	REGISTER_CMD_SE	(COMMAND_EDITOR_PREF,    		"Editor Preference",	CommandEditorPrefs);
	REGISTER_CMD_SEn(COMMAND_CHANGE_ACTION,  		"Change Action",      	"Select,Add,Move,Rotate,Scale",	CommandChangeAction);
	REGISTER_CMD_SEn(COMMAND_CHANGE_AXIS,    		"Change Axis",			"X,Y,Z,XZ",						CommandChangeAxis);
	REGISTER_CMD_S	(COMMAND_SET_SETTINGS,			CommandSetSettings);
	REGISTER_CMD_S	(COMMAND_SOUND_EDITOR,   		CommandSoundEditor);
	REGISTER_CMD_S	(COMMAND_SYNC_SOUNDS,    		CommandSyncSounds);
    REGISTER_CMD_S	(COMMAND_IMAGE_EDITOR,   		CommandImageEditor);
	REGISTER_CMD_S	(COMMAND_CHECK_TEXTURES,     	CommandCheckTextures);
	REGISTER_CMD_S	(COMMAND_REFRESH_TEXTURES,   	CommandRefreshTextures);	
	REGISTER_CMD_S	(COMMAND_RELOAD_TEXTURES,    	CommandReloadTextures);
	REGISTER_CMD_S	(COMMAND_CHANGE_SNAP,        	CommandChangeSnap);
    REGISTER_CMD_S	(COMMAND_UNLOAD_TEXTURES,    	CommandUnloadTextures);
    REGISTER_CMD_S	(COMMAND_EVICT_OBJECTS,      	CommandEvictObjects);
    REGISTER_CMD_S	(COMMAND_EVICT_TEXTURES,     	CommandEvictTextures);
    REGISTER_CMD_S	(COMMAND_CHECK_MODIFIED,     	CommandCheckModified);
	REGISTER_CMD_S	(COMMAND_SHOW_PROPERTIES,    	CommandShowProperties);
	REGISTER_CMD_S	(COMMAND_UPDATE_PROPERTIES,  	CommandUpdateProperties);
	REGISTER_CMD_S	(COMMAND_REFRESH_PROPERTIES, 	CommandRefreshProperties);
	REGISTER_CMD_S	(COMMAND_ZOOM_EXTENTS,       	CommandZoomExtents);
    REGISTER_CMD_S	(COMMAND_RENDER_WIRE,        	CommandRenderWire);
    REGISTER_CMD_C	(COMMAND_RENDER_FOCUS,       	this,TUI::CommandRenderFocus);
	REGISTER_CMD_C	(COMMAND_BREAK_LAST_OPERATION,	this,TUI::CommandBreakLastOperation);
    REGISTER_CMD_S	(COMMAND_TOGGLE_SAFE_RECT,   	CommandToggleSafeRect);
	REGISTER_CMD_C	(COMMAND_RENDER_RESIZE,      	this,TUI::CommandRenderResize);
    REGISTER_CMD_S	(COMMAND_TOGGLE_GRID,        	CommandToggleGrid);
	REGISTER_CMD_S	(COMMAND_UPDATE_GRID,        	CommandUpdateGrid);
    REGISTER_CMD_S	(COMMAND_GRID_NUMBER_OF_SLOTS,	CommandGridNumberOfSlots);
    REGISTER_CMD_S	(COMMAND_GRID_SLOT_SIZE,     	CommandGridSlotSize);
    REGISTER_CMD_S	(COMMAND_CREATE_SOUND_LIB,   	CommandCreateSoundLib);
    REGISTER_CMD_S	(COMMAND_MUTE_SOUND,         	CommandMuteSound);
}                                                                        

//---------------------------------------------------------------------------
bool TUI::ApplyShortCut(WORD Key, TShiftState Shift)
{
	VERIFY(m_bReady);

    if (ApplyGlobalShortCut(Key,Shift))	return true;

    xr_shortcut SC; 
    SC.key							= Key;
    SC.ext.assign					(Shift.Contains(ssShift)?xr_shortcut::flShift:0|
    								 Shift.Contains(ssCtrl) ?xr_shortcut::flCtrl:0|
                                     Shift.Contains(ssAlt)  ?xr_shortcut::flAlt:0);
    if (ExecCommand(SC))			return true;
    
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

