//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "topbar.h"
#include "leftbar.h"
#include "bottombar.h"
#include "EditorPref.h"
#include "main.h"
#include "ShaderTools.h"
#include "PropertiesShader.h"

#include "UI_Main.h"

bool TUI::Command( int _Command, int p1 ){
//	char filebuffer[MAX_PATH]="";

    bool bRes = true;

	switch( _Command ){

	case COMMAND_EXIT:{
    	if (!SHTools.IfModified()) return false;
        OnDestroy();
		}break;
	case COMMAND_EDITOR_PREF:
	    frmEditorPreferences->ShowModal();
        break;
	case COMMAND_SAVE:
    	SHTools.Engine.Save();
    	SHTools.Compiler.Save();
		Command(COMMAND_UPDATE_CAPTION);
    	break;
    case COMMAND_RELOAD:
    	if (SHTools.ActiveEditor()==aeEngine){
	    	if (!SHTools.Engine.IfModified()) return false;
            if (ELog.DlgMsg(mtConfirmation,"Reload shaders?")==mrYes)
                SHTools.Engine.Reload();
    	}else if (SHTools.ActiveEditor()==aeCompiler){
	    	if (!SHTools.Compiler.IfModified()) return false;
            if (ELog.DlgMsg(mtConfirmation,"Reload shaders?")==mrYes)
                SHTools.Compiler.Reload();
        }
		Command(COMMAND_UPDATE_CAPTION);
    	break;
	case COMMAND_CLEAR:
		{
			Device.m_Camera.Reset();
            SHTools.ResetPreviewObject();
			Command(COMMAND_UPDATE_CAPTION);
		}
		break;

	case COMMAND_REFRESH_TEXTURES:
		Device.RefreshTextures(bool(p1));
		break;

	case COMMAND_ZOOM_EXTENTS:
		SHTools.ZoomObject();
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
    case COMMAND_SHADER_PROPERTIES:
    	TfrmShaderProperties::ShowProperties();
    	break;
    case COMMAND_SELECT_PREVIEW_OBJ:
		SHTools.SelectPreviewObject(p1);
    	break;
    case COMMAND_APPLY_CHANGES:
    	if (SHTools.ActiveEditor()==aeEngine)		SHTools.Engine.ApplyChanges();
    	else if (SHTools.ActiveEditor()==aeCompiler)SHTools.Compiler.ApplyChanges();
    	break;
 	default:
		ELog.DlgMsg( mtError, "Warning: Undefined command: %04d", _Command );
        bRes = false;
		}

    RedrawScene();
    return bRes;
}



