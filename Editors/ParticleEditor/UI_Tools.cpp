//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Tools.h"
#include "EditObject.h"
#include "ChoseForm.h"
#include "ui_main.h"
#include "leftbar.h"
#include "topbar.h"
#include "PSLibrary.h"
#include "ParticleSystem.h"
#include "ItemList.h"
#include "xr_trims.h"
#include "library.h"
#include "Render.h"
#include "folderlib.h"
#include "ItemDialog.h"

#include "TextForm.h"
#include "d3dutils.h"
//------------------------------------------------------------------------------
CParticleTools Tools;
//------------------------------------------------------------------------------
#define CHECK_SNAP(R,A,C){ R+=A; if(fabsf(R)>=C){ A=snapto(R,C); R=0; }else{A=0;}}
static Fvector zero_vec={0.f,0.f,0.f};

CParticleTools::CParticleTools()
{
    m_ItemProps 		= 0;
	m_EditObject		= 0;
    m_EditText			= 0;
    m_bModified			= false;
    m_bReady			= false;
    m_Transform.identity();
    fFogness			= 0.9f;
    dwFogColor			= 0xffffffff;
}
//---------------------------------------------------------------------------

CParticleTools::~CParticleTools()
{
}
//---------------------------------------------------------------------------

bool CParticleTools::OnCreate()
{
	// shader test locking
	AnsiString fn; 
    FS.update_path(fn,_game_data_,PSLIB_FILENAME);
	if (EFS.CheckLocking(0,fn.c_str(),false,true)) return false;

    Device.seqDevCreate.Add(this);
    Device.seqDevDestroy.Add(this);

    m_bReady = true;

    Load();

    ChangeAction(eaSelect);

	// lock
    EFS.LockFile(0,fn.c_str());
/*    
    AnsiString fn1;
    FS.update_path(fn1,_game_data_,"a.txt");
	HANDLE handle;
    handle=CreateFile(fn1.c_str(),GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
    handle=CreateFile(fn1.c_str(),GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
    handle=CreateFile(fn1.c_str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
    handle=CreateFile(fn1.c_str(),GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
    LPVOID lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &lpMsgBuf, 0, NULL);
	LocalFree( lpMsgBuf );
*/
    m_EditPE 		= (PS::CParticleEffect*)::Render->Models->CreatePE(0);
    m_EditPG		= (PS::CParticleGroup*)::Render->Models->CreatePG(0);
    m_ItemProps 	= TProperties::CreateForm("",fraLeftBar->paItemProps,alClient,OnItemModified);
    // item list
    m_PList			= TItemList::CreateForm("",fraLeftBar->paItemList,alClient,TItemList::ilEditMenu|TItemList::ilDragAllowed);
    m_PList->OnItemsFocused	= OnParticleItemFocused;
	m_PList->OnItemRename	= OnParticleItemRename;
    m_PList->OnItemRemove	= OnParticleItemRemove;
    m_PList->SetImages		(fraLeftBar->ilModeIcons);

    return true;
}

void CParticleTools::OnDestroy()
{
	VERIFY(m_bReady);
    m_bReady			= false;
	// unlock
    EFS.UnlockFile		(_game_data_,PSLIB_FILENAME);

	Lib.RemoveEditObject(m_EditObject);
    TItemList::DestroyForm(m_PList);
	TProperties::DestroyForm(m_ItemProps);
    xr_delete			(m_EditPG);
    xr_delete			(m_EditPE);
    Device.seqDevCreate.Remove(this);
    Device.seqDevDestroy.Remove(this);
}
//---------------------------------------------------------------------------

bool CParticleTools::IfModified()
{
    if (m_bModified){
        int mr = ELog.DlgMsg(mtConfirmation, "The particles has been modified.\nDo you want to save your changes?");
        switch(mr){
        case mrYes: if (!UI.Command(COMMAND_SAVE)) return false; else m_bModified = FALSE; break;
        case mrNo: m_bModified = FALSE; break;
        case mrCancel: return false;
        }
    }
    return true;
}

void CParticleTools::Modified()
{
	m_bModified = true;
	UI.Command(COMMAND_UPDATE_CAPTION);
}
//---------------------------------------------------------------------------

