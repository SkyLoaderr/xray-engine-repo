#include "stdafx.h"
#pragma hdrstop

#include "ui_Glowtools.h"
#include "ui_tools.h"

//---------------------------------------------------------------------------
TUI_GlowTools::TUI_GlowTools():TUI_CustomTools(OBJCLASS_GLOW){
    AddControlCB(new TUI_CustomControl	(estSelf,eaSelect,this));
    AddControlCB(new TUI_CustomControl	(estSelf,eaAdd,this));
    AddControlCB(new TUI_CustomControl	(estSelf,eaMove,this));
    AddControlCB(new TUI_CustomControl	(estSelf,eaScale,this));
    AddControlCB(new TUI_CustomControl	(estSelf,eaRotate,this));
}
//---------------------------------------------------------------------------


