//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ChoseForm.h"
#include "UI_ParticleTools.h"
#include "EditObject.h"
#include "ui_main.h"
#include "leftbar.h"
#include "PSLibrary.h"
#include "ParticleSystem.h"
#include "ItemList.h"
#include "xr_trims.h"
#include "library.h"
#include "Render.h"
#include "folderlib.h"
#include "itemdialog.h"

#include "TextForm.h"
#include "d3dutils.h"
#include "StatGraph.h"
//------------------------------------------------------------------------------
CParticleTools*&	PTools=(CParticleTools*)Tools;
//------------------------------------------------------------------------------
#define CHECK_SNAP(R,A,C){ R+=A; if(fabsf(R)>=C){ A=snapto(R,C); R=0; }else{A=0;}}
static Fvector zero_vec={0.f,0.f,0.f};

CParticleTools::CParticleTools()
{
	m_EditMode			= emNone;
    m_ItemProps 		= 0;
	m_EditObject		= 0;
    m_bModified			= false;
    m_bReady			= false;
    m_Transform.identity();
    fFogness			= 0.9f;
    dwFogColor			= 0xffffffff;
    m_Flags.zero		();
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

    Load(0,0);

    SetAction(etaSelect);

	// lock
    EFS.LockFile	(0,fn.c_str());

    m_EditPE 		= (PS::CParticleEffect*)::Render->Models->CreatePE(0);
    m_EditPG		= (PS::CParticleGroup*)::Render->Models->CreatePG(0);
    m_ItemProps 	= TProperties::CreateForm("Item Props",fraLeftBar->paItemProps,alClient,OnItemModified,0,0,TProperties::plFolderStore|TProperties::plFullExpand|TProperties::plItemFolders|TProperties::plIFTop);
    // item list
    m_PList			= TItemList::CreateForm("Items",fraLeftBar->paItemList,alClient,TItemList::ilEditMenu|TItemList::ilDragAllowed);
    m_PList->OnItemsFocused	= OnParticleItemFocused;
	m_PList->OnItemRename	= OnParticleItemRename;
    m_PList->OnItemRemove	= OnParticleItemRemove;
    m_PList->SetImages		(fraLeftBar->ilModeIcons);

//	stat_graph				= xr_new<CStatGraph>();
//	stat_graph->SetRect		(100,0,300,200, 0xFFFF0000, 0xFFFF0000);
//	stat_graph->SetMinMax	(0,1,100);
//	stat_graph->SetStyle	(CStatGraph::stBar);  
//	stat_graph->SetGrid		(20,1,0xFF00a000);

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

//	xr_delete			(stat_graph);
}
//---------------------------------------------------------------------------

bool CParticleTools::IfModified()
{
    if (m_bModified){
        int mr = ELog.DlgMsg(mtConfirmation, "The particles has been modified.\nDo you want to save your changes?");
        switch(mr){
        case mrYes: if (!UI->Command(COMMAND_SAVE)) return false; else m_bModified = FALSE; break;
        case mrNo: m_bModified = FALSE; break;
        case mrCancel: return false;
        }
    }
    return true;
}

void CParticleTools::Modified()
{
	m_bModified = true;
	UI->Command(COMMAND_UPDATE_CAPTION);
}
//---------------------------------------------------------------------------

void CParticleTools::OnItemModified()  
{
	Modified();
    if (m_LibPED){
    	CompileEffect				();
	    m_LibPED->m_ModifName		= AnsiString().sprintf("\\\\%s\\%s",Core.CompName,Core.UserName).c_str();
    	m_LibPED->m_ModifTime		= time(NULL);
    }
    if (m_LibPGD){
	    m_LibPGD->m_ModifName		= AnsiString().sprintf("\\\\%s\\%s",Core.CompName,Core.UserName).c_str();
    	m_LibPGD->m_ModifTime		= time(NULL);
    }
	UI->Command(COMMAND_UPDATE_PROPERTIES);
}


void CParticleTools::Render()
{
	if (!m_bReady) return;

	if (m_EditObject)	m_EditObject->RenderSingle(Fidentity);
	// draw parent axis
    DU.DrawObjectAxis			(m_Transform,0.05f,true);
	// draw domains
    switch(m_EditMode){
    case emNone: break;
    case emEffect:{	
		if (m_EditPE&&m_EditPE->GetDefinition())	
        	m_EditPE->GetDefinition()->Render(m_Transform);
    }break;
    case emGroup:{
    	if (m_EditPG){
         	int cnt 		= m_EditPG->items.size();
            for (int k=0; k<cnt; k++){
                PS::CParticleEffect* E		= (PS::CParticleEffect*)m_EditPG->items[k]._effect;
				if (E&&E->GetDefinition()&&m_LibPGD->m_Effects[k].m_Flags.is(PS::CPGDef::SEffect::flEnabled))	
                	E->GetDefinition()->Render(m_Transform);
            }
        }
    }break;
    default: THROW;
    }
	// Draw the particles.
    ::Render->Models->RenderSingle(m_EditPG,Fidentity,1.f);	
    ::Render->Models->RenderSingle(m_EditPE,Fidentity,1.f);	
}

void CParticleTools::OnFrame()
{
	if (!m_bReady) return;
	if (m_EditObject)
    	m_EditObject->OnFrame();

    if (m_Flags.is(flRemoveAction))
    	RealRemoveAction();
	if (m_Flags.is(flApplyParent))
    	RealApplyParent();
	if (m_Flags.is(flCompileEffect))
    	RealCompileEffect();

    m_EditPE->OnFrame(Device.dwTimeDelta);
    m_EditPG->OnFrame(Device.dwTimeDelta);

	if (m_Flags.is(flRefreshProps))
    	RealUpdateProperties();

    AnsiString tmp;                              
    switch(m_EditMode){                 
    case emNone: break;            
    case emEffect:	                       
    	if (m_EditPE->IsPlaying())
        	UI->SetStatus(AnsiString().sprintf(" PE Playing...[%d]",m_EditPE->ParticlesCount()).c_str(),false); 
        else 
        	UI->SetStatus(" Stopped.",false); 
    break;
    case emGroup:	                 
    	if (m_EditPG->IsPlaying())
        	UI->SetStatus(AnsiString().sprintf(" PE Playing...[%d]",m_EditPG->ParticlesCount()).c_str(),false); 
        else 
        	UI->SetStatus(" Stopped.",false); 
    break;
    default: THROW;
    }

//	stat_graph->AppendItem(Random.randF(),0xFFFFFFFF);
}

void CParticleTools::ZoomObject(bool bSelOnly)
{
	VERIFY(m_bReady);
    if (!bSelOnly&&m_EditObject){
        Device.m_Camera.ZoomExtents(m_EditObject->GetBox());
	}else{
    	Fbox box; box.invalidate();
        switch(m_EditMode){
        case emNone: break;
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
    if (!TfrmChoseItem::SelectItem(smObject,fn,1,m_EditObject?m_EditObject->GetName():0)) return;
    Lib.RemoveEditObject(m_EditObject);
    m_EditObject = fn?Lib.CreateEditObject(fn):0;
//	ZoomObject(TRUE);
    UI->RedrawScene();
}

void CParticleTools::ResetPreviewObject()
{
	VERIFY(m_bReady);
    UI->RedrawScene();
}

bool CParticleTools::Load(LPCSTR path, LPCSTR name)
{
	VERIFY(m_bReady);
    UpdateProperties();
    return true;
}

bool CParticleTools::Save(LPCSTR path, LPCSTR name, bool bInternal)
{
	VERIFY			(m_bReady);
	m_bModified 	= false;
	// backup
    EFS.BackupFile	(_game_data_,PSLIB_FILENAME);
	// save   
    EFS.UnlockFile	(_game_data_,PSLIB_FILENAME,false);
	::Render->PSLibrary.Save();
    EFS.LockFile	(_game_data_,PSLIB_FILENAME,false);
    return true;
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
/*
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
            	PS::CPEDef* x = ::Render->PSLibrary.FindPED(*(*b_it)->m_Name);
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
                        	::Render->PSLibrary.Remove(*x->m_Name);
                            ::Render->PSLibrary.AppendPED(*b_it);
                            Msg(". Replace: '%s'",*(*b_it)->m_Name);
                            replace_p++;
                        }break;
                        case 4: bRenameAll	= true;
                        case 1:{
                            AnsiString pref 	= AnsiString(*x->m_Name)+"_old";
                            AnsiString new_name = FHelper.GenerateName(pref.c_str(),2,::Render->PSLibrary.FindByName);
                            x->m_Name		= new_name.c_str();
                            ::Render->PSLibrary.AppendPED(*b_it);
                            Msg(". Rename: '%s'",new_name.c_str());
                            rename_p++;
                        }break;
                        case 5: bSkipAll	= true;
                        case 2: 
                            Msg(". Skip: '%s'",*(*b_it)->m_Name);
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
            	PS::CPGDef* x = ::Render->PSLibrary.FindPGD(*(*b_it)->m_Name);
                if (x){
                	if (!x->Equal(*b_it)){
                        int res;
                        if (bReplaceAll)	res = 0;
                        else if (bRenameAll)res = 1;
                        else if (bSkipAll)	res = 2;
                        else				res = TfrmItemDialog::Run("Item", AnsiString().sprintf("Overwrite particle group: '%s'",*x->m_Name).c_str(), "Replace,Rename,Skip,Replace All,Rename All,Skip All,Cancel");
                        switch(res){
                        case 3: bReplaceAll	= true; 
                        case 0:{
                        	::Render->PSLibrary.Remove(*x->m_Name);
                            ::Render->PSLibrary.AppendPGD(*b_it);
                            Msg(". Replace: '%s'",*(*b_it)->m_Name);
                            replace_p++;
                        }break;
                        case 4: bRenameAll	= true;
                        case 1:{
                            AnsiString pref 	= AnsiString(*x->m_Name)+"_old";
                            AnsiString new_name = FHelper.GenerateName(pref.c_str(),2,::Render->PSLibrary.FindByName);
                            x->m_Name		= new_name.c_str();
                            ::Render->PSLibrary.AppendPGD(*b_it);
                            Msg(". Rename: '%s'",new_name.c_str());
                            rename_p++;
                        }break;
                        case 5: bSkipAll	= true;
                        case 2: 
                            Msg(". Skip: '%s'",*(*b_it)->m_Name);
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
		    UI->Command	(COMMAND_UPDATE_PROPERTIES);
            Modified	();
        }
	    ps_new.OnDestroy();
    }
*/    
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
	::Render->PSLibrary.Remove	(name);
	SetCurrentPE				(0);
	SetCurrentPG				(0);
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
    m_EditPG->Compile		(0);
	if (m_LibPED!=P){
	    m_LibPED = P;
        m_EditPE->Compile	(m_LibPED);
		if (m_LibPED)
			m_EditMode		= emEffect;
    }
}

void CParticleTools::SetCurrentPG(PS::CPGDef* P)
{
	VERIFY(m_bReady);
	m_EditPE->Compile		(0);
	if (m_LibPGD!=P){
	    m_LibPGD = P;
        m_EditPG->Compile	(m_LibPGD);
        if (m_LibPGD)
			m_EditMode		= emGroup;
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

void CParticleTools::PlayCurrent(int idx)
{
	VERIFY(m_bReady);
    StopCurrent		(false);
    switch(m_EditMode){
    case emNone: break;
    case emEffect:	m_EditPE->Play(); 		break;
    case emGroup:	
    	if (idx>-1){
        	VERIFY(idx<m_EditPG->items.size());
            m_LibPED = ((PS::CParticleEffect*)m_EditPG->items[idx]._effect)->GetDefinition();
			m_EditPE->Compile(m_LibPED);
        	m_EditPE->Play	();
        }else{
        	// play all
	    	m_EditPG->Play(); 	
        }
    break;
    default: THROW;
    }
    ApplyParent		();
}

void CParticleTools::StopCurrent(bool bFinishPlaying)
{
	VERIFY(m_bReady);
    m_EditPE->Stop(bFinishPlaying);
    m_EditPG->Stop(bFinishPlaying);
}

void CParticleTools::SelectEffect(LPCSTR name)
{
	m_PList->SelectItem(name,true,false,true);
}

void CParticleTools::OnShowHint(AStringVec& SS)
{
}

bool __fastcall CParticleTools::MouseStart(TShiftState Shift)
{
	inherited::MouseStart(Shift);
	switch(m_Action){
    case etaSelect: break;
    case etaAdd:	break;
    case etaMove:{
        if (Shift.Contains(ssCtrl)){
        	if (m_EditObject){
                float dist = flt_max;
                SRayPickInfo pinf;
                if (m_EditObject->RayPick(dist,UI->m_CurrentRStart,UI->m_CurrentRNorm,Fidentity,&pinf))
                    m_Transform.c.set(pinf.pt);
            }else{
                // pick grid
                Fvector normal={0.f, 1.f, 0.f};
                float clcheck = UI->m_CurrentRNorm.dotproduct( normal );
                if( fis_zero( clcheck ) ) return false;
                float alpha = - UI->m_CurrentRStart.dotproduct(normal) / clcheck;
                if( alpha <= 0 ) return false;

                m_Transform.c.mad(UI->m_CurrentRStart,UI->m_CurrentRNorm,alpha);

                if (m_Settings.is(etfGSnap)){
                    m_Transform.c.x = snapto( m_Transform.c.x, m_MoveSnap );
                    m_Transform.c.z = snapto( m_Transform.c.z, m_MoveSnap );
                    m_Transform.c.y = 0.f;
                }
            }
        }
    }break;
    case etaRotate:	break;
    case etaScale:  break;
    }
    ApplyParent		();
	return m_bHiddenMode;
}

bool __fastcall CParticleTools::MouseEnd(TShiftState Shift)
{
	inherited::MouseEnd(Shift);
	return true;
}

void __fastcall CParticleTools::MouseMove(TShiftState Shift)
{
	inherited::MouseMove(Shift);
	switch(m_Action){
    case etaSelect: break;
    case etaAdd: 	break;
    case etaMove:	m_Transform.c.add(m_MoveAmount); break;
    case etaRotate:{
        Fmatrix mR;
        mR.rotation		(m_RotateVector,m_RotateAmount);
        m_Transform.mulB(mR);
    }break;
    case etaScale:	break;
    }
    ApplyParent		();
}       
//------------------------------------------------------------------------------

void CParticleTools::RealApplyParent()
{
    switch(m_EditMode){
    case emNone: break;
    case emEffect:	m_EditPE->UpdateParent(m_Transform,zero_vec,FALSE); break;    
    case emGroup:	m_EditPG->UpdateParent(m_Transform,zero_vec,FALSE); break;
    default: THROW;
    }
	m_Flags.set		(flApplyParent,FALSE);
}

void CParticleTools::RealCompileEffect()
{
    m_LibPED->Compile();
	m_Flags.set		(flCompileEffect,FALSE);
}

void CParticleTools::RealRemoveAction()
{
	xr_delete		(m_LibPED->m_EActionList[remove_action_num]);
	m_LibPED->m_EActionList.erase	(m_LibPED->m_EActionList.begin()+remove_action_num);
	m_Flags.set		(flRemoveAction,FALSE);
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
    if (src){ 			strcpy(pref,*src->m_Name);folder_name="";}
    else strconcat		(pref,folder_name.c_str(),"pe");
    AnsiString new_name	= FHelper.GenerateName(pref,2,::Render->PSLibrary.FindByName,false);
    PS::CPEDef* S 		= ::Render->PSLibrary.AppendPED(src);
    S->m_Name			= new_name.c_str();
    S->m_OwnerName		= AnsiString().sprintf("\\\\%s\\%s",Core.CompName,Core.UserName).c_str();
    S->m_ModifName		= S->m_OwnerName;
    S->m_CreateTime		= time(NULL);
    S->m_ModifTime		= S->m_CreateTime;

    UI->Command			(COMMAND_UPDATE_PROPERTIES,true);
    if (!new_name.IsEmpty()) SelectListItem(0,new_name.c_str(),true,false,true);
    return S;
}

PS::CPGDef*	CParticleTools::AppendPG(PS::CPGDef* src)
{
	VERIFY(m_bReady);
	AnsiString folder_name;
	FHelper.MakeName	(m_PList->GetSelected(),0,folder_name,true);
    string64 pref		={0};
    if (src){ 			strcpy(pref,*src->m_Name);folder_name="";}
    else strconcat		(pref,folder_name.c_str(),"pg");
    AnsiString new_name	= FHelper.GenerateName(pref,2,::Render->PSLibrary.FindByName,false);
    PS::CPGDef* S 		= ::Render->PSLibrary.AppendPGD(src);
    S->m_Name			= new_name.c_str();
    S->m_OwnerName		= AnsiString().sprintf("\\\\%s\\%s",Core.CompName,Core.UserName).c_str();
    S->m_ModifName		= S->m_OwnerName;
    S->m_CreateTime		= time(NULL);
    S->m_ModifTime		= S->m_CreateTime;

    UI->Command			(COMMAND_UPDATE_PROPERTIES,true);
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
 
