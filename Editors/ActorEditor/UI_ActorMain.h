//---------------------------------------------------------------------------
#ifndef UI_ActorMainH
#define UI_ActorMainH
//---------------------------------------------------------------------------
#include "ui_main.h"

enum {
	COMMAND_EXTFIRST_EXT = COMMAND_MAIN_LAST-1,

    COMMAND_MAKE_PREVIEW,
    COMMAND_IMPORT,
    COMMAND_EXPORT_OGF,
    COMMAND_EXPORT_OMF,
    COMMAND_EXPORT_DM,
    COMMAND_PREVIEW_OBJ_PREF,
    COMMAND_SELECT_PREVIEW_OBJ,
    COMMAND_RESET_ANIMATION,
	COMMAND_SHOW_CLIPMAKER,

    COMMAND_FILE_MENU,
    COMMAND_LOAD_FIRSTRECENT,
};
//------------------------------------------------------------------------------

class CActorMain: public TUI{
	typedef TUI inherited;
    
    virtual void 	RealUpdateScene			();
    virtual void 	RealQuit				();
public:
    				CActorMain 				();
    virtual 		~CActorMain				();

    virtual LPSTR	GetCaption				();

    virtual void 	ResetStatus				();
    virtual void 	SetStatus				(LPSTR s, bool bOutLog);
    virtual void 	ProgressInfo			(LPCSTR text, bool bWarn);
    virtual void 	ProgressStart			(float max_val, const char* text);
    virtual void 	ProgressEnd				();
    virtual void 	ProgressUpdate			(float val);
    virtual void 	ProgressInc				(const char* info, bool bWarn);
    virtual void 	OutCameraPos			();
    virtual void 	OutUICursorPos			();
    virtual void 	OutGridSize				();
    virtual void 	OutInfo					();

    virtual LPCSTR	EditorName				(){return "actor";}
    virtual LPCSTR	EditorDesc				(){return "Actor Editor";}

    virtual bool	Command					(int _Command, int p = 0, int p2 = 0);
    virtual bool 	ApplyShortCut			(WORD Key, TShiftState Shift);
    virtual bool 	ApplyGlobalShortCut		(WORD Key, TShiftState Shift);
};    
extern CActorMain*&	AUI;
//---------------------------------------------------------------------------
#endif
