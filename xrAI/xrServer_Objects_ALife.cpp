////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects for ALife simulator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#ifndef XRGAME_EXPORTS
#	include "net_utils.h"
#	include "bone.h"
#else
	#include "..\bone.h"
#endif

#include "xrServer_Objects_ALife.h"
#include "game_base_space.h"
#include "object_broker.h"

#ifdef _EDITOR
	#include "SkeletonAnimated.h"
#endif
struct SFillPropData{
    RTokenVec 	locations[4];
    RStringVec	level_ids;
    RTokenVec 	story_names;
    u32			counter;
                SFillPropData	()
    {
        counter = 0;
    }
                ~SFillPropData	()
    {
        int y=0;
    }
    void		load			()
    {
        // create ini
        CInifile				*Ini = 0;
        string256				gm_name;
        FS.update_path			(gm_name,_game_data_,"game.ltx");
        R_ASSERT2				(FS.exist(gm_name),"Couldn't find file 'game.ltx'");
        Ini						= xr_new<CInifile>(gm_name);
        // location type
        LPCSTR					N,V;
        u32 					k;
        for (int i=0; i<LOCATION_TYPE_COUNT; ++i){
            VERIFY				(locations[i].empty());
            string256			caSection, T;
            strconcat			(caSection,SECTION_HEADER,itoa(i,T,10));
            R_ASSERT			(Ini->section_exist(caSection));
            for (k = 0; Ini->r_line(caSection,k,&N,&V); ++k)
                locations[i].push_back	(xr_rtoken(V,atoi(N)));
        }
        // level names/ids
        VERIFY					(level_ids.empty());
        for (k = 0; Ini->r_line("levels",k,&N,&V); ++k)
            level_ids.push_back	(*Ini->r_string_wb(N,"caption"));
        // story names
        VERIFY					(story_names.empty());
        LPCSTR section 			= "story_ids";
        R_ASSERT				(Ini->section_exist(section));
        for (k = 0; Ini->r_line(section,k,&N,&V); ++k)
            story_names.push_back	(xr_rtoken(V,atoi(N)));
        // destroy ini
        xr_delete				(Ini);
    }
    void		unload			()
    {
    }        
    void 		dec				()
    {
        VERIFY(counter > 0);
        --counter;
        if (!counter)
            unload	();
    }                           
    void 		inc				()
    {
        VERIFY(counter < 0xffffffff);
        if (!counter)
            load	();
        ++counter;
    }
};
static SFillPropData			fp_data;

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeGraphPoint
////////////////////////////////////////////////////////////////////////////
CSE_ALifeGraphPoint::CSE_ALifeGraphPoint	(LPCSTR caSection) : CSE_Abstract(caSection)
{
	s_gameid					= GAME_DUMMY;
	m_tLocations[0]				= 0;
	m_tLocations[1]				= 0;
	m_tLocations[2]				= 0;
	m_tLocations[3]				= 0;

    fp_data.inc					();
}

CSE_ALifeGraphPoint::~CSE_ALifeGraphPoint	()
{
    fp_data.dec					();
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
	PHelper().CreateRToken8		(items,	PHelper().PrepareKey(pref,s_name,"Location\\1"),				&m_tLocations[0],			&fp_data.locations[0]);
	PHelper().CreateRToken8		(items,	PHelper().PrepareKey(pref,s_name,"Location\\2"),				&m_tLocations[1],			&fp_data.locations[1]);
	PHelper().CreateRToken8		(items,	PHelper().PrepareKey(pref,s_name,"Location\\3"),				&m_tLocations[2],			&fp_data.locations[2]);
	PHelper().CreateRToken8		(items,	PHelper().PrepareKey(pref,s_name,"Location\\4"),				&m_tLocations[3],			&fp_data.locations[3]);

	PHelper().CreateList	 	(items,	PHelper().PrepareKey(pref,s_name,"Connection\\Level name"),		&m_caConnectionLevelName,	&fp_data.level_ids);
	PHelper().CreateRText	 	(items,	PHelper().PrepareKey(pref,s_name,"Connection\\Point name"),		&m_caConnectionPointName);
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
	m_story_id					= INVALID_STORY_ID;
#ifdef XRGAME_EXPORTS
	m_alife_simulator			= 0;
#endif
    fp_data.inc					();
}

