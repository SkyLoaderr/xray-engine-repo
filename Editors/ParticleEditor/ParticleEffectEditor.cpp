//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#ifdef _PARTICLE_EDITOR

#include "ChoseForm.h"
#include "ParticleEffect.h"
#include "ParticleEffectActions.h"
#include "PropertiesListHelper.h"
#include "ui_particletools.h"
#include "ui_main.h"
#include "FolderLib.h"
             
BOOL PS::CPEDef::Equal(const CPEDef* pe)
{
    if (!m_Flags.equal(pe->m_Flags)) 						return FALSE;
    if (!m_ShaderName.equal(pe->m_ShaderName)) 				return FALSE;
    if (!m_TextureName.equal(pe->m_TextureName))			return FALSE;
    if (0!=memcmp(&m_Frame,&pe->m_Frame,sizeof(m_Frame))) 	return FALSE;
	if (!fsimilar(m_fTimeLimit,pe->m_fTimeLimit))			return FALSE;
    if (m_MaxParticles!=pe->m_MaxParticles)					return FALSE;
    if (m_Actions.size()!=pe->m_Actions.size())				return FALSE;
    if (!m_VelocityScale.similar(pe->m_VelocityScale))		return FALSE;
	if (!fsimilar(m_fCollideOneMinusFriction,pe->m_fCollideOneMinusFriction))	return FALSE;
    if (!fsimilar(m_fCollideResilience,pe->m_fCollideResilience))				return FALSE;
    if (!fsimilar(m_fCollideSqrCutoff,pe->m_fCollideSqrCutoff))					return FALSE;
    return TRUE;
}

void PS::CPEDef::Copy(const CPEDef& src)
{
    m_Name				= src.m_Name; 
    m_Flags				= src.m_Flags;
    m_ShaderName		= src.m_ShaderName;
    m_TextureName		= src.m_TextureName;
    m_Frame				= src.m_Frame;
	m_fTimeLimit		= src.m_fTimeLimit;
    m_MaxParticles		= src.m_MaxParticles;
	m_CachedShader		= src.m_CachedShader;
    m_VelocityScale.set			(src.m_VelocityScale);
	m_fCollideOneMinusFriction	= src.m_fCollideOneMinusFriction;
    m_fCollideResilience		= src.m_fCollideResilience;
    m_fCollideSqrCutoff			= src.m_fCollideSqrCutoff; 

    m_EActionList.resize(src.m_EActionList.size());
    for (int k=0; k<src.m_EActionList.size(); k++){
        PAPI::PDomainEnum type 	= src.m_EActionList[k]->type;
        m_EActionList[k]		= pCreateEAction(type);
        *m_EActionList[k]		= *src.m_EActionList[k];
    }
	Compile				();
}

void __fastcall PS::CPEDef::OnControlClick(PropValue* sender, bool& bDataModified, bool& bSafe)
{
	ButtonValue* B 		= dynamic_cast<ButtonValue*>(sender); R_ASSERT(B);
    switch (B->btn_num){
    case 0: 			PTools->PlayCurrent();		break;
    case 1: 			PTools->StopCurrent(false);	break;
    case 2: 			PTools->StopCurrent(true);	break;
    }
    bDataModified		= false;
}

bool __fastcall PS::CPEDef::FindActionByName(LPCSTR new_name)
{
	for (EPAVecIt s_it=m_EActionList.begin(); s_it!=m_EActionList.end(); s_it++)
    	if (0==strcmp(new_name,*(*s_it)->actionName))return true;
    return false;
}

IC __fastcall void PS::CPEDef::FillActionList(ChooseItemVec& items)
{
    for(int i=0; actions_token[i].name; i++)
        items.push_back(SChooseItem(actions_token[i].name,actions_token[i].info));
}

