#include "stdafx.h"
#pragma hdrstop

#include "ESceneProperties.h"
#include "PropertiesList.h"
#include "Scene.h"
#include "communicate.h"

xr_token		js_token	[ ]={
	{ "1 - Low",			1	},
	{ "4 - Medium",			4	},
	{ "9 - High",			9	},
	{ 0,					0 	}
};

void __stdcall  OnControlClick	(PropValue* sender, bool& bModif, bool& bSafe)
{
	ButtonValue* V = dynamic_cast<ButtonValue*>(sender); R_ASSERT(V);
    switch (V->btn_num){
    case 0: Scene->m_LevelOp.SetLowQuality();		break;
    case 1: Scene->m_LevelOp.SetHighQuality();		break;
    case 2: Scene->m_LevelOp.SetCustomQuality();	break;
	}
}

int __cdecl ScenePropertiesEdit(b_params* params, u32 version, bool bBuildMode)
{
	if (version!=XRCL_CURRENT_VERSION){
    	ELog.DlgMsg(mtError,"Incorrect version of level options dll.");
    	return mrCancel;
    }

    TProperties* m_Props		= TProperties::CreateModalForm(bBuildMode?"Build Level":"Scene Options",bBuildMode?true:false,0,0,0,TProperties::plFolderStore|TProperties::plItemFolders|TProperties::plFullExpand);
    m_Props->ebOK->Caption		= bBuildMode?"Build":"Ok";

    PropItemVec items;   
	// common
    ButtonValue* B;
    B=PHelper().CreateButton	(items,"Common\\Quality", 					"Low,High,Custom",0);
    B->OnBtnClickEvent.bind		(OnControlClick);

    PropValue* V;
    BOOL enabled				= (params->m_quality==ebqCustom);
    V=PHelper().CreateU8		(items,"Lighting\\Hemisphere quality",		&Scene->m_LevelOp.m_LightHemiQuality,	0,3);		V->Owner()->Enable(enabled);
    V=PHelper().CreateU8		(items,"Lighting\\Sun shadow quality",		&Scene->m_LevelOp.m_LightSunQuality,	0,3);       V->Owner()->Enable(enabled);

    // Build Options
    // Normals & optimization
    V=PHelper().CreateFloat		(items,"Optimizing\\Normal smooth angle",	&params->m_sm_angle);								V->Owner()->Enable(enabled);
    V=PHelper().CreateFloat		(items,"Optimizing\\Weld distance (m)",		&params->m_weld_distance);							V->Owner()->Enable(enabled);

    // Light maps
    V=PHelper().CreateFloat		(items,"Lighting\\Pixel per meter",			&params->m_lm_pixels_per_meter,			0.f,20.f);	V->Owner()->Enable(enabled);
    V=PHelper().CreateU32		(items,"Lighting\\Error (LM collapsing)",	&params->m_lm_rms,						0,255);		V->Owner()->Enable(enabled);
    V=PHelper().CreateU32		(items,"Lighting\\Error (LM zero)",			&params->m_lm_rms_zero,					0,255);		V->Owner()->Enable(enabled);
    V=PHelper().CreateToken32	(items,"Lighting\\Jitter samples",			&params->m_lm_jitter_samples, 			js_token);	V->Owner()->Enable(enabled);

    m_Props->AssignItems		(items);
    
	int res						= m_Props->ShowPropertiesModal();
    TProperties::DestroyForm	(m_Props);

    return res;
}

