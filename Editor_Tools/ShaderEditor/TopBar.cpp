#include "stdafx.h"
#pragma hdrstop

#include "TopBar.h"
#include "UI_Main.h"
#include "UI_Tools.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfraTopBar *fraTopBar;
//---------------------------------------------------------------------------
__fastcall TfraTopBar::TfraTopBar(TComponent* Owner)
        : TFrame(Owner)
{
    char buf[MAX_PATH] 		= {"shader_ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName 	= buf;
}
//---------------------------------------------------------------------------

void TfraTopBar::OnTimer(){
    for (int j=0; j<ComponentCount; j++){
        TComponent* temp = Components[j];
        if (dynamic_cast<TExtBtn *>(temp) != NULL)
            ((TExtBtn*)temp)->UpdateMouseInControl();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ebViewClick(TObject *Sender)
{
    TExtBtn* btn=dynamic_cast<TExtBtn*>(Sender);
    if (btn){
        if (btn==ebViewFront)   Device.m_Camera.ViewFront();
        if (btn==ebViewBack)    Device.m_Camera.ViewBack();
        if (btn==ebViewLeft)    Device.m_Camera.ViewLeft();
        if (btn==ebViewRight)   Device.m_Camera.ViewRight();
        if (btn==ebViewTop)     Device.m_Camera.ViewTop();
        if (btn==ebViewBottom)  Device.m_Camera.ViewBottom();
    }
    UI->RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ebZoomExtentsClick(TObject *Sender)
{
 	UI->Command( COMMAND_ZOOM_EXTENTS );
}
//---------------------------------------------------------------------------
void __fastcall TfraTopBar::ebCameraClick(TObject *Sender)
{
	Device.m_Camera.SetStyle(ECameraStyle(((TExtBtn*)Sender)->Tag));
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::fsStorageRestorePlacement(TObject *Sender)
{
	if (ebCameraPlane->Down) 			Device.m_Camera.SetStyle(csPlaneMove);
	else if (ebCameraArcBall->Down) 	Device.m_Camera.SetStyle(cs3DArcBall);
	else if (ebCameraFly->Down) 		Device.m_Camera.SetStyle(csFreeFly);
    UI->RedrawScene();
}
//---------------------------------------------------------------------------

