//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "UI_MainExtern.h"

#include "UI_Tools.h"
#include "EditLibrary.h"
#include "ImageEditor.h"
#include "topbar.h"
#include "leftbar.h"
#include "scene.h"
#include "sceneobject.h"
#include "EditorPref.h"
#include "Cursor3D.h"
#include "bottombar.h"
#include "xr_trims.h"
#include "main.h"
#include "xr_input.h"
#include "ui_main.h"
#include "d3dutils.h"
#include "frustum.h"
#include "EditLightAnim.h"
#include "folderLib.h"
#include "sceneproperties.h"
#include "builder.h"

bool TUI::CommandExt(int _Command, int p1, int p2)
{
	bool bRes = true;
	string256 filebuffer;
	switch (_Command){
	case COMMAND_CHANGE_TARGET:
	  	Tools.ChangeTarget(p1);
        Command(COMMAND_UPDATE_PROPERTIES);
        break;
	case COMMAND_SHOW_OBJECTLIST:
        if (GetEState()==esEditScene) Tools.ShowObjectList();
        break;
    case COMMAND_LIBRARY_EDITOR:
        if (Scene.ObjCount()||(GetEState()!=esEditScene)){
        	if (GetEState()==esEditLibrary)	TfrmEditLibrary::ShowEditor();
            else							ELog.DlgMsg(mtError, "Scene must be empty before editing library!");
        }else{
            TfrmEditLibrary::ShowEditor();
        }
        break;
    case COMMAND_LANIM_EDITOR:
    	TfrmEditLightAnim::ShowEditor();
    	break;
    case COMMAND_FILE_MENU:
		FHelper.ShowPPMenu(fraLeftBar->pmSceneFile,0);
    	break;
	case COMMAND_LOAD:
		if( !Scene.locked() ){
        	if (p1)	strcpy( filebuffer, (char*)p1 );
            else	strcpy( filebuffer, m_LastFileName );
			if( p1 || Engine.FS.GetOpenName( Engine.FS.m_Maps, filebuffer, sizeof(filebuffer) ) ){
                if (!Scene.IfModified()){
                	bRes=false;
                    break;
                }
                if ((0!=stricmp(filebuffer,m_LastFileName))&&Engine.FS.CheckLocking(0,filebuffer,false,true)){
                	bRes=false;
                    break;
                }
                if ((0==stricmp(filebuffer,m_LastFileName))&&Engine.FS.CheckLocking(0,filebuffer,true,false)){
	                Engine.FS.UnlockFile(0,filebuffer);
                }
                SetStatus("Level loading...");
            	Command( COMMAND_CLEAR );
				Scene.Load( filebuffer );
				strcpy(m_LastFileName,filebuffer);
                SetStatus("");
              	Scene.UndoClear();
				Scene.UndoSave();
                Scene.m_Modified = false;
			    Command(COMMAND_UPDATE_CAPTION);
                Command(COMMAND_CHANGE_ACTION,eaSelect);
                // lock
                Engine.FS.LockFile(0,filebuffer);
                fraLeftBar->AppendRecentFile(filebuffer);
                // update props
		        Command(COMMAND_UPDATE_PROPERTIES);
                RedrawScene();
			}
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_RELOAD:
		if( !Scene.locked() ){
        	Command(COMMAND_LOAD,(int)m_LastFileName);
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_SAVE:
		if( !Scene.locked() ){
			if( m_LastFileName[0] ){
                SetStatus("Level saving...");
				Scene.Save( m_LastFileName, false );
                SetStatus("");
                Scene.m_Modified = false;
			    Command(COMMAND_UPDATE_CAPTION);
			}else{
				bRes = Command( COMMAND_SAVEAS ); }
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

    case COMMAND_SAVE_BACKUP:{
    	AnsiString fn = AnsiString(Engine.FS.m_CompName)+"_"+Engine.FS.m_UserName+"_backup.level";
        Engine.FS.m_Maps.Update(fn);
    	Command(COMMAND_SAVEAS,(int)fn.c_str());
    }break;
	case COMMAND_SAVEAS:
		if( !Scene.locked() ){
			filebuffer[0] = 0;
			if(p1 || Engine.FS.GetSaveName( Engine.FS.m_Maps, filebuffer, sizeof(filebuffer) ) ){
            	if (p1)	strcpy(filebuffer,(LPCSTR)p1);
                SetStatus("Level saving...");
				Scene.Save( filebuffer, false );
                SetStatus("");
                Scene.m_Modified = false;
				// unlock
    	        Engine.FS.UnlockFile(0,m_LastFileName);
                // set new name
                Engine.FS.LockFile(0,filebuffer);
				strcpy(m_LastFileName,filebuffer);
			    bRes = Command(COMMAND_UPDATE_CAPTION);
                fraLeftBar->AppendRecentFile(filebuffer);
			}else
            	bRes = false;
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_CLEAR:
		if( !Scene.locked() ){
            if (!Scene.IfModified()) return false;
			// unlock
			Engine.FS.UnlockFile(0,m_LastFileName);
			Device.m_Camera.Reset();
			Scene.Unload();
            Scene.m_LevelOp.Reset();
			m_LastFileName[0] = 0;
           	Scene.UndoClear();
            Scene.m_Modified = false;
			Command(COMMAND_UPDATE_CAPTION);
			Command(COMMAND_CHANGE_TARGET,etObject);
			Command(COMMAND_CHANGE_ACTION,eaSelect);
		    Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        Command(COMMAND_UPDATE_PROPERTIES);
		break;

    case COMMAND_LOAD_FIRSTRECENT:
    	if (fraLeftBar->FirstRecentFile()){
        	bRes = Command(COMMAND_LOAD,(int)fraLeftBar->FirstRecentFile());
        }
    	break;

	case COMMAND_CLEAR_COMPILER_ERROR:
    	Scene.ClearCompilerErrors();
        break;
        
    case COMMAND_IMPORT_COMPILER_ERROR:{
    	AnsiString fn;
    	if(Engine.FS.GetOpenName(Engine.FS.m_ServerRoot, fn)){
        	Scene.LoadCompilerError(fn.c_str());
        }
    	}break;
    
	case COMMAND_VALIDATE_SCENE:
		if( !Scene.locked() ){
            Scene.Validate(true,false);
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

    case COMMAND_REFRESH_LIBRARY:
        if (!Scene.ObjCount()&&!Scene.locked()){
	    	Lib.RefreshLibrary();
        }else{
            ELog.DlgMsg(mtError, "Scene must be empty before refreshing library!");
			bRes = false;
        }
    	break;

    case COMMAND_RELOAD_OBJECTS:
    	Lib.ReloadObjects();
    	break;

	case COMMAND_CUT:
		if( !Scene.locked() ){
			Scene.CutSelection(Tools.CurrentClassID());
            fraLeftBar->miPaste->Enabled = true;
            fraLeftBar->miPaste2->Enabled = true;
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_COPY:
		if( !Scene.locked() ){
			Scene.CopySelection(Tools.CurrentClassID());
            fraLeftBar->miPaste->Enabled = true;
            fraLeftBar->miPaste2->Enabled = true;
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_PASTE:
		if( !Scene.locked() ){
			Scene.PasteSelection();
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_UNDO:
		if( !Scene.locked() ){
			if( !Scene.Undo() ) ELog.DlgMsg( mtInformation, "Undo buffer empty" );
            else{
	            Tools.Reset();
			    Command(COMMAND_CHANGE_ACTION, eaSelect);
            }
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_REDO:
		if( !Scene.locked() ){
			if( !Scene.Redo() ) ELog.DlgMsg( mtInformation, "Redo buffer empty" );
            else{
	            Tools.Reset();
			    Command(COMMAND_CHANGE_ACTION, eaSelect);
            }
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_OPTIONS:
		if( !Scene.locked() ){
            if (mrOk==frmScenePropertiesRun(&Scene.m_LevelOp.m_BuildParams,false))
            	Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_BUILD:
		if( !Scene.locked() ){
            if (frmScenePropertiesRun(&Scene.m_LevelOp.m_BuildParams,true)==mrOk)
                Builder.Compile( );
        }else{
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_MAKE_GAME:
		if( !Scene.locked() ){
            if (frmScenePropertiesRun(&Scene.m_LevelOp.m_BuildParams,true)==mrOk)
                Builder.MakeGame( );
        }else{
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;
    case COMMAND_MAKE_DETAILS:
		if( !Scene.locked() ){
            Builder.MakeDetails();
        }else{
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
	case COMMAND_INVERT_SELECTION_ALL:
		if( !Scene.locked() ){
			Scene.InvertSelection(Tools.CurrentClassID());
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_SELECT_ALL:
		if( !Scene.locked() ){
			Scene.SelectObjects(true,Tools.CurrentClassID());
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_DESELECT_ALL:
		if( !Scene.locked() ){
			Scene.SelectObjects(false,Tools.CurrentClassID());
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_DELETE_SELECTION:
		if( !Scene.locked() ){
			Scene.RemoveSelection( Tools.CurrentClassID() );
			Scene.UndoSave();
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;

	case COMMAND_HIDE_UNSEL:
		if( !Scene.locked() ){
			Scene.ShowObjects( false, Tools.CurrentClassID(), true, false );
			Scene.UndoSave();
	        Command(COMMAND_UPDATE_PROPERTIES);
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
	case COMMAND_HIDE_SEL:
		if( !Scene.locked() ){
			Scene.ShowObjects( bool(p1), Tools.CurrentClassID(), true, true );
			Scene.UndoSave();
	        Command(COMMAND_UPDATE_PROPERTIES);
		} else {
			ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
	case COMMAND_HIDE_ALL:
		if( !Scene.locked() ){
			Scene.ShowObjects( bool(p1), Tools.CurrentClassID(), false );
			Scene.UndoSave();
	        Command(COMMAND_UPDATE_PROPERTIES);
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
    case COMMAND_LOCK_ALL:
		if( !Scene.locked() ){
			Scene.LockObjects(bool(p1),Tools.CurrentClassID(),false);
			Scene.UndoSave();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_LOCK_SEL:
		if( !Scene.locked() ){
			Scene.LockObjects(bool(p1),Tools.CurrentClassID(),true,true);
			Scene.UndoSave();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_LOCK_UNSEL:
		if( !Scene.locked() ){
			Scene.LockObjects(bool(p1),Tools.CurrentClassID(),true,false);
			Scene.UndoSave();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_RESET_ANIMATION:
		if( !Scene.locked() ){
	    	Scene.ResetAnimation();
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
		break;
    case COMMAND_SET_SNAP_OBJECTS:
		if( !Scene.locked() ){
	    	int cnt=Scene.SetSnapList();
 			Scene.UndoSave();
        	ELog.Msg( mtInformation, "%d snap object(s) selected.", cnt );
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
        break;
    case COMMAND_ADD_SNAP_OBJECTS:
		if( !Scene.locked() ){
	    	int cnt=Scene.AddToSnapList();
 			Scene.UndoSave();
        	ELog.Msg( mtInformation, "%d object(s) appended.", cnt );
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    case COMMAND_CLEAR_SNAP_OBJECTS:
		if( !Scene.locked() ){
	    	Scene.ClearSnapList();
 			Scene.UndoSave();
	       	ELog.Msg( mtInformation, "Snap list empty.");
		}else{
        	ELog.DlgMsg( mtError, "Scene sharing violation" );
			bRes = false;
        }
    	break;
    default:
		ELog.DlgMsg( mtError, "Warning: Undefined command: %04d", _Command );
        bRes = false;
    }
    return 	bRes;
}

char* TUI::GetCaption()
{
	return GetEditFileName()[0]?GetEditFileName():"noname";
}

bool __fastcall TUI::ApplyShortCutExt(WORD Key, TShiftState Shift)
{
	bool bExec = false;
    if (Shift.Contains(ssCtrl)){
        if (Key==VK_F5)    				{Command(COMMAND_BUILD);                		bExec=true;}
        else if (Key==VK_F7)    		{Command(COMMAND_OPTIONS);                      bExec=true;}
        else if (Key=='A')    			{Command(COMMAND_SELECT_ALL);                   bExec=true;}
        else if (Key=='I')    			{Command(COMMAND_INVERT_SELECTION_ALL);         bExec=true;}
       	else if (Key=='1') 	 			{Command(COMMAND_CHANGE_TARGET, etGroup);       bExec=true;}
		else if (Key=='2')				{Command(COMMAND_CHANGE_TARGET, etPS);          bExec=true;}
        else if (Key=='3')				{Command(COMMAND_CHANGE_TARGET, etShape);       bExec=true;}
    }else{                                                                              
        if (Shift.Contains(ssAlt)){                                                     
        	if (Key=='F')   			{Command(COMMAND_FILE_MENU);                    bExec=true;}
        }else{                                                                          
            if (Key=='1')     			{Command(COMMAND_CHANGE_TARGET, etObject);      bExec=true;}
        	else if (Key=='2')  		{Command(COMMAND_CHANGE_TARGET, etLight);       bExec=true;}
        	else if (Key=='3')  		{Command(COMMAND_CHANGE_TARGET, etSound);       bExec=true;}
        	else if (Key=='4')  		{Command(COMMAND_CHANGE_TARGET, etEvent);       bExec=true;}
        	else if (Key=='5')  		{Command(COMMAND_CHANGE_TARGET, etGlow);        bExec=true;}
        	else if (Key=='6')  		{Command(COMMAND_CHANGE_TARGET, etDO);          bExec=true;}
        	else if (Key=='7')  		{Command(COMMAND_CHANGE_TARGET, etSpawnPoint);  bExec=true;}
        	else if (Key=='8')  		{Command(COMMAND_CHANGE_TARGET, etWay);         bExec=true;}
        	else if (Key=='9')  		{Command(COMMAND_CHANGE_TARGET, etSector);      bExec=true;}
        	else if (Key=='0')  		{Command(COMMAND_CHANGE_TARGET, etPortal);      bExec=true;}
            // simple press             
        	else if (Key=='W')			{Command(COMMAND_SHOW_OBJECTLIST);              bExec=true;}
        	else if (Key==VK_DELETE)	{Command(COMMAND_DELETE_SELECTION);             bExec=true;}
        	else if (Key==VK_RETURN)	{Command(COMMAND_SHOW_PROPERTIES);              bExec=true;}
            else if (Key==VK_OEM_MINUS)	{Command(COMMAND_HIDE_SEL, FALSE);              bExec=true;}
            else if (Key==VK_OEM_PLUS)	{Command(COMMAND_HIDE_UNSEL, FALSE);            bExec=true;}
            else if (Key==VK_OEM_5)		{Command(COMMAND_HIDE_ALL, TRUE);               bExec=true;}
        	else if (Key=='N'){
            	switch (Tools.GetAction()){
            	case eaMove: 			{Command(COMMAND_SET_NUMERIC_POSITION); bExec=true;} 	break;
			    case eaRotate: 			{Command(COMMAND_SET_NUMERIC_ROTATION); bExec=true;} 	break;
            	case eaScale: 			{Command(COMMAND_SET_NUMERIC_SCALE);    bExec=true;}	break;
            	}
            }
        }
    }
    return bExec;
}
//---------------------------------------------------------------------------

bool __fastcall TUI::ApplyGlobalShortCutExt(WORD Key, TShiftState Shift)
{
	bool bExec = false;
    if (Shift.Contains(ssCtrl)){
        if (Key=='V')    				{Command(COMMAND_PASTE);                bExec=true;}
        else if (Key=='C')    			{Command(COMMAND_COPY);                 bExec=true;}
        else if (Key=='X')    			{Command(COMMAND_CUT);                  bExec=true;}
        else if (Key=='Z')    			{Command(COMMAND_UNDO);                 bExec=true;}
        else if (Key=='Y')    			{Command(COMMAND_REDO);                 bExec=true;}
		else if (Key=='R')				{Command(COMMAND_LOAD_FIRSTRECENT);     bExec=true;}
    }
    return bExec;
}
//---------------------------------------------------------------------------

bool TUI::PickGround(Fvector& hitpoint, const Fvector& start, const Fvector& direction, int bSnap, Fvector* hitnormal){
	VERIFY(m_bReady);
    // pick object geometry
    if ((bSnap==-1)||(fraTopBar->ebOSnap->Down&&(bSnap==1))){
        bool bPickObject;
        SRayPickInfo pinf;
	    EEditorState est = GetEState();
        switch(est){
        case esEditLibrary:		bPickObject = !!TfrmEditLibrary::RayPick(start,direction,&pinf); break;
        case esEditScene:		bPickObject = !!Scene.RayPick(start,direction,OBJCLASS_SCENEOBJECT,&pinf,false,Scene.GetSnapList()); break;
        default: return false;
        }
        if (bPickObject){
		    if (fraTopBar->ebVSnap->Down&&bSnap){
                Fvector pn;
                float u = pinf.inf.u;
                float v = pinf.inf.v;
                float w = 1-(u+v);
				Fvector verts[3];
                pinf.s_obj->GetFaceWorld(pinf.e_mesh,pinf.inf.id,verts);
                if ((w>u) && (w>v)) pn.set(verts[0]);
                else if ((u>w) && (u>v)) pn.set(verts[1]);
                else pn.set(verts[2]);
                if (pn.distance_to(pinf.pt) < movesnap()) hitpoint.set(pn);
                else hitpoint.set(pinf.pt);
            }else{
            	hitpoint.set(pinf.pt);
            }
            if (hitnormal){
	            Fvector verts[3];
    	        pinf.s_obj->GetFaceWorld(pinf.e_mesh,pinf.inf.id,verts);
        	    hitnormal->mknormal(verts[0],verts[1],verts[2]);
            }
			return true;
        }
    }

    // pick grid
	Fvector normal;
	normal.set( 0, 1, 0 );
	float clcheck = direction.dotproduct( normal );
	if( fis_zero( clcheck ) ) return false;
	float alpha = - start.dotproduct(normal) / clcheck;
	if( alpha <= 0 ) return false;

	hitpoint.x = start.x + direction.x * alpha;
	hitpoint.y = start.y + direction.y * alpha;
	hitpoint.z = start.z + direction.z * alpha;

    if (fraTopBar->ebGSnap->Down && bSnap){
        hitpoint.x = snapto( hitpoint.x, movesnap() );
        hitpoint.z = snapto( hitpoint.z, movesnap() );
        hitpoint.y = 0.f;
    }
	if (hitnormal) hitnormal->set(0,1,0);
	return true;
}
//----------------------------------------------------

bool TUI::SelectionFrustum(CFrustum& frustum){
	VERIFY(m_bReady);
    Fvector st,d,p[4];
    Ivector2 pt[4];

    float depth = 0;

    float x1=m_StartCp.x, x2=m_CurrentCp.x;
    float y1=m_StartCp.y, y2=m_CurrentCp.y;

	if(!(x1!=x2&&y1!=y2)) return false;

	pt[0].set(min(x1,x2),min(y1,y2));
	pt[1].set(max(x1,x2),min(y1,y2));
	pt[2].set(max(x1,x2),max(y1,y2));
	pt[3].set(min(x1,x2),max(y1,y2));

    SRayPickInfo pinf;
    for (int i=0; i<4; i++){
	    Device.m_Camera.MouseRayFromPoint(st, d, pt[i]);
        if (frmEditorPreferences->cbBoxPickLimitedDepth->Checked){
			pinf.inf.range = Device.m_Camera.m_Zfar; // max pick range
            if (Scene.RayPick(st, d, OBJCLASS_SCENEOBJECT, &pinf, false, 0))
	            if (pinf.inf.range > depth) depth = pinf.inf.range;
        }
    }
    if (depth<Device.m_Camera.m_Znear) depth = Device.m_Camera.m_Zfar;
    else depth += frmEditorPreferences->seBoxPickDepthTolerance->Value;

    for (i=0; i<4; i++){
	    Device.m_Camera.MouseRayFromPoint(st, d, pt[i]);
        p[i].mad(st,d,depth);
    }

    frustum.CreateFromPoints(p,4,Device.m_Camera.m_Position);

    Fplane P; P.build(p[0],p[1],p[2]);
    if (P.classify(st)>0) P.build(p[2],p[1],p[0]);
	frustum._add(P);

	return true;
}
//----------------------------------------------------
void TUI::RealUpdateScene(){
	if (GetEState()==esEditScene){
	    Scene.OnObjectsUpdate();
    	Tools.OnObjectsUpdate(); // обновить все что как-то связано с объектами
	    RedrawScene();
    }
    m_Flags.set(flUpdateScene,FALSE);
}
//---------------------------------------------------------------------------


void TUI::ShowContextMenu(int cls)
{
	VERIFY(m_bReady);
    POINT pt;
    GetCursorPos(&pt);
    fraLeftBar->miProperties->Enabled = false;
    if (Scene.SelectionCount( true, (EObjClass)cls )) fraLeftBar->miProperties->Enabled = true;
    RedrawScene(true);
    fraLeftBar->pmObjectContext->TrackButton = tbRightButton;
    fraLeftBar->pmObjectContext->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------

