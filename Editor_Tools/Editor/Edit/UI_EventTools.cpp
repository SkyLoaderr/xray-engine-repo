#include "stdafx.h"
#pragma hdrstop

#include "UI_EventTools.h"
#include "ui_tools.h"

//----------------------------------------------------------------------
TUI_EventTools::TUI_EventTools():TUI_CustomTools(OBJCLASS_EVENT){
    AddControlCB(new TUI_CustomControl	(estSelf,eaSelect,	this));
    AddControlCB(new TUI_CustomControl	(estSelf,eaAdd,		this));
    AddControlCB(new TUI_CustomControl	(estSelf,eaMove,	this));
    AddControlCB(new TUI_CustomControl	(estSelf,eaRotate,	this));
    AddControlCB(new TUI_CustomControl	(estSelf,eaScale,	this));
}

