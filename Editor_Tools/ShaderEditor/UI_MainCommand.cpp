//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "topbar.h"
#include "leftbar.h"
#include "bottombar.h"
#include "EditorPref.h"
#include "main.h"
#include "ActorTools.h"

#include "UI_Main.h"

bool TUI::Command( int _Command, int p1 ){
	char filebuffer[MAX_PATH]="";

    bool bRes = true;

	switch( _Command ){

	case COMMAND_EXIT:{
    	if (!m_ActorTools->IfModified()) return false;
        Clear();
		}break;
	case COMMAND_SHOWPROPERTIES:
///        m_Tools->ShowProperties();
        break;
	case COMMAND_EDITOR_PREF:
	    frmEditorPreferences->ShowModal();
        break;
	case COMMAND_LOAD:
    	{
        	if (p1)	strcpy( filebuffer, (char*)p1 );
            else	strcpy( filebuffer, m_LastFileName );
			if( p1 || FS.GetOpenName( &FS.m_Objects, filebuffer ) ){
                if (!m_ActorTools->IfModified()) return false;
            	Command(COMMAND_CLEAR);
                if (m_ActorTools->Load(filebuffer))	strcpy(m_LastFileName,filebuffer);
                else								strcpy(m_LastFileName,"");
			    Command(COMMAND_UPDATE_CAPTION);
			}
		}
		break;

	case COMMAND_SAVE:
		{
			if(m_LastFileName[0]){
				m_ActorTools->Save(m_LastFileName);
			    UI->Command(COMMAND_UPDATE_CAPTION);
			}else{
				bRes = Command( COMMAND_SAVEAS ); }
		}
		break;

	case COMMAND_SAVEAS:
		{
			filebuffer[0] = 0;
			if( FS.GetSaveName( &FS.m_Objects, filebuffer ) ){
            	m_ActorTools->Save(filebuffer);
				strcpy(m_LastFileName,filebuffer);
			    bRes = UI->Command(COMMAND_UPDATE_CAPTION);
			}else
            	bRes = false;
		}
		break;

	case COMMAND_CLEAR:
		{
	    	if (!m_ActorTools->IfModified()) return false;
			Device.m_Camera.Reset();
            m_ActorTools->Clear();
			m_LastFileName[0] = 0;
			UI->Command(COMMAND_UPDATE_CAPTION);
		}
		break;

	case COMMAND_REFRESH_TEXTURES:
		Device.RefreshTextures(bool(p1));
		break;

	case COMMAND_ZOOM_EXTENTS:
		m_ActorTools->ZoomObject();
    	break;
    case COMMAND_RENDER_FOCUS:
		if (frmMain->Visible&&g_bEditorValid)
        	m_D3DWindow->SetFocus();
    	break;
    case COMMAND_UPDATE_CAPTION:
    	frmMain->UpdateCaption();
   	 	break;
   	case COMMAND_UPDATE_TOOLBAR:
    	fraLeftBar->UpdateBar();
    	break;
	case COMMAND_RESET_ANIMATION:
   		break;
 	default:
		ELog.DlgMsg( mtError, "Warning: Undefined command: %04d", _Command );
        bRes = false;
		}

    RedrawScene();
    return bRes;
}