void __fastcall PS::CPEDef::OnActionsClick(PropValue* sender, bool& bDataModified, bool& bSafe)
{
	ButtonValue* B 		= dynamic_cast<ButtonValue*>(sender); R_ASSERT(B);
    switch (B->btn_num){
    case 0:{
    	LPCSTR 		nm;
	    if (TfrmChoseItem::SelectItem(smCustom,nm,1,0,FillActionList)&&nm){
            for(int i=0; actions_token[i].name; i++){
                if (0==strcmp(actions_token[i].name,nm)){
                    EParticleAction* A = pCreateEAction(actions_token[i].id);
                    A->actionName	= FHelper.GenerateName(*A->actionName,2,FindActionByName).LowerCase().c_str();
                    m_EActionList.push_back(A);
                    UI->Command		(COMMAND_UPDATE_PROPERTIES);
                    bDataModified	= true;
                    bSafe			= true;
                	return ;
                }
			}        	
        }
    }break;
    }
    bDataModified	= false;
}

void __fastcall PS::CPEDef::OnFlagChange(PropValue* sender)
{
    UI->Command			(COMMAND_UPDATE_PROPERTIES);
}          

void __fastcall PS::CPEDef::OnShaderChange(PropValue* sender)
{
	m_CachedShader.destroy	();
	if (*m_ShaderName&&*m_TextureName)
		m_CachedShader.create(*m_ShaderName,*m_TextureName);
}          

void __fastcall PS::CPEDef::OnFrameResize(PropValue* sender)
{
	m_Frame.m_iFrameDimX	= iFloor(1.f/m_Frame.m_fTexSize.x);
}

void __fastcall PS::CPEDef::CollisionFrictionOnBeforeEdit(PropItem* sender, LPVOID edit_val)
{    *(float*)edit_val = 1.f-(*(float*)edit_val);}
void __fastcall PS::CPEDef::CollisionFrictionOnAfterEdit(PropItem* sender, LPVOID edit_val)
{    *(float*)edit_val = 1.f-(*(float*)edit_val);}
void __fastcall PS::CPEDef::CollisionFrictionOnDraw(PropValue* sender, LPVOID draw_val)
{    *(float*)draw_val = 1.f-(*(float*)draw_val);}
void __fastcall PS::CPEDef::CollisionCutoffOnBeforeEdit(PropItem* sender, LPVOID edit_val)
{    *(float*)edit_val = _sqrt(*(float*)edit_val);}
void __fastcall PS::CPEDef::CollisionCutoffOnAfterEdit(PropItem* sender, LPVOID edit_val)
{    *(float*)edit_val = (*(float*)edit_val)*(*(float*)edit_val);}
void __fastcall PS::CPEDef::CollisionCutoffOnDraw(PropValue* sender, LPVOID draw_val)
{    *(float*)draw_val = _sqrt(*(float*)draw_val);}

void __fastcall PS::CPEDef::OnActionEditClick(PropValue* sender, bool& bDataModified, bool& bSafe)
{
    bDataModified	= false;
	ButtonValue* B 	= dynamic_cast<ButtonValue*>(sender); R_ASSERT(B);
    int idx			= B->Owner()->tag;
    switch (B->btn_num){
    case 0:		    // up
    	if (idx>0){
        	EParticleAction* E	= m_EActionList[idx-1];
            m_EActionList[idx-1]= m_EActionList[idx];
            m_EActionList[idx]	= E;
            UI->Command			(COMMAND_UPDATE_PROPERTIES);
	        bDataModified		= true;
        }
    break;
    case 1:		    // down
    	if (idx<(m_EActionList.size()-1)){
        	EParticleAction* E	= m_EActionList[idx+1];
            m_EActionList[idx+1]= m_EActionList[idx];
            m_EActionList[idx]	= E;
            UI->Command			(COMMAND_UPDATE_PROPERTIES);
	        bDataModified		= true;
        }
        bDataModified	= true;
    break;
    case 2:        
        if (ELog.DlgMsg(mtConfirmation, TMsgDlgButtons() << mbYes << mbNo,"Remove action?") == mrYes){
            PTools->RemoveAction(idx);
            UI->Command		(COMMAND_UPDATE_PROPERTIES);
            bDataModified	= true;
        }
    break;
    }
}

