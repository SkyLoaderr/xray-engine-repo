////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Retsky and Dmitriy Iassenev
//	Description : Server objects for ALife simulator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma hdrstop

#include "xrServer_Objects_ALife.h"
#include "game_base.h"

#ifdef _EDITOR
	#include "BodyInstance.h"
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeGraphPoint
////////////////////////////////////////////////////////////////////////////
#ifdef _EDITOR
	#include "BodyInstance.h"
	static TokenValue4::ItemVec locations[4];
	static TokenValue4::ItemVec	level_ids;
#endif

CSE_ALifeGraphPoint::CSE_ALifeGraphPoint	(LPCSTR caSection) : CSE_Abstract(caSection)
{
	s_gameid					= GAME_DUMMY;
	m_caConnectionPointName[0]	= 0;
	m_tLevelID					= 0;
	m_tLocations[0]				= 0;
	m_tLocations[1]				= 0;
	m_tLocations[2]				= 0;
	m_tLocations[3]				= 0;
}

void CSE_ALifeGraphPoint::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	tNetPacket.r_string			(m_caConnectionPointName);
	tNetPacket.r_u32			(m_tLevelID);
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
	tNetPacket.w_u32			(m_tLevelID);
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
		R_ASSERT				(Ini->section_exist("levels"));
        LPCSTR					N,V;
        for (u32 k = 0; Ini->r_line("levels",k,&N,&V); k++) {
			R_ASSERT			(Ini->section_exist(N));
   			level_ids.push_back	(TokenValue4::Item());
            TokenValue4::Item	&val = level_ids.back();
			LPCSTR				S = Ini->r_string(N,"caption");
			val.str				= S;
            val.ID				= Ini->r_u32(N,"id");
        }
    }
    if (Ini)
		xr_delete				(Ini);
	
	PHelper.CreateToken4		(items,	PHelper.PrepareKey(pref,s_name,"Location\\1"),				(u32*)&m_tLocations[0],			&locations[0],					1);
	PHelper.CreateToken4		(items,	PHelper.PrepareKey(pref,s_name,"Location\\2"),				(u32*)&m_tLocations[1],			&locations[1],					1);
	PHelper.CreateToken4		(items,	PHelper.PrepareKey(pref,s_name,"Location\\3"),				(u32*)&m_tLocations[2],			&locations[2],					1);
	PHelper.CreateToken4		(items,	PHelper.PrepareKey(pref,s_name,"Location\\4"),				(u32*)&m_tLocations[3],			&locations[3],					1);
	PHelper.CreateToken4		(items,	PHelper.PrepareKey(pref,s_name,"Connection\\Level name"),	(u32*)&m_tLevelID,				&level_ids,						1);
	PHelper.CreateText			(items,	PHelper.PrepareKey(pref,s_name,"Connection\\Point name"),	m_caConnectionPointName,		sizeof(m_caConnectionPointName));
}
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
	tNetPacket.w				(&m_tBattleResult,	sizeof(m_tBattleResult));
	m_tpMonsterGroup1->UPDATE_Write(tNetPacket);
	m_tpMonsterGroup2->UPDATE_Write(tNetPacket);
};

void CSE_ALifeEvent::UPDATE_Read			(NET_Packet &tNetPacket)
{
	tNetPacket.r				(&m_tEventID,		sizeof(m_tEventID));
	tNetPacket.r				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.r				(&m_tGraphID,		sizeof(m_tGraphID));
	tNetPacket.r				(&m_tBattleResult,	sizeof(m_tBattleResult));
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
	save_base_vector			(m_tpItemIDs,		tNetPacket);
}

void CSE_ALifePersonalEvent::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	load_base_vector			(m_tpItemIDs,		tNetPacket);
}

void CSE_ALifePersonalEvent::UPDATE_Write	(NET_Packet &tNetPacket)
{
	tNetPacket.w				(&m_tEventID,		sizeof(m_tEventID));
	tNetPacket.w				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.w				(&m_tTaskID,		sizeof(m_tTaskID));
	tNetPacket.w_u32			(m_iHealth);
	tNetPacket.w				(&m_tRelation,		sizeof(m_tRelation));
};

void CSE_ALifePersonalEvent::UPDATE_Read	(NET_Packet &tNetPacket)
{
	tNetPacket.r				(&m_tEventID,		sizeof(m_tEventID));
	tNetPacket.r				(&m_tTimeID,		sizeof(m_tTimeID));
	tNetPacket.r				(&m_tTaskID,		sizeof(m_tTaskID));
	tNetPacket.r_s32			(m_iHealth);
	tNetPacket.r				(&m_tRelation,		sizeof(m_tRelation));
};

#ifdef _EDITOR
void CSE_ALifePersonalEvent::FillProp	(LPCSTR pref, PropItemVec& values)
{
}
#endif
////////////////////////////////////////////////////////////////////////////
// CSE_ALifeTask
////////////////////////////////////////////////////////////////////////////
void CSE_ALifeTask::STATE_Write				(NET_Packet &tNetPacket)
{
}

void CSE_ALifeTask::STATE_Read				(NET_Packet &tNetPacket, u16 size)
{
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
			tNetPacket.w		(&m_tClassID,		sizeof(m_tClassID));
			tNetPacket.w		(&m_tLocationID,	sizeof(m_tLocationID));
			break;
		}
		case eTaskTypeSearchForItemCG : {
			tNetPacket.w		(&m_tClassID,		sizeof(m_tClassID));
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
			tNetPacket.r		(&m_tClassID,	sizeof(m_tClassID));
			tNetPacket.r		(&m_tLocationID,sizeof(m_tLocationID));
			break;
		}
		case eTaskTypeSearchForItemCG : {
			tNetPacket.r		(&m_tClassID,	sizeof(m_tClassID));
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
// CSE_ALifePersonalTask
////////////////////////////////////////////////////////////////////////////
void CSE_ALifePersonalTask::STATE_Write		(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifePersonalTask::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CSE_ALifePersonalTask::UPDATE_Write	(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	tNetPacket.w_u32			(m_dwTryCount);
};

void CSE_ALifePersonalTask::UPDATE_Read		(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	tNetPacket.r_u32			(m_dwTryCount);
};

#ifdef _EDITOR
void CSE_ALifePersonalTask::FillProp	(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProp			(pref,values);
}
#endif
////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObject
////////////////////////////////////////////////////////////////////////////
void CSE_ALifeObject::STATE_Write			(NET_Packet &tNetPacket)
{
	tNetPacket.w_u8				(m_ucProbability);
	tNetPacket.w_u32			(m_dwSpawnGroup);
	tNetPacket.w				(&m_tGraphID,	sizeof(m_tGraphID));
	tNetPacket.w_float			(m_fDistance);
	tNetPacket.w_u32			(m_bDirectControl);
	tNetPacket.w_u32			(m_tNodeID);
}

void CSE_ALifeObject::STATE_Read			(NET_Packet &tNetPacket, u16 size)
{
	if (m_wVersion >= 1) {
		tNetPacket.r_u8			(m_ucProbability);
		tNetPacket.r_u32		(m_dwSpawnGroup);
		if (m_wVersion < 4) {
			u16					wDummy;
			tNetPacket.r_u16	(wDummy);
		}
		tNetPacket.r			(&m_tGraphID,	sizeof(m_tGraphID));
		tNetPacket.r_float		(m_fDistance);
		m_tObjectID				= ID;
	}
	if (m_wVersion >= 4) {
		u32						dwDummy;
		tNetPacket.r_u32		(dwDummy);
		m_bDirectControl		= !!dwDummy;
	}
	if (m_wVersion >= 8)
		tNetPacket.r_u32		(m_tNodeID);
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
	PHelper.CreateU8			(items,	PHelper.PrepareKey(pref, "ALife\\Probability"),		&m_ucProbability,	0,100);
	PHelper.CreateU32			(items,	PHelper.PrepareKey(pref, "ALife\\Spawn group ID"),	&m_dwSpawnGroup,	0,0xffffffff);
}
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
void CSE_ALifeDynamicObjectVisual::STATE_Write(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
}

void CSE_ALifeDynamicObjectVisual::STATE_Read(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
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
	inherited2::FillProp		(PHelper.PrepareKey(pref,s_name),items);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeAnomalousZone
////////////////////////////////////////////////////////////////////////////
CSE_ALifeAnomalousZone::CSE_ALifeAnomalousZone(LPCSTR caSection) : CSE_ALifeDynamicObject(caSection), CSE_Abstract(caSection)
{
	m_maxPower					= 100.f;
	m_attn						= 1.f;
	m_period					= 1000;
	m_fRadius					= 30.f;
	m_fGlobalProbability		= pSettings->r_float(caSection,"GlobalProbability");
	
	LPCSTR						l_caParameters = pSettings->r_string(caSection,"artefacts");
	m_wItemCount				= (u16)_GetItemCount(l_caParameters);
	R_ASSERT2					(!(m_wItemCount & 1),"Invalid number of parameters in string 'artefacts' in the 'system.ltx'!");
	m_wItemCount				>>= 1;
	
	m_dwaWeights				= (u32*)xr_malloc(m_wItemCount*sizeof(u32));
	m_cppArtefactSections		= (string64*)xr_malloc(m_wItemCount*sizeof(string64));
	string512					l_caBuffer;
	for (u32 i=0; i<m_wItemCount; i++) {
		m_dwaWeights[i]			= atoi(_GetItem(l_caParameters,i << 1,l_caBuffer));
		strcpy					(m_cppArtefactSections[i],_GetItem(l_caParameters,(i << 1) | 1,l_caBuffer));
	}
}

CSE_ALifeAnomalousZone::~CSE_ALifeAnomalousZone()
{
	xr_free						(m_dwaWeights);
	xr_free						(m_cppArtefactSections);
}

void CSE_ALifeAnomalousZone::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	// CForm
	if (m_wVersion >= 15)
		inherited1::STATE_Read	(tNetPacket,size);
	
	cform_read					(tNetPacket);

	tNetPacket.r_float			(m_maxPower);
	tNetPacket.r_float			(m_attn);
	tNetPacket.r_u32			(m_period);
	tNetPacket.r_float			(m_fRadius);
	tNetPacket.r_float			(m_fGlobalProbability);
	
	u16							l_wItemCount;
	tNetPacket.r_u16			(l_wItemCount);
	u32							*l_dwaWeights			= (u32*)xr_malloc(l_wItemCount*sizeof(u32));
	string64					*l_cppArtefactSections	= (string64*)xr_malloc(l_wItemCount*sizeof(string64));

	for (u16 i=0; i<l_wItemCount; i++) {
		tNetPacket.r_string		(l_cppArtefactSections[i]);
		tNetPacket.r_u32		(l_dwaWeights[i]);
	}

	for ( i=0; i<l_wItemCount; i++)
		for (u16 j=0; j<m_wItemCount; j++)
			if (!strstr(l_cppArtefactSections[i],m_cppArtefactSections[j])) {
				m_dwaWeights[j] = l_dwaWeights[i];
				break;
			}

	xr_free						(l_dwaWeights);
	xr_free						(l_cppArtefactSections);
}

void CSE_ALifeAnomalousZone::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	// CForm
	cform_write					(tNetPacket);

	tNetPacket.w_float			(m_maxPower);
	tNetPacket.w_float			(m_attn);
	tNetPacket.w_u32			(m_period);
	tNetPacket.w_float			(m_fRadius);
	tNetPacket.w_float			(m_fGlobalProbability);
	tNetPacket.w_u16			(m_wItemCount);
	for (u16 i=0; i<m_wItemCount; i++) {
		tNetPacket.w_string		(m_cppArtefactSections[i]);
		tNetPacket.w_u32		(m_dwaWeights[i]);
	}
}

void CSE_ALifeAnomalousZone::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeAnomalousZone::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
}

#ifdef _EDITOR
void CSE_ALifeAnomalousZone::FillProp		(LPCSTR pref, PropItemVec& items)
{
	inherited1::FillProp		(pref,items);
    PHelper.CreateFloat			(items,PHelper.PrepareKey(pref,s_name,"Power"),				&m_maxPower,0.f,1000.f);
    PHelper.CreateFloat			(items,PHelper.PrepareKey(pref,s_name,"Attenuation"),		&m_attn,0.f,100.f);
    PHelper.CreateU32			(items,PHelper.PrepareKey(pref,s_name,"Period"),			&m_period,20,10000);
    PHelper.CreateFloat			(items,PHelper.PrepareKey(pref,s_name,"Radius"),			&m_fRadius,0.f,100.f);
	string512					S;
	for (u32 i=0; i<m_wItemCount; i++) {
		strcpy					(S,s_name);
		strconcat				(S,"\\",m_cppArtefactSections[i]);
		PHelper.CreateU32		(items,PHelper.PrepareKey(pref,S,"Weight"),					m_dwaWeights + i,20,10000);
	}
    PHelper.CreateFloat			(items,PHelper.PrepareKey(pref,s_name,"Global probability"),&m_fGlobalProbability,0.f,1.f);
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeObjectPhysic
////////////////////////////////////////////////////////////////////////////
CSE_ALifeObjectPhysic::CSE_ALifeObjectPhysic(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection) , CSE_Abstract(caSection)
{
	type 						= epotBox;
	mass 						= 10.f;
    fixed_bone[0]				=0;
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
    	set_visual				(pSettings->r_string(caSection,"visual"));
}

CSE_ALifeObjectPhysic::~CSE_ALifeObjectPhysic		() 
{
}

void CSE_ALifeObjectPhysic::STATE_Read		(NET_Packet	&tNetPacket, u16 size) 
{
	if (m_wVersion >= 14)
		if (m_wVersion >= 16) {
			inherited::STATE_Read(tNetPacket,size);
			visual_read			(tNetPacket);
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
}

void CSE_ALifeObjectPhysic::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	visual_write				(tNetPacket);
	tNetPacket.w_u32			(type);
	tNetPacket.w_float			(mass);
	tNetPacket.w_string			(fixed_bone);
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

void CSE_ALifeObjectPhysic::FillProp		(LPCSTR pref, PropItemVec& values) {
	inherited::FillProp			(pref,	 values);
	PHelper.CreateToken			(values, PHelper.PrepareKey(pref,s_name,"Type"), &type,	po_types, 1);
	PHelper.CreateFloat			(values, PHelper.PrepareKey(pref,s_name,"Mass"), &mass, 0.1f, 10000.f);
	PHelper.CreateText			(values, PHelper.PrepareKey(pref,s_name,"Fixed bone"),	fixed_bone,	sizeof(fixed_bone));
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

	visual_read					(tNetPacket);
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
	visual_write				(tNetPacket);
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
	PHelper.CreateColor			(values, PHelper.PrepareKey(pref,s_name,"Color"),			&color);
	PHelper.CreateFlag16		(values, PHelper.PrepareKey(pref,s_name,"Physic"),			&flags,				flPhysic);
	PHelper.CreateLightAnim		(values, PHelper.PrepareKey(pref,s_name,"Color animator"),	color_animator,		sizeof(color_animator));
	PHelper.CreateTexture		(values, PHelper.PrepareKey(pref,s_name,"Texture"),			spot_texture,		sizeof(spot_texture));
	PHelper.CreateFloat			(values, PHelper.PrepareKey(pref,s_name,"Range"),			&spot_range,		0.1f, 1000.f);
	PHelper.CreateAngle			(values, PHelper.PrepareKey(pref,s_name,"Angle"),			&spot_cone_angle,	0, deg2rad(120.f));
    PHelper.CreateFloat			(values, PHelper.PrepareKey(pref,s_name,"Brightness"),		&spot_brightness,	0.1f, 5.f);
    PHelper.CreateFloat			(values, PHelper.PrepareKey(pref,s_name,"Mass"),			&mass,				1.f, 1000.f);

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
        PropValue				*tNetPacket = PHelper.CreateList	(values,	PHelper.PrepareKey(pref,s_name,"Startup animation"), startup_animation, &vec);
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
		PHelper.CreateList		(values, PHelper.PrepareKey(pref,s_name,"Guide bone"),		spot_bone,			&vec);
    }
}
#endif
