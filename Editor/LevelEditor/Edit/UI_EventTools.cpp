#include "stdafx.h"
#pragma hdrstop

#include "UI_EventTools.h"
#include "ui_tools.h"
#include "FrameEvent.h"

//----------------------------------------------------------------------
TUI_EventTools::TUI_EventTools():TUI_CustomTools(OBJCLASS_EVENT){
}

void TUI_EventTools::OnActivate  (){
    pFrame = new TfraEvent(0);
	TUI_CustomTools::OnActivate();
}
void TUI_EventTools::OnDeactivate(){
	TUI_CustomTools::OnDeactivate();
    _DELETE(pFrame);
}

