#ifndef UI_MainCommandH
#define UI_MainCommandH

#define COMMAND_EXIT        		0
#define COMMAND_CLEAR       		1
#define COMMAND_SAVE	      		2
#define COMMAND_RELOAD				3

#define COMMAND_SHADER_PROPERTIES	10

#define COMMAND_EDITOR_PREF 		50
#define COMMAND_UPDATE_CAPTION 		51
#define COMMAND_REFRESH_TEXTURES 	52	// p1 - refresh only new (true,false)
#define COMMAND_RENDER_FOCUS 		53
#define COMMAND_ZOOM_EXTENTS		54
#define COMMAND_UPDATE_TOOLBAR		55
#define COMMAND_RESET_ANIMATION		56
#define COMMAND_SELECT_PREVIEW_OBJ	57	// p1 - 0-plane, 1-box, 2-sphere, 3-teapot, -1-custom
#define COMMAND_APPLY_CHANGES		58
#endif //UI_MainCommandH

