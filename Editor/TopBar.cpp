#include "stdafx.h"
#pragma hdrstop

#include "TopBar.h"
#include "UI_Main.h"
#include "UI_Tools.h"
#include "main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "Placemnt"
#pragma resource "*.dfm"
TfraTopBar *fraTopBar;
//---------------------------------------------------------------------------
__fastcall TfraTopBar::TfraTopBar(TComponent* Owner)
        : TFrame(Owner)
{
    ebActionSelect->Tag     = eaSelect;
    ebActionAdd->Tag        = eaAdd;
    ebActionMove->Tag       = eaMove;
    ebActionRotate->Tag     = eaRotate;
    ebActionScale->Tag      = eaScale;

    char buf[MAX_PATH] 		= {"ed.ini"};  FS.m_ExeRoot.Update(buf);
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

void __fastcall TfraTopBar::ebEditUndoClick(TObject *Sender)
{
 	UI->Command( COMMAND_UNDO );
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ebEditRedoClick(TObject *Sender)
{
 	UI->Command( COMMAND_REDO );
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ActionClick(TObject *Sender)
{
    TExtBtn* btn=dynamic_cast<TExtBtn*>(Sender);
    VERIFY(btn);
    UI->Command(COMMAND_CHANGE_ACTION, btn->Tag);
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ebViewClick(TObject *Sender)
{
    TExtBtn* btn=dynamic_cast<TExtBtn*>(Sender);
    if (btn){
        if (btn==ebViewFront)   UI->Device.m_Camera.ViewFront();
        if (btn==ebViewBack)    UI->Device.m_Camera.ViewBack();
        if (btn==ebViewLeft)    UI->Device.m_Camera.ViewLeft();
        if (btn==ebViewRight)   UI->Device.m_Camera.ViewRight();
        if (btn==ebViewTop)     UI->Device.m_Camera.ViewTop();
        if (btn==ebViewBottom)  UI->Device.m_Camera.ViewBottom();
    }
    UI->RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ChangeAction(int act){
    TExtBtn* btn=0;
	//select button from action
    switch(act){
        case eaSelect:  btn=ebActionSelect; break;
        case eaAdd:     btn=ebActionAdd; 	break;
        case eaMove:    btn=ebActionMove; 	break;
        case eaRotate:  btn=ebActionRotate; break;
        case eaScale:   btn=ebActionScale; 	break;
        default: throw -1;
    }
	btn->Down = true;
    UI->RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ChangeAxis(int ax){
    TExtBtn* btn=0;
	switch (ax){
	    case eAxisX: 	btn=ebAxisX; 	break;
    	case eAxisY: 	btn=ebAxisY; 	break;
	    case eAxisZ: 	btn=ebAxisZ; 	break;
    	case eAxisZX: 	btn=ebAxisZX; 	break;
    default: THROW;
    }
	btn->Down = true;
}
void __fastcall TfraTopBar::ebZoomExtentsClick(TObject *Sender)
{
 	UI->Command( COMMAND_ZOOM_EXTENTS, FALSE );
}
//---------------------------------------------------------------------------

void __fastcall TfraTopBar::ebZoomExtentsSelectedClick(TObject *Sender)
{
 	UI->Command( COMMAND_ZOOM_EXTENTS, TRUE );
}
//---------------------------------------------------------------------------

