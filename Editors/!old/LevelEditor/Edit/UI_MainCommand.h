#ifndef UI_MainCommandH
#define UI_MainCommandH

enum{
	COMMAND_INITIALIZE=0,		// p1 - D3DWindow, p2 - TPanel
	COMMAND_DESTROY,
	COMMAND_QUIT,
	COMMAND_EDITOR_PREF,
	COMMAND_CHANGE_ACTION,
    COMMAND_IMAGE_EDITOR,
	COMMAND_CHECK_TEXTURES,
	COMMAND_REFRESH_TEXTURES,
	COMMAND_RELOAD_TEXTURES,
	COMMAND_CHANGE_AXIS,
	COMMAND_CHANGE_SNAP,
    COMMAND_CHANGE_SETTINGS,
    COMMAND_UNLOAD_TEXTURES,
    COMMAND_EVICT_OBJECTS,
    COMMAND_EVICT_TEXTURES,
    COMMAND_CHECK_MODIFIED,
	COMMAND_EXIT,
	COMMAND_SHOW_PROPERTIES,
	COMMAND_UPDATE_PROPERTIES,	// p1 - forced update if needed
    COMMAND_REFRESH_PROPERTIES,
	COMMAND_ZOOM_EXTENTS,
    COMMAND_RENDER_FOCUS,
    COMMAND_RENDER_RESIZE,		
    COMMAND_RENDER_WIRE,		// p1 - toggle to wireframe
    COMMAND_UPDATE_CAPTION,
	COMMAND_BREAK_LAST_OPERATION,
	COMMAND_UPDATE_TOOLBAR,
    COMMAND_TOGGLE_SAFE_RECT,
    COMMAND_TOGGLE_GRID,
	COMMAND_UPDATE_GRID,
    COMMAND_GRID_NUMBER_OF_SLOTS,
    COMMAND_GRID_SLOT_SIZE,
    
    COMMAND_REFRESH_UI_BAR,
    COMMAND_RESTORE_UI_BAR,
    COMMAND_SAVE_UI_BAR,

    COMMAND_MUTE_SOUND,

    // ����� ������ ����������
    COMMAND_CLEAR,
    COMMAND_LOAD,
    COMMAND_RELOAD,
    COMMAND_SAVE,
    COMMAND_SAVEAS,
    COMMAND_SAVE_BACKUP,

    COMMAND_CREATE_SOUND_LIB,

    // sound
	COMMAND_SOUND_EDITOR,
	COMMAND_SYNC_SOUNDS,
    
	COMMAND_UNDO,
	COMMAND_REDO,

    COMMAND_MAIN_LAST
};
//------------------------------------------------------------------------------

typedef fastdelegate::FastDelegate3<u32,u32,u32&> TECommandEvent;

struct ECORE_API SECommand{
	bool			editable;
    LPSTR			caption;
	xr_shortcut		shortcut;
    TECommandEvent	command;
    				SECommand		(LPCSTR capt, bool edit, const xr_shortcut& shrt, TECommandEvent cmd):editable(edit),shortcut(shrt),command(cmd){caption=xr_strdup(capt);}
					~SECommand		(){xr_free(caption);}
    IC LPCSTR		Caption			(){return caption&&caption[0]?caption:"Unknown";}
};
DEFINE_VECTOR(SECommand*,ECommandVec,ECommandVecIt);

ECORE_API u32 					ExecCommand				(u32 cmd, u32 p1=0, u32 p2=0);
ECORE_API void					RegisterCommand 		(u32 cmd_type, SECommand* cmd_impl);
ECORE_API void					EnableReceiveCommands	();
ECORE_API ECommandVec&  		GetEditorCommands		();

#define MAKE_SHORTCUT(k,a,c,s)  xr_shortcut(k,a,c,s)
#define MAKE_EMPTY_SHORTCUT  	MAKE_SHORTCUT(0,0,0,0)
#define BIND_CMD_EVENT_S(a) 	TECommandEvent().bind(a)
#define BIND_CMD_EVENT_C(a,b)	TECommandEvent().bind(a,&b)

#endif //UI_MainCommandH



