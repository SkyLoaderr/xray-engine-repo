#ifndef UI_ShaderMainH
#define UI_ShaderMainH

#include "UI_MainCommand.h"
#include "UI_Main.h"

enum {
	COMMAND_EXTFIRST_EXT = COMMAND_MAIN_LAST-1,

	COMMAND_RESET_ANIMATION,
};
//------------------------------------------------------------------------------
class CShaderMain: public TUI{
	typedef TUI inherited;
    
    virtual void 	RealUpdateScene			();
    virtual void 	RealQuit				();
public:
    				CShaderMain 			();
    virtual 		~CShaderMain			();

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

    virtual LPCSTR	EditorName				(){return "shader";}
    virtual LPCSTR	EditorDesc				(){return "Shader Editor";}

    virtual bool	Command					(int _Command, int p = 0, int p2 = 0);
    virtual bool 	ApplyShortCut			(WORD Key, TShiftState Shift);
    virtual bool 	ApplyGlobalShortCut		(WORD Key, TShiftState Shift);
};    
extern CShaderMain*&	PUI;

#endif //UI_MainCommandH



