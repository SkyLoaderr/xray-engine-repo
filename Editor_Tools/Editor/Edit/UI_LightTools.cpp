#include "stdafx.h"
#pragma hdrstop

#include "ui_lighttools.h"
#include "ui_tools.h"
#include "FrameLight.h"
//---------------------------------------------------------------------------
TUI_LightTools::TUI_LightTools():TUI_CustomTools(OBJCLASS_LIGHT){
    AddControlCB(new TUI_CustomControl(estSelf,eaSelect,this));
    AddControlCB(new TUI_CustomControl(estSelf,eaAdd,	this));
    AddControlCB(new TUI_CustomControl(estSelf,eaMove,	this));
    AddControlCB(new TUI_CustomControl(estSelf,eaScale,	this));
    AddControlCB(new TUI_CustomControl(estSelf,eaRotate,this));
}

void TUI_LightTools::OnActivate  (){
    pFrame = new TfraLight(0);
	TUI_CustomTools::OnActivate();
}
void TUI_LightTools::OnDeactivate(){
	TUI_CustomTools::OnDeactivate();
    _DELETE(pFrame);
}

