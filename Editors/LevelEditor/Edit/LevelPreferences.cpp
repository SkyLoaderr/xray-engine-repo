//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "LevelPreferences.h"
#include "ESceneCustomMTools.h"
#include "scene.h"
#include "ui_levelmain.h"
//---------------------------------------------------------------------------

void CLevelPreferences::Load(CInifile* I)
{
	inherited::Load		(I);                         
    SceneToolsMapPairIt _I 	= Scene->FirstTools();
    SceneToolsMapPairIt _E 	= Scene->LastTools();
    for (; _I!=_E; _I++)
        if (_I->second&&(_I->first!=OBJCLASS_DUMMY))	
        	_I->second->m_bEnabled	= R_U32_SAFE("targets",_I->second->ClassName(),TRUE);
}

void CLevelPreferences::Save(CInifile* I)
{
	inherited::Save		(I);
    SceneToolsMapPairIt _I 	= Scene->FirstTools();
    SceneToolsMapPairIt _E 	= Scene->LastTools();
    for (; _I!=_E; _I++)
        if (_I->second&&(_I->first!=OBJCLASS_DUMMY))	I->w_u32	("targets",_I->second->ClassName(),_I->second->m_bEnabled);
}

void CLevelPreferences::OnEnabledChange(PropValue* prop)
{
	ESceneCustomMTools* M	= Scene->GetMTools(prop->tag); VERIFY(M);
	ExecCommand				(COMMAND_ENABLE_TARGET,prop->tag,M->m_bEnabled);
}	

void CLevelPreferences::FillProp(PropItemVec& items)
{
	inherited::FillProp	(items);
    SceneToolsMapPairIt _I 	= Scene->FirstTools();
    SceneToolsMapPairIt _E 	= Scene->LastTools();
    for (; _I!=_E; _I++)
        if (_I->second&&(_I->first!=OBJCLASS_DUMMY)&&_I->second->AllowEnabling()){ 
		    PropValue* V 	= PHelper().CreateBOOL	(items,PrepareKey("Scene\\Enable Targets",_I->second->ClassDesc()),	&_I->second->m_bEnabled);
            V->tag			= _I->second->ClassID;
            V->OnChangeEvent.bind(this,&CLevelPreferences::OnEnabledChange);
        }
}

