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
	#include "bone.h"
#else
	#include "..\bone.h"
#endif

#include "xrServer_Objects_ALife.h"
#include "game_base.h"
#include "ai_alife_templates.h"

#ifdef _EDITOR
	#include "SkeletonAnimated.h"
	static TokenValue3Custom::ItemVec locations[4];
	static AStringVec	level_ids;
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeEvent
////////////////////////////////////////////////////////////////////////////
CSE_ALifeEvent::CSE_ALifeEvent				()
{
}

CSE_ALifeEvent::~CSE_ALifeEvent				()
{
}

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
CSE_ALifePersonalEvent::CSE_ALifePersonalEvent()
{
}

CSE_ALifePersonalEvent::~CSE_ALifePersonalEvent()
{
}

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

CSE_ALifeTask::~CSE_ALifeTask				()
{
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
		case ALife::eTaskTypeSearchForItemCL : {
			tNetPacket.w_string	(m_caSection);
			tNetPacket.w		(&m_tLocationID,	sizeof(m_tLocationID));
			break;
		}
		case ALife::eTaskTypeSearchForItemCG : {
			tNetPacket.w_string	(m_caSection);
			tNetPacket.w		(&m_tGraphID,		sizeof(m_tGraphID));
			break;
		}
		case ALife::eTaskTypeSearchForItemOL : {
			tNetPacket.w		(&m_tObjectID,		sizeof(m_tObjectID));
			tNetPacket.w		(&m_tLocationID,	sizeof(m_tLocationID));
			break;
		}
		case ALife::eTaskTypeSearchForItemOG : {
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
		case ALife::eTaskTypeSearchForItemCL : {
			tNetPacket.r_string	(m_caSection);
			tNetPacket.r		(&m_tLocationID,sizeof(m_tLocationID));
			break;
		}
		case ALife::eTaskTypeSearchForItemCG : {
			tNetPacket.r_string	(m_caSection);
			tNetPacket.r		(&m_tGraphID,	sizeof(m_tGraphID));
			break;
		}
		case ALife::eTaskTypeSearchForItemOL : {
			tNetPacket.r		(&m_tObjectID,	sizeof(m_tObjectID));
			tNetPacket.r		(&m_tLocationID,sizeof(m_tLocationID));
			break;
		}
		case ALife::eTaskTypeSearchForItemOG : {
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

CSE_ALifeGraphPoint::~CSE_ALifeGraphPoint	()
{
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

	for (int i=0; i<LOCATION_TYPE_COUNT; ++i)
		if(locations[i].empty()) {
			string256			caSection, T;
			strconcat			(caSection,SECTION_HEADER,itoa(i,T,10));
			R_ASSERT			(Ini->section_exist(caSection));
			LPCSTR				N,V;
			for (u32 k = 0; Ini->r_line(caSection,k,&N,&V); ++k)
				locations[i].push_back	(TokenValue3Custom::Item(atoi(N),V));
		}

	if(level_ids.empty()) {
		LPCSTR					N,V;
		for (u32 k = 0; Ini->r_line("levels",k,&N,&V); ++k)
			level_ids.push_back	(Ini->r_string(N,"caption"));
	}
	if (Ini)
		xr_delete				(Ini);

	PHelper.CreateToken3<u8>	(items,	FHelper.PrepareKey(pref,s_name,"Location\\1"),				&m_tLocations[0],			&locations[0]);
	PHelper.CreateToken3<u8>	(items,	FHelper.PrepareKey(pref,s_name,"Location\\2"),				&m_tLocations[1],			&locations[1]);
	PHelper.CreateToken3<u8>	(items,	FHelper.PrepareKey(pref,s_name,"Location\\3"),				&m_tLocations[2],			&locations[2]);
	PHelper.CreateToken3<u8>	(items,	FHelper.PrepareKey(pref,s_name,"Location\\4"),				&m_tLocations[3],			&locations[3]);

	PHelper.CreateList			(items,	FHelper.PrepareKey(pref,s_name,"Connection\\Level name"),	m_caConnectionLevelName,		sizeof(m_caConnectionLevelName),		&level_ids);
	PHelper.CreateText			(items,	FHelper.PrepareKey(pref,s_name,"Connection\\Point name"),	m_caConnectionPointName,		sizeof(m_caConnectionPointName));
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeGroupAbstract
////////////////////////////////////////////////////////////////////////////
CSE_ALifeGroupAbstract::CSE_ALifeGroupAbstract(LPCSTR caSection) : CSE_Abstract(caSection)
{
	m_tpMembers.clear			();
	m_bCreateSpawnPositions		= true;
	m_wCount					= 1;
	m_tNextBirthTime			= 0;
}

CSE_ALifeGroupAbstract::~CSE_ALifeGroupAbstract()
{
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObject
////////////////////////////////////////////////////////////////////////////
CSE_ALifeObject::CSE_ALifeObject			(LPCSTR caSection) : CSE_Abstract(caSection)
{
	m_bOnline					= false;
	m_fDistance					= 0.0f;
	m_tClassID					= TEXT2CLSID(pSettings->r_string(caSection,"class"));	
	ID							= ALife::_OBJECT_ID(-1);
	m_tGraphID					= ALife::_GRAPH_ID(-1);
	m_tGraphID					= ALife::_SPAWN_ID(-1);
	m_fProbability				= 1.f;
	m_dwSpawnGroup				= 0;
	m_bDirectControl			= true;
	m_bALifeControl				= true;
	m_tNodeID					= u32(-1);
	m_caGroupControl			= "";
	m_flags.one					();
}

CSE_ALifeObject::~CSE_ALifeObject			()
{
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
	tNetPacket.w_string			(*m_caGroupControl?*m_caGroupControl:"");
	tNetPacket.w_u32			(m_flags.get());
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
	if (m_wVersion > 23){
    	string256 tmp;
		tNetPacket.r_string		(tmp); m_caGroupControl=tmp;
    }
	if (m_wVersion > 49) {
		tNetPacket.r_u32		(m_flags.flags);
	}
}

void CSE_ALifeObject::UPDATE_Write			(NET_Packet &tNetPacket)
{
}

void CSE_ALifeObject::UPDATE_Read			(NET_Packet &tNetPacket)
{
};

#ifdef _EDITOR
#include "scene.h"
void __fastcall	CSE_ALifeObject::OnChooseGroupControl(PropValue* sender, ChooseItemVec& lst)
{
	LPCSTR gcs					= pSettings->r_string(s_name,"GroupControlSection");
    ObjectList objects;
    Scene.GetQueryObjects		(objects,OBJCLASS_SPAWNPOINT,-1,-1,-1);
    
    for (ObjectIt it=objects.begin(); it!=objects.end(); it++)
        if ((*it)->OnChooseQuery(gcs))	lst.push_back(SChooseItem((*it)->Name,""));
}
void CSE_ALifeObject::FillProp				(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp				(pref, items);
	PHelper.CreateFloat				(items,	FHelper.PrepareKey(pref,s_name,"ALife\\Probability"),		&m_fProbability,	0,100);
    RChooseValue* V;
    V=PHelper.CreateChoose			(items, FHelper.PrepareKey(pref,s_name,"ALife\\Group control"),		&m_caGroupControl, smCustom);
    V->OnChooseEvent				= OnChooseGroupControl;
	if (m_flags.is(flUseSwitches)) {
		PHelper.CreateFlag<Flags32>	(items,	FHelper.PrepareKey(pref,s_name,"ALife\\Can switch online"),	&m_flags,			flSwitchOnline);
		PHelper.CreateFlag<Flags32>	(items,	FHelper.PrepareKey(pref,s_name,"ALife\\Can switch offline"),&m_flags,			flSwitchOffline);
	}
}
#endif

bool CSE_ALifeObject::used_ai_locations		() const
{
	return						(true);
}

bool CSE_ALifeObject::can_switch_online		() const
{
	return						(!!m_flags.is(flSwitchOnline));
}

bool CSE_ALifeObject::can_switch_offline	() const
{
	return						(!!m_flags.is(flSwitchOffline));
}

bool CSE_ALifeObject::can_save				() const
{
	return						(true);
}

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
CSE_ALifeEventGroup::CSE_ALifeEventGroup(LPCSTR caSection) : CSE_ALifeObject(caSection), CSE_Abstract(caSection)
{
	m_wCountAfter				= m_wCountBefore;
}

CSE_ALifeEventGroup::~CSE_ALifeEventGroup()
{
}

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

CSE_ALifeDynamicObject::~CSE_ALifeDynamicObject()
{
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

CSE_ALifeDynamicObjectVisual::~CSE_ALifeDynamicObjectVisual()
{
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
// CSE_ALifeScriptZone
////////////////////////////////////////////////////////////////////////////
CSE_ALifeScriptZone::CSE_ALifeScriptZone(LPCSTR caSection) : CSE_ALifeDynamicObject(caSection), CSE_Abstract(caSection)
{
}

CSE_ALifeScriptZone::~CSE_ALifeScriptZone()
{
}

void CSE_ALifeScriptZone::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket,size);
	cform_read					(tNetPacket);
}

void CSE_ALifeScriptZone::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	cform_write					(tNetPacket);
}

void CSE_ALifeScriptZone::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeScriptZone::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeScriptZone::FillProp		(LPCSTR pref, PropItemVec& items)
{
	inherited1::FillProp		(pref,items);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeLevelChanger
////////////////////////////////////////////////////////////////////////////
CSE_ALifeLevelChanger::CSE_ALifeLevelChanger(LPCSTR caSection) : CSE_ALifeScriptZone(caSection), CSE_Abstract(caSection)
{
	m_tNextGraphID				= ALife::_GRAPH_ID(-1);
	m_dwNextNodeID				= u32(-1);
	m_tNextPosition.set			(0.f,0.f,0.f);
	m_tAngles.set				(0.f,0.f,0.f);
	m_caLevelToChange[0]		= 0;
}

CSE_ALifeLevelChanger::~CSE_ALifeLevelChanger()
{
}

void CSE_ALifeLevelChanger::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
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
		if (m_wVersion <= 53)
			m_tAngles.set		(0.f,tNetPacket.r_float(),0.f);
		else
			tNetPacket.r_vec3	(m_tAngles);
	}
	tNetPacket.r_string			(m_caLevelToChange);
	tNetPacket.r_string			(m_caLevelPointToChange);
}

void CSE_ALifeLevelChanger::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w				(&m_tNextGraphID,sizeof(m_tNextGraphID));
	tNetPacket.w_u32			(m_dwNextNodeID);
	tNetPacket.w_float			(m_tNextPosition.x);
	tNetPacket.w_float			(m_tNextPosition.y);
	tNetPacket.w_float			(m_tNextPosition.z);
	tNetPacket.w_vec3			(m_tAngles);
	tNetPacket.w_string			(m_caLevelToChange);
	tNetPacket.w_string			(m_caLevelPointToChange);
}

void CSE_ALifeLevelChanger::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeLevelChanger::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeLevelChanger::FillProp		(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref,items);
	
	CInifile					*Ini = 0;

	if (level_ids.empty()) {
		string256				gm_name;
		FS.update_path			(gm_name,_game_data_,"game.ltx");
		R_ASSERT2				(FS.exist(gm_name),"Couldn't find file 'game.ltx'");
		Ini						= xr_new<CInifile>(gm_name);
	}

    if(level_ids.empty()) {
        LPCSTR				N,V;
        for (u32 k = 0; Ini->r_line("levels",k,&N,&V); ++k)
            level_ids.push_back	(Ini->r_string(N,"caption"));
    }
	if (Ini)
		xr_delete				(Ini);
	
	PHelper.CreateList			(items,FHelper.PrepareKey(pref,s_name,"Level to change"),		m_caLevelToChange,	sizeof(m_caLevelToChange),	&level_ids);
	PHelper.CreateRText			(items,FHelper.PrepareKey(pref,s_name,"Level point to change"),	&m_caLevelPointToChange);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObjectPhysic
////////////////////////////////////////////////////////////////////////////
CSE_ALifeObjectPhysic::CSE_ALifeObjectPhysic(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_Abstract(caSection)
{
	type 						= epotBox;
	mass 						= 10.f;
	source_id					= u16(-1);
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
    	set_visual				(pSettings->r_string(caSection,"visual"));
    flags.zero					();
	m_flags.set					(flUseSwitches,false);
	m_flags.set					(flSwitchOffline,false);
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
		tNetPacket.r_string		(fixed_bones);

	if (m_wVersion > 28)
		tNetPacket.r_string		(startup_animation);

	if	(m_wVersion > 39)		// > 39 		
		tNetPacket.r_u8			(flags.flags);

	if (m_wVersion>56)
		tNetPacket.r_u16		(source_id);

#ifdef _EDITOR    
	PlayAnimation				(*startup_animation?*startup_animation:"$editor");
#endif
}

void CSE_ALifeObjectPhysic::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u32			(type);
	tNetPacket.w_float			(mass);
	tNetPacket.w_string			(fixed_bones);
	tNetPacket.w_string			(startup_animation);
	tNetPacket.w_u8				(flags.flags);
	tNetPacket.w_u16			(source_id);
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
	PlayAnimation				(*startup_animation);
}

void __fastcall	CSE_ALifeObjectPhysic::OnChooseAnim(PropValue* sender, ChooseItemVec& lst)
{
    CSkeletonAnimated::accel_map *ll_motions	= PSkeletonAnimated(visual)->LL_Motions();
    CSkeletonAnimated::accel_map::iterator _I, _E;
    _I							= ll_motions->begin();
    _E							= ll_motions->end();
    for (; _I!=_E; ++_I) 		lst.push_back(SChooseItem(*_I->first,""));
}

void __fastcall	CSE_ALifeObjectPhysic::OnChooseBone(PropValue* sender, ChooseItemVec& lst)
{
    CSkeletonAnimated::accel  	*ll_bones	= PKinematics(visual)->LL_Bones();
    CSkeletonAnimated::accel::iterator _I, _E;
    _I							= ll_bones->begin();
    _E							= ll_bones->end();
    for (; _I!=_E; ++_I) 		lst.push_back(SChooseItem(*_I->first,""));
}

void CSE_ALifeObjectPhysic::FillProp		(LPCSTR pref, PropItemVec& values) {
	inherited::FillProp			(pref,	 values);
	PHelper.CreateToken<u32>	(values, FHelper.PrepareKey(pref,s_name,"Type"), &type,	po_types);
	PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Mass"), &mass, 0.1f, 10000.f);
    PHelper.CreateFlag<Flags8>	(values, FHelper.PrepareKey(pref,s_name,"Active"), &flags, flActive);

    // motions
    if (visual && PSkeletonAnimated(visual))
    {
        RChooseValue* V			= PHelper.CreateChoose	(values,	FHelper.PrepareKey(pref,s_name,"Startup animation"), &startup_animation, smCustom);
        V->OnChangeEvent		= OnChangeAnim;
        V->OnChooseEvent		= OnChooseAnim;
    }

    // bones
    if (visual && PKinematics(visual))
    {
	    RChooseValue* V 		= PHelper.CreateChoose	(values, 	FHelper.PrepareKey(pref,s_name,"Fixed bones"),		&fixed_bones, smCustom);
        V->OnChooseEvent		= OnChooseBone;
        V->Owner()->subitem		= 8;
    }
}
#endif

bool CSE_ALifeObjectPhysic::used_ai_locations	() const
{
	return						(false);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObjectHangingLamp
////////////////////////////////////////////////////////////////////////////
CSE_ALifeObjectHangingLamp::CSE_ALifeObjectHangingLamp(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_Abstract(caSection)
{
	flags.set					(flPhysic,FALSE);
	flags.set					(flCastShadow,FALSE);
	flags.set					(flR1,FALSE);
	flags.set					(flR2,FALSE);
	flags.set					(flPointAmbient,FALSE);
	flags.set					(flTypeSpot,TRUE);

	range						= 10.f;
	color						= 0xffffffff;
    brightness					= 1.f;
	m_health					= 100.f;
	m_flags.set					(flUseSwitches,FALSE);
	m_flags.set					(flSwitchOffline,FALSE);

	m_virtual_size				= 0.1f;
	m_ambient_radius			= 10.f;
    m_ambient_power				= 0.1f;
    spot_cone_angle				= deg2rad(120.f);
    glow_radius					= 0.7f;
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

	if (m_wVersion < 49){
		ref_str s_tmp;
		float	f_tmp;
		// model
		tNetPacket.r_u32			(color);
		tNetPacket.r_string			(color_animator);
		tNetPacket.r_string			(s_tmp);
		tNetPacket.r_string			(s_tmp);
		tNetPacket.r_float			(range);
		tNetPacket.r_angle8			(f_tmp);
		if (m_wVersion>10)
			tNetPacket.r_float		(brightness);
		if (m_wVersion>11)
			tNetPacket.r_u16		(flags.flags);
		if (m_wVersion>12)
			tNetPacket.r_float		(f_tmp);
		if (m_wVersion>17)
			tNetPacket.r_string		(startup_animation);

#ifdef _EDITOR    
		PlayAnimation				(*startup_animation?*startup_animation:"$editor");
#endif

		if (m_wVersion > 42) {
			tNetPacket.r_string		(s_tmp);
			tNetPacket.r_float		(f_tmp);
		}

		if (m_wVersion > 43){
			tNetPacket.r_string		(fixed_bones);
		}

		if (m_wVersion > 44){
			tNetPacket.r_float		(m_health);
		}
	}else{
		// model
		tNetPacket.r_u32			(color);
		tNetPacket.r_float			(brightness);
		tNetPacket.r_string			(color_animator);
		tNetPacket.r_float			(range);
    	tNetPacket.r_u16			(flags.flags);
		tNetPacket.r_string			(startup_animation);
	#ifdef _EDITOR    
		PlayAnimation				(*startup_animation?*startup_animation:"$editor");
	#endif
		tNetPacket.r_string			(fixed_bones);
		tNetPacket.r_float			(m_health);
	}

	if (m_wVersion > 55)
	{
		tNetPacket.r_float			(m_virtual_size);
	    tNetPacket.r_float			(m_ambient_radius);
    	tNetPacket.r_float			(m_ambient_power);
	    tNetPacket.r_string			(m_ambient_texture);
        tNetPacket.r_string			(light_texture);
        tNetPacket.r_string			(guid_bone);
        tNetPacket.r_float			(spot_cone_angle);
        tNetPacket.r_string			(glow_texture);
        tNetPacket.r_float			(glow_radius);
	}
}

void CSE_ALifeObjectHangingLamp::STATE_Write(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	// model
	tNetPacket.w_u32			(color);
	tNetPacket.w_float			(brightness);
	tNetPacket.w_string			(color_animator);
	tNetPacket.w_float			(range);
   	tNetPacket.w_u16			(flags.flags);
	tNetPacket.w_string			(startup_animation);
    tNetPacket.w_string			(fixed_bones);
	tNetPacket.w_float			(m_health);
	tNetPacket.w_float			(m_virtual_size);
    tNetPacket.w_float			(m_ambient_radius);
    tNetPacket.w_float			(m_ambient_power);
    tNetPacket.w_string			(m_ambient_texture);

    tNetPacket.w_string			(light_texture);
    tNetPacket.w_string			(guid_bone);
    tNetPacket.w_float			(spot_cone_angle);
    tNetPacket.w_string			(glow_texture);
    tNetPacket.w_float			(glow_radius);
    
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
#include "ui_main.h"
void __fastcall	CSE_ALifeObjectHangingLamp::OnChangeAnim(PropValue* sender)
{
	PlayAnimation				(*startup_animation);
}

void __fastcall	CSE_ALifeObjectHangingLamp::OnChooseAnim(PropValue* sender, ChooseItemVec& lst)
{
    CSkeletonAnimated::accel_map *ll_motions	= PSkeletonAnimated(visual)->LL_Motions();
    CSkeletonAnimated::accel_map::iterator _I, _E;
    _I							= ll_motions->begin();
    _E							= ll_motions->end();
    for (; _I!=_E; ++_I) 		lst.push_back(SChooseItem(*_I->first,""));
}

void __fastcall	CSE_ALifeObjectHangingLamp::OnChooseBone(PropValue* sender, ChooseItemVec& lst)
{
    CSkeletonAnimated::accel  	*ll_bones	= PKinematics(visual)->LL_Bones();
    CSkeletonAnimated::accel::iterator _I, _E;
    _I							= ll_bones->begin();
    _E							= ll_bones->end();
    for (; _I!=_E; ++_I) 		lst.push_back(SChooseItem(*_I->first,""));
}
void __fastcall	CSE_ALifeObjectHangingLamp::OnChangeFlag(PropValue* sender)
{
	UI->Command					(COMMAND_UPDATE_PROPERTIES);
}

void CSE_ALifeObjectHangingLamp::FillProp	(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);

    PropValue* P				= 0;
	PHelper.CreateFlag<Flags16>	(values, FHelper.PrepareKey(pref,s_name,"Flags\\Physic"),		&flags,			flPhysic);
	PHelper.CreateFlag<Flags16>	(values, FHelper.PrepareKey(pref,s_name,"Flags\\Cast Shadow"),	&flags,			flCastShadow);
	PHelper.CreateFlag<Flags16>	(values, FHelper.PrepareKey(pref,s_name,"Flags\\Allow R1"),		&flags,			flR1);
	PHelper.CreateFlag<Flags16>	(values, FHelper.PrepareKey(pref,s_name,"Flags\\Allow R2"),		&flags,			flR2);
	P=PHelper.CreateFlag<Flags16>(values,FHelper.PrepareKey(pref,s_name,"Flags\\Allow Ambient"),&flags,			flPointAmbient);
    P->OnChangeEvent			= OnChangeFlag;
	// 
	P=PHelper.CreateFlag<Flags16>(values, FHelper.PrepareKey(pref,s_name,"Light\\Type"), 		&flags,				flTypeSpot, "Point", "Spot");
    P->OnChangeEvent			= OnChangeFlag;
	PHelper.CreateColor			(values, FHelper.PrepareKey(pref,s_name,"Light\\Color"),		&color);
    PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Light\\Brightness"),	&brightness,		0.1f, 5.f);
	PHelper.CreateChoose		(values, FHelper.PrepareKey(pref,s_name,"Light\\Color Animator"),&color_animator, 	smLAnim);
	PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Light\\Range"),		&range,				0.1f, 1000.f);
	PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Light\\Virtual Size"),	&m_virtual_size,	0.f, 100.f);
	PHelper.CreateChoose		(values, FHelper.PrepareKey(pref,s_name,"Light\\Texture"),	    &light_texture, 	smTexture, "lights");

    if (flags.is(flTypeSpot))
		PHelper.CreateAngle		(values, FHelper.PrepareKey(pref,s_name,"Light\\Cone Angle"),	&spot_cone_angle,	deg2rad(1.f), deg2rad(120.f));

	// motions
    if (visual && PSkeletonAnimated(visual))
    {
        RChooseValue* V			= PHelper.CreateChoose	(values,	FHelper.PrepareKey(pref,s_name,"Visual\\Startup animation"), &startup_animation, smCustom);
        V->OnChangeEvent		= OnChangeAnim;
        V->OnChooseEvent		= OnChooseAnim;
    }

    // bones
    if (visual && PKinematics(visual))
    {
	    RChooseValue* V;
        V        				= PHelper.CreateChoose	(values, 	FHelper.PrepareKey(pref,s_name,"Visual\\Fixed bones"),		&fixed_bones, smCustom);
        V->OnChooseEvent		= OnChooseBone;
        V->Owner()->subitem		= 8;
        V        				= PHelper.CreateChoose	(values, 	FHelper.PrepareKey(pref,s_name,"Visual\\Guid bone"),		&guid_bone, smCustom);
        V->OnChooseEvent		= OnChooseBone;
    }
    if (flags.is(flPointAmbient)){
        PHelper.CreateFloat		(values, FHelper.PrepareKey(pref,s_name,"Ambient\\Radius"),		&m_ambient_radius,	0.f, 1000.f);
        PHelper.CreateFloat		(values, FHelper.PrepareKey(pref,s_name,"Ambient\\Power"),		&m_ambient_power);
		PHelper.CreateChoose	(values, FHelper.PrepareKey(pref,s_name,"Ambient\\Texture"),	&m_ambient_texture,	smTexture, 	"lights");
    }
    // glow
	PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Glow\\Radius"),	    &glow_radius,		0.01f, 100.f);
	PHelper.CreateChoose		(values, FHelper.PrepareKey(pref,s_name,"Glow\\Texture"),	    &glow_texture, 		smTexture,	"glow");
	// game
	PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Game\\Health"),		&m_health,			0.f, 100.f);
}
#endif

bool CSE_ALifeObjectHangingLamp::used_ai_locations	() const
{
	return						(false);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObjectSearchlight
////////////////////////////////////////////////////////////////////////////

CSE_ALifeObjectProjector::CSE_ALifeObjectProjector(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_Abstract(caSection)
{
	m_flags.set					(flUseSwitches,false);
	m_flags.set					(flSwitchOffline,false);
}

CSE_ALifeObjectProjector::~CSE_ALifeObjectProjector()
{
}

void CSE_ALifeObjectProjector::STATE_Read	(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read	(tNetPacket,size);
}

void CSE_ALifeObjectProjector::STATE_Write(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeObjectProjector::UPDATE_Read(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeObjectProjector::UPDATE_Write(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeObjectProjector::FillProp			(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,	 values);
}
#endif

bool CSE_ALifeObjectProjector::used_ai_locations() const
{
	return						(false);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeSchedulable
////////////////////////////////////////////////////////////////////////////

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

CSE_ALifeSchedulable::~CSE_ALifeSchedulable()
{
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeHelicopter
////////////////////////////////////////////////////////////////////////////

CSE_ALifeHelicopter::CSE_ALifeHelicopter	(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_Abstract(caSection), CSE_Motion() 
{
	m_flags.set					(flUseSwitches,false);
	m_flags.set					(flSwitchOffline,false);
}

CSE_ALifeHelicopter::~CSE_ALifeHelicopter	()
{
}

void CSE_ALifeHelicopter::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket,size);
    CSE_Motion::motion_read		(tNetPacket);
    
    tNetPacket.r_string			(startup_animation);
	tNetPacket.r_string			(engine_sound);

#ifdef _EDITOR    
	CSE_Visual::PlayAnimation	(*startup_animation?*startup_animation:"$editor");
	CSE_Motion::PlayMotion		();
#endif
}

void CSE_ALifeHelicopter::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
    CSE_Motion::motion_write	(tNetPacket);

    tNetPacket.w_string			(startup_animation);
    tNetPacket.w_string			(engine_sound);
}

void CSE_ALifeHelicopter::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeHelicopter::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void __fastcall	CSE_ALifeHelicopter::OnChangeAnim(PropValue* sender)
{
	CSE_Visual::PlayAnimation	(*startup_animation);
}

void __fastcall	CSE_ALifeHelicopter::OnChooseAnim(PropValue* sender, ChooseItemVec& lst)
{
    CSkeletonAnimated::accel_map  	*ll_motions	= PSkeletonAnimated(visual)->LL_Motions();
    CSkeletonAnimated::accel_map::iterator _I, _E;
    _I							= ll_motions->begin();
    _E							= ll_motions->end();
    for (; _I!=_E; ++_I) 		lst.push_back(SChooseItem(*_I->first,""));
}
void CSE_ALifeHelicopter::FillProp(LPCSTR pref, PropItemVec& values)
{
	inherited1::FillProp		(pref,	 values);

    // motions
    if (visual && PSkeletonAnimated(visual))
    {
        RChooseValue* V			= PHelper.CreateChoose	(values,	FHelper.PrepareKey(pref,s_name,"Startup Animation"), &startup_animation, smCustom);
        V->OnChangeEvent		= OnChangeAnim;
        V->OnChooseEvent		= OnChooseAnim;
    }

	CSE_Motion::FillProp		(FHelper.PrepareKey(pref,s_name).c_str(),	 values);
    PHelper.CreateChoose		(values,	FHelper.PrepareKey(pref,s_name,"Engine Sound"), &engine_sound, smSoundSource);
}
#endif

bool CSE_ALifeHelicopter::used_ai_locations	() const
{
	return						(false);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeCar
////////////////////////////////////////////////////////////////////////////
CSE_ALifeCar::CSE_ALifeCar				(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_Abstract(caSection)
{
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
    	set_visual				(pSettings->r_string(caSection,"visual"));
	m_flags.set					(flUseSwitches,false);
	m_flags.set					(flSwitchOffline,false);
}

CSE_ALifeCar::~CSE_ALifeCar				()
{
}

void CSE_ALifeCar::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
	if ((m_wVersion > 52) && (m_wVersion < 55))
		tNetPacket.r_float		();
}

void CSE_ALifeCar::STATE_Write			(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeCar::UPDATE_Read			(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeCar::UPDATE_Write			(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

bool CSE_ALifeCar::used_ai_locations() const
{
	return						(false);
}

#ifdef _EDITOR
void CSE_ALifeCar::FillProp				(LPCSTR pref, PropItemVec& values)
{
  	inherited::FillProp			(pref,values);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObjectBreakable
////////////////////////////////////////////////////////////////////////////
CSE_ALifeObjectBreakable::CSE_ALifeObjectBreakable	(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_Abstract(caSection)
{
	m_health					= 100.f;
}

CSE_ALifeObjectBreakable::~CSE_ALifeObjectBreakable	()
{
}

void CSE_ALifeObjectBreakable::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
	tNetPacket.r_float			(m_health);
}

void CSE_ALifeObjectBreakable::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_float			(m_health);
}

void CSE_ALifeObjectBreakable::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeObjectBreakable::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeObjectBreakable::FillProp		(LPCSTR pref, PropItemVec& values)
{
  	inherited::FillProp			(pref,values);
	PHelper.CreateFloat			(values, FHelper.PrepareKey(pref,s_name,"Health"),			&m_health,			0.f, 100.f);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeMountedWeapon
////////////////////////////////////////////////////////////////////////////
CSE_ALifeMountedWeapon::CSE_ALifeMountedWeapon	(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_Abstract(caSection)
{
}

CSE_ALifeMountedWeapon::~CSE_ALifeMountedWeapon	()
{
}

void CSE_ALifeMountedWeapon::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
	tNetPacket.r_float			(m_health);
}

void CSE_ALifeMountedWeapon::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_float			(m_health);
}

void CSE_ALifeMountedWeapon::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeMountedWeapon::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeMountedWeapon::FillProp			(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
}
#endif
