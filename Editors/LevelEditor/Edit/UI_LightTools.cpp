#include "stdafx.h"
#pragma hdrstop

#include "ui_lighttools.h"
#include "ui_tools.h"
#include "FrameLight.h"
//---------------------------------------------------------------------------
TUI_LightTools::TUI_LightTools():TUI_CustomTools(OBJCLASS_LIGHT){
}

void TUI_LightTools::OnActivate  (){
    pFrame = xr_new<TfraLight>((TComponent*)0);
	TUI_CustomTools::OnActivate();
}
void TUI_LightTools::OnDeactivate(){
	TUI_CustomTools::OnDeactivate();
    xr_delete(pFrame);
}

