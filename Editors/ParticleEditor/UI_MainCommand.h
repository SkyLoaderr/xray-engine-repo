#ifndef UI_MainCommandH
#define UI_MainCommandH

enum ECommands{
	COMMAND_INITIALIZE,
	COMMAND_DESTROY,
	COMMAND_EXIT,
	COMMAND_CLEAR,
	COMMAND_SAVE,
	COMMAND_RELOAD,

	COMMAND_UPDATE_GRID,
    COMMAND_GRID_NUMBER_OF_SLOTS,
    COMMAND_GRID_SLOT_SIZE,
    COMMAND_CHECK_MODIFIED,

	COMMAND_EDITOR_PREF,
	COMMAND_UPDATE_CAPTION,
	COMMAND_REFRESH_TEXTURES,	// p1 - refresh only new (true,false)
    COMMAND_CHECK_TEXTURES,
	COMMAND_IMAGE_EDITOR,
	COMMAND_RENDER_FOCUS,
	COMMAND_ZOOM_EXTENTS,       // p1 - true if object
	COMMAND_UPDATE_TOOLBAR,
	COMMAND_RESET_ANIMATION,
	COMMAND_SELECT_PREVIEW_OBJ,	// p1 - 0-plane, 1-box, 2-sphere, 3-teapot, -1-custom
	COMMAND_APPLY_CHANGES,

    COMMAND_BREAK_LAST_OPERATION,

    COMMAND_CHANGE_ACTION,
    COMMAND_CHANGE_AXIS,
    // unused (only for compatibility)
    COMMAND_UNDO,
    COMMAND_REDO
    
};
#endif //UI_MainCommandH

