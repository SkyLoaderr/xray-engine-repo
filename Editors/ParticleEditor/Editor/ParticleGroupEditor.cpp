//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleGroup.h"
#include "PropertiesListHelper.h"
#include "ui_main.h"
#include "ui_tools.h"

void __fastcall PS::CPGDef::OnEffectsEditClick(PropValue* sender, bool& bDataModified)
{
	ButtonValue* B 		= dynamic_cast<ButtonValue*>(sender); R_ASSERT(B);
    switch (B->btn_num){
    case 0:
        m_Effects.push_back(SEffect());
        UI.Command		(COMMAND_UPDATE_PROPERTIES);
        OnParamsChange	(sender);
        bDataModified	= true;
    break;
    }
}

void __fastcall PS::CPGDef::OnEffectTypeChange(PropValue* sender)
{
    UI.Command			(COMMAND_UPDATE_PROPERTIES);
    OnParamsChange		(sender);
}

void __fastcall PS::CPGDef::OnControlClick(PropValue* sender, bool& bDataModified)
{
	ButtonValue* B 		= dynamic_cast<ButtonValue*>(sender); R_ASSERT(B);
    switch (B->btn_num){
    case 0: 			Tools.PlayCurrent();		break;
    case 1: 			Tools.StopCurrent(false);	break;
    case 2: 			Tools.StopCurrent(true);	break;
    }
    bDataModified		= false;
}

void __fastcall PS::CPGDef::OnEffectEditClick(PropValue* sender, bool& bDataModified)
{
	ButtonValue* B 		= dynamic_cast<ButtonValue*>(sender); R_ASSERT(B);
    switch (B->btn_num){
    case 0:
        if (ELog.DlgMsg(mtConfirmation, TMsgDlgButtons() << mbYes << mbNo,"Delete selected folder?") == mrYes){
            m_Effects.erase	(m_Effects.begin()+B->Owner()->tag);
            UI.Command		(COMMAND_UPDATE_PROPERTIES);
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
	Tools.SetCurrentPG	(this);
}

xr_token			effect_token					[ ]={
    { "Stop/End",	PS::CPGDef::SEffect::etStopEnd	},        
    { 0,			0				  				}
};
void PS::CPGDef::FillProp(LPCSTR pref, ::PropItemVec& items, ::ListItem* owner)
{
    ButtonValue* B;
	B=PHelper.CreateButton		(items,FHelper.PrepareKey(pref,"Control"),"Play (F5),Stop,Stop...",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent			= OnControlClick;
    B=PHelper.CreateButton		(items,FHelper.PrepareKey(pref,"Edit"),"Append Effect",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent			= OnEffectsEditClick;
    PropValue* V;
	PHelper.CreateName			(items,FHelper.PrepareKey(pref,"Name"),m_Name,sizeof(m_Name),owner);
    V=PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Time Limit (s)"),	&m_fTimeLimit,	0.f,1000.f);
    V->OnChangeEvent			= OnParamsChange;
    for (EffectIt it=m_Effects.begin(); it!=m_Effects.end(); it++){
        AnsiString nm 			= AnsiString("Effect #")+(it-m_Effects.begin()+1);
        B=PHelper.CreateButton(items,FHelper.PrepareKey(pref,nm.c_str()),"Remove",ButtonValue::flFirstOnly); B->Owner()->tag = it-m_Effects.begin();
        B->OnBtnClickEvent		= OnEffectEditClick;
        V=PHelper.CreateLibPE	(items,FHelper.PrepareKey(pref,nm.c_str(),"Name"),it->m_EffectName,sizeof(it->m_EffectName));
        V->OnChangeEvent		= OnParamsChange;
	    switch (it->m_Type){
        case SEffect::etStopEnd: 
            V=PHelper.CreateFloat	(items,FHelper.PrepareKey(pref,nm.c_str(),"Start Time (s)"),&it->m_Time0,		0.f,1000.f);
            V->OnChangeEvent		= OnParamsChange;
            V=PHelper.CreateFloat	(items,FHelper.PrepareKey(pref,nm.c_str(),"End Time (s)"),	&it->m_Time1,		0.f,1000.f);
            V->OnChangeEvent		= OnParamsChange;
            V=PHelper.CreateFlag32	(items,FHelper.PrepareKey(pref,nm.c_str(),"Deferred Stop"),	&it->m_Flags,		SEffect::flDeferredStop);
            V->OnChangeEvent		= OnParamsChange;
            V=PHelper.CreateFlag32	(items,FHelper.PrepareKey(pref,nm.c_str(),"Enabled"),		&it->m_Flags,		SEffect::flEnabled);
            V->OnChangeEvent		= OnParamsChange;
        break;
        }
    }
}

 