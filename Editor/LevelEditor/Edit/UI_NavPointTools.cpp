#include "stdafx.h"
#pragma hdrstop

#include "ui_NavPointtools.h"
#include "ui_tools.h"
//---------------------------------------------------------------------------

TUI_NavPointTools::TUI_NavPointTools(TFrame* f):TUI_CustomTools(f){
    AddControlCB(new TUI_CustomControl	(estSelf,eaSelect,OBJCLASS_NAVPOINT));
    AddControlCB(new TUI_CustomControl	(estSelf,eaAdd,OBJCLASS_NAVPOINT));
    AddControlCB(new TUI_CustomControl	(estSelf,eaMove,OBJCLASS_NAVPOINT));
}

