//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleEffect.h"
#include "PropertiesListHelper.h"
#include "ui_tools.h"

void __fastcall PS::CPEDef::OnSourceTextEdit(PropValue* sender, bool& bDataModified)
{
	ButtonValue* B 		= dynamic_cast<ButtonValue*>(sender); R_ASSERT(B);
    switch (B->btn_num){
    case 0: 			Tools.EditActionList();	break;
    }
}

void PS::CPEDef::FillProp(LPCSTR pref, ::PropItemVec& items, ::ListItem* owner)
{
	::PHelper.CreateName(items,FHelper.PrepareKey(pref,"Name"),m_Name,sizeof(m_Name),owner);
	ButtonValue* B 		= ::PHelper.CreateButton(items,FHelper.PrepareKey(pref,"Source Text"),"Edit");
    B->OnBtnClickEvent	= OnSourceTextEdit;
}


