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
    COMMAND_SET_SETTINGS,
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

    // имеют разную реализацию
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

class SECommand;

struct ECORE_API SESubCommand{
    SECommand* 	parent;
    LPSTR		desc;
    u32			p0;
    u32			p1;
    xr_shortcut	shortcut;
public:
                SESubCommand	(LPCSTR d, SECommand* p, u32 _p0, u32 _p1){desc=xr_strdup(d);parent=p;p0=_p0;p1=_p1;}
                ~SESubCommand	(){xr_free(desc);}
    IC LPCSTR	Desc			(){return desc&&desc[0]?desc:"";}
};
DEFINE_VECTOR	(SESubCommand*,ESubCommandVec,ESubCommandVecIt);
struct ECORE_API SECommand{
	bool			editable;
    LPSTR			name;
    LPSTR			desc;
    ESubCommandVec	sub_commands;
    TECommandEvent	command;
public:
    				SECommand		(LPCSTR n, LPCSTR d, bool edit, bool multi, TECommandEvent cmd):editable(edit),command(cmd)
                    {
                    	name		= xr_strdup(n);
                    	desc		= xr_strdup(d);
                        if (!multi)	AppendSubCommand("",0,0);
                    }
					~SECommand		(){xr_free(name);xr_free(desc);}
    IC LPCSTR		Name			(){return name&&name[0]?name:"";}
	IC LPCSTR		Desc			(){return desc&&desc[0]?desc:"";}
    void			AppendSubCommand(LPCSTR desc, u32 p0, u32 p1){sub_commands.push_back(xr_new<SESubCommand>(desc,this,p0,p1));}
};
DEFINE_VECTOR(SECommand*,ECommandVec,ECommandVecIt);

ECORE_API u32 			    ExecCommand				(u32 cmd, u32 p1=0, u32 p2=0);
ECORE_API u32 			    ExecCommand				(const xr_shortcut& val);
ECORE_API void			    RegisterCommand 		(u32 cmd, SECommand* cmd_impl);
ECORE_API void			    RegisterSubCommand 		(SECommand* cmd_impl, LPCSTR desc, u32 p0, u32 p1);
ECORE_API void			    EnableReceiveCommands	();
ECORE_API ECommandVec&      GetEditorCommands		();
ECORE_API SESubCommand* 	FindCommandByShortcut	(const xr_shortcut& val);
ECORE_API BOOL				LoadShortcuts			(CInifile* ini);
ECORE_API BOOL				SaveShortcuts			(CInifile* ini);

#define BIND_CMD_EVENT_S(a) 						TECommandEvent().bind(a)
#define BIND_CMD_EVENT_C(a,b)						TECommandEvent().bind(a,&b)

#define REGISTER_CMD_S(id,cmd)  					RegisterCommand(id, xr_new<SECommand>(#id,"",false,false,BIND_CMD_EVENT_S(cmd)));
#define REGISTER_CMD_C(id,owner,cmd) 				RegisterCommand(id, xr_new<SECommand>(#id,"",false,false,BIND_CMD_EVENT_C(owner,cmd)));
#define REGISTER_CMD_SE(id,desc,cmd)  				RegisterCommand(id, xr_new<SECommand>(#id,desc,true,false,BIND_CMD_EVENT_S(cmd)));
#define REGISTER_CMD_CE(id,desc,owner,cmd) 			RegisterCommand(id, xr_new<SECommand>(#id,desc,true,false,BIND_CMD_EVENT_C(owner,cmd)));
#define REGISTER_SUB_CMD_SE(id,desc,cmd){  			SECommand* SUB_CMD_HOLDER; RegisterCommand(id, SUB_CMD_HOLDER=xr_new<SECommand>(#id,desc,true,true,BIND_CMD_EVENT_S(cmd)));
#define REGISTER_SUB_CMD_CE(id,desc,owner,cmd){ 	SECommand* SUB_CMD_HOLDER; RegisterCommand(id, SUB_CMD_HOLDER=xr_new<SECommand>(#id,desc,true,true,BIND_CMD_EVENT_C(owner,cmd)));
#define APPEND_SUB_CMD(desc,p0,p1)					RegisterSubCommand(SUB_CMD_HOLDER,desc,p0,p1);
#define REGISTER_SUB_CMD_END }

#endif //UI_MainCommandH



