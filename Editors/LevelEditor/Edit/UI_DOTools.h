#ifndef UI_DOToolsH
#define UI_DOToolsH

#include "ui_control.h"
#include "ui_customtools.h"

// refs
class TfraDetailObject;

//---------------------------------------------------------------------------
class TUI_DOTools:public TUI_CustomTools{
public:
                    TUI_DOTools();
	virtual	void 	OnActivate();
	virtual	void 	OnDeactivate();
};
//---------------------------------------------------------------------------

#endif //UI_DOToolsH
