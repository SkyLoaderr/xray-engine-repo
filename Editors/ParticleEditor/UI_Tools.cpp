//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "UI_Tools.h"
#include "EditObject.h"
#include "ChoseForm.h"
#include "ui_main.h"
#include "leftbar.h"
#include "topbar.h"
#include "PSObject.h"
#include "PSLibrary.h"
#include "ParticleSystem.h"
#include "PropertiesPSDef.h"
#include "ItemList.h"
#include "xr_trims.h"
#include "library.h"

#include "TextForm.h"
#include "d3dutils.h"
//------------------------------------------------------------------------------
CParticleTools Tools;
//------------------------------------------------------------------------------
#define CHECK_SNAP(R,A,C){ R+=A; if(fabsf(R)>=C){ A=snapto(R,C); R=0; }else{A=0;}}

CParticleTools::CParticleTools()
{
    m_ItemProps 		= 0;
	m_EditObject		= 0;
	m_TestObject 		= 0;
    m_LibPS				= 0;
    m_PSProps			= 0;
    m_EditText			= 0;
    m_bModified			= false;
    m_bReady			= false;
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
    FS.update_path(fn,_game_data_,"particles2.xr");
	if (EFS.CheckLocking(0,fn.c_str(),false,true)){
    	ELog.DlgMsg(mtInformation,"Particle Editor locked.");
    	return false;
    }

    Device.seqDevCreate.Add(this);
    Device.seqDevDestroy.Add(this);
    m_PSProps = xr_new<TfrmPropertiesPSDef>(fraLeftBar->paItemProps);
	m_PSProps->Parent = fraLeftBar->paItemProps;
    m_PSProps->Align = alClient;
    m_PSProps->BorderStyle = bsNone;
    m_PSProps->ShowProperties();

    m_bReady = true;

    Load();

	m_PSProps->SetCurrent(0);
    ChangeAction(eaSelect);

	// lock
    EFS.LockFile(0,fn.c_str());

    m_EditPE 		= xr_new<PS::CParticleEffect>();
    m_EditPG		= xr_new<PS::CParticleGroup>();
    m_ItemProps 	= TProperties::CreateForm(fraLeftBar->paItemProps,alClient,OnItemModified);
	m_ItemProps->HideProperties();
    m_PList			= TItemList::CreateForm(fraLeftBar->paItemList,alClient,OnParticleItemFocused,0,false);
    m_PList->SetImages(fraLeftBar->ilModeIcons);

    return true;
}

void CParticleTools::OnDestroy()
{
	VERIFY(m_bReady);
    m_bReady			= false;
	// unlock
    EFS.UnlockFile		(_game_data_,"particles2.xr");

	Lib.RemoveEditObject(m_EditObject);
    TItemList::DestroyForm(m_PList);
	TProperties::DestroyForm(m_ItemProps);
    xr_delete			(m_EditPG);
    xr_delete			(m_EditPE);
	xr_delete			(m_TestObject);
    m_LibPS				= 0;
    m_PSProps->HideProperties();
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
}


void CParticleTools::Render()
{
	if (!m_bReady) return;

	if (m_EditObject)	m_EditObject->RenderSingle(Fidentity);
	// Draw the particles.
	RCache.set_xform_world		(Fidentity);
    if (m_LibPED) 		Device.Models.RenderSingle(m_EditPE,Fidentity,1.f);
    else if (m_EditPG)	Device.Models.RenderSingle(m_EditPG,Fidentity,1.f);
    else if (m_LibPS){
        if (m_TestObject){	
        	m_TestObject->RenderSingle	();
        }
    }
}

void CParticleTools::OnFrame()
{
	if (!m_bReady) return;
	if (m_TestObject){
    	m_TestObject->OnFrame();
        if (m_TestObject->IsPlaying())	fraLeftBar->lbCurState->Caption = "generate&&playing";
        else 							fraLeftBar->lbCurState->Caption = m_TestObject->ParticleCount()?"playing":"stopped";
        fraLeftBar->lbParticleCount->Caption 	= m_TestObject->ParticleCount();
    }
	if (m_EditObject)
    	m_EditObject->OnFrame();
        
    m_EditPE->OnFrame(Device.dwTimeDelta);
    m_EditPG->OnFrame(Device.dwTimeDelta);

	if (m_Flags.is(flRefreshProps))
    	RealUpdateProperties();
}