void CParticleTools::OnItemModified()
{
	Modified();
    if (m_LibPED){
	    m_LibPED->m_ModifName		= AnsiString().sprintf("\\\\%s\\%s",Core.CompName,Core.UserName).c_str();
    	m_LibPED->m_ModifTime		= time(NULL);
    }
    if (m_LibPGD){
	    m_LibPGD->m_ModifName		= AnsiString().sprintf("\\\\%s\\%s",Core.CompName,Core.UserName).c_str();
    	m_LibPGD->m_ModifTime		= time(NULL);
    }
	UI.Command(COMMAND_UPDATE_PROPERTIES);
}


void CParticleTools::Render()
{
	if (!m_bReady) return;

	if (m_EditObject)	m_EditObject->RenderSingle(Fidentity);
	// draw parent
	RCache.set_xform_world(m_Transform);
    Device.SetShader(Device.m_WireShader);
    DU.DrawCross	(zero_vec,0.20f,0.25f,0.20f,0.20f,0.25f,0.20f,0xFFFFEBAA,false);
	// Draw the particles.
	RCache.set_xform_world		(Fidentity);
    switch(m_EditMode){
    case emEffect:{	
		if (m_Flags.is(flDrawDomain)&&m_EditPE&&m_EditPE->GetDefinition())	
        	m_EditPE->GetDefinition()->Render();
    	::Render->Models->RenderSingle(m_EditPE,Fidentity,1.f);	
    }break;
    case emGroup:{
    	if (m_EditPG&&m_Flags.is(flDrawDomain)){
         	int cnt = m_EditPG->children.size();
            for (int k=0; k<cnt; k++){
                IRender_Visual*  V  = m_EditPG->children[k];
                PS::CParticleEffect* E	= (PS::CParticleEffect*)V;
				if (E&&E->GetDefinition())	E->GetDefinition()->Render();
            }
        }
    	::Render->Models->RenderSingle(m_EditPG,Fidentity,1.f);	
    }break;
    default: THROW;
    }
}

void CParticleTools::OnFrame()
{
	if (!m_bReady) return;
	if (m_EditObject)
    	m_EditObject->OnFrame();
        
    m_EditPE->OnFrame(Device.dwTimeDelta);
    m_EditPG->OnFrame(Device.dwTimeDelta);

	if (m_Flags.is(flRefreshProps))
    	RealUpdateProperties();

	if (m_Flags.is(flApplyParent))
    	RealApplyParent();

    switch(m_EditMode){
    case emEffect:	if (m_EditPE->IsPlaying())UI.SetStatus(" PE Playing...",false); else UI.SetStatus(" Stopped.",false); break;
    case emGroup:	if (m_EditPG->IsPlaying())UI.SetStatus(" PG Playing...",false); else UI.SetStatus(" Stopped.",false); break;
    default: THROW;
    }
}

void CParticleTools::ZoomObject(BOOL bSelOnly)
{
	VERIFY(m_bReady);
    if (!bSelOnly&&m_EditObject){
        Device.m_Camera.ZoomExtents(m_EditObject->GetBox());
	}else{
    	Fbox box; box.invalidate();
        switch(m_EditMode){
        case emEffect:	box.set(m_EditPE->vis.box);	break;
        case emGroup:	box.set(m_EditPG->vis.box);	break;
	    default: THROW;
        }
        if (box.is_valid()){ box.grow(1.f); Device.m_Camera.ZoomExtents(box); }
    }
}

