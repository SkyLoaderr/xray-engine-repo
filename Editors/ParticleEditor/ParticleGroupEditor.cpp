//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleGroup.h"
#include "PropertiesListHelper.h"
#include "ui_main.h"
#include "ui_particletools.h"

BOOL PS::CPGDef::SEffect::Equal(const SEffect& src)
{
	if (!m_Flags.equal(src.m_Flags))	return FALSE;
    if (!m_EffectName.equal(src.m_EffectName)) return FALSE;
	if (!fsimilar(m_Time0,src.m_Time0))	return FALSE;
	if (!fsimilar(m_Time1,src.m_Time1))	return FALSE;
	return TRUE;
}

BOOL PS::CPGDef::Equal(const CPGDef* pg)
{
	if (!m_Flags.equal(pg->m_Flags))				return FALSE;
	if (!fsimilar(m_fTimeLimit,pg->m_fTimeLimit))	return FALSE;
    if (m_Effects.size()!=pg->m_Effects.size())		return FALSE;
    EffectIt s_it=m_Effects.begin(); 
    for (EffectIt d_it=m_Effects.begin(); d_it!=m_Effects.end(); s_it++,d_it++)
    	if (!s_it->Equal(*d_it)) return FALSE;
	return TRUE;
}

void __fastcall PS::CPGDef::OnEffectsEditClick(PropValue* sender, bool& bDataModified, bool& bSafe)
{
	ButtonValue* B 		= dynamic_cast<ButtonValue*>(sender); R_ASSERT(B);
    switch (B->btn_num){
    case 0:
        m_Effects.push_back(SEffect());
        m_Effects.back().m_Flags.set(CPGDef::SEffect::flEnabled,FALSE);
        UI->Command		(COMMAND_UPDATE_PROPERTIES);
        OnParamsChange	(sender);
        bDataModified	= true;
    break;
    }
}

void __fastcall PS::CPGDef::OnEffectTypeChange(PropValue* sender)
{
    UI->Command			(COMMAND_UPDATE_PROPERTIES);
    OnParamsChange		(sender);
}

void __fastcall PS::CPGDef::OnControlClick(PropValue* sender, bool& bDataModified, bool& bSafe)
{
	ButtonValue* B 		= dynamic_cast<ButtonValue*>(sender); R_ASSERT(B);
    switch (B->btn_num){
    case 0: 			PTools->PlayCurrent();		break;
    case 1: 			PTools->StopCurrent(false);	break;
    case 2: 			PTools->StopCurrent(true);	break;
    }
    bDataModified		= false;
}

void __fastcall PS::CPGDef::OnEffectEditClick(PropValue* sender, bool& bDataModified, bool& bSafe)
{
	ButtonValue* B 		= dynamic_cast<ButtonValue*>(sender); R_ASSERT(B);
    switch (B->btn_num){
    case 0:		    	
    	PTools->PlayCurrent	(B->Owner()->tag);    
		bDataModified	= false;
    break;
    case 1:{
    	CPGDef::SEffect* eff = m_Effects.begin()+B->Owner()->tag; VERIFY(eff);
		PTools->SelectEffect(*eff->m_EffectName);
		bDataModified	= false;
        bSafe			= true;
    }break;
    case 2:        
        if (ELog.DlgMsg(mtConfirmation, TMsgDlgButtons() << mbYes << mbNo,"Remove effect?") == mrYes){
            m_Effects.erase	(m_Effects.begin()+B->Owner()->tag);
            UI->Command		(COMMAND_UPDATE_PROPERTIES);
            OnParamsChange	(sender);
            bDataModified	= true;
        }else{
			bDataModified	= false;
        }
    break;
    }
}

void __fastcall PS::CPGDef::OnParamsChange(PropValue* sender)
{
	PTools->SetCurrentPG	(0);
	PTools->SetCurrentPG	(this);
}

