//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#ifdef _PARTICLE_EDITOR

#include "ParticleEffect.h"
#include "ParticleEffectActions.h"
#include "PropertiesListHelper.h"
#include "ui_tools.h"
#include "ui_main.h"
             
BOOL PS::CPEDef::Equal(const CPEDef* pe)
{
	if (m_SourceText!=pe->m_SourceText)						return FALSE;
    if (!m_Flags.equal(pe->m_Flags)) 						return FALSE;
    if (((m_ShaderName==0)||(pe->m_ShaderName==0))&&(m_ShaderName!=pe->m_ShaderName)) 		return FALSE;
    if ((m_ShaderName!=pe->m_ShaderName)&&(0!=strcmp(m_ShaderName,pe->m_ShaderName))) 		return FALSE;
    if (((m_TextureName==0)||(pe->m_TextureName==0))&&(m_TextureName!=pe->m_TextureName)) 	return FALSE;
    if ((m_TextureName!=pe->m_TextureName)&&(0!=strcmp(m_TextureName,pe->m_TextureName)))	return FALSE;
    if (0!=memcmp(&m_Frame,&pe->m_Frame,sizeof(m_Frame))) 	return FALSE;
	if (!fsimilar(m_fTimeLimit,pe->m_fTimeLimit))			return FALSE;
    if (m_MaxParticles!=pe->m_MaxParticles)					return FALSE;
    if (m_ActionList.size()!=pe->m_ActionList.size())		return FALSE;
    if (!m_VelocityScale.similar(pe->m_VelocityScale))		return FALSE;
	if (!fsimilar(m_fCollideOneMinusFriction,pe->m_fCollideOneMinusFriction))	return FALSE;
    if (!fsimilar(m_fCollideResilience,pe->m_fCollideResilience))				return FALSE;
    if (!fsimilar(m_fCollideSqrCutoff,pe->m_fCollideSqrCutoff))					return FALSE;
    return TRUE;
}

void PS::CPEDef::Copy(const CPEDef& src)
{
    strcpy				(m_Name,src.m_Name); VERIFY(strlen(m_Name)<sizeof(m_Name));
    m_Flags				= src.m_Flags;
    m_ShaderName		= src.m_ShaderName?xr_strdup(src.m_ShaderName):0;
    m_TextureName		= src.m_TextureName?xr_strdup(src.m_TextureName):0;
    m_Frame				= src.m_Frame;
	m_fTimeLimit		= src.m_fTimeLimit;
    m_MaxParticles		= src.m_MaxParticles;
    m_ActionList.clear	();
	m_CachedShader		= src.m_CachedShader;
    m_VelocityScale.set			(src.m_VelocityScale);
	m_fCollideOneMinusFriction	= src.m_fCollideOneMinusFriction;
    m_fCollideResilience		= src.m_fCollideResilience;
    m_fCollideSqrCutoff			= src.m_fCollideSqrCutoff; 

	m_SourceText		= src.m_SourceText;

    Compile				();
}

void __fastcall PS::CPEDef::OnSourceTextEdit(PropValue* sender, bool& bDataModified)
{
	ButtonValue* B 		= dynamic_cast<ButtonValue*>(sender); R_ASSERT(B);
    switch (B->btn_num){
    case 0: 			Tools.EditActionList();	break;
    }
    bDataModified		= false;
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

void __fastcall PS::CPEDef::OnActionsClick(PropValue* sender, bool& bDataModified)
{
	ButtonValue* B 		= dynamic_cast<ButtonValue*>(sender); R_ASSERT(B);
    switch (B->btn_num){
    case 0:
    	m_EActionList.push_back(pCreateEAction(PAPI::PASourceID));
        UI.Command		(COMMAND_UPDATE_PROPERTIES);
    break;
    }
}

void PS::CPEDef::FillProp(LPCSTR pref, ::PropItemVec& items, ::ListItem* owner)
{
    PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey(pref,"Domain Draw"), 		&Tools.m_Flags, CParticleTools::flDrawDomain);
    PHelper.CreateFlag<Flags32>(items,FHelper.PrepareKey(pref,"Domain Solid"),	  	&Tools.m_Flags, CParticleTools::flDrawSolid);
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Owner Name"),	*m_OwnerName);
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Modif Name"),	*m_ModifName);
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Creation Time"),Trim(AnsiString(ctime(&m_CreateTime))));
	PHelper.CreateCaption	(items,FHelper.PrepareKey(pref,"Version\\Modified Time"),Trim(AnsiString(ctime(&m_ModifTime))));
	ButtonValue* B;
	B=::PHelper.CreateButton(items,FHelper.PrepareKey(pref,"Control"),"Play (F5),Stop,Stop...",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent		= OnControlClick;
	::PHelper.CreateName	(items,FHelper.PrepareKey(pref,"Name"),m_Name,sizeof(m_Name),owner);
    B=::PHelper.CreateButton(items,FHelper.PrepareKey(pref,"Source Text"),"Edit",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent		= OnSourceTextEdit;
/*
	B=::PHelper.CreateButton(items,FHelper.PrepareKey(pref,"Actions"),"Append",ButtonValue::flFirstOnly);
    B->OnBtnClickEvent		= OnActionsClick;
	for (EPAVecIt s_it=m_EActionList.begin(); s_it!=m_EActionList.end(); s_it++)
    	(*s_it)->FillProp	(items,FHelper.PrepareKey(pref,"Actions",AnsiString().sprintf("%02d - %s",s_it-m_EActionList.begin(),*(*s_it)->actionType).c_str()).c_str());
*/
}
#endif