#ifdef XRGAME_EXPORTS
CALifeSimulator	&CSE_ALifeObject::alife	() const
{
	VERIFY						(m_alife_simulator);
	return						(*m_alife_simulator);
}
#endif

CSE_ALifeObject::~CSE_ALifeObject			()
{
    fp_data.dec					();
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
	tNetPacket.w_string			(m_ini_string);
	tNetPacket.w				(&m_story_id,sizeof(m_story_id));
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
	
	if (m_wVersion > 57)
		tNetPacket.r_string		(m_ini_string);

	if (m_wVersion > 61)
		tNetPacket.r			(&m_story_id,sizeof(m_story_id));

	if (xr_strlen(m_ini_string)) {
#pragma warning(push)
#pragma warning(disable:4238)
		CInifile					ini(
			&IReader			(
			(void*)(*(m_ini_string)),
			m_ini_string.size()
			)
			);
#pragma warning(pop)

//		if (ini.section_exist("alife") && ini.line_exist("alife","interactive"))
//			m_flags.set			(flInteractive,ini.r_bool("alife","interactive"));
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
void CSE_ALifeObject::OnChooseGroupControl(ChooseItemVec& lst)
{
	LPCSTR gcs					= pSettings->r_string(s_name,"GroupControlSection");
    ObjectList objects;
    Scene->GetQueryObjects		(objects,OBJCLASS_SPAWNPOINT,-1,-1,-1);
    
    for (ObjectIt it=objects.begin(); it!=objects.end(); it++)
        if ((*it)->OnChooseQuery(gcs))	lst.push_back(SChooseItem((*it)->Name,""));
}
void CSE_ALifeObject::FillProp				(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref, 	items);
	PHelper().CreateRText		(items,	PHelper().PrepareKey(pref,s_name,"Custom data"),&m_ini_string);
	PHelper().CreateFloat		(items,	PHelper().PrepareKey(pref,s_name,"ALife\\Probability"),		&m_fProbability,	0,100);
    ChooseValue* V;
    V=PHelper().CreateChoose	(items, PHelper().PrepareKey(pref,s_name,"ALife\\Group control"),		&m_caGroupControl, smCustom);
    V->OnChooseFillEvent		= OnChooseGroupControl;
	if (m_flags.is(flUseSwitches)) {
		PHelper().CreateFlag32	(items,	PHelper().PrepareKey(pref,s_name,"ALife\\Can switch online"),	&m_flags,			flSwitchOnline);
		PHelper().CreateFlag32	(items,	PHelper().PrepareKey(pref,s_name,"ALife\\Can switch offline"),&m_flags,			flSwitchOffline);
	}                            
	PHelper().CreateFlag32		(items,	PHelper().PrepareKey(pref,s_name,"ALife\\Interactive"),		&m_flags,			flInteractive);
	PHelper().CreateFlag32		(items,	PHelper().PrepareKey(pref,s_name,"ALife\\Visible for AI"),	&m_flags,			flVisibleForAI);
	PHelper().CreateRToken32	(items,	PHelper().PrepareKey(pref,s_name,"ALife\\Story ID"),		&m_story_id,		&fp_data.story_names);
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

bool CSE_ALifeObject::interactive			() const
{
	return						(
		!!m_flags.is(flInteractive) &&
		!!m_flags.is(flVisibleForAI) &&
		!!m_flags.is(flUsefulForAI)
	);
}

void CSE_ALifeObject::can_switch_online		(bool value)
{
	m_flags.set					(flSwitchOnline,BOOL(value));
}

void CSE_ALifeObject::can_switch_offline	(bool value)
{
	m_flags.set					(flSwitchOffline,BOOL(value));
}

void CSE_ALifeObject::interactive			(bool value)
{
	m_flags.set					(flInteractive,BOOL(value));
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeGroupAbstract
////////////////////////////////////////////////////////////////////////////
CSE_ALifeGroupAbstract::CSE_ALifeGroupAbstract(LPCSTR caSection)
{
	m_tpMembers.clear			();
	m_bCreateSpawnPositions		= true;
	m_wCount					= 1;
	m_tNextBirthTime			= 0;
}

CSE_Abstract *CSE_ALifeGroupAbstract::init	()
{
	return						(base());
}

CSE_ALifeGroupAbstract::~CSE_ALifeGroupAbstract()
{
}

void CSE_ALifeGroupAbstract::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	u16 m_wVersion = base()->m_wVersion;
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
	PHelper().CreateU16			(items,	PHelper().PrepareKey(pref, "ALife\\Count"),			&m_wCount,			0,0xff);
};	
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeDynamicObject
////////////////////////////////////////////////////////////////////////////

CSE_ALifeDynamicObject::CSE_ALifeDynamicObject(LPCSTR caSection) : CSE_ALifeObject(caSection)
{
	m_tTimeID					= 0;
	m_switch_counter			= u64(-1);
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
CSE_ALifeDynamicObjectVisual::CSE_ALifeDynamicObjectVisual(LPCSTR caSection) : CSE_ALifeDynamicObject(caSection), CSE_Visual()
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
	inherited2::FillProp		(PHelper().PrepareKey(pref,s_name).c_str(),items);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifePHSkeletonObject
////////////////////////////////////////////////////////////////////////////
CSE_ALifePHSkeletonObject::CSE_ALifePHSkeletonObject(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection),CSE_PHSkeleton(caSection)
{
	m_flags.set					(flUseSwitches,FALSE);
	m_flags.set					(flSwitchOffline,FALSE);
}

CSE_ALifePHSkeletonObject::~CSE_ALifePHSkeletonObject()
{

}


void CSE_ALifePHSkeletonObject::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited1::STATE_Read(tNetPacket,size);
	if (m_wVersion>=64)
		inherited2::STATE_Read(tNetPacket,size);

}

void CSE_ALifePHSkeletonObject::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
}


void CSE_ALifePHSkeletonObject::load(NET_Packet &tNetPacket)
{
	inherited1::load				(tNetPacket);
	inherited2::load				(tNetPacket);
}
void CSE_ALifePHSkeletonObject::UPDATE_Write(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);
};

void CSE_ALifePHSkeletonObject::UPDATE_Read(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
};

bool CSE_ALifePHSkeletonObject::can_save			() const
{
	return						(!flags.test(flNotSave));
}

bool CSE_ALifePHSkeletonObject::used_ai_locations () const
{
	return false;
}

#ifdef _EDITOR
void CSE_ALifePHSkeletonObject::FillProp(LPCSTR pref, PropItemVec& items)
{
	inherited1::FillProp			(pref,items);
	inherited2::FillProp			(pref,items);
}
#endif


////////////////////////////////////////////////////////////////////////////
// CSE_ALifeScriptZone
////////////////////////////////////////////////////////////////////////////
CSE_ALifeScriptZone::CSE_ALifeScriptZone(LPCSTR caSection) : CSE_ALifeDynamicObject(caSection)
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
CSE_ALifeLevelChanger::CSE_ALifeLevelChanger(LPCSTR caSection) : CSE_ALifeScriptZone(caSection)
{
	m_tNextGraphID				= ALife::_GRAPH_ID(-1);
	m_dwNextNodeID				= u32(-1);
	m_tNextPosition.set			(0.f,0.f,0.f);
	m_tAngles.set				(0.f,0.f,0.f);
    fp_data.inc					();
}

CSE_ALifeLevelChanger::~CSE_ALifeLevelChanger()
{
    fp_data.dec					();
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
	
	PHelper().CreateList		(items,PHelper().PrepareKey(pref,s_name,"Level to change"),			&m_caLevelToChange,		&fp_data.level_ids);
	PHelper().CreateRText		(items,PHelper().PrepareKey(pref,s_name,"Level point to change"),	&m_caLevelPointToChange);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObjectPhysic
////////////////////////////////////////////////////////////////////////////
CSE_ALifeObjectPhysic::CSE_ALifeObjectPhysic(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection),CSE_PHSkeleton(caSection)
{
	type 						= epotBox;
	mass 						= 10.f;

	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
    	set_visual				(pSettings->r_string(caSection,"visual"));

	m_flags.set					(flUseSwitches,FALSE);
	m_flags.set					(flSwitchOffline,FALSE);

}

CSE_ALifeObjectPhysic::~CSE_ALifeObjectPhysic		() 
{
}

void CSE_ALifeObjectPhysic::STATE_Read		(NET_Packet	&tNetPacket, u16 size) 
{
	if (m_wVersion >= 14)
		if (m_wVersion >= 16) {
			inherited1::STATE_Read(tNetPacket,size);
			if (m_wVersion < 32)
				visual_read		(tNetPacket);
		}
		else {
			CSE_ALifeObject::STATE_Read(tNetPacket,size);
			visual_read			(tNetPacket);
		}

	if (m_wVersion>=64) inherited2::STATE_Read(tNetPacket,size);
		
	tNetPacket.r_u32			(type);
	tNetPacket.r_float			(mass);
    
	if (m_wVersion > 9)
		tNetPacket.r_string		(fixed_bones);

	if (m_wVersion<65&&m_wVersion > 28)
		tNetPacket.r_string		(startup_animation);

	if(m_wVersion<64)
		{
		if	(m_wVersion > 39)		// > 39 		
			tNetPacket.r_u8			(flags.flags);

		if (m_wVersion>56)
			tNetPacket.r_u16		(source_id);

		if (m_wVersion>60	&&	flags.test(flSavedData)) {
			data_load(tNetPacket);
		}
	}
#ifdef _EDITOR    
	PlayAnimation				(*startup_animation?*startup_animation:"$editor");
#endif
}




void CSE_ALifeObjectPhysic::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
	tNetPacket.w_u32			(type);
	tNetPacket.w_float			(mass);
	tNetPacket.w_string			(fixed_bones);

}



void CSE_ALifeObjectPhysic::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeObjectPhysic::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write		(tNetPacket);
	inherited2::UPDATE_Write		(tNetPacket);
}




void CSE_ALifeObjectPhysic::load(NET_Packet &tNetPacket)
{
	inherited1::load(tNetPacket);
	inherited2::load(tNetPacket);
}


#ifdef _EDITOR
xr_token po_types[]={
	{ "Box",			epotBox			},
	{ "Fixed chain",	epotFixedChain	},
	{ "Free chain",		epotFreeChain	},
	{ "Skeleton",		epotSkeleton	},
	{ 0,				0				}
};

void CSE_ALifeObjectPhysic::OnChangeAnim(PropValue* sender)
{
	PlayAnimation				(*startup_animation);
}

void CSE_ALifeObjectPhysic::OnChooseAnim(ChooseItemVec& lst)
{
    CSkeletonAnimated::accel_map *ll_motions	= PSkeletonAnimated(visual)->LL_Motions();
    CSkeletonAnimated::accel_map::iterator _I, _E;
    _I							= ll_motions->begin();
    _E							= ll_motions->end();
    for (; _I!=_E; ++_I) 		lst.push_back(SChooseItem(*_I->first,""));
}

void CSE_ALifeObjectPhysic::OnChooseBone(ChooseItemVec& lst)
{
    CSkeletonAnimated::accel  	*ll_bones	= PKinematics(visual)->LL_Bones();
    CSkeletonAnimated::accel::iterator _I, _E;
    _I							= ll_bones->begin();
    _E							= ll_bones->end();
    for (; _I!=_E; ++_I) 		lst.push_back(SChooseItem(*_I->first,""));
}

void CSE_ALifeObjectPhysic::FillProp		(LPCSTR pref, PropItemVec& values) {
	inherited1::FillProp		(pref,	 values);
	inherited2::FillProp		(pref,	 values);
	PHelper().CreateToken32		(values, PHelper().PrepareKey(pref,s_name,"Type"), &type,	po_types);
	PHelper().CreateFloat			(values, PHelper().PrepareKey(pref,s_name,"Mass"), &mass, 0.1f, 10000.f);
    PHelper().CreateFlag8			(values, PHelper().PrepareKey(pref,s_name,"Active"), &flags, flActive);

    // motions
    if (visual && PSkeletonAnimated(visual))
    {
        ChooseValue* V			= PHelper().CreateChoose	(values,	PHelper().PrepareKey(pref,s_name,"Startup animation"), &startup_animation, smCustom);
        V->OnChangeEvent		= OnChangeAnim;
        V->OnChooseFillEvent	= OnChooseAnim;
    }

    // bones
    if (visual && PKinematics(visual))
    {
	    ChooseValue* V 			= PHelper().CreateChoose	(values, 	PHelper().PrepareKey(pref,s_name,"Fixed bones"),		&fixed_bones, smCustom);
        V->OnChooseFillEvent	= OnChooseBone;
        V->Owner()->subitem		= 8;
    }
}
#endif

bool CSE_ALifeObjectPhysic::used_ai_locations	() const
{
	return						(false);
}

bool CSE_ALifeObjectPhysic::can_save			() const
{
	return						(!flags.test(flNotSave));
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObjectHangingLamp
////////////////////////////////////////////////////////////////////////////
CSE_ALifeObjectHangingLamp::CSE_ALifeObjectHangingLamp(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection),CSE_PHSkeleton(caSection)
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
		inherited1::STATE_Read	(tNetPacket,size);
	if (m_wVersion>=68)//69 hack!!!
		inherited2::STATE_Read	(tNetPacket,size);
#pragma todo("change 68 -> 69")
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
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);

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
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeObjectHangingLamp::UPDATE_Write(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write		(tNetPacket);
	inherited2::UPDATE_Write		(tNetPacket);

}

void CSE_ALifeObjectHangingLamp::load(NET_Packet &tNetPacket)
{
	inherited1::load(tNetPacket);
	inherited2::load(tNetPacket);
}
#ifdef _EDITOR
#include "ui_main.h"
void CSE_ALifeObjectHangingLamp::OnChangeAnim(PropValue* sender)
{
	PlayAnimation				(*startup_animation);
}

void CSE_ALifeObjectHangingLamp::OnChooseAnim(ChooseItemVec& lst)
{
    CSkeletonAnimated::accel_map *ll_motions	= PSkeletonAnimated(visual)->LL_Motions();
    CSkeletonAnimated::accel_map::iterator _I, _E;
    _I							= ll_motions->begin();
    _E							= ll_motions->end();
    for (; _I!=_E; ++_I) 		lst.push_back(SChooseItem(*_I->first,""));
}

void CSE_ALifeObjectHangingLamp::OnChooseBone(ChooseItemVec& lst)
{
    CSkeletonAnimated::accel  	*ll_bones	= PKinematics(visual)->LL_Bones();
    CSkeletonAnimated::accel::iterator _I, _E;
    _I							= ll_bones->begin();
    _E							= ll_bones->end();
    for (; _I!=_E; ++_I) 		lst.push_back(SChooseItem(*_I->first,""));
}
void CSE_ALifeObjectHangingLamp::OnChangeFlag(PropValue* sender)
{
	UI->Command					(COMMAND_UPDATE_PROPERTIES);
}

void CSE_ALifeObjectHangingLamp::FillProp	(LPCSTR pref, PropItemVec& values)
{
	inherited1::FillProp			(pref,values);
	inherited2::FillProp			(pref,values);

    PropValue* P				= 0;
	PHelper().CreateFlag16		(values, PHelper().PrepareKey(pref,s_name,"Flags\\Physic"),		&flags,			flPhysic);
	PHelper().CreateFlag16		(values, PHelper().PrepareKey(pref,s_name,"Flags\\Cast Shadow"),	&flags,			flCastShadow);
	PHelper().CreateFlag16		(values, PHelper().PrepareKey(pref,s_name,"Flags\\Allow R1"),		&flags,			flR1);
	PHelper().CreateFlag16		(values, PHelper().PrepareKey(pref,s_name,"Flags\\Allow R2"),		&flags,			flR2);
	P=PHelper().CreateFlag16		(values,PHelper().PrepareKey(pref,s_name,"Flags\\Allow Ambient"),&flags,			flPointAmbient);
    P->OnChangeEvent			= OnChangeFlag;
	// 
	P=PHelper().CreateFlag16		(values, PHelper().PrepareKey(pref,s_name,"Light\\Type"), 		&flags,				flTypeSpot, "Point", "Spot");
    P->OnChangeEvent			= OnChangeFlag;
	PHelper().CreateColor			(values, PHelper().PrepareKey(pref,s_name,"Light\\Color"),		&color);
    PHelper().CreateFloat			(values, PHelper().PrepareKey(pref,s_name,"Light\\Brightness"),	&brightness,		0.1f, 5.f);
	PHelper().CreateChoose		(values, PHelper().PrepareKey(pref,s_name,"Light\\Color Animator"),&color_animator, 	smLAnim);
	PHelper().CreateFloat			(values, PHelper().PrepareKey(pref,s_name,"Light\\Range"),		&range,				0.1f, 1000.f);
	PHelper().CreateFloat			(values, PHelper().PrepareKey(pref,s_name,"Light\\Virtual Size"),	&m_virtual_size,	0.f, 100.f);
	PHelper().CreateChoose		(values, PHelper().PrepareKey(pref,s_name,"Light\\Texture"),	    &light_texture, 	smTexture, "lights");

    if (flags.is(flTypeSpot))
		PHelper().CreateAngle		(values, PHelper().PrepareKey(pref,s_name,"Light\\Cone Angle"),	&spot_cone_angle,	deg2rad(1.f), deg2rad(120.f));

	// motions
    if (visual && PSkeletonAnimated(visual))
    {
        ChooseValue* V			= PHelper().CreateChoose	(values,	PHelper().PrepareKey(pref,s_name,"Visual\\Startup animation"), &startup_animation, smCustom);
        V->OnChangeEvent		= OnChangeAnim;
        V->OnChooseFillEvent	= OnChooseAnim;
    }

    // bones
    if (visual && PKinematics(visual))
    {
	    ChooseValue* V;
        V        				= PHelper().CreateChoose	(values, 	PHelper().PrepareKey(pref,s_name,"Visual\\Fixed bones"),		&fixed_bones, smCustom);
        V->OnChooseFillEvent	= OnChooseBone;
        V->Owner()->subitem		= 8;
        V        				= PHelper().CreateChoose	(values, 	PHelper().PrepareKey(pref,s_name,"Visual\\Guid bone"),		&guid_bone, smCustom);
        V->OnChooseFillEvent	= OnChooseBone;
    }
    if (flags.is(flPointAmbient)){
        PHelper().CreateFloat		(values, PHelper().PrepareKey(pref,s_name,"Ambient\\Radius"),		&m_ambient_radius,	0.f, 1000.f);
        PHelper().CreateFloat		(values, PHelper().PrepareKey(pref,s_name,"Ambient\\Power"),		&m_ambient_power);
		PHelper().CreateChoose	(values, PHelper().PrepareKey(pref,s_name,"Ambient\\Texture"),	&m_ambient_texture,	smTexture, 	"lights");
    }
    // glow
	PHelper().CreateFloat			(values, PHelper().PrepareKey(pref,s_name,"Glow\\Radius"),	    &glow_radius,		0.01f, 100.f);
	PHelper().CreateChoose		(values, PHelper().PrepareKey(pref,s_name,"Glow\\Texture"),	    &glow_texture, 		smTexture,	"glow");
	// game
	PHelper().CreateFloat			(values, PHelper().PrepareKey(pref,s_name,"Game\\Health"),		&m_health,			0.f, 100.f);
}
#endif