void PS::CPGDef::FillProp(LPCSTR pref, ::PropItemVec& items, ::ListItem* owner)
{
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Owner Name"),*m_OwnerName);
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Modif Name"),*m_ModifName);
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Creation Time"),Trim(AnsiString(ctime(&m_CreateTime))));
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Modified Time"),Trim(AnsiString(ctime(&m_ModifTime))));
    ButtonValue* B;
	B=PHelper.CreateButton		(items,FHelper.PrepareKey(pref,"Control"),"Play(F5),Stop(F6),Stop...(F7)",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent			= OnControlClick;
    B=PHelper.CreateButton		(items,FHelper.PrepareKey(pref,"Edit"),"Append Effect",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent			= OnEffectsEditClick;
    PropValue* V;
	PHelper.CreateRName			(items,FHelper.PrepareKey(pref,"Name"),&m_Name,owner);
    V=PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Time Limit (s)"),	&m_fTimeLimit,	-1.f,1000.f);
    V->OnChangeEvent			= OnParamsChange;
    for (EffectIt it=m_Effects.begin(); it!=m_Effects.end(); it++){
    	u32 clr					= it->m_Flags.is(CPGDef::SEffect::flEnabled)?clBlack:clSilver;
        AnsiString nm 			= AnsiString("Effect #")+(it-m_Effects.begin()+1);
        B=PHelper.CreateButton(items,FHelper.PrepareKey(pref,nm.c_str()),"Preview,Select,Remove",ButtonValue::flFirstOnly); B->Owner()->tag = it-m_Effects.begin();
        B->OnBtnClickEvent		= OnEffectEditClick;
        B->Owner()->prop_color	= clr;
        V=PHelper.CreateChoose	(items,FHelper.PrepareKey(pref,nm.c_str(),"Name"),&it->m_EffectName,smPE);
        V->OnChangeEvent		= OnParamsChange;
        V->Owner()->prop_color	= clr;
        V=PHelper.CreateFloat	(items,FHelper.PrepareKey(pref,nm.c_str(),"Start Time (s)"),&it->m_Time0,		0.f,1000.f);
        V->OnChangeEvent		= OnParamsChange;
        V->Owner()->prop_color	= clr;
        V=PHelper.CreateFloat	(items,FHelper.PrepareKey(pref,nm.c_str(),"End Time (s)"),	&it->m_Time1,		0.f,1000.f);
        V->OnChangeEvent		= OnParamsChange;
        V->Owner()->prop_color	= clr;
        V=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey(pref,nm.c_str(),"Deferred Stop"),&it->m_Flags,	SEffect::flDefferedStop);
        V->OnChangeEvent		= OnParamsChange;
        V->Owner()->prop_color	= clr;
        V=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey(pref,nm.c_str(),"Enabled"),									&it->m_Flags, 	SEffect::flEnabled);
        V->OnChangeEvent		= OnParamsChange;
        V->Owner()->prop_color	= clr;
        V=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey(pref,nm.c_str(),"Children\\On Birth"),						&it->m_Flags,	SEffect::flOnBirthChild);
        V->OnChangeEvent		= OnParamsChange;
        V->Owner()->prop_color	= clr;
        if (it->m_Flags.is(SEffect::flOnBirthChild)){
	        V=PHelper.CreateChoose	(items,FHelper.PrepareKey(pref,nm.c_str(),"Children\\On Birth\\Effect Name"),			&it->m_OnBirthChildName,smPE);
    	    V->OnChangeEvent	= OnParamsChange;
	        V->Owner()->prop_color	= clr;
        }
        V=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey(pref,nm.c_str(),"Children\\On Play"),						&it->m_Flags,	SEffect::flOnPlayChild);
        V->OnChangeEvent		= OnParamsChange;
        V->Owner()->prop_color	= clr;
        if (it->m_Flags.is(SEffect::flOnPlayChild)){
	        V=PHelper.CreateChoose	(items,FHelper.PrepareKey(pref,nm.c_str(),"Children\\On Play\\Effect Name"),			&it->m_OnPlayChildName,smPE);
    	    V->OnChangeEvent	= OnParamsChange;
	        V->Owner()->prop_color	= clr;
            V=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey(pref,nm.c_str(),"Children\\On Play\\Play After Stop"),		&it->m_Flags,	SEffect::flOnPlayChildRewind);
            V->OnChangeEvent		= OnParamsChange;
            V->Owner()->prop_color	= clr;
        }
        V=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey(pref,nm.c_str(),"Children\\On Dead"),						&it->m_Flags,	SEffect::flOnDeadChild);
        V->OnChangeEvent		= OnParamsChange;
        V->Owner()->prop_color	= clr;
        if (it->m_Flags.is(SEffect::flOnDeadChild)){
	        V=PHelper.CreateChoose	(items,FHelper.PrepareKey(pref,nm.c_str(),"Children\\On Dead\\Effect Name"),			&it->m_OnDeadChildName,smPE);
    	    V->OnChangeEvent	= OnParamsChange;    
	        V->Owner()->prop_color	= clr;
        }
    }
}

 