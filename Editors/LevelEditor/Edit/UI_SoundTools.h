#ifndef UI_SoundToolsH
#define UI_SoundToolsH

#include "ui_control.h"
#include "ui_customtools.h"

//---------------------------------------------------------------------------
class TUI_SoundSrcTools:public TUI_CustomTools{
public:
                    TUI_SoundSrcTools();
    virtual         ~TUI_SoundSrcTools(){;}
};
//---------------------------------------------------------------------------
class TUI_SoundEnvTools:public TUI_CustomTools{
public:
                    TUI_SoundEnvTools();
    virtual         ~TUI_SoundEnvTools(){;}
};
//---------------------------------------------------------------------------
#endif