bool CSE_ALifeObjectHangingLamp::used_ai_locations	() const
{
	return						(false);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObjectSearchlight
////////////////////////////////////////////////////////////////////////////

CSE_ALifeObjectProjector::CSE_ALifeObjectProjector(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection)
{
	m_flags.set					(flUseSwitches,FALSE);
	m_flags.set					(flSwitchOffline,FALSE);
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

CSE_ALifeSchedulable::CSE_ALifeSchedulable	(LPCSTR caSection)
{
	m_tpCurrentBestWeapon		= 0;
	m_tpBestDetector			= 0;
	m_schedule_counter			= u64(-1);
}

CSE_ALifeSchedulable::~CSE_ALifeSchedulable	()
{
}

bool CSE_ALifeSchedulable::need_update		(CSE_ALifeDynamicObject *object)
{
	return						(!object || (object->m_bDirectControl && object->interactive() && object->used_ai_locations() && !object->m_bOnline));
}

CSE_Abstract *CSE_ALifeSchedulable::init	()
{
	return						(base());
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeHelicopter
////////////////////////////////////////////////////////////////////////////

CSE_ALifeHelicopter::CSE_ALifeHelicopter	(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_Motion(),CSE_PHSkeleton(caSection)
{
	m_flags.set					(flUseSwitches,		FALSE);
	m_flags.set					(flSwitchOffline,	FALSE);
	m_flags.set					(flInteractive,		FALSE);
}

CSE_ALifeHelicopter::~CSE_ALifeHelicopter	()
{
}

void CSE_ALifeHelicopter::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket,size);
    CSE_Motion::motion_read		(tNetPacket);
	if(m_wVersion>=68)// 69 !! hack!!!
		inherited3::STATE_Read		(tNetPacket,size);
#pragma todo("change 68 -> 69")
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
	inherited3::STATE_Write		(tNetPacket);
    tNetPacket.w_string			(startup_animation);
    tNetPacket.w_string			(engine_sound);
}

void CSE_ALifeHelicopter::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited3::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeHelicopter::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write		(tNetPacket);
	inherited3::UPDATE_Write		(tNetPacket);
}

void CSE_ALifeHelicopter::load		(NET_Packet &tNetPacket)
{
	inherited1::load(tNetPacket);
	inherited3::load(tNetPacket);
}
#ifdef _EDITOR
void CSE_ALifeHelicopter::OnChangeAnim(PropValue* sender)
{
	CSE_Visual::PlayAnimation	(*startup_animation);
}

void CSE_ALifeHelicopter::OnChooseAnim(ChooseItemVec& lst)
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
	inherited3::FillProp		(pref,	 values);
    // motions
    if (visual && PSkeletonAnimated(visual))
    {
        ChooseValue* V			= PHelper().CreateChoose	(values,	PHelper().PrepareKey(pref,s_name,"Startup Animation"), &startup_animation, smCustom);
        V->OnChangeEvent		= OnChangeAnim;
        V->OnChooseFillEvent	= OnChooseAnim;
    }

	CSE_Motion::FillProp		(PHelper().PrepareKey(pref,s_name).c_str(),	 values);
  
    PHelper().CreateChoose		(values,	PHelper().PrepareKey(pref,s_name,"Engine Sound"), &engine_sound, smSoundSource);

}
#endif

bool CSE_ALifeHelicopter::used_ai_locations	() const
{
	return						(false);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeCar
////////////////////////////////////////////////////////////////////////////
CSE_ALifeCar::CSE_ALifeCar				(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection),CSE_PHSkeleton(caSection)
{

	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
    	set_visual				(pSettings->r_string(caSection,"visual"));
	m_flags.set					(flUseSwitches,FALSE);
	m_flags.set					(flSwitchOffline,FALSE);
}

CSE_ALifeCar::~CSE_ALifeCar				()
{
}

void CSE_ALifeCar::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket,size);

	if(m_wVersion>65)
		inherited2::STATE_Read		(tNetPacket,size);
		if ((m_wVersion > 52) && (m_wVersion < 55))
		tNetPacket.r_float		();
}

void CSE_ALifeCar::STATE_Write			(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
}

void CSE_ALifeCar::UPDATE_Read			(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeCar::UPDATE_Write			(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write		(tNetPacket);
	inherited2::UPDATE_Write		(tNetPacket);
}

bool CSE_ALifeCar::used_ai_locations() const
{
	return						(false);
}

void CSE_ALifeCar::load(NET_Packet &tNetPacket)
{
	inherited1::load(tNetPacket);
	inherited2::load(tNetPacket);
}

void CSE_ALifeCar::data_load(NET_Packet	&tNetPacket)
{
	//inherited1::data_load(tNetPacket);
	inherited2::data_load(tNetPacket);
	VERIFY(door_states.empty());
	u16 doors_number=tNetPacket.r_u16();
	for(u16 i=0;i<doors_number;++i)
		door_states.push_back(tNetPacket.r_u8());
}
void CSE_ALifeCar::data_save(NET_Packet &tNetPacket)
{
	//inherited1::data_save(tNetPacket);
	inherited2::data_save(tNetPacket);
	
	tNetPacket.w_u16(u16(door_states.size()));
	xr_vector<u8>::iterator i=door_states.begin(),e=door_states.end();
	for(;e!=i;++i)
	{
		tNetPacket.w_u8(*i);
	}
	door_states.clear();
}	
#ifdef _EDITOR
void CSE_ALifeCar::FillProp				(LPCSTR pref, PropItemVec& values)
{
  	inherited1::FillProp			(pref,values);
	inherited2::FillProp			(pref,values);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObjectBreakable
////////////////////////////////////////////////////////////////////////////
CSE_ALifeObjectBreakable::CSE_ALifeObjectBreakable	(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection)
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
	PHelper().CreateFloat			(values, PHelper().PrepareKey(pref,s_name,"Health"),			&m_health,			0.f, 100.f);
}
#endif

bool CSE_ALifeObjectBreakable::used_ai_locations	() const
{
	return						(false);
}

bool CSE_ALifeObjectBreakable::can_switch_offline	() const
{
	return						(false);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeMountedWeapon
////////////////////////////////////////////////////////////////////////////
CSE_ALifeMountedWeapon::CSE_ALifeMountedWeapon	(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection)
{             
}

CSE_ALifeMountedWeapon::~CSE_ALifeMountedWeapon	()
{
}

void CSE_ALifeMountedWeapon::STATE_Read			(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CSE_ALifeMountedWeapon::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
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

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeTeamBaseZone
////////////////////////////////////////////////////////////////////////////
CSE_ALifeTeamBaseZone::CSE_ALifeTeamBaseZone(LPCSTR caSection) : CSE_ALifeScriptZone(caSection)
{
	m_team						= 0;
}

CSE_ALifeTeamBaseZone::~CSE_ALifeTeamBaseZone()
{
}

void CSE_ALifeTeamBaseZone::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
	tNetPacket.r_u8				(m_team);
}

void CSE_ALifeTeamBaseZone::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u8				(m_team);
}

void CSE_ALifeTeamBaseZone::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeTeamBaseZone::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeTeamBaseZone::FillProp		(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProp			(pref,items);
	PHelper().CreateU8			(items, PHelper().PrepareKey(pref,s_name,"team"),			&m_team,			0, 16);
}
#endif