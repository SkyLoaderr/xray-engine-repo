#ifndef UI_MainCommandH
#define UI_MainCommandH

#define COMMAND_EXIT        0
#define COMMAND_CLEAR       1
#define COMMAND_LOAD        2
#define COMMAND_RELOAD      3
#define COMMAND_SAVE        4
#define COMMAND_SAVEAS      5
#define COMMAND_BUILD       6
#define COMMAND_OPTIONS     7

#define COMMAND_VALIDATE_SCENE  	10
#define COMMAND_REFRESH_TEXTURES 	11	// p1 - refresh only new (true,false)
#define COMMAND_RELOAD_LIBRARY 		12
#define COMMAND_REFRESH_LIBRARY 	13
#define COMMAND_UNLOAD_LIBMESHES 	14
#define COMMAND_EDITOR_PREF 		15
#define COMMAND_OBJECT_LIST 		16
#define COMMAND_RENDER_FOCUS 		17
#define COMMAND_UPDATE_CAPTION 		18
#define COMMAND_BREAK_LAST_OPERATION 20
#define COMMAND_RESET_ANIMATION 	21
#define COMMAND_CLEAN_LIBRARY 		22
#define COMMAND_MAKE_LTX      		23
#define COMMAND_MAKE_DETAILS		24
#define COMMAND_EDIT_PREFERENCES 	25
#define COMMAND_LIBRARY_EDITOR 		26
#define COMMAND_SHADER_EDITOR 		27
#define COMMAND_PARTICLE_EDITOR 	28
#define COMMAND_IMAGE_EDITOR        29
#define COMMAND_CHECK_TEXTURES		30
#define COMMAND_UPDATE_TOOLBAR		31

#define COMMAND_CUT         		50
#define COMMAND_COPY        		51
#define COMMAND_PASTE       		52
#define COMMAND_UNDO        		53
#define COMMAND_REDO        		54

#define COMMAND_INVERT_SELECTION_ALL    70
#define COMMAND_SELECT_ALL              71
#define COMMAND_DESELECT_ALL            72

#define COMMAND_DELETE_SELECTION        100
#define COMMAND_SHOWPROPERTIES          101
#define COMMAND_SHOWCONTEXTMENU			102
#define COMMAND_HIDE_ALL		        103
#define COMMAND_HIDE_SEL	            104
#define COMMAND_HIDE_UNSEL				105
#define COMMAND_ZOOM_EXTENTS			108		// p1 -> selected/all (true/false)
#define COMMAND_GROUP_CREATE	        109
#define COMMAND_GROUP_SAVE		        110
#define COMMAND_GROUP_DESTROY	        111
#define COMMAND_GROUP_DESTROYALL		112
#define COMMAND_SET_SNAP_OBJECTS 		113
#define COMMAND_ADD_SNAP_OBJECTS 		114
#define COMMAND_CLEAR_SNAP_OBJECTS		115

#define COMMAND_CHANGE_TARGET           150		// p1 -> target
#define COMMAND_CHANGE_ACTION           151		// p1 -> action
#define COMMAND_CHANGE_AXIS             152		// p1 -> enum EAxis
#define COMMAND_CHANGE_SNAP             153

#define COMMAND_SET_NUMERIC_POSITION	201
#define COMMAND_SET_NUMERIC_ROTATION	202
#define COMMAND_SET_NUMERIC_SCALE		203

#define COMMAND_LOCK_ALL				250
#define COMMAND_LOCK_SEL				251
#define COMMAND_LOCK_UNSEL				252
//------------------------------------------------------------------------------

#endif //UI_MainCommandH
 