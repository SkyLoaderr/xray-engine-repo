//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#ifdef _PARTICLE_EDITOR

#include "ParticleEffect.h"
#include "PropertiesListHelper.h"
#include "ui_tools.h"

void PS::CPEDef::Copy(const CPEDef& src)
{
    strcpy				(m_Name,src.m_Name); VERIFY(strlen(m_Name)<sizeof(m_Name));
    m_Flags				= src.m_Flags;
    m_ShaderName		= src.m_ShaderName?xr_strdup(src.m_ShaderName):0;
    m_TextureName		= src.m_TextureName?xr_strdup(src.m_TextureName):0;
    m_Frame				= src.m_Frame;
	m_fTimeLimit		= src.m_fTimeLimit;
    m_MaxParticles		= src.m_MaxParticles;
    m_ActionCount 		= 0;
    m_ActionList		= 0;
	m_CachedShader		= src.m_CachedShader;
	m_SourceText		= src.m_SourceText;
    Compile				();
}

void __fastcall PS::CPEDef::OnSourceTextEdit(PropValue* sender, bool& bDataModified)
{
	ButtonValue* B 		= dynamic_cast<ButtonValue*>(sender); R_ASSERT(B);
    switch (B->btn_num){
    case 0: 			Tools.EditActionList();	break;
    }
}

void __fastcall PS::CPEDef::OnControlClick(PropValue* sender, bool& bDataModified)
{
	ButtonValue* B 		= dynamic_cast<ButtonValue*>(sender); R_ASSERT(B);
    switch (B->btn_num){
    case 0: 			Tools.PlayCurrent();		break;
    case 1: 			Tools.StopCurrent(false);	break;
    case 2: 			Tools.StopCurrent(true);	break;
    }
    bDataModified		= false;
}

void PS::CPEDef::FillProp(LPCSTR pref, ::PropItemVec& items, ::ListItem* owner)
{
	ButtonValue* B;
	B=::PHelper.CreateButton(items,FHelper.PrepareKey(pref,"Control"),"Play (F5),Stop,Stop...",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent		= OnControlClick;
	::PHelper.CreateName	(items,FHelper.PrepareKey(pref,"Name"),m_Name,sizeof(m_Name),owner);
    B=::PHelper.CreateButton(items,FHelper.PrepareKey(pref,"Source Text"),"Edit",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent		= OnSourceTextEdit;
}
#endif