void CParticleTools::ZoomObject(BOOL bObjectOnly){
	VERIFY(m_bReady);
    if (bObjectOnly&&m_EditObject){
        Device.m_Camera.ZoomExtents(m_EditObject->GetBox());
	}else{
    	if (m_TestObject){
			Fbox BB;
	        m_TestObject->GetBox(BB);
    	    Device.m_Camera.ZoomExtents(BB);
        }else{
			Fbox BB;
    	    BB.set(-5,-5,-5,5,5,5);
        	Device.m_Camera.ZoomExtents(BB);
        }
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

	if (m_TestObject) m_TestObject->OnDeviceCreate();
    m_EditPE->OnDeviceCreate();
}

void CParticleTools::OnDeviceDestroy()
{
    m_EditPE->OnDeviceDestroy();
	if (m_TestObject) m_TestObject->OnDeviceDestroy();
}

void CParticleTools::SelectPreviewObject(int p){
    LPCSTR fn;
    if (!TfrmChoseItem::SelectItem(TfrmChoseItem::smObject,fn,1,m_EditObject?m_EditObject->GetName():0)) return;
    Lib.RemoveEditObject(m_EditObject);
    m_EditObject = fn?Lib.CreateEditObject(fn):0;
	ZoomObject(TRUE);
    UI.RedrawScene();
}

void CParticleTools::ResetPreviewObject(){
	VERIFY(m_bReady);
    m_LibPS 	= 0;
    UI.RedrawScene();
}

void CParticleTools::Load()
{
	VERIFY(m_bReady);
    UpdateProperties();
}

void CParticleTools::Save()
{
	VERIFY(m_bReady);
    ApplyChanges();
	m_bModified = false;
	// backup
    EFS.BackupFile	(_game_data_,"particles2.xr");
	// save   
    EFS.UnlockFile	(_game_data_,"particles2.xr",false);
	::Render->PSystems.Save();
    EFS.LockFile	(_game_data_,"particles2.xr",false);
}

void CParticleTools::Reload()
{
	VERIFY(m_bReady);
	::Render->PSystems.Reload();
    // visual part
    fraLeftBar->ClearParticleList();
    m_ItemProps->ClearProperties();
    Load();
    ResetCurrent();
}


void CParticleTools::Rename(LPCSTR old_full_name, LPCSTR ren_part, int level)
{
    VERIFY(level<_GetItemCount(old_full_name,'\\'));
    char new_full_name[255];
    _ReplaceItem(old_full_name,level,ren_part,new_full_name,'\\');
    Rename(old_full_name, new_full_name);
}

void CParticleTools::Rename(LPCSTR old_full_name, LPCSTR new_full_name)
{
	VERIFY(m_bReady);
    if (m_LibPED){
        ::Render->PSystems.RenamePED(m_LibPED,new_full_name);
    }else if (m_LibPGD){
        ::Render->PSystems.RenamePGD(m_LibPGD,new_full_name);
    }else if (m_LibPS){
        ApplyChanges();
        PS::SDef* S = ::Render->PSystems.FindPS(old_full_name); R_ASSERT(S);
        ::Render->PSystems.RenamePS(S,new_full_name);
        if (S==m_LibPS){
            m_EditPS = *S;
        }
    }
}

void CParticleTools::RemovePS(LPCSTR name)
{
	VERIFY(m_bReady);
	::Render->PSystems.Remove(name);
}

void CParticleTools::ResetCurrent()
{
	VERIFY(m_bReady);
	m_LibPS = 0;
    m_LibPED= 0;
    m_LibPGD= 0;
	xr_delete(m_TestObject);
}

void CParticleTools::SetCurrentPS(PS::SDef* P)
{
	VERIFY(m_bReady);
    // save changes
    if (m_LibPS)
    	*m_LibPS = m_EditPS;
    // load shader
	if (m_LibPS!=P){
        m_LibPS = P;
        if (m_LibPS){ 
        	m_EditPS 		= *m_LibPS;
			m_EditMode		= emSystem;
        }
        m_PSProps->SetCurrent(m_LibPS?&m_EditPS:0);
        // update visual
        xr_delete(m_TestObject);
		m_TestObject = xr_new<CPSObject>((LPVOID)0,"Test");
		m_TestObject->Compile(&m_EditPS);
    }
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
        	EditActionList	();
        }else if (m_EditText) TfrmText::DestroyForm(m_EditText);
    }
}

void CParticleTools::SetCurrentPG(PS::CPGDef* P)
{
	VERIFY(m_bReady);
	if (m_LibPGD!=P){
	    m_LibPGD = P;
        if (m_LibPGD){
			m_EditMode		= emGroup;
	        m_EditPG->Compile(m_LibPGD);
        }
    }
}

PS::SDef* CParticleTools::FindPS(LPCSTR name)
{
	return ::Render->PSystems.FindPS(name);
}

PS::CPEDef*	CParticleTools::FindPE(LPCSTR name)
{
	return ::Render->PSystems.FindPED(name);
}

PS::CPGDef*	CParticleTools::FindPG(LPCSTR name)
{
	return ::Render->PSystems.FindPGD(name);
}

void CParticleTools::UpdateCurrent()
{
	VERIFY(m_bReady);
    VERIFY(m_LibPS);
	xr_delete(m_TestObject);
    m_TestObject = xr_new<CPSObject>((LPVOID)0,"Test");
    m_TestObject->Compile(&m_EditPS);
}
//---------------------------------------------------------------------------

void CParticleTools::UpdateEmitter()
{
	VERIFY(m_bReady);
    if (!m_LibPS) return;
    if (m_TestObject) m_TestObject->UpdateEmitter(&m_EditPS.m_DefaultEmitter);
}
//---------------------------------------------------------------------------

PS::SDef* CParticleTools::ClonePS(LPCSTR name)
{
	VERIFY(m_bReady);
	PS::SDef* S = ::Render->PSystems.FindPS(name); R_ASSERT(S);
	return AppendPS(S);
}

void CParticleTools::ApplyChanges()
{
	VERIFY(m_bReady);
    if (m_LibPS){
		*m_LibPS = m_EditPS;
		Modified();
    }
}

void CParticleTools::PlayCurrent()
{
	VERIFY(m_bReady);
    if (m_LibPED){
    	m_EditPE->Play();
    }else if (m_LibPS){
		if (m_TestObject) m_TestObject->Play();
    }
}

void CParticleTools::StopCurrent()
{
	VERIFY(m_bReady);
    if (m_LibPED){
    	m_EditPE->Stop();
    }else if (m_LibPS){
		if (m_TestObject) m_TestObject->Stop();
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
	if (!m_TestObject) return false;
	switch(m_Action){
    case eaSelect: return false;
    case eaAdd:{
        if (m_EditObject){
            Fvector p;
            float dist=UI.ZFar();
            SRayPickInfo pinf;
            if (m_EditObject->RayPick(dist,UI.m_CurrentRStart,UI.m_CurrentRNorm,Fidentity,&pinf)){
                R_ASSERT(pinf.e_mesh);
                m_EditPS.m_DefaultEmitter.m_Position.set(pinf.pt);
            }
        }
    }break;
    case eaMove:{
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
    }break;
    case eaRotate:{
        m_RotateCenter.set(0,0,0);
        m_RotateVector.set(0,0,0);
        if (fraTopBar->ebAxisX->Down) m_RotateVector.set(0,1,0);
        else if (fraTopBar->ebAxisY->Down) m_RotateVector.set(1,0,0);
        else if (fraTopBar->ebAxisZ->Down) m_RotateVector.set(0,0,1);
        m_fRotateSnapAngle = 0;
    }break;
    case eaScale:{
		m_ScaleCenter.set(0,0,0);
    }break;
    }
    UpdateEmitter();
    m_PSProps->fraEmitter->GetInfoFirst(m_EditPS.m_DefaultEmitter);
	return m_bHiddenMode;
}

bool __fastcall CParticleTools::MouseEnd(TShiftState Shift)
{
	return true;
}

void __fastcall CParticleTools::MouseMove(TShiftState Shift)
{
	if (!m_TestObject) return;
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
		m_EditPS.m_DefaultEmitter.m_Position.add(amount);
    }break;
    case eaRotate:{
        float amount = -UI.m_DeltaCpH.x * UI.m_MouseSR;
        if( fraTopBar->ebASnap->Down ) CHECK_SNAP(m_fRotateSnapAngle,amount,UI.anglesnap());
        switch (m_EditPS.m_DefaultEmitter.m_EmitterType){
        case PS::SEmitterDef::emPoint:
        case PS::SEmitterDef::emBox:
        case PS::SEmitterDef::emSphere: 	break;
        case PS::SEmitterDef::emCone:
        	m_EditPS.m_DefaultEmitter.m_ConeHPB.mad(m_TestObject->m_Emitter.m_ConeHPB,m_RotateVector,amount);
            m_EditPS.m_DefaultEmitter.UpdateConeOrientation();
        break;
        }
		//m_RotateVector, amount
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

        switch (m_EditPS.m_DefaultEmitter.m_EmitterType){
        case PS::SEmitterDef::emCone:
        case PS::SEmitterDef::emPoint:		break;
        case PS::SEmitterDef::emBox:
	        m_EditPS.m_DefaultEmitter.m_BoxSize.add(amount);
        break;
        case PS::SEmitterDef::emSphere:
	        m_EditPS.m_DefaultEmitter.m_SphereRadius += dy;
        break;
        }
    }break;
    }
    UpdateEmitter();
    m_PSProps->fraEmitter->GetInfoFirst(m_EditPS.m_DefaultEmitter);
}       

//------------------------------------------------------------------------------

void __fastcall	CParticleTools::OnApplyClick()
{
	if (m_LibPED) m_LibPED->Compile();
	m_EditPE->Compile(m_LibPED);
    if (m_EditText&&m_EditText->Modified()) Modified();
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
    	m_EditText->SetFocus		();
    }
}

void __fastcall CParticleTools::ResetState()
{
	PAPI::pResetState();
}

void CParticleTools::GetCurrentFog(u32& fog_color, float& s_fog, float& e_fog)
{
	s_fog		= UI.ZFar();
	e_fog		= UI.ZFar();
	fog_color	= DEFAULT_CLEARCOLOR;
}

LPCSTR CParticleTools::GetInfo()
{
	return 0;
}
//------------------------------------------------------------------------------

void CParticleTools::SelectListItem(LPCSTR pref, LPCSTR name, bool bVal, bool bLeaveSel, bool bExpand)
{
	AnsiString nm = (name&&name[0])?FHelper.PrepareKey(pref,name):pref;
	m_PList->SelectItem(nm,bVal,bLeaveSel,bExpand);
	if (pref){
    	m_PList->SelectItem(pref,true,true,bExpand);
    }
}
//------------------------------------------------------------------------------

PS::SDef* CParticleTools::AppendPS(PS::SDef* src)
{
	VERIFY(m_bReady);
	AnsiString folder_name;
	FHelper.MakeName	(m_PList->GetSelected(),0,folder_name,true);
    string64 new_name;
    string64 pref		={0};
    if (src) 			strcat(pref,src->m_Name);
    ::Render->PSystems.GenerateName	(new_name,folder_name.c_str(),pref);
    PS::SDef* S 		= ::Render->PSystems.AppendPS(src);
    strcpy				(S->m_Name,new_name);

    UI.Command			(COMMAND_UPDATE_PROPERTIES,true);
    if (new_name[0]) 	SelectListItem(0,new_name,true,false,true);
    return S;
}

