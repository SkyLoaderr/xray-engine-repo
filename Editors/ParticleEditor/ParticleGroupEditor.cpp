//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleGroup.h"
#include "PropertiesListHelper.h"
#include "ui_main.h"
#include "ui_particletools.h"

BOOL PS::CPGDef::SEffect::Equal(const SEffect& src)
{
	if (m_Type!=src.m_Type) 			return FALSE;
	if (!m_Flags.equal(src.m_Flags))	return FALSE;
    if (0!=strcmp(m_EffectName,src.m_EffectName)) return FALSE;
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
    case 1:        
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
	PTools->SetCurrentPG	(this);
}

xr_token			effect_token					[ ]={
    { "Stop/End",	PS::CPGDef::SEffect::etStopEnd	},        
    { 0,			0				  				}
};
void PS::CPGDef::FillProp(LPCSTR pref, ::PropItemVec& items, ::ListItem* owner)
{
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Owner Name"),*m_OwnerName);
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Modif Name"),*m_ModifName);
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Creation Time"),Trim(AnsiString(ctime(&m_CreateTime))));
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Modified Time"),Trim(AnsiString(ctime(&m_ModifTime))));
    ButtonValue* B;
	B=PHelper.CreateButton		(items,FHelper.PrepareKey(pref,"Control"),"Play (F5),Stop,Stop...",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent			= OnControlClick;
    B=PHelper.CreateButton		(items,FHelper.PrepareKey(pref,"Edit"),"Append Effect",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent			= OnEffectsEditClick;
    PropValue* V;
	PHelper.CreateRName			(items,FHelper.PrepareKey(pref,"Name"),&m_Name,owner);
    V=PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Time Limit (s)"),	&m_fTimeLimit,	-1.f,1000.f);
    V->OnChangeEvent			= OnParamsChange;
    for (EffectIt it=m_Effects.begin(); it!=m_Effects.end(); it++){
        AnsiString nm 			= AnsiString("Effect #")+(it-m_Effects.begin()+1);
        B=PHelper.CreateButton(items,FHelper.PrepareKey(pref,nm.c_str()),"Preview,Remove",ButtonValue::flFirstOnly); B->Owner()->tag = it-m_Effects.begin();
        B->OnBtnClickEvent		= OnEffectEditClick;
        V=PHelper.CreateChoose	(items,FHelper.PrepareKey(pref,nm.c_str(),"Name"),it->m_EffectName,sizeof(it->m_EffectName),smPE);
        V->OnChangeEvent		= OnParamsChange;
	    switch (it->m_Type){
        case SEffect::etStopEnd: 
            V=PHelper.CreateFloat	(items,FHelper.PrepareKey(pref,nm.c_str(),"Start Time (s)"),&it->m_Time0,		0.f,1000.f);
            V->OnChangeEvent		= OnParamsChange;
            V=PHelper.CreateFloat	(items,FHelper.PrepareKey(pref,nm.c_str(),"End Time (s)"),	&it->m_Time1,		0.f,1000.f);
            V->OnChangeEvent		= OnParamsChange;
            V=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey(pref,nm.c_str(),"Deferred Stop"),	&it->m_Flags,		SEffect::flDeferredStop);
            V->OnChangeEvent		= OnParamsChange;
            V=PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey(pref,nm.c_str(),"Enabled"),		&it->m_Flags,		SEffect::flEnabled);
            V->OnChangeEvent		= OnParamsChange;
        break;
        }
    }
}

 