////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects for ALife simulator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma hdrstop

#ifdef AI_COMPILER
	#include "net_utils.h"
#endif

#include "xrServer_Objects_ALife.h"
#include "game_base.h"
#include "ai_alife_templates.h"

#ifdef _EDITOR
	#include "BodyInstance.h"
	static TokenValue4::ItemVec locations[4];
	static AStringVec	level_ids;
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeEvent
////////////////////////////////////////////////////////////////////////////
void CSE_ALifeEvent::STATE_Write			(NET_Packet &tNetPacket)
{
}

void CSE_ALifeEvent::STATE_Read				(NET_Packet &tNetPacket, u16 size)
{
}

void CSE_ALifeEvent::UPDATE_Write			(NET_Packet &tNetPacket)
{
	tNetPacket.w				(&m_tEventID,		sizeof(m_tEventID));
	tNetPacket.w				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.w				(&m_tGraphID,		sizeof(m_tGraphID));
	tNetPacket.w				(&m_tCombatResult,	sizeof(m_tCombatResult));
	m_tpMonsterGroup1->UPDATE_Write(tNetPacket);
	m_tpMonsterGroup2->UPDATE_Write(tNetPacket);
};

void CSE_ALifeEvent::UPDATE_Read			(NET_Packet &tNetPacket)
{
	tNetPacket.r				(&m_tEventID,		sizeof(m_tEventID));
	tNetPacket.r				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.r				(&m_tGraphID,		sizeof(m_tGraphID));
	tNetPacket.r				(&m_tCombatResult,	sizeof(m_tCombatResult));
	m_tpMonsterGroup1->UPDATE_Read(tNetPacket);
	m_tpMonsterGroup2->UPDATE_Read(tNetPacket);
};

#ifdef _EDITOR
void CSE_ALifeEvent::FillProp	(LPCSTR pref, PropItemVec& values)
{
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifePersonalEvent
////////////////////////////////////////////////////////////////////////////
void CSE_ALifePersonalEvent::STATE_Write	(NET_Packet &tNetPacket)
{
	save_data					(m_tpItemIDs,		tNetPacket);
}

void CSE_ALifePersonalEvent::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	load_data					(m_tpItemIDs,		tNetPacket);
}

void CSE_ALifePersonalEvent::UPDATE_Write	(NET_Packet &tNetPacket)
{
	tNetPacket.w				(&m_tEventID,		sizeof(m_tEventID));
	tNetPacket.w				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.w				(&m_tTaskID,		sizeof(m_tTaskID));
	tNetPacket.w_u32			(m_iHealth);
	tNetPacket.w				(&m_tEventRelationType,sizeof(m_tEventRelationType));
};

void CSE_ALifePersonalEvent::UPDATE_Read	(NET_Packet &tNetPacket)
{
	tNetPacket.r				(&m_tEventID,		sizeof(m_tEventID));
	tNetPacket.r				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.r				(&m_tTaskID,		sizeof(m_tTaskID));
	tNetPacket.r_s32			(m_iHealth);
	tNetPacket.r				(&m_tEventRelationType,	sizeof(m_tEventRelationType));
};

#ifdef _EDITOR
void CSE_ALifePersonalEvent::FillProp		(LPCSTR pref, PropItemVec& values)
{
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeTask
////////////////////////////////////////////////////////////////////////////
CSE_ALifeTask::CSE_ALifeTask				()
{
	m_dwTryCount				= 0;
	m_caSection[0]				= 0;
}

void CSE_ALifeTask::STATE_Write				(NET_Packet &tNetPacket)
{
	tNetPacket.w_u32			(m_dwTryCount);
}

void CSE_ALifeTask::STATE_Read				(NET_Packet &tNetPacket, u16 size)
{
	tNetPacket.r_u32			(m_dwTryCount);
}

void CSE_ALifeTask::UPDATE_Write			(NET_Packet &tNetPacket)
{
	tNetPacket.w				(&m_tTaskID,		sizeof(m_tTaskID));
	tNetPacket.w				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.w				(&m_tCustomerID,	sizeof(m_tCustomerID));
	tNetPacket.w_float			(m_fCost);
	tNetPacket.w				(&m_tTaskType,		sizeof(m_tTaskType));
	switch (m_tTaskType) {
		case eTaskTypeSearchForItemCL : {
			tNetPacket.w_string	(m_caSection);
			tNetPacket.w		(&m_tLocationID,	sizeof(m_tLocationID));
			break;
		}
		case eTaskTypeSearchForItemCG : {
			tNetPacket.w_string	(m_caSection);
			tNetPacket.w		(&m_tGraphID,		sizeof(m_tGraphID));
			break;
		}
		case eTaskTypeSearchForItemOL : {
			tNetPacket.w		(&m_tObjectID,		sizeof(m_tObjectID));
			tNetPacket.w		(&m_tLocationID,	sizeof(m_tLocationID));
			break;
		}
		case eTaskTypeSearchForItemOG : {
			tNetPacket.w		(&m_tObjectID,		sizeof(m_tObjectID));
			tNetPacket.w		(&m_tGraphID,		sizeof(m_tGraphID));
			break;
		}
		default : {
			break;
		}
	}
}

void CSE_ALifeTask::UPDATE_Read				(NET_Packet &tNetPacket)
{
	tNetPacket.r				(&m_tTaskID,	sizeof(m_tTaskID));
	tNetPacket.r				(&m_tTimeID,	sizeof(m_tTimeID));
	tNetPacket.r				(&m_tCustomerID,sizeof(m_tCustomerID));
	tNetPacket.r_float			(m_fCost);
	tNetPacket.r				(&m_tTaskType,	sizeof(m_tTaskType));
	switch (m_tTaskType) {
		case eTaskTypeSearchForItemCL : {
			tNetPacket.r_string	(m_caSection);
			tNetPacket.r		(&m_tLocationID,sizeof(m_tLocationID));
			break;
		}
		case eTaskTypeSearchForItemCG : {
			tNetPacket.r_string	(m_caSection);
			tNetPacket.r		(&m_tGraphID,	sizeof(m_tGraphID));
			break;
		}
		case eTaskTypeSearchForItemOL : {
			tNetPacket.r		(&m_tObjectID,	sizeof(m_tObjectID));
			tNetPacket.r		(&m_tLocationID,sizeof(m_tLocationID));
			break;
		}
		case eTaskTypeSearchForItemOG : {
			tNetPacket.r		(&m_tObjectID,	sizeof(m_tObjectID));
			tNetPacket.r		(&m_tGraphID,	sizeof(m_tGraphID));
			break;
		}
		default : {
			break;
		}
	};
}

#ifdef _EDITOR
void CSE_ALifeTask::FillProp	(LPCSTR pref, PropItemVec& values)
{
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeLevelPoint
////////////////////////////////////////////////////////////////////////////
CSE_LevelPoint::CSE_LevelPoint				(LPCSTR caSection) : CSE_Abstract(caSection)
{
}

CSE_LevelPoint::~CSE_LevelPoint				()
{
}

void CSE_LevelPoint::STATE_Read				(NET_Packet	&tNetPacket, u16 size)
{
};

void CSE_LevelPoint::STATE_Write			(NET_Packet	&tNetPacket)
{
};
void CSE_LevelPoint::UPDATE_Read			(NET_Packet	&tNetPacket)
{
}

void CSE_LevelPoint::UPDATE_Write			(NET_Packet	&tNetPacket)
{
}

#ifdef _EDITOR              
void CSE_LevelPoint::FillProp				(LPCSTR pref, PropItemVec& items)
{
}
#endif
////////////////////////////////////////////////////////////////////////////
// CSE_ALifeGraphPoint
////////////////////////////////////////////////////////////////////////////
CSE_ALifeGraphPoint::CSE_ALifeGraphPoint	(LPCSTR caSection) : CSE_Abstract(caSection)
{
	s_gameid					= GAME_DUMMY;
	m_caConnectionPointName[0]	= 0;
	m_caConnectionLevelName[0]	= 0;
	m_tLocations[0]				= 0;
	m_tLocations[1]				= 0;
	m_tLocations[2]				= 0;
	m_tLocations[3]				= 0;
}

void CSE_ALifeGraphPoint::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_string			(m_caConnectionPointName);
	if (m_wVersion < 33)
		tNetPacket.r_u32		();
	else
		tNetPacket.r_string		(m_caConnectionLevelName);
	tNetPacket.r_u8				(m_tLocations[0]);
	tNetPacket.r_u8				(m_tLocations[1]);
	tNetPacket.r_u8				(m_tLocations[2]);
	tNetPacket.r_u8				(m_tLocations[3]);
	if (!tNetPacket.r_eof()) {
		tNetPacket.r_pos		-= 4;
		u32						dwTemp;
		tNetPacket.r_u32		(dwTemp);
		m_tLocations[0]			= (u8)dwTemp;
		tNetPacket.r_u32		(dwTemp);
		m_tLocations[1]			= (u8)dwTemp;
	}
};

void CSE_ALifeGraphPoint::STATE_Write		(NET_Packet	&tNetPacket)
{
	tNetPacket.w_string			(m_caConnectionPointName);
	tNetPacket.w_string			(m_caConnectionLevelName);
	tNetPacket.w_u8				(m_tLocations[0]);
	tNetPacket.w_u8				(m_tLocations[1]);
	tNetPacket.w_u8				(m_tLocations[2]);
	tNetPacket.w_u8				(m_tLocations[3]);
};
void CSE_ALifeGraphPoint::UPDATE_Read		(NET_Packet	&tNetPacket)
{
}

void CSE_ALifeGraphPoint::UPDATE_Write		(NET_Packet	&tNetPacket)
{
}

#ifdef _EDITOR              
void CSE_ALifeGraphPoint::FillProp			(LPCSTR pref, PropItemVec& items)
{
	CInifile					*Ini = 0;

	if(locations[0].empty()||locations[1].empty()||locations[2].empty()||locations[3].empty()||level_ids.empty()){
		string256				gm_name;
		FS.update_path			(gm_name,_game_data_,"game.ltx");
		R_ASSERT2				(FS.exist(gm_name),"Couldn't find file 'game.ltx'");
		Ini						= xr_new<CInifile>(gm_name);
	}

	for (int i=0; i<LOCATION_TYPE_COUNT; i++)
		if(locations[i].empty()) {
			string256			caSection, T;
			strconcat			(caSection,SECTION_HEADER,itoa(i,T,10));
			R_ASSERT			(Ini->section_exist(caSection));
			LPCSTR				N,V;
			for (u32 k = 0; Ini->r_line(caSection,k,&N,&V); k++) {
				locations[i].push_back	(TokenValue4::Item());
				TokenValue4::Item		&val = locations[i].back();
				val.str					= V;
				val.ID					= atoi(N);
			}
		}

	if(level_ids.empty()) {
		LPCSTR					N,V;
		for (u32 k = 0; Ini->r_line("levels",k,&N,&V); k++)
			level_ids.push_back	(Ini->r_string(N,"caption"));
	}
	if (Ini)
		xr_delete				(Ini);

	PHelper.CreateToken4		(items,	FHelper.PrepareKey(pref,s_name,"Location\\1"),				(u32*)&m_tLocations[0],			&locations[0],					1);
	PHelper.CreateToken4		(items,	FHelper.PrepareKey(pref,s_name,"Location\\2"),				(u32*)&m_tLocations[1],			&locations[1],					1);
	PHelper.CreateToken4		(items,	FHelper.PrepareKey(pref,s_name,"Location\\3"),				(u32*)&m_tLocations[2],			&locations[2],					1);
	PHelper.CreateToken4		(items,	FHelper.PrepareKey(pref,s_name,"Location\\4"),				(u32*)&m_tLocations[3],			&locations[3],					1);

	PHelper.CreateList			(items,	FHelper.PrepareKey(pref,s_name,"Connection\\Level name"),	m_caConnectionLevelName,		sizeof(m_caConnectionLevelName),		&level_ids);
	PHelper.CreateText			(items,	FHelper.PrepareKey(pref,s_name,"Connection\\Point name"),	m_caConnectionPointName,		sizeof(m_caConnectionPointName));
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObject
////////////////////////////////////////////////////////////////////////////
CSE_ALifeObject::CSE_ALifeObject			(LPCSTR caSection) : CSE_Abstract(caSection)
{
	m_bOnline					= false;
	m_fDistance					= 0.0f;
	m_tClassID					= TEXT2CLSID(pSettings->r_string(caSection,"class"));	
	ID							= _OBJECT_ID(-1);
	m_tGraphID					= _GRAPH_ID(-1);
	m_tGraphID					= _SPAWN_ID(-1);
	m_fProbability				= 1.f;
	m_dwSpawnGroup				= 0;
	m_bDirectControl			= true;
	m_bALifeControl				= true;
	m_tNodeID					= u32(-1);
	strcpy						(m_caGroupControl,"");
}

void CSE_ALifeObject::STATE_Write			(NET_Packet &tNetPacket)
{
	tNetPacket.w_float			(m_fProbability);
	tNetPacket.w_u32			(m_dwSpawnGroup);
	tNetPacket.w				(&m_tGraphID,	sizeof(m_tGraphID));
	tNetPacket.w_float			(m_fDistance);
	tNetPacket.w_u32			(m_bDirectControl);
	tNetPacket.w_u32			(m_tNodeID);
	tNetPacket.w				(&m_tSpawnID,	sizeof(m_tSpawnID));
	tNetPacket.w_string			(m_caGroupControl);
}

void CSE_ALifeObject::STATE_Read			(NET_Packet &tNetPacket, u16 size)
{
	if (m_wVersion >= 1) {
		if (m_wVersion > 24)
			tNetPacket.r_float		(m_fProbability);
		else {
			u8						l_ucTemp;
			tNetPacket.r_u8			(l_ucTemp);
			m_fProbability			= (float)l_ucTemp;
		}
		tNetPacket.r_u32		(m_dwSpawnGroup);
		if (m_wVersion < 4) {
			u16					wDummy;
			tNetPacket.r_u16	(wDummy);
		}
		tNetPacket.r			(&m_tGraphID,	sizeof(m_tGraphID));
		tNetPacket.r_float		(m_fDistance);
	}
	if (m_wVersion >= 4) {
		u32						dwDummy;
		tNetPacket.r_u32		(dwDummy);
		m_bDirectControl		= !!dwDummy;
	}
	if (m_wVersion >= 8)
		tNetPacket.r_u32		(m_tNodeID);
	if (m_wVersion > 22)
		tNetPacket.r			(&m_tSpawnID,	sizeof(m_tSpawnID));
	if (m_wVersion > 23)
		tNetPacket.r_string		(m_caGroupControl);
}

void CSE_ALifeObject::UPDATE_Write			(NET_Packet &tNetPacket)
{
}

void CSE_ALifeObject::UPDATE_Read			(NET_Packet &tNetPacket)
{
};

#ifdef _EDITOR
void CSE_ALifeObject::FillProp				(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref, items);
	PHelper.CreateFloat			(items,	FHelper.PrepareKey(pref,s_name,"ALife\\Probability"),	&m_fProbability,	0,100);
	PHelper.CreateSceneItem		(items, FHelper.PrepareKey(pref,s_name,"ALife\\Group control"),	m_caGroupControl,  sizeof(m_caGroupControl), OBJCLASS_SPAWNPOINT, pSettings->r_string(s_name,"GroupControlSection"));
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeGroupAbstract
////////////////////////////////////////////////////////////////////////////
void CSE_ALifeGroupAbstract::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	u32							dwDummy;
	tNetPacket.r_u32			(dwDummy);
	m_bCreateSpawnPositions		= !!dwDummy;
	tNetPacket.r_u16			(m_wCount);
	if (m_wVersion > 19)
		load_data				(m_tpMembers,tNetPacket);
};

void CSE_ALifeGroupAbstract::STATE_Write	(NET_Packet	&tNetPacket)
{
	tNetPacket.w_u32			(m_bCreateSpawnPositions);
	tNetPacket.w_u16			(m_wCount);
	save_data					(m_tpMembers,tNetPacket);
};

void CSE_ALifeGroupAbstract::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	u32							dwDummy;
	tNetPacket.r_u32			(dwDummy);
	m_bCreateSpawnPositions		= !!dwDummy;
};

void CSE_ALifeGroupAbstract::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	tNetPacket.w_u32			(m_bCreateSpawnPositions);
};

#ifdef _EDITOR
void CSE_ALifeGroupAbstract::FillProp		(LPCSTR pref, PropItemVec& items)
{
	PHelper.CreateU16			(items,	FHelper.PrepareKey(pref, "ALife\\Count"),			&m_wCount,			0,0xff);
};	
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeEventGroup
////////////////////////////////////////////////////////////////////////////
void CSE_ALifeEventGroup::STATE_Write		(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeEventGroup::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
}

void CSE_ALifeEventGroup::UPDATE_Write		(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	tNetPacket.w				(&m_wCountAfter,sizeof(m_wCountAfter));
};

void CSE_ALifeEventGroup::UPDATE_Read		(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	tNetPacket.r				(&m_wCountAfter,sizeof(m_wCountAfter));
};

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeDynamicObject
////////////////////////////////////////////////////////////////////////////
CSE_ALifeDynamicObject::CSE_ALifeDynamicObject(LPCSTR caSection) : CSE_ALifeObject(caSection), CSE_Abstract(caSection)
{
	m_tTimeID					= 0;
	m_qwSwitchCounter			= u64(-1);
}

void CSE_ALifeDynamicObject::STATE_Write	(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeDynamicObject::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
}

void CSE_ALifeDynamicObject::UPDATE_Write	(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
};

void CSE_ALifeDynamicObject::UPDATE_Read	(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
};

#ifdef _EDITOR
void CSE_ALifeDynamicObject::FillProp	(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
}
#endif
////////////////////////////////////////////////////////////////////////////
// CSE_ALifeDynamicObjectVisual
////////////////////////////////////////////////////////////////////////////
CSE_ALifeDynamicObjectVisual::CSE_ALifeDynamicObjectVisual(LPCSTR caSection) : CSE_ALifeDynamicObject(caSection), CSE_Visual(), CSE_Abstract(caSection)
{
	if (pSettings->line_exist(caSection,"visual"))
		set_visual				(pSettings->r_string(caSection,"visual"));
}

void CSE_ALifeDynamicObjectVisual::STATE_Write(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	visual_write				(tNetPacket);
}

void CSE_ALifeDynamicObjectVisual::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
	if (m_wVersion > 31)
		visual_read				(tNetPacket);
}

void CSE_ALifeDynamicObjectVisual::UPDATE_Write(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
};

void CSE_ALifeDynamicObjectVisual::UPDATE_Read(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
};

#ifdef _EDITOR
void CSE_ALifeDynamicObjectVisual::FillProp	(LPCSTR pref, PropItemVec& items)
{
	inherited1::FillProp		(pref,items);
	inherited2::FillProp		(FHelper.PrepareKey(pref,s_name).c_str(),items);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeLevelChanger
////////////////////////////////////////////////////////////////////////////
CSE_ALifeLevelChanger::CSE_ALifeLevelChanger(LPCSTR caSection) : CSE_ALifeDynamicObject(caSection), CSE_Abstract(caSection)
{
	m_tNextGraphID				= _GRAPH_ID(-1);
	m_dwNextNodeID				= u32(-1);
	m_tNextPosition.set			(0.f,0.f,0.f);
	m_fAngle					= 0.f;
	m_caLevelToChange[0]		= 0;
	m_caLevelPointToChange[0]	= 0;
}

CSE_ALifeLevelChanger::~CSE_ALifeLevelChanger()
{
}

void CSE_ALifeLevelChanger::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket,size);
	cform_read					(tNetPacket);
	if (m_wVersion < 34) {
		tNetPacket.r_u32		();
		tNetPacket.r_u32		();
	}
	else {
		tNetPacket.r			(&m_tNextGraphID,sizeof(m_tNextGraphID));
		tNetPacket.r_u32		(m_dwNextNodeID);
		tNetPacket.r_float		(m_tNextPosition.x);
		tNetPacket.r_float		(m_tNextPosition.y);
		tNetPacket.r_float		(m_tNextPosition.z);
		tNetPacket.r_float		(m_fAngle);
	}
	tNetPacket.r_string			(m_caLevelToChange);
	tNetPacket.r_string			(m_caLevelPointToChange);
}

void CSE_ALifeLevelChanger::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	cform_write					(tNetPacket);
	tNetPacket.w				(&m_tNextGraphID,sizeof(m_tNextGraphID));
	tNetPacket.w_u32			(m_dwNextNodeID);
	tNetPacket.w_float			(m_tNextPosition.x);
	tNetPacket.w_float			(m_tNextPosition.y);
	tNetPacket.w_float			(m_tNextPosition.z);
	tNetPacket.w_float			(m_fAngle);
	tNetPacket.w_string			(m_caLevelToChange);
	tNetPacket.w_string			(m_caLevelPointToChange);
}

void CSE_ALifeLevelChanger::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeLevelChanger::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeLevelChanger::FillProp		(LPCSTR pref, PropItemVec& items)
{
	inherited1::FillProp		(pref,items);
	
	CInifile					*Ini = 0;

	if (level_ids.empty()) {
		string256				gm_name;
		FS.update_path			(gm_name,_game_data_,"game.ltx");
		R_ASSERT2				(FS.exist(gm_name),"Couldn't find file 'game.ltx'");
		Ini						= xr_new<CInifile>(gm_name);
	}

    if(level_ids.empty()) {
        LPCSTR				N,V;
        for (u32 k = 0; Ini->r_line("levels",k,&N,&V); k++)
            level_ids.push_back	(Ini->r_string(N,"caption"));
    }
	if (Ini)
		xr_delete				(Ini);
	
	PHelper.CreateList			(items,FHelper.PrepareKey(pref,s_name,"Level to change"),		m_caLevelToChange,			sizeof(m_caLevelToChange),	&level_ids);
	PHelper.CreateText			(items,FHelper.PrepareKey(pref,s_name,"Level point to change"),	m_caLevelPointToChange,		sizeof(m_caLevelPointToChange));
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObjectPhysic
////////////////////////////////////////////////////////////////////////////
CSE_ALifeObjectPhysic::CSE_ALifeObjectPhysic(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_Abstract(caSection)
{
	type 						= epotBox;
	mass 						= 10.f;
    fixed_bone[0]				=0;
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
    	set_visual				(pSettings->r_string(caSection,"visual"));
	startup_animation[0]		= 0;
}

CSE_ALifeObjectPhysic::~CSE_ALifeObjectPhysic		() 
{
}

void CSE_ALifeObjectPhysic::STATE_Read		(NET_Packet	&tNetPacket, u16 size) 
{
	if (m_wVersion >= 14)
		if (m_wVersion >= 16) {
			inherited::STATE_Read(tNetPacket,size);
			if (m_wVersion < 32)
				visual_read		(tNetPacket);
		}
		else {
			CSE_ALifeObject::STATE_Read(tNetPacket,size);
			visual_read			(tNetPacket);
		}
	tNetPacket.r_u32			(type);
	tNetPacket.r_float			(mass);
    
	if (m_wVersion > 9)
		tNetPacket.r_string		(fixed_bone);
	// internal
	strlwr						(fixed_bone);
	if (m_wVersion > 28)
		tNetPacket.r_string		(startup_animation);

#ifdef _EDITOR    
	PlayAnimation				(startup_animation[0]?startup_animation:"$editor");
#endif
}

void CSE_ALifeObjectPhysic::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u32			(type);
	tNetPacket.w_float			(mass);
	tNetPacket.w_string			(fixed_bone);
	tNetPacket.w_string			(startup_animation);
}

void CSE_ALifeObjectPhysic::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeObjectPhysic::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
xr_token po_types[]={
	{ "Box",			epotBox			},
	{ "Fixed chain",	epotFixedChain	},
	{ "Free chain",		epotFreeChain	},
	{ "Skeleton",		epotSkeleton	},
	{ 0,				0				}
};

void __fastcall	CSE_ALifeObjectPhysic::OnChangeAnim(PropValue* sender)
{
	PlayAnimation				(startup_animation);
}

void CSE_ALifeObjectPhysic::FillProp		(LPCSTR pref, PropItemVec& values) {
	inherited::FillProp			(pref,	 values);
	PHelper.CreateToken			(values, FHelper.PrepareKey(pref,s_name,"Type"), &type,	po_types, 1);
	PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Mass"), &mass, 0.1f, 10000.f);
	PHelper.CreateText			(values, FHelper.PrepareKey(pref,s_name,"Fixed bone"),	fixed_bone,	sizeof(fixed_bone));
    
	if (visual && PKinematics(visual))
	{
		CKinematics::accel		*ll_motions	= PKinematics(visual)->LL_Motions();
		CKinematics::accel::iterator _I, _E;
		AStringVec				vec;
		bool					bFound;
		// bones
		_I						= ll_motions->begin();
		_E						= ll_motions->end();
		bFound					= false;

		for (; _I!=_E; _I++) {
			vec.push_back(_I->first);
			if (!bFound && startup_animation[0] && (vec.back() == startup_animation))
				bFound=true;
		}

		if (!bFound)
			startup_animation[0]= 0;
		PropValue				*tNetPacket = PHelper.CreateList	(values,	FHelper.PrepareKey(pref,s_name,"Startup animation"), startup_animation, sizeof(startup_animation), &vec);
		tNetPacket->OnChangeEvent			= OnChangeAnim;
	}
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObjectHangingLamp
////////////////////////////////////////////////////////////////////////////
CSE_ALifeObjectHangingLamp::CSE_ALifeObjectHangingLamp(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_Abstract(caSection)
{
	flags.set					(flPhysic,TRUE);
    mass						= 10.f;
    startup_animation[0]		= 0;
	spot_texture[0]				= 0;
	color_animator[0]			= 0;
	spot_bone[0]				= 0;
	spot_range					= 10.f;
	spot_cone_angle				= PI_DIV_3;
	color						= 0xffffffff;
    spot_brightness				= 1.f;
}

CSE_ALifeObjectHangingLamp::~CSE_ALifeObjectHangingLamp()
{
}

void CSE_ALifeObjectHangingLamp::STATE_Read	(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion > 20)
		inherited::STATE_Read	(tNetPacket,size);

	if (m_wVersion < 32)
		visual_read				(tNetPacket);
	// model
	tNetPacket.r_u32			(color);
	tNetPacket.r_string			(color_animator);
	tNetPacket.r_string			(spot_texture);
	tNetPacket.r_string			(spot_bone);
	tNetPacket.r_float			(spot_range);
	tNetPacket.r_angle8			(spot_cone_angle);
    if (m_wVersion>10)
		tNetPacket.r_float		(spot_brightness);
    if (m_wVersion>11)
    	tNetPacket.r_u16		(flags.flags);
    if (m_wVersion>12)
    	tNetPacket.r_float		(mass);
    if (m_wVersion>17)
		tNetPacket.r_string		(startup_animation);

#ifdef _EDITOR    
	PlayAnimation				(startup_animation[0]?startup_animation:"$editor");
#endif

	// internal
	strlwr						(spot_bone);
}

void CSE_ALifeObjectHangingLamp::STATE_Write(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	// model
	tNetPacket.w_u32			(color);
	tNetPacket.w_string			(color_animator);
	tNetPacket.w_string			(spot_texture);
	tNetPacket.w_string			(spot_bone);
	tNetPacket.w_float			(spot_range);
	tNetPacket.w_angle8			(spot_cone_angle);
	tNetPacket.w_float			(spot_brightness);
   	tNetPacket.w_u16			(flags.flags);
	tNetPacket.w_float			(mass);
	tNetPacket.w_string			(startup_animation);
}

void CSE_ALifeObjectHangingLamp::UPDATE_Read(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeObjectHangingLamp::UPDATE_Write(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void __fastcall	CSE_ALifeObjectHangingLamp::OnChangeAnim(PropValue* sender)
{
	PlayAnimation				(startup_animation);
}

void CSE_ALifeObjectHangingLamp::FillProp	(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
	PHelper.CreateColor			(values, FHelper.PrepareKey(pref,s_name,"Color"),			&color);
	PHelper.CreateFlag16		(values, FHelper.PrepareKey(pref,s_name,"Physic"),			&flags,				flPhysic);
	PHelper.CreateLightAnim		(values, FHelper.PrepareKey(pref,s_name,"Color animator"),	color_animator,		sizeof(color_animator));
	PHelper.CreateTexture		(values, FHelper.PrepareKey(pref,s_name,"Texture"),			spot_texture,		sizeof(spot_texture));
	PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Range"),			&spot_range,		0.1f, 1000.f);
	PHelper.CreateAngle			(values, FHelper.PrepareKey(pref,s_name,"Angle"),			&spot_cone_angle,	0, deg2rad(120.f));
    PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Brightness"),		&spot_brightness,	0.1f, 5.f);
    PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Mass"),			&mass,				1.f, 1000.f);

    if (visual && PKinematics(visual))
    {
    	CKinematics::accel		*ll_bones	= PKinematics(visual)->LL_Bones();
    	CKinematics::accel		*ll_motions	= PKinematics(visual)->LL_Motions();
        CKinematics::accel::iterator _I, _E;
        AStringVec				vec;
        bool					bFound;
        // bones
        _I						= ll_motions->begin();
        _E						= ll_motions->end();
        bFound					= false;
        
		for (; _I!=_E; _I++) {
			vec.push_back(_I->first);
			if (!bFound && startup_animation[0] && (vec.back() == startup_animation))
				bFound=true;
		}
        
		if (!bFound)
			startup_animation[0]= 0;
        PropValue				*tNetPacket = PHelper.CreateList	(values,	FHelper.PrepareKey(pref,s_name,"Startup animation"), startup_animation, sizeof(startup_animation), &vec);
        tNetPacket->OnChangeEvent			= OnChangeAnim;
		// motions
        vec.clear				();
        _I						= ll_bones->begin();
        _E						= ll_bones->end();
        bFound					= false;
        for (; _I!=_E; _I++) {
			vec.push_back(_I->first);
			if (!bFound && spot_bone[0] && (vec.back() == spot_bone))
				bFound=true;
		}
        
		if (!bFound)
			spot_bone[0]		= 0;
		PHelper.CreateList		(values, FHelper.PrepareKey(pref,s_name,"Guide bone"),		spot_bone,	sizeof(spot_bone), &vec);
    }
}
#endif

CSE_ALifeSchedulable::CSE_ALifeSchedulable(LPCSTR caSection) : CSE_Abstract(caSection)
{
	m_tpCurrentBestWeapon		= 0;
#ifndef _EDITOR
#ifndef AI_COMPILER
	m_tpALife					= 0;
#endif
#endif
	m_tpBestDetector			= 0;
	m_qwUpdateCounter			= u64(-1);
}