PS::CPEDef* CParticleTools::AppendPE(PS::CPEDef* src)
{
	VERIFY(m_bReady);
	AnsiString folder_name;
	FHelper.MakeName	(m_PList->GetSelected(),0,folder_name,true);
    string64 new_name;
    string64 pref		={0};
    if (src) 			strcat(pref,src->m_Name);
    ::Render->PSystems.GenerateName	(new_name,folder_name.c_str(),pref);
    PS::CPEDef* S 		= ::Render->PSystems.AppendPED(src);
    strcpy				(S->m_Name,new_name);

    UI.Command			(COMMAND_UPDATE_PROPERTIES,true);
    if (new_name[0]) 	SelectListItem(0,new_name,true,false,true);
    return S;
}

PS::CPGDef*	CParticleTools::AppendPG(PS::CPGDef* src)
{
	VERIFY(m_bReady);
	AnsiString folder_name;
	FHelper.MakeName	(m_PList->GetSelected(),0,folder_name,true);
    string64 new_name;
    string64 pref		={0};
    if (src) 			strcat(pref,src->m_Name);
    ::Render->PSystems.GenerateName	(new_name,folder_name.c_str(),pref);
    PS::CPGDef* S 		= ::Render->PSystems.AppendPGD(src);
    strcpy				(S->m_Name,new_name);

    UI.Command			(COMMAND_UPDATE_PROPERTIES,true);
    if (new_name[0]) 	SelectListItem(0,new_name,true,false,true);
    return S;
}

void CParticleTools::RealUpdateProperties()
{
	m_Flags.set(flRefreshProps,FALSE); 

	ListItemsVec items;
    {
        PS::PSIt Ps = ::Render->PSystems.FirstPS();
        PS::PSIt Es = ::Render->PSystems.LastPS();
        for (; Ps!=Es; Ps++){
            ListItem* I=LHelper.CreateItem(items,Ps->m_Name,emSystem,0,Ps);
            I->SetIcon(0);
        }
	}
    {
        PS::PEDIt Pe = ::Render->PSystems.FirstPED();
        PS::PEDIt Ee = ::Render->PSystems.LastPED();
        for (; Pe!=Ee; Pe++){
            ListItem* I=LHelper.CreateItem(items,(*Pe)->m_Name,emEffect,0,*Pe);
            I->SetIcon(1);
        }
	}
    {
        PS::PGDIt Pg = ::Render->PSystems.FirstPGD();
        PS::PGDIt Eg = ::Render->PSystems.LastPGD();
        for (; Pg!=Eg; Pg++){
            ListItem* I=LHelper.CreateItem(items,(*Pg)->m_Name,emGroup,0,*Pg);
            I->SetIcon(2);
        }
	}
	m_PList->AssignItems(items,false,"",true);
}

/*
	PropItemVec props;
	m_EditMode	= emObject;
    
    // unselect
    if (m_pEditObject){
    	m_pEditObject->ResetSAnimation(false);
	    StopMotion					();
    	m_pEditObject->SelectBones	(false);
    }
    
	if (!items.empty()){
	    for (ListItemsIt it=items.begin(); it!=items.end(); it++){
            ListItem* prop = *it;
            m_ObjectItems->pmItems->Items->Clear();
            if (prop){
                m_EditMode = EEditMode(prop->Type());
                switch(m_EditMode){
                case emObject:
                    FillObjectProperties(props,MOTIONS_PREFIX,prop);
                break;
                case emMotion:{ 	
                    FillMotionProperties(props,MOTIONS_PREFIX,prop);
                    CSMotion* MOT		= ((CSMotion*)prop->m_Object);
                    SetCurrentMotion	(MOT?MOT->Name():"");
                }break;
                case emBone:{
                    FillBoneProperties	(props,BONES_PREFIX,prop);
                    CBone* BONE 		= (CBone*)prop->m_Object;
                    if (BONE) 			BONE->Select(TRUE);
                }break;
                case emSurface: 
                    FillSurfaceProperties(props,SURFACES_PREFIX,prop);
                break;
                case emMesh: 
                break;
                }
            }
        }
    }
	m_ItemProps->AssignItems(props,true);
*/