void CParticleTools::OnDeviceCreate()
{
	VERIFY(m_bReady);
    // add directional light
    Flight L;
    ZeroMemory(&L,sizeof(Flight));
    L.type = D3DLIGHT_DIRECTIONAL;
    L.diffuse.set(1,1,1,1);
    L.direction.set(1,-1,1); L.direction.normalize();
	Device.SetLight(0,L);
	Device.LightEnable(0,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(-1,-1,-1); L.direction.normalize();
	Device.SetLight(1,L);
	Device.LightEnable(1,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(1,-1,-1); L.direction.normalize();
	Device.SetLight(2,L);
	Device.LightEnable(2,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(-1,-1,1); L.direction.normalize();
	Device.SetLight(3,L);
	Device.LightEnable(3,true);

	L.diffuse.set(1.0,0.8,0.7,1);
    L.direction.set(0,1,0); L.direction.normalize();
	Device.SetLight(4,L);
	Device.LightEnable(4,true);

    m_EditPE->OnDeviceCreate();
    m_EditPG->OnDeviceCreate();
}

void CParticleTools::OnDeviceDestroy()
{
    m_EditPG->OnDeviceDestroy();
    m_EditPE->OnDeviceDestroy();
}

void CParticleTools::SelectPreviewObject(int p){
    LPCSTR fn;
    if (!TfrmChoseItem::SelectItem(TfrmChoseItem::smObject,fn,1,m_EditObject?m_EditObject->GetName():0)) return;
    Lib.RemoveEditObject(m_EditObject);
    m_EditObject = fn?Lib.CreateEditObject(fn):0;
//	ZoomObject(TRUE);
    UI.RedrawScene();
}

void CParticleTools::ResetPreviewObject()
{
	VERIFY(m_bReady);
    UI.RedrawScene();
}

void CParticleTools::Load()
{
	VERIFY(m_bReady);
    UpdateProperties();
}

void CParticleTools::Save()
{
	VERIFY			(m_bReady);
	m_bModified 	= false;
	// backup
    EFS.BackupFile	(_game_data_,PSLIB_FILENAME);
	// save   
    EFS.UnlockFile	(_game_data_,PSLIB_FILENAME,false);
	::Render->PSLibrary.Save();
    EFS.LockFile	(_game_data_,PSLIB_FILENAME,false);
}

void CParticleTools::Reload()
{
	VERIFY(m_bReady);
    ResetCurrent	();
	::Render->PSLibrary.Reload();
    // visual part
    m_ItemProps->ClearProperties();
    UpdateProperties(true);
}

void CParticleTools::Merge()
{
	AnsiString fn;
    if (EFS.GetOpenName(_import_,fn,false,NULL,3)){
    	// load lib
        CPSLibrary 	ps_new;
        ps_new.Load	(fn.c_str());
        // compare effects
        u32 append_p=0, rename_p=0, replace_p=0, skip_p=0;
        Log("Merging particle systems...");
        bool bReplaceAll=false,bRenameAll=false,bSkipAll=false;
        {
            for (PS::PEDIt b_it=ps_new.FirstPED(); b_it!=ps_new.LastPED(); b_it++){
            	PS::CPEDef* x = ::Render->PSLibrary.FindPED((*b_it)->m_Name);
                if (x){
                	(*b_it)->Compile();
                	if (!x->Equal(*b_it)){
                        int res;
                        if (bReplaceAll)	res = 0;
                        else if (bRenameAll)res = 1;
                        else if (bSkipAll)	res = 2;
                        else				res = TfrmItemDialog::Run("Item", AnsiString().sprintf("Overwrite particle effect: '%s'",x->m_Name).c_str(), "Replace,Rename,Skip,Replace All,Rename All,Skip All,Cancel");
                        switch(res){
                        case 3: bReplaceAll	= true; 
                        case 0:{
                        	::Render->PSLibrary.Remove(x->m_Name);
                            ::Render->PSLibrary.AppendPED(*b_it);
                            Msg(". Replace: '%s'",(*b_it)->m_Name);
                            replace_p++;
                        }break;
                        case 4: bRenameAll	= true;
                        case 1:{
                            AnsiString pref 	= AnsiString(x->m_Name)+"_old";
                            AnsiString new_name = FHelper.GenerateName(pref.c_str(),2,::Render->PSLibrary.FindByName);
                            strcpy				(x->m_Name,new_name.c_str());
                            ::Render->PSLibrary.AppendPED(*b_it);
                            Msg(". Rename: '%s'",new_name.c_str());
                            rename_p++;
                        }break;
                        case 5: bSkipAll	= true;
                        case 2: 
                            Msg(". Skip: '%s'",(*b_it)->m_Name);
                        	skip_p++;
                        break;
                        case -1:
                        case 6: return;
                        }
                    }
                }else{
                    // append new
                    ::Render->PSLibrary.AppendPED(*b_it);
                    Msg(". Append: '%s'",(*b_it)->m_Name);
                    append_p++;
                }
            }
        }
        // compare groups
        {
            for (PS::PGDIt b_it=ps_new.FirstPGD(); b_it!=ps_new.LastPGD(); b_it++){
            	PS::CPGDef* x = ::Render->PSLibrary.FindPGD((*b_it)->m_Name);
                if (x){
                	if (!x->Equal(*b_it)){
                        int res;
                        if (bReplaceAll)	res = 0;
                        else if (bRenameAll)res = 1;
                        else if (bSkipAll)	res = 2;
                        else				res = TfrmItemDialog::Run("Item", AnsiString().sprintf("Overwrite particle group: '%s'",x->m_Name).c_str(), "Replace,Rename,Skip,Replace All,Rename All,Skip All,Cancel");
                        switch(res){
                        case 3: bReplaceAll	= true; 
                        case 0:{
                        	::Render->PSLibrary.Remove(x->m_Name);
                            ::Render->PSLibrary.AppendPGD(*b_it);
                            Msg(". Replace: '%s'",(*b_it)->m_Name);
                            replace_p++;
                        }break;
                        case 4: bRenameAll	= true;
                        case 1:{
                            AnsiString pref 	= AnsiString(x->m_Name)+"_old";
                            AnsiString new_name = FHelper.GenerateName(pref.c_str(),2,::Render->PSLibrary.FindByName);
                            strcpy				(x->m_Name,new_name.c_str());
                            ::Render->PSLibrary.AppendPGD(*b_it);
                            Msg(". Rename: '%s'",new_name.c_str());
                            rename_p++;
                        }break;
                        case 5: bSkipAll	= true;
                        case 2: 
                            Msg(". Skip: '%s'",(*b_it)->m_Name);
                        	skip_p++;
                        break;
                        case -1:
                        case 6: return;
                        }
                    }
                }else{
                    // append new
                    ::Render->PSLibrary.AppendPGD(*b_it);
                    Msg(". Append: '%s'",(*b_it)->m_Name);
                    append_p++;
                }
            }
        }
        if (append_p+rename_p+replace_p+skip_p){
	        ELog.DlgMsg(mtInformation,"Merge result: [%d appended / %d replaced / %d renamed / %d skipped]",append_p,replace_p,rename_p,skip_p);
        	::Render->PSLibrary.OnDeviceDestroy();
        	::Render->PSLibrary.OnDeviceCreate();
		    UI.Command	(COMMAND_UPDATE_PROPERTIES);
            Modified	();
        }
	    ps_new.OnDestroy();
    }
}

void CParticleTools::Rename(LPCSTR old_full_name, LPCSTR ren_part, int level)
{
    VERIFY(level<_GetItemCount(old_full_name,'\\'));
    AnsiString new_full_name;
    _ReplaceItem(old_full_name,level,ren_part,new_full_name,'\\');
    Rename(old_full_name, new_full_name.c_str());
}

void CParticleTools::Rename(LPCSTR old_full_name, LPCSTR new_full_name)
{
	VERIFY(m_bReady);
    // is effect
	PS::CPEDef* E = ::Render->PSLibrary.FindPED(old_full_name);
    if (E){
        ::Render->PSLibrary.RenamePED(E,new_full_name);
    	return;
    }
    // is group
	PS::CPGDef* G = ::Render->PSLibrary.FindPGD(old_full_name);
    if (G){
        ::Render->PSLibrary.RenamePGD(G,new_full_name);
    	return;
    }
}

void CParticleTools::Remove(LPCSTR name)
{
	VERIFY(m_bReady);
	::Render->PSLibrary.Remove(name);
}

void CParticleTools::RemoveCurrent()
{
    TElTreeItem* pNode = m_PList->GetSelected();
    if (pNode)
		FHelper.RemoveItem(m_PList->tvItems,pNode,OnParticleItemRemove);
}

void CParticleTools::CloneCurrent()
{
    TElTreeItem* pNode = m_PList->GetSelected();
    if (pNode&&FHelper.IsObject(pNode)){
		AnsiString full_name;
		FHelper.MakeName(pNode,0,full_name,false);
        PS::CPEDef* PE=FindPE(full_name.c_str());
        if (PE){
        	AppendPE(PE);
            Modified();
        }else{
	        PS::CPGDef* PG=FindPG(full_name.c_str());
            if (PG){
	        	AppendPG(PG);
    	        Modified();
            }
        }
    }else{                                    
		ELog.DlgMsg(mtInformation, "At first select object.");
    }
}

void CParticleTools::ResetCurrent()
{
	VERIFY(m_bReady);
    if (m_LibPED) m_EditPE->Stop(FALSE);
    if (m_LibPGD) m_EditPG->Stop(FALSE);
    m_LibPED= 0;
    m_LibPGD= 0;
}

void CParticleTools::SetCurrentPE(PS::CPEDef* P)
{
	VERIFY(m_bReady);
    // load shader
    if (m_LibPED&&m_EditText&&m_EditText->Modified()){
//    	if (ELog.DlgMsg(mtConfirmation, "The commands has been modified.\nDo you want to apply your changes?")==mrYes)
	    	m_EditText->ApplyEdit();
    }
	if (m_LibPED!=P){
	    m_LibPED = P;
        m_EditPE->Compile(m_LibPED);
		if (m_LibPED){ 
			m_EditMode		= emEffect;
		    if (m_EditText)	EditActionList();
        }else if (m_EditText) TfrmText::DestroyForm(m_EditText);
    }
}

void CParticleTools::SetCurrentPG(PS::CPGDef* P)
{
	VERIFY(m_bReady);
//	if (m_LibPGD!=P)
    {
	    m_LibPGD = P;
        if (m_LibPGD){
			m_EditMode		= emGroup;
	        m_EditPG->Compile(m_LibPGD);
        }
    }
}

PS::CPEDef*	CParticleTools::FindPE(LPCSTR name)
{
	return ::Render->PSLibrary.FindPED(name);
}

PS::CPGDef*	CParticleTools::FindPG(LPCSTR name)
{
	return ::Render->PSLibrary.FindPGD(name);
}

void CParticleTools::PlayCurrent()
{
	VERIFY(m_bReady);
    StopCurrent		(false);
    switch(m_EditMode){
    case emEffect:	m_EditPE->Play(); break;
    case emGroup:	m_EditPG->Play(); break;
    default: THROW;
    }
    ApplyParent		();
}

void CParticleTools::StopCurrent(bool bFinishPlaying)
{
	VERIFY(m_bReady);
    switch(m_EditMode){
    case emEffect:	m_EditPE->Stop(bFinishPlaying);	break;
    case emGroup:	m_EditPG->Stop(bFinishPlaying);	break;
    default: THROW;
    }
}

void CParticleTools::OnShowHint(AStringVec& SS)
{
}

void CParticleTools::ChangeAction(EAction action)
{
	switch(action){
    case eaSelect: m_bHiddenMode=false; break;
    case eaAdd:
    case eaMove:
    case eaRotate:
    case eaScale:  m_bHiddenMode=true; break;
    }
    m_Action = action;
    switch(m_Action){
        case eaSelect:  UI.GetD3DWindow()->Cursor = crCross;     break;
        case eaAdd:     UI.GetD3DWindow()->Cursor = crArrow;     break;
        case eaMove:    UI.GetD3DWindow()->Cursor = crSizeAll;   break;
        case eaRotate:  UI.GetD3DWindow()->Cursor = crSizeWE;    break;
        case eaScale:   UI.GetD3DWindow()->Cursor = crVSplit;    break;
        default:        UI.GetD3DWindow()->Cursor = crHelp;
    }
    UI.RedrawScene();
    fraTopBar->ChangeAction(m_Action);
}

bool __fastcall CParticleTools::MouseStart(TShiftState Shift)
{
	switch(m_Action){
    case eaSelect: return false;
    case eaAdd:{
        if (m_EditObject){
            Fvector p;
            float dist=UI.ZFar();
            SRayPickInfo pinf;
            if (m_EditObject->RayPick(dist,UI.m_CurrentRStart,UI.m_CurrentRNorm,Fidentity,&pinf)){
                R_ASSERT(pinf.e_mesh);
//.				m_EditPS.m_DefaultEmitter.m_Position.set(pinf.pt);
            }
        }
    }break;
    case eaMove:{
        if (Shift.Contains(ssCtrl)){
        	if (m_EditObject){
                float dist = flt_max;
                SRayPickInfo pinf;
                if (m_EditObject->RayPick(dist,UI.m_CurrentRStart,UI.m_CurrentRNorm,Fidentity,&pinf))
                    m_Transform.c.set(pinf.pt);
            }else{
                // pick grid
                Fvector normal={0.f, 1.f, 0.f};
                float clcheck = UI.m_CurrentRNorm.dotproduct( normal );
                if( fis_zero( clcheck ) ) return false;
                float alpha = - UI.m_CurrentRStart.dotproduct(normal) / clcheck;
                if( alpha <= 0 ) return false;

                m_Transform.c.mad(UI.m_CurrentRStart,UI.m_CurrentRNorm,alpha);

                if (fraTopBar->ebGSnap->Down){
                    m_Transform.c.x = snapto( m_Transform.c.x, UI.movesnap() );
                    m_Transform.c.z = snapto( m_Transform.c.z, UI.movesnap() );
                    m_Transform.c.y = 0.f;
                }
            }
        }else{
            if (fraTopBar->ebAxisY->Down){
                m_MovingXVector.set(0,0,0);
                m_MovingYVector.set(0,1,0);
            }else{
                m_MovingXVector.set( Device.m_Camera.GetRight() );
                m_MovingXVector.y = 0;
                m_MovingYVector.set( Device.m_Camera.GetDirection() );
                m_MovingYVector.y = 0;
                m_MovingXVector.normalize_safe();
                m_MovingYVector.normalize_safe();
            }
            m_MovingReminder.set(0,0,0);
        }
    }break;
    case eaRotate:{
        m_RotateCenter.set(0,0,0);
        m_RotateVector.set(0,0,0);
        if (fraTopBar->ebAxisX->Down) m_RotateVector.set(1,0,0);
        else if (fraTopBar->ebAxisY->Down) m_RotateVector.set(0,1,0);
        else if (fraTopBar->ebAxisZ->Down) m_RotateVector.set(0,0,1);
        m_fRotateSnapAngle = 0;
    }break;
    case eaScale:{
		m_ScaleCenter.set(0,0,0);
    }break;
    }
    ApplyParent		();
	return m_bHiddenMode;
}

bool __fastcall CParticleTools::MouseEnd(TShiftState Shift)
{
	return true;
}

void __fastcall CParticleTools::MouseMove(TShiftState Shift)
{
	switch(m_Action){
    case eaSelect: return;
    case eaAdd: break;
    case eaMove:{
    	Fvector amount;
        amount.mul( m_MovingXVector, UI.m_MouseSM * UI.m_DeltaCpH.x );
        amount.mad( amount, m_MovingYVector, -UI.m_MouseSM * UI.m_DeltaCpH.y );

        if( fraTopBar->ebMSnap->Down ){
        	CHECK_SNAP(m_MovingReminder.x,amount.x,UI.movesnap());
        	CHECK_SNAP(m_MovingReminder.y,amount.y,UI.movesnap());
        	CHECK_SNAP(m_MovingReminder.z,amount.z,UI.movesnap());
        }

        if (!fraTopBar->ebAxisX->Down&&!fraTopBar->ebAxisZX->Down) amount.x = 0.f;
        if (!fraTopBar->ebAxisZ->Down&&!fraTopBar->ebAxisZX->Down) amount.z = 0.f;
        if (!fraTopBar->ebAxisY->Down) amount.y = 0.f;

		m_Transform.c.add(amount);
    }break;
    case eaRotate:{
        float amount = -UI.m_DeltaCpH.x * UI.m_MouseSR;
        if( fraTopBar->ebASnap->Down ) CHECK_SNAP(m_fRotateSnapAngle,amount,UI.anglesnap());

        Fmatrix mR;
        mR.rotation		(m_RotateVector,amount);
        m_Transform.mulB(mR);
    }break;
    case eaScale:{
        float dy = UI.m_DeltaCpH.x * UI.m_MouseSS;
        if (dy>1.f) dy=1.f; else if (dy<-1.f) dy=-1.f;

        Fvector amount;
        amount.set( dy, dy, dy );

        if (fraTopBar->ebNonUniformScale->Down){
            if (!fraTopBar->ebAxisX->Down && !fraTopBar->ebAxisZX->Down) amount.x = 0.f;
            if (!fraTopBar->ebAxisZ->Down && !fraTopBar->ebAxisZX->Down) amount.z = 0.f;
            if (!fraTopBar->ebAxisY->Down) amount.y = 0.f;
        }
    }break;
    }
    ApplyParent		();
}       
//------------------------------------------------------------------------------

void CParticleTools::RealApplyParent()
{
    switch(m_EditMode){
    case emEffect:	m_EditPE->UpdateParent(m_Transform,zero_vec,FALSE); break;    
    case emGroup:	m_EditPG->UpdateParent(m_Transform,zero_vec,FALSE); break;
    default: THROW;
    }
	m_Flags.set		(flApplyParent,FALSE);
}

void __fastcall	CParticleTools::OnApplyClick()
{
	if (m_LibPED)   	m_LibPED->Compile	();
	m_EditPE->Compile	(m_LibPED);
    RealApplyParent	 	();
    if (m_EditText&&m_EditText->Modified()) OnItemModified();
}

void __fastcall	CParticleTools::OnCloseClick(bool& can_close)
{
	m_EditText->ApplyEdit();
	m_EditText = 0;
}

bool __fastcall CParticleTools::OnCodeInsight(const AnsiString& src_line, AnsiString& hint)
{
	LPCSTR dest=0;
	if (PS::CPEDef::FindCommandPrototype(src_line.c_str(),dest)){
    	if (dest)	hint = dest;
        else		hint = "Can't insight selected line.";
		return true;
    }else{
		hint = "Unknown command";
		return false;
    }
}

extern void FillStateMenu(TMenuItem* root, TNotifyEvent on_click);
extern void FillActionMenu(TMenuItem* root, TNotifyEvent on_click);
extern const AnsiString GetFunctionTemplate(const AnsiString& command);

void __fastcall	CParticleTools::OnPPMenuItemClick(TObject* sender)
{
	TMenuItem* mi = dynamic_cast<TMenuItem*>(sender);
    if (m_EditText&&mi){ 
		LPCSTR T;
    	switch(mi->Tag){
        case -2: 	if (TfrmChoseItem::SelectItem(TfrmChoseItem::smTexture,T)) m_EditText->InsertTextCP(T); break;
        case -1: 	if (TfrmChoseItem::SelectItem(TfrmChoseItem::smShader,T)) m_EditText->InsertTextCP(T); break;
        case 0: 	m_EditText->InsertLine(GetFunctionTemplate(mi->Caption)); break;
        }
    	
    }
}

void __fastcall CParticleTools::EditActionList()
{
	if (!m_EditText){
        m_EditText 					= TfrmText::CreateForm(m_LibPED->m_SourceText,"Edit action list",TfrmText::flOurPPMenu,0,OnApplyClick,OnCloseClick,OnCodeInsight);

        m_EditText->pmTextMenu->Items->Clear();
        // make popup menu
        TMenuItem* mi 				= xr_new<TMenuItem>((TComponent*)0);
        mi->Caption 				= "-";
        m_EditText->pmTextMenu->Items->Add(mi);

        TMenuItem* miStateCommands	= xr_new<TMenuItem>((TComponent*)0);
        miStateCommands->Caption	= "Insert State Commands";
        m_EditText->pmTextMenu->Items->Add(miStateCommands);

        TMenuItem* miActionCommands	= xr_new<TMenuItem>((TComponent*)0);
        miActionCommands->Caption 	= "Insert Action Commands";
        m_EditText->pmTextMenu->Items->Add(miActionCommands);

        mi 							= xr_new<TMenuItem>((TComponent*)0);
        mi->Caption 				= "-";
        m_EditText->pmTextMenu->Items->Add(mi);
        mi 							= xr_new<TMenuItem>((TComponent*)0);
        mi->Caption 				= "Insert Shader";
        mi->OnClick					= OnPPMenuItemClick;
        mi->Tag						= -1;
        m_EditText->pmTextMenu->Items->Add(mi);
        mi 							= xr_new<TMenuItem>((TComponent*)0);
        mi->Caption 				= "Insert Texture";
        mi->OnClick					= OnPPMenuItemClick;
        mi->Tag						= -2;
        m_EditText->pmTextMenu->Items->Add(mi);

        mi 							= xr_new<TMenuItem>((TComponent*)0);
        mi->Caption 				= "-";
        m_EditText->pmTextMenu->Items->Add(mi);
        mi 							= xr_new<TMenuItem>((TComponent*)0);
        mi->Caption 				= "Load";
        mi->OnClick					= m_EditText->ebLoadClick;
        m_EditText->pmTextMenu->Items->Add(mi);
        mi 							= xr_new<TMenuItem>((TComponent*)0);
        mi->Caption 				= "Save";
        mi->OnClick					= m_EditText->ebSaveClick;
        m_EditText->pmTextMenu->Items->Add(mi);
    
        mi 							= xr_new<TMenuItem>((TComponent*)0);
        mi->Caption 				= "-";
        m_EditText->pmTextMenu->Items->Add(mi);
        mi 							= xr_new<TMenuItem>((TComponent*)0);
        mi->Caption 				= "Clear";
        mi->OnClick					= m_EditText->ebClearClick;
        m_EditText->pmTextMenu->Items->Add(mi);

        // fill commands
        FillStateMenu				(miStateCommands, OnPPMenuItemClick);
        FillActionMenu				(miActionCommands, OnPPMenuItemClick);
    }else{
    	if (m_LibPED) 				m_EditText->SetText(m_LibPED->m_SourceText);
    	m_EditText->SetFocus		();
    }
}

void __fastcall CParticleTools::ResetState()
{
	PAPI::pResetState();
}

void CParticleTools::GetCurrentFog(u32& fog_color, float& s_fog, float& e_fog)
{
    s_fog				= psDeviceFlags.is(rsFog)?(1.0f - fFogness)* 0.85f * UI.ZFar():0.99f*UI.ZFar();
    e_fog				= psDeviceFlags.is(rsFog)?0.91f * UI.ZFar():UI.ZFar();
    fog_color 			= dwFogColor;
}

LPCSTR CParticleTools::GetInfo()
{
	return 0;
}
//------------------------------------------------------------------------------

void CParticleTools::SelectListItem(LPCSTR pref, LPCSTR name, bool bVal, bool bLeaveSel, bool bExpand)
{
	AnsiString nm = (name&&name[0])?FHelper.PrepareKey(pref,name).c_str():pref;
	m_PList->SelectItem(nm,bVal,bLeaveSel,bExpand);
	if (pref){
    	m_PList->SelectItem(pref,true,true,bExpand);
    }
}
//------------------------------------------------------------------------------

PS::CPEDef* CParticleTools::AppendPE(PS::CPEDef* src)
{
	VERIFY(m_bReady);
	AnsiString folder_name;
	FHelper.MakeName	(m_PList->GetSelected(),0,folder_name,true);
    string64 pref		={0};
    if (src){ 			strcpy(pref,src->m_Name);folder_name="";}
    else strconcat		(pref,folder_name.c_str(),"pe");
    AnsiString new_name	= FHelper.GenerateName(pref,2,::Render->PSLibrary.FindByName);
    PS::CPEDef* S 		= ::Render->PSLibrary.AppendPED(src);
    strcpy				(S->m_Name,new_name.c_str());
    S->m_OwnerName		= AnsiString().sprintf("\\\\%s\\%s",Core.CompName,Core.UserName).c_str();
    S->m_ModifName		= S->m_OwnerName;
    S->m_CreateTime		= time(NULL);
    S->m_ModifTime		= S->m_CreateTime;

    UI.Command			(COMMAND_UPDATE_PROPERTIES,true);
    if (!new_name.IsEmpty()) SelectListItem(0,new_name.c_str(),true,false,true);
    return S;
}

PS::CPGDef*	CParticleTools::AppendPG(PS::CPGDef* src)
{
	VERIFY(m_bReady);
	AnsiString folder_name;
	FHelper.MakeName	(m_PList->GetSelected(),0,folder_name,true);
    string64 pref		={0};
    if (src){ 			strcpy(pref,src->m_Name);folder_name="";}
    else strconcat		(pref,folder_name.c_str(),"pg");
    AnsiString new_name	= FHelper.GenerateName(pref,2,::Render->PSLibrary.FindByName);
    PS::CPGDef* S 		= ::Render->PSLibrary.AppendPGD(src);
    strcpy				(S->m_Name,new_name.c_str());
    S->m_OwnerName		= AnsiString().sprintf("\\\\%s\\%s",Core.CompName,Core.UserName).c_str();
    S->m_ModifName		= S->m_OwnerName;
    S->m_CreateTime		= time(NULL);
    S->m_ModifTime		= S->m_CreateTime;

    UI.Command			(COMMAND_UPDATE_PROPERTIES,true);
    if (!new_name.IsEmpty()) SelectListItem(0,new_name.c_str(),true,false,true);
    return S;
}

#include "EditMesh.h"

bool CParticleTools::RayPick(const Fvector& start, const Fvector& dir, float& dist, Fvector* pt, Fvector* n)
{
    if (m_EditObject){
		SRayPickInfo pinf;
		if (m_EditObject->RayPick(dist,start,dir,Fidentity,&pinf)){
        	if (pt) pt->set(pinf.pt); 
            if (n){	
                const Fvector* PT[3];
                pinf.e_mesh->GetFacePT(pinf.inf.id, PT);
            	n->mknormal(*PT[0],*PT[1],*PT[2]);
            }
            return true;
        }else return false;
    }else{
    	Fvector np; np.mad(start,dir,dist);
    	if ((start.y>0)&&(np.y<0.f)){
            if (pt) pt->set(start); 
            if (n)	n->set(0.f,1.f,0.f);
            return true;
        }else return false;
    }
}
 
