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
    COMMAND_TOGGLE_RENDER_WIRE,
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

    COMMAND_EDIT_COMMAND_LIST,
    COMMAND_EXECUTE_COMMAND_LIST,
    COMMAND_LOG_COMMANDS,
    COMMAND_RUN_MACRO,				// p1 - file name
    COMMAND_ASSIGN_MACRO,			// p1 - slot, p2 - file_name

    COMMAND_MAIN_LAST
};
//------------------------------------------------------------------------------

class CCommandVar{
    enum EType{
        tpStr,
        tpInt
    };
    u32				i;
    xr_string		s;
    EType			type;
public:
       		     	CCommandVar		():i(0),type(tpInt)			{}
       		     	CCommandVar		(xr_string str)	:type(tpStr){s=str;}
            		CCommandVar		(u32 val)		:type(tpInt){i=val;}
	IC operator 	u32 			()							{VERIFY(type==tpInt);return i;}
	IC operator 	xr_string 		()							{VERIFY(type==tpStr);return s;}
    IC bool			IsString		()							{return type==tpStr;}
    IC bool			IsInteger		()							{return type==tpInt;}
//	IC operator 	LPCSTR 			()							{VERIFY(type==tpStr);return s.c_str();}
};

typedef fastdelegate::FastDelegate2<CCommandVar,CCommandVar,CCommandVar> TECommandEvent;

class SECommand;

struct ECORE_API SESubCommand{
    SECommand* 		parent;
    xr_string		desc;
    CCommandVar 	p0;
    CCommandVar 	p1;
    xr_shortcut		shortcut;
public:
                	SESubCommand	(LPCSTR d, SECommand* p, CCommandVar _p0, CCommandVar _p1){desc=d;parent=p;p0=_p0;p1=_p1;}
};
DEFINE_VECTOR	(SESubCommand*,ESubCommandVec,ESubCommandVecIt);
struct ECORE_API SECommand{
	bool			editable;
    LPSTR			name;
    LPSTR			desc;
    ESubCommandVec	sub_commands;
    TECommandEvent	command;
    u32				idx;
public:
    				SECommand		(LPCSTR n, LPCSTR d, bool edit, bool multi, TECommandEvent cmd, u32 i):editable(edit),command(cmd),idx(i)
                    {
                    	name		= xr_strdup(n);
                    	desc		= xr_strdup(d);
                        if (!multi)	AppendSubCommand("",0,0);
                    }
					~SECommand		(){xr_free(name);xr_free(desc);}
    IC LPCSTR		Name			(){return name&&name[0]?name:"";}
	IC LPCSTR		Desc			(){return desc&&desc[0]?desc:"";}
    void			AppendSubCommand(LPCSTR desc, CCommandVar p0, CCommandVar p1){sub_commands.push_back(xr_new<SESubCommand>(desc,this,p0,p1));}
};
DEFINE_VECTOR(SECommand*,ECommandVec,ECommandVecIt);

ECORE_API CCommandVar	    ExecCommand				(u32 cmd, CCommandVar p1=0, CCommandVar p2=0);
ECORE_API CCommandVar	    ExecCommand				(const xr_shortcut& val);
ECORE_API void			    RegisterCommand 		(u32 cmd, SECommand* cmd_impl);
ECORE_API void			    RegisterSubCommand 		(SECommand* cmd_impl, LPCSTR desc, CCommandVar p0, CCommandVar p1);
ECORE_API void			    EnableReceiveCommands	();
ECORE_API ECommandVec&      GetEditorCommands		();
ECORE_API SESubCommand* 	FindCommandByShortcut	(const xr_shortcut& val);
ECORE_API BOOL				LoadShortcuts			(CInifile* ini);
ECORE_API BOOL				SaveShortcuts			(CInifile* ini);
ECORE_API BOOL				AllowLogCommands		();

#define BIND_CMD_EVENT_S(a) 						fastdelegate::bind<TECommandEvent>(a)
#define BIND_CMD_EVENT_C(a,b)						fastdelegate::bind<TECommandEvent>(a,&b)

#define REGISTER_CMD_S(id,cmd)  					RegisterCommand(id, xr_new<SECommand>(#id,"",false,false,BIND_CMD_EVENT_S(cmd),id));
#define REGISTER_CMD_C(id,owner,cmd) 				RegisterCommand(id, xr_new<SECommand>(#id,"",false,false,BIND_CMD_EVENT_C(owner,cmd),id));
#define REGISTER_CMD_SE(id,desc,cmd)  				RegisterCommand(id, xr_new<SECommand>(#id,desc,true,false,BIND_CMD_EVENT_S(cmd),id));
#define REGISTER_CMD_CE(id,desc,owner,cmd) 			RegisterCommand(id, xr_new<SECommand>(#id,desc,true,false,BIND_CMD_EVENT_C(owner,cmd),id));
#define REGISTER_SUB_CMD_SE(id,desc,cmd){  			SECommand* SUB_CMD_HOLDER; RegisterCommand(id, SUB_CMD_HOLDER=xr_new<SECommand>(#id,desc,true,true,BIND_CMD_EVENT_S(cmd),id));
#define REGISTER_SUB_CMD_CE(id,desc,owner,cmd){ 	SECommand* SUB_CMD_HOLDER; RegisterCommand(id, SUB_CMD_HOLDER=xr_new<SECommand>(#id,desc,true,true,BIND_CMD_EVENT_C(owner,cmd),id));
#define APPEND_SUB_CMD(desc,p0,p1)					RegisterSubCommand(SUB_CMD_HOLDER,desc,p0,p1);
#define REGISTER_SUB_CMD_END }

#endif //UI_MainCommandH



