#ifndef UI_ParticleMainH
#define UI_ParticleMainH

#include "ui_main.h"

enum {
	COMMAND_EXTFIRST_EXT = COMMAND_MAIN_LAST-1,

    COMMAND_SELECT_PREVIEW_OBJ,
    COMMAND_EDIT_PREVIEW_PROPS,

    COMMAND_PLAY_CURRENT,
    COMMAND_STOP_CURRENT,

	COMMAND_MERGE,
};
//------------------------------------------------------------------------------

class CParticleMain: public TUI{
	typedef TUI inherited;
    
    virtual void 	RealUpdateScene			();
    virtual void 	RealQuit				();
public:
    				CParticleMain 			();
    virtual 		~CParticleMain			();

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

    virtual LPCSTR	EditorName				(){return "particle";}
    virtual LPCSTR	EditorDesc				(){return "Particle Editor";}

    virtual bool	Command					(int _Command, int p = 0, int p2 = 0);
    virtual bool 	ApplyShortCut			(WORD Key, TShiftState Shift);
    virtual bool 	ApplyGlobalShortCut		(WORD Key, TShiftState Shift);
};    
extern CParticleMain*&	PUI;
//---------------------------------------------------------------------------
#endif //UI_MainCommandH



