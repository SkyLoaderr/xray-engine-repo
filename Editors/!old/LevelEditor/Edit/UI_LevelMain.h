#ifndef UI_LevelMainH
#define UI_LevelMainH

#include "ui_main.h"
#include "UI_MainCommand.h"

enum {
	COMMAND_EXTFIRST_EXT = COMMAND_MAIN_LAST-1,

	COMMAND_CHANGE_TARGET,

	COMMAND_SHOW_OBJECTLIST,

	COMMAND_REFRESH_SOUND_ENVS,
    COMMAND_REFRESH_SOUND_ENV_GEOMETRY,
    
	COMMAND_REFRESH_LIBRARY,
    COMMAND_LIBRARY_EDITOR,
    COMMAND_LANIM_EDITOR,
    COMMAND_FILE_MENU,
	COMMAND_CLEAR_COMPILER_ERROR,
    COMMAND_IMPORT_COMPILER_ERROR,
	COMMAND_VALIDATE_SCENE,
    COMMAND_RELOAD_OBJECTS,

	COMMAND_CUT,
	COMMAND_COPY,
	COMMAND_PASTE,
	COMMAND_LOAD_SELECTION,
	COMMAND_SAVE_SELECTION,

	COMMAND_SCENE_SUMMARY,

	COMMAND_OPTIONS,
	COMMAND_BUILD,

	COMMAND_MAKE_GAME,
    COMMAND_MAKE_DETAILS,
	COMMAND_MAKE_HOM,
    COMMAND_MAKE_AIMAP,
    COMMAND_MAKE_WM,

	COMMAND_INVERT_SELECTION_ALL,
	COMMAND_SELECT_ALL,
	COMMAND_DESELECT_ALL,
	COMMAND_DELETE_SELECTION,
	COMMAND_HIDE_UNSEL,
	COMMAND_HIDE_SEL,
	COMMAND_HIDE_ALL,
    COMMAND_LOCK_ALL,
    COMMAND_LOCK_SEL,
    COMMAND_LOCK_UNSEL,

    COMMAND_SET_SNAP_OBJECTS,
    COMMAND_ADD_SEL_SNAP_OBJECTS,
	COMMAND_DEL_SEL_SNAP_OBJECTS,
    COMMAND_CLEAR_SNAP_OBJECTS,
	COMMAND_SELECT_SNAP_OBJECTS,
	COMMAND_REFRESH_SNAP_OBJECTS,

    COMMAND_LOAD_FIRSTRECENT,
    COMMAND_MOVE_CAMERA_TO,

    COMMAND_SHOWCONTEXTMENU,
};
//------------------------------------------------------------------------------

class CLevelMain: public TUI{
	typedef TUI inherited;
    
    virtual void 	RealUpdateScene			();
    virtual void 	RealQuit				();

public:
    C3DCursor*   m_Cursor;
public:
    				CLevelMain 				();
    virtual 		~CLevelMain				();

    virtual LPSTR	GetCaption				();

    virtual void 	ResetStatus				();
    virtual void 	SetStatus				(LPSTR s, bool bOutLog=true);
    virtual void	ProgressDraw			();
    virtual void 	OutCameraPos			();
    virtual void 	OutUICursorPos			();
    virtual void 	OutGridSize				();
    virtual void 	OutInfo					();

    virtual LPCSTR	EditorName				(){return "level";}
    virtual LPCSTR	EditorDesc				(){return "Level Editor";}

    void 			ShowContextMenu			(int cls);
	bool 			PickGround				(Fvector& hitpoint, const Fvector& start, const Fvector& direction, int bSnap=1, Fvector* hitnormal=0);
	bool 			SelectionFrustum		(CFrustum& frustum);

    virtual bool 	ApplyShortCut			(WORD Key, TShiftState Shift);
    virtual bool 	ApplyGlobalShortCut		(WORD Key, TShiftState Shift);

    // commands
	virtual	void	RegisterCommands		(); 
};    
extern CLevelMain*&	LUI;

#endif //UI_MainCommandH



