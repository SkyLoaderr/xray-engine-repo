#ifndef UI_LightToolsH
#define UI_LightToolsH

#include "ui_control.h"
#include "ui_customtools.h"

//------------------------------------------------------------------------------
class TUI_LightTools:public TUI_CustomTools{
public:
                    TUI_LightTools		();
    virtual         ~TUI_LightTools		(){;}
	virtual void 	OnActivate();
	virtual	void 	OnDeactivate();
};
//------------------------------------------------------------------------------
#endif