void PS::CPEDef::OnAfterActionNameEdit(PropItem* sender, LPVOID edit_val)
{
	AnsiString& new_name	= *(AnsiString*)edit_val; new_name=new_name.LowerCase();
    AnsiString old_name		= *((RTextValue*)sender->GetFrontValue())->GetValue();
    if (FindActionByName(new_name.c_str())) new_name = old_name;
}
void PS::CPEDef::FillProp(LPCSTR pref, ::PropItemVec& items, ::ListItem* owner)
{
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Owner Name"),	*m_OwnerName);
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Modif Name"),	*m_ModifName);
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Creation Time"),Trim(AnsiString(ctime(&m_CreateTime))));
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Modified Time"),Trim(AnsiString(ctime(&m_ModifTime))));
	ButtonValue* B;
	B=::PHelper.CreateButton(items,FHelper.PrepareKey(pref,"Control"),"Play (F5),Stop,Stop...",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent		= OnControlClick;
	::PHelper.CreateRName	(items,FHelper.PrepareKey(pref,"Name"),&m_Name,owner);

    PropValue* P = 0;
    // max particles
    PHelper.CreateS32		(items,FHelper.PrepareKey				(pref,"Max Particles"),					&m_MaxParticles,  0, 100000);
//    P->OnChangeEvent		= OnFlagChange;
	// time limit
    P=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey			(pref,"Time Limit"),		  			&m_Flags, dfTimeLimit);
    P->OnChangeEvent		= OnFlagChange;
    if (m_Flags.is(dfTimeLimit))
	    PHelper.CreateFloat	(items,FHelper.PrepareKey				(pref,"Time Limit\\Value (sec)"),		&m_fTimeLimit,  0, 10000.f);
	// sprite
    P=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey			(pref,"Sprite"),		 	   			&m_Flags, dfSprite);
    P->OnChangeEvent		= OnFlagChange;
    if (m_Flags.is(dfSprite)){
	    P=PHelper.CreateChoose(items,FHelper.PrepareKey				(pref,"Sprite\\Texture"), 	   			&m_TextureName, smTexture);
        P->OnChangeEvent		= OnShaderChange;
        P->Owner()->subitem		= 2;
	    P=PHelper.CreateChoose(items,FHelper.PrepareKey				(pref,"Sprite\\Shader"), 	   			&m_ShaderName,	smEShader);
        P->OnChangeEvent		= OnShaderChange;
    	// frame
        P=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey		(pref,"Sprite\\Frame"),		 		 	&m_Flags, dfFramed);
        P->OnChangeEvent		= OnFlagChange;
        if (m_Flags.is(dfFramed)){
            PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey	(pref,"Sprite\\Frame\\Random Init"), 	&m_Flags, dfRandomFrame);
            PHelper.CreateS32	(items,FHelper.PrepareKey			(pref,"Sprite\\Frame\\Count"),			&m_Frame.m_iFrameCount, 1,256);
            P=PHelper.CreateFloat(items,FHelper.PrepareKey			(pref,"Sprite\\Frame\\Size U (0..1)"),	&m_Frame.m_fTexSize.x, EPS_S,1.f,0.001f,8);
            P->OnChangeEvent	= OnFrameResize;
            PHelper.CreateFloat	(items,FHelper.PrepareKey			(pref,"Sprite\\Frame\\Size V (0..1)"),	&m_Frame.m_fTexSize.y, EPS_S,1.f,0.001f,8);
	        // animate
            P=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey	(pref,"Sprite\\Animated"),				&m_Flags, dfAnimated);
            P->OnChangeEvent	= OnFlagChange;
            if (m_Flags.is(dfAnimated)){
                PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey(pref,"Sprite\\Animated\\Random Playback"),	&m_Flags, dfRandomPlayback);
		    	PHelper.CreateFloat(items,FHelper.PrepareKey		(pref,"Sprite\\Animated\\Speed"),		&m_Frame.m_fSpeed, 0.f,1000.f);
            }
        }
    }
	// align to path
    P=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey	(pref,"Movement\\Align To Path"), 				&m_Flags, dfAlignToPath);
    P->OnChangeEvent		= OnFlagChange;
    if (m_Flags.is(dfAlignToPath))
    	PHelper.CreateAngle3(items,FHelper.PrepareKey		(pref,"Movement\\Align To Path\\Default Rotate"),	&m_APDefaultRotation);
	// velocity scale
    P=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey	(pref,"Movement\\Velocity Scale"),				&m_Flags, dfVelocityScale);
    P->OnChangeEvent		= OnFlagChange;
    if (m_Flags.is(dfVelocityScale))
    	PHelper.CreateVector(items,FHelper.PrepareKey		(pref,"Movement\\Velocity Scale\\Value"),		&m_VelocityScale, -1000.f, 1000.f);
	// collision
    P=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey	(pref,"Movement\\Collision"),					&m_Flags, dfCollision);
    P->OnChangeEvent		= OnFlagChange;
    if (m_Flags.is(dfCollision)){
    	PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey(pref,"Movement\\Collision\\Destroy On Contact"),&m_Flags, dfCollisionDel);
	    P=PHelper.CreateFloat	(items,FHelper.PrepareKey	(pref,"Movement\\Collision\\Friction"),			&m_fCollideOneMinusFriction,0.f, 1.f);
        P->Owner()->OnBeforeEditEvent 	= CollisionFrictionOnBeforeEdit;
        P->Owner()->OnAfterEditEvent 	= CollisionFrictionOnAfterEdit;
        P->Owner()->OnDrawTextEvent 	= CollisionFrictionOnDraw;
	    PHelper.CreateFloat		(items,FHelper.PrepareKey	(pref,"Movement\\Collision\\Resilence"),		&m_fCollideResilience, 		0.f, 1.f);
	    P=PHelper.CreateFloat	(items,FHelper.PrepareKey	(pref,"Movement\\Collision\\Cutoff"),			&m_fCollideSqrCutoff, 		0.f, P_MAXFLOAT);
        P->Owner()->OnBeforeEditEvent 	= CollisionCutoffOnBeforeEdit;
        P->Owner()->OnAfterEditEvent 	= CollisionCutoffOnAfterEdit;
        P->Owner()->OnDrawTextEvent 	= CollisionCutoffOnDraw;
    }
    // actions
	B=::PHelper.CreateButton(items,FHelper.PrepareKey(pref,"Actions\\Edit"),"Append",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent		= OnActionsClick;
	for (EPAVecIt s_it=m_EActionList.begin(); s_it!=m_EActionList.end(); s_it++){
    	u32 clr				= (*s_it)->flags.is(EParticleAction::flEnabled)?clBlack:clSilver;
    	AnsiString a_pref	= FHelper.PrepareKey(pref,"Actions",AnsiString().sprintf("%s (%s)",*(*s_it)->actionType,*(*s_it)->actionName).c_str());
        ButtonValue* B		= PHelper.CreateButton(items,a_pref,"Up,Down,Remove",ButtonValue::flFirstOnly); B->Owner()->tag = (s_it-m_EActionList.begin());
        B->Owner()->prop_color	= clr;
        B->OnBtnClickEvent	= OnActionEditClick;
		P=PHelper.CreateRText	(items,FHelper.PrepareKey(a_pref.c_str(),"Name"),&(*s_it)->actionName);
        P->Owner()->OnAfterEditEvent = OnAfterActionNameEdit;
        P->Owner()->prop_color	= clr;
    	(*s_it)->FillProp	(items,a_pref.c_str(),clr);
    }
}
void PS::CPEDef::Compile()
{
	m_Actions.clear	();
    m_Actions.w_u32	(m_EActionList.size());
    int cnt			= 0;
    for (EPAVecIt it=m_EActionList.begin(); it!=m_EActionList.end(); it++){
		if ((*it)->flags.is(EParticleAction::flEnabled)){
    	    (*it)->Compile(m_Actions);
            cnt++;
        }
    }
    m_Actions.seek	(0);
    m_Actions.w_u32 (cnt);
}
#endif

