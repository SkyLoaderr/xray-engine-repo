#include "stdafx.h"
#pragma hdrstop

#include "ui_soundtools.h"
#include "ui_tools.h"

//---------------------------------------------------------------------------
TUI_SoundTools::TUI_SoundTools():TUI_CustomTools(OBJCLASS_SOUND){
    AddControlCB(new TUI_CustomControl(estSelf,eaSelect,	this));
    AddControlCB(new TUI_CustomControl(estSelf,eaAdd,		this));
    AddControlCB(new TUI_CustomControl(estSelf,eaMove,		this));
}

