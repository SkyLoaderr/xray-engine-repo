////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects monsters for ALife simulator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "net_utils.h"
#include "xrServer_Objects_ALife_Items.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "object_broker.h"
#include "alife_event_personal.h"

#ifndef AI_COMPILER
#	include "ai_space.h"
#endif

#ifdef XRGAME_EXPORTS
#	include "ef_storage.h"
#	include "character_info.h"
#	include "specific_character.h"
#endif


//////////////////////////////////////////////////////////////////////////

//возможное отклонение от значения репутации
//заданого в профиле и для конкретного персонажа
#define REPUTATION_DELTA	10
#define RANK_DELTA			10


//////////////////////////////////////////////////////////////////////////

using namespace ALife;

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeTraderAbstract
////////////////////////////////////////////////////////////////////////////
CSE_ALifeTraderAbstract::CSE_ALifeTraderAbstract(LPCSTR caSection)
{
	m_fCumulativeItemMass		= 0.f;
	m_iCumulativeItemVolume		= 0;
	m_dwMoney					= 0;
	if (pSettings->line_exist(caSection, "money"))
		m_dwMoney 				= pSettings->r_u32(caSection, "money");
	m_tRank						= ALife::EStalkerRank(pSettings->r_u32(caSection, "rank"));
	m_fMaxItemMass				= pSettings->r_float(caSection, "max_item_mass");
	m_tpEvents.clear			();

	m_iCharacterProfile			= NO_PROFILE;
	m_iSpecificCharacter		= NO_SPECIFIC_CHARACTER;
	m_character_profile_init	= false;
	m_trader_flags.zero			();
	m_trader_flags.set			(eTraderFlagInfiniteAmmo,FALSE);
}

CSE_Abstract *CSE_ALifeTraderAbstract::init	()
{
	string4096					S;
	sprintf						(S,"%s\r\n[game_info]\r\nname_id = default\r\n",!*base()->m_ini_string ? "" : *base()->m_ini_string);
	base()->m_ini_string		= S;
	m_character_profile_init	= false;

	return						(base());
}

CSE_ALifeTraderAbstract::~CSE_ALifeTraderAbstract()
{
	delete_data					(m_tpEvents);
}

void CSE_ALifeTraderAbstract::STATE_Write	(NET_Packet &tNetPacket)
{
	save_data					(m_tpEvents,tNetPacket);
	tNetPacket.w_u32			(m_dwMoney);
	tNetPacket.w_s32			(m_iSpecificCharacter);
	tNetPacket.w_u32			(m_trader_flags.get());
}

void CSE_ALifeTraderAbstract::STATE_Read	(NET_Packet &tNetPacket, u16 size)
{
	u16 m_wVersion = base()->m_wVersion;
	if (m_wVersion > 19) {
		load_data				(m_tpEvents,tNetPacket);
		ALife::TASK_VECTOR		l_tpTaskIDs;
		if (m_wVersion < 36)
			load_data			(l_tpTaskIDs,tNetPacket);
		if (m_wVersion > 62)
			tNetPacket.r_u32	(m_dwMoney);
		if (m_wVersion > 75)
			tNetPacket.r_s32	(m_iSpecificCharacter);
		if (m_wVersion > 77)
			m_trader_flags.assign(tNetPacket.r_u32());
	}
}

#ifdef XRGAME_EXPORTS
SPECIFIC_CHARACTER_INDEX CSE_ALifeTraderAbstract::specific_character()
{
	if(NO_SPECIFIC_CHARACTER != m_iSpecificCharacter) 
		return m_iSpecificCharacter;

	VERIFY(character_profile() != NO_PROFILE);
	CCharacterInfo char_info;
	char_info.Load(character_profile());

	//профиль задан индексом
	if(NO_SPECIFIC_CHARACTER != char_info.m_iSpecificCharacterIndex)
	{
		set_specific_character(char_info.m_iSpecificCharacterIndex);
		return m_iSpecificCharacter;
	}
	//профиль задан шаблоном
	//
	//проверяем все информации о персонаже, запоминаем подходящие,
	//а потом делаем случайный выбор
	else
	{	
		m_CheckedCharacters.clear();

		SPECIFIC_CHARACTER_INDEX team_default_index = NO_SPECIFIC_CHARACTER;
		for(SPECIFIC_CHARACTER_INDEX i=0; i<=CSpecificCharacter::GetMaxIndex(); i++)
		{
			CSpecificCharacter spec_char;
			spec_char.Load(i);

			if(spec_char.data()->m_bNoRandom) continue;

			if(char_info.data()->m_Community == NO_COMMUNITY || !xr_strcmp(spec_char.Community(), char_info.data()->m_Community))
			{
				//запомнить первый (если группировка явно не задана) подходящий персонаж с флажком m_bDefaultForCommunity
				if(team_default_index == NO_SPECIFIC_CHARACTER && spec_char.data()->m_bDefaultForCommunity)
					team_default_index = i;

				if(char_info.data()->m_Rank == NO_RANK || _abs(spec_char.Rank() - char_info.data()->m_Rank)<RANK_DELTA)
				{
					if(char_info.data()->m_Reputation == NO_REPUTATION || _abs(spec_char.Reputation() - char_info.data()->m_Reputation)<REPUTATION_DELTA)
					{
						int* count = NULL;
						if(ai().get_alife())
							count = ai().alife().registry(specific_characters).object(i, true);
						//если индекс еще не был использован
						if(NULL == count)
							m_CheckedCharacters.push_back(i);
					}
				}
			}
		}
		R_ASSERT3(NO_SPECIFIC_CHARACTER != team_default_index, 
			"no default spec character set for team", *char_info.data()->m_Community);


		if(m_CheckedCharacters.empty())
			char_info.m_iSpecificCharacterIndex = team_default_index;
		else
			char_info.m_iSpecificCharacterIndex = m_CheckedCharacters[Random.randI(m_CheckedCharacters.size())];

		set_specific_character(char_info.m_iSpecificCharacterIndex);
		return m_iSpecificCharacter;
	}
}

void CSE_ALifeTraderAbstract::set_specific_character	(SPECIFIC_CHARACTER_INDEX new_spec_char)
{
	R_ASSERT(new_spec_char != NO_SPECIFIC_CHARACTER);

	//убрать предыдущий номер из реестра
	if (NO_SPECIFIC_CHARACTER != m_iSpecificCharacter) 
	{
		if(ai().get_alife())
			ai().alife().registry(specific_characters).remove(m_iSpecificCharacter, true);
	}

	m_iSpecificCharacter = new_spec_char;

	if(ai().get_alife())
	{
		//запомнить, то что мы использовали индекс
		int a = 1;
		ai().alife().registry(specific_characters).add(m_iSpecificCharacter, a, true);
	}
}

void CSE_ALifeTraderAbstract::set_character_profile(PROFILE_INDEX new_profile)
{
	m_iCharacterProfile = new_profile;
}

PROFILE_INDEX CSE_ALifeTraderAbstract::character_profile()
{
	if(m_character_profile_init) return	m_iCharacterProfile;

	//запоминаем index profile
	if (xr_strlen(base()->m_ini_string)) {
#pragma warning(push)
#pragma warning(disable:4238)
		CInifile					ini(
			&IReader			(
			(void*)(*(base()->m_ini_string)),
			base()->m_ini_string.size()
			)
			);
#pragma warning(pop)

		LPCSTR profile_id = NULL;
		if (ini.section_exist("game_info") && ini.line_exist("game_info","name_id"))
			profile_id = ini.r_string("game_info", "name_id");

#ifdef XRGAME_EXPORTS
		if(NULL == profile_id)
			m_iCharacterProfile = NO_PROFILE;
		else
			m_iCharacterProfile = CCharacterInfo::IdToIndex(PROFILE_ID(profile_id));
#endif
	}

	m_character_profile_init = true;
	return	m_iCharacterProfile;
}
#endif

void CSE_ALifeTraderAbstract::UPDATE_Write	(NET_Packet &tNetPacket)
{
	tNetPacket.w_float			(m_fCumulativeItemMass);
//	tNetPacket.w_float			(m_iCumulativeItemVolume);
	tNetPacket.w_u32			(m_dwMoney);
	tNetPacket.w_u32			(m_tRank);
};

void CSE_ALifeTraderAbstract::UPDATE_Read	(NET_Packet &tNetPacket)
{
	tNetPacket.r_float			(m_fCumulativeItemMass);
//	tNetPacket.r_float			(m_iCumulativeItemVolume);
	tNetPacket.r_u32			(m_dwMoney);
	u32							dwDummy;
	tNetPacket.r_u32			(dwDummy);
	m_tRank						= ALife::EStalkerRank(m_tRank);
};

void CSE_ALifeTraderAbstract::FillProps	(LPCSTR pref, PropItemVec& items)
{
	PHelper().CreateU32			(items, PrepareKey(pref,base()->s_name,"Money"), 	&m_dwMoney,	0, u32(-1));
	PHelper().CreateFlag32		(items,	PrepareKey(pref,base()->s_name,"Trader\\Infinite ammo"),&m_trader_flags, eTraderFlagInfiniteAmmo);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeTrader
////////////////////////////////////////////////////////////////////////////

CSE_ALifeTrader::CSE_ALifeTrader			(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_ALifeTraderAbstract(caSection)
{
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
		set_visual				(pSettings->r_string(caSection,"visual"));
	
	m_tpSupplies.clear			();
	m_tOrgID					= 1;
}

CSE_ALifeTrader::~CSE_ALifeTrader			()
{
	delete_data					(m_tpOrderedArtefacts);
}

CSE_Abstract *CSE_ALifeTrader::init			()
{
	inherited1::init			();
	inherited2::init			();
	return						(base());
}

CSE_Abstract *CSE_ALifeTrader::base			()
{
	return						(inherited1::base());
}

const CSE_Abstract *CSE_ALifeTrader::base	() const
{
	return						(inherited1::base());
}

void CSE_ALifeTrader::STATE_Write			(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
	tNetPacket.w				(&m_tOrgID,sizeof(m_tOrgID));
	{
		tNetPacket.w_u32		(m_tpOrderedArtefacts.size());
		ALife::ARTEFACT_TRADER_ORDER_PAIR_IT	I = m_tpOrderedArtefacts.begin();
		ALife::ARTEFACT_TRADER_ORDER_PAIR_IT	E = m_tpOrderedArtefacts.end();
		for ( ; I != E; ++I) {
			tNetPacket.w_stringZ	((*I).second->m_caSection);
			tNetPacket.w_u32	((*I).second->m_dwTotalCount);
			save_data			((*I).second->m_tpOrders,tNetPacket);
		}
	}
	{
		tNetPacket.w_u32		(m_tpSupplies.size());
		ALife::TRADER_SUPPLY_IT		I = m_tpSupplies.begin();
		ALife::TRADER_SUPPLY_IT		E = m_tpSupplies.end();
		for ( ; I != E; ++I) {
			tNetPacket.w_stringZ	((*I).m_caSections);
			tNetPacket.w_u32	((*I).m_dwCount);
			tNetPacket.w_float	((*I).m_fMinFactor);
			tNetPacket.w_float	((*I).m_fMaxFactor);
		}
	}
}

void CSE_ALifeTrader::STATE_Read			(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
	inherited2::STATE_Read		(tNetPacket, size);
	if (m_wVersion > 35)
		tNetPacket.r			(&m_tOrgID,sizeof(m_tOrgID));
		
	if (m_wVersion > 29) {
		delete_data				(m_tpOrderedArtefacts);
		u32						l_dwCount	= tNetPacket.r_u32();
		for (int i=0 ; i<(int)l_dwCount; ++i) {
			ALife::SArtefactTraderOrder	*l_tpArtefactOrder = xr_new<ALife::SArtefactTraderOrder>();
			tNetPacket.r_stringZ(l_tpArtefactOrder->m_caSection);
			tNetPacket.r_u32	(l_tpArtefactOrder->m_dwTotalCount);
			load_data			(l_tpArtefactOrder->m_tpOrders,tNetPacket);
			m_tpOrderedArtefacts.insert(mk_pair(l_tpArtefactOrder->m_caSection,l_tpArtefactOrder));
		}
	}
	if (m_wVersion > 30) {
		m_tpSupplies.resize		(tNetPacket.r_u32());
		ALife::TRADER_SUPPLY_IT		I = m_tpSupplies.begin();
		ALife::TRADER_SUPPLY_IT		E = m_tpSupplies.end();
		for ( ; I != E; ++I) {
			tNetPacket.r_stringZ((*I).m_caSections);
			tNetPacket.r_u32	((*I).m_dwCount);
			tNetPacket.r_float	((*I).m_fMinFactor);
			tNetPacket.r_float	((*I).m_fMaxFactor);
		}
	}
}

void CSE_ALifeTrader::UPDATE_Write			(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);
};

void CSE_ALifeTrader::UPDATE_Read			(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
};

bool CSE_ALifeTrader::interactive			() const
{
	return						(false);
}

void CSE_ALifeTrader::OnSuppliesCountChange	(PropValue* sender)
{
    m_tpSupplies.resize			(supplies_count);
	set_editor_flag				(flUpdateProperties);
}

void CSE_ALifeTrader::FillProps				(LPCSTR _pref, PropItemVec& items)
{
	inherited1::FillProps		(_pref,items);
	inherited2::FillProps		(_pref,items);
	PHelper().CreateU32			(items, PrepareKey(_pref,s_name,"Organization ID"), 	&m_tOrgID,	0, 255);

	shared_str						S;
    shared_str	pref 				= PrepareKey(_pref,s_name,"ALife\\Supplies");

    supplies_count				= m_tpSupplies.size();
	PropValue					*V = PHelper().CreateS32(items, PrepareKey(pref.c_str(),"Count"), 	&supplies_count,	0, 64);
    V->OnChangeEvent.bind		(this,&CSE_ALifeTrader::OnSuppliesCountChange);
    
	TRADER_SUPPLY_IT			B = m_tpSupplies.begin(), I = B;
	TRADER_SUPPLY_IT			E = m_tpSupplies.end();
	for ( ; I != E; ++I) {
    	S.sprintf				("Slot #%d",I-B+1);
		V=PHelper().CreateChoose(items, PrepareKey(pref.c_str(),S.c_str(),"Sections"),&(*I).m_caSections, smEntityType,0,0,8);
		PHelper().CreateU32		(items, PrepareKey(pref.c_str(),S.c_str(),"Count"), 	&(*I).m_dwCount,	1, 256);
		PHelper().CreateFloat	(items, PrepareKey(pref.c_str(),S.c_str(),"Min Factor"),&(*I).m_fMinFactor,0.f, 1.f);
		PHelper().CreateFloat	(items, PrepareKey(pref.c_str(),S.c_str(),"Max Factor"),&(*I).m_fMaxFactor,0.f, 1.f);
	}
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeCustomZone
////////////////////////////////////////////////////////////////////////////
CSE_ALifeCustomZone::CSE_ALifeCustomZone	(LPCSTR caSection) : CSE_ALifeSpaceRestrictor(caSection)
{
	m_maxPower					= 100.f;
	m_attn						= 1.f;
	m_period					= 1000;
	m_tAnomalyType				= ALife::eAnomalousZoneTypeGravi;
	m_maxPower					= 0.f;
	if (pSettings->line_exist(caSection,"hit_type"))
		m_tHitType				= ALife::g_tfString2HitType(pSettings->r_string(caSection,"hit_type"));
	else
		m_tHitType				= ALife::eHitTypeMax;
}

CSE_ALifeCustomZone::~CSE_ALifeCustomZone	()
{
}

void CSE_ALifeCustomZone::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
	tNetPacket.r_float			(m_maxPower);
	tNetPacket.r_float			(m_attn);
	tNetPacket.r_u32			(m_period);
	if (m_wVersion > 66) {
		u32						l_dwDummy;
		tNetPacket.r_u32		(l_dwDummy);
		m_tAnomalyType			= ALife::EAnomalousZoneType(l_dwDummy);
	}
}

void CSE_ALifeCustomZone::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_float			(m_maxPower);
	tNetPacket.w_float			(m_attn);
	tNetPacket.w_u32			(m_period);
	tNetPacket.w_u32			(m_tAnomalyType);
}

void CSE_ALifeCustomZone::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeCustomZone::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

//xr_token TokenAnomalyType[]={
//	{ "Gravi",			eAnomalousZoneTypeGravi			},
//	{ "Fog",			eAnomalousZoneTypeFog			},
//	{ "Radioactive",	eAnomalousZoneTypeRadio			},
//	{ "Plant",			eAnomalousZoneTypePlant			},
//	{ "Gelatine",		eAnomalousZoneTypeGelatine		},
//	{ "Fluff",			eAnomalousZoneTypeFluff			},
//	{ "Rusty Hair",		eAnomalousZoneTypeRustyHair		},
//	{ "RustyWhistlers",	eAnomalousZoneTypeRustyWhistlers},
//	{ 0,				0}
//};

void CSE_ALifeCustomZone::FillProps		(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProps		(pref,items);
//	PHelper().CreateToken<u8>	(items,PrepareKey(pref,s_name,"Type"),			(u8*)&m_tAnomalyType,	TokenAnomalyType);
	PHelper().CreateFloat		(items,PrepareKey(pref,s_name,"Power"),			&m_maxPower,0.f,1000.f);
	PHelper().CreateFloat		(items,PrepareKey(pref,s_name,"Attenuation"),	&m_attn,0.f,100.f);
	PHelper().CreateU32			(items,PrepareKey(pref,s_name,"Period"),		&m_period,20,10000);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeAnomalousZone
////////////////////////////////////////////////////////////////////////////
CSE_ALifeAnomalousZone::CSE_ALifeAnomalousZone(LPCSTR caSection) : CSE_ALifeSchedulable(caSection), CSE_ALifeCustomZone(caSection)
{
	m_fRadius					= 30.f;
	m_fBirthProbability			= pSettings->r_float(caSection,"BirthProbability");

	LPCSTR						l_caParameters = pSettings->r_string(caSection,"artefacts");
	m_wItemCount				= (u16)_GetItemCount(l_caParameters);
	R_ASSERT2					(!(m_wItemCount & 1),"Invalid number of parameters in string 'artefacts' in the 'system.ltx'!");
	m_wItemCount				>>= 1;

	m_faWeights					= (float*)xr_malloc(m_wItemCount*sizeof(float));
	m_cppArtefactSections		= (string64*)xr_malloc(m_wItemCount*sizeof(string64));
	string512					l_caBuffer;
	for (u16 i=0; i<m_wItemCount; ++i) {
		strcpy					(m_cppArtefactSections[i],_GetItem(l_caParameters,i << 1,l_caBuffer));
		m_faWeights[i]			= (float)atof(_GetItem(l_caParameters,(i << 1) | 1,l_caBuffer));
	}
	m_wArtefactSpawnCount		= 32;
	m_fStartPower				= 0.f;
}

CSE_Abstract *CSE_ALifeAnomalousZone::init			()
{
	inherited1::init			();
	inherited2::init			();
	return						(base());
}

CSE_Abstract *CSE_ALifeAnomalousZone::base			()
{
	return						(inherited1::base());
}

const CSE_Abstract *CSE_ALifeAnomalousZone::base	() const
{
	return						(inherited1::base());
}


CSE_ALifeAnomalousZone::~CSE_ALifeAnomalousZone()
{
	xr_free						(m_faWeights);
	xr_free						(m_cppArtefactSections);
}

void CSE_ALifeAnomalousZone::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket,size);

	if (m_wVersion > 21) {
		tNetPacket.r_float		(m_fRadius);
		tNetPacket.r_float		(m_fBirthProbability);
		u16						l_wItemCount;
		tNetPacket.r_u16		(l_wItemCount);
		float					*l_faWeights			= (float*)xr_malloc(l_wItemCount*sizeof(float));
		string64				*l_cppArtefactSections	= (string64*)xr_malloc(l_wItemCount*sizeof(string64));

		for (u16 i=0; i<l_wItemCount; ++i) {
			tNetPacket.r_stringZ(l_cppArtefactSections[i]);
			if (m_wVersion > 26)
				tNetPacket.r_float	(l_faWeights[i]);
			else {
				u32					l_dwValue;
				tNetPacket.r_u32	(l_dwValue);
				l_faWeights[i]		= float(l_dwValue);
			}
		}

		for ( i=0; i<l_wItemCount; ++i)
			for (u16 j=0; j<m_wItemCount; ++j)
				if (!strstr(l_cppArtefactSections[i],m_cppArtefactSections[j])) {
					m_faWeights[j] = l_faWeights[i];
					break;
				}

				xr_free					(l_faWeights);
				xr_free					(l_cppArtefactSections);
	}
	
	if (m_wVersion > 25) {
		tNetPacket.r_u16		(m_wArtefactSpawnCount);
		tNetPacket.r_u32		(m_dwStartIndex);
	}

	if ((m_wVersion < 67) && (m_wVersion > 27)) {
		u32						l_dwDummy;
		tNetPacket.r_u32		(l_dwDummy);
		m_tAnomalyType			= ALife::EAnomalousZoneType(l_dwDummy);
	}

	if (m_wVersion > 38)
		tNetPacket.r_float		(m_fStartPower);
}

void CSE_ALifeAnomalousZone::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	tNetPacket.w_float			(m_fRadius);
	tNetPacket.w_float			(m_fBirthProbability);
	tNetPacket.w_u16			(m_wItemCount);
	for (u16 i=0; i<m_wItemCount; ++i) {
		tNetPacket.w_stringZ		(m_cppArtefactSections[i]);
		tNetPacket.w_float		(m_faWeights[i]);
	}
	tNetPacket.w_u16			(m_wArtefactSpawnCount);
	tNetPacket.w_u32			(m_dwStartIndex);
	tNetPacket.w_float			(m_fStartPower);
}

void CSE_ALifeAnomalousZone::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeAnomalousZone::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
}

void CSE_ALifeAnomalousZone::FillProps		(LPCSTR pref, PropItemVec& items)
{
	inherited1::FillProps		(pref,items);
	PHelper().CreateFloat			(items,PrepareKey(pref,s_name,"Radius"),							&m_fRadius,0.f,100.f);
	for (u16 i=0; i<m_wItemCount; ++i)
		PHelper().CreateFloat		(items,PrepareKey(pref,s_name,"ALife\\Artefact Weights",			m_cppArtefactSections[i]), m_faWeights + i,0.f,1.f);
	PHelper().CreateFloat			(items,PrepareKey(pref,s_name,"ALife\\Artefact birth probability"),	&m_fBirthProbability,0.f,1.f);
	PHelper().CreateU16			(items,PrepareKey(pref,s_name,"ALife\\Artefact spawn places count"),&m_wArtefactSpawnCount,32,256);
}

bool CSE_ALifeAnomalousZone::need_update	(CSE_ALifeDynamicObject *object)
{
	return						(CSE_ALifeSchedulable::need_update(object) && (m_maxPower > EPS_L));
}

//////////////////////////////////////////////////////////////////////////
//SE_ALifeTorridZone
//////////////////////////////////////////////////////////////////////////
CSE_ALifeTorridZone::CSE_ALifeTorridZone	(LPCSTR caSection)
:CSE_ALifeCustomZone(caSection),CSE_Motion()
{
}

CSE_ALifeTorridZone::~CSE_ALifeTorridZone	()
{
}

CSE_Motion* CSE_ALifeTorridZone::motion		()
{
	return						(this);
}

void CSE_ALifeTorridZone::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket,size);
	CSE_Motion::motion_read		(tNetPacket);
	set_editor_flag				(flMotionChange);
}

void CSE_ALifeTorridZone::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	CSE_Motion::motion_write	(tNetPacket);

}

void CSE_ALifeTorridZone::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeTorridZone::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
}

void CSE_ALifeTorridZone::FillProps(LPCSTR pref, PropItemVec& values)
{
	inherited1::FillProps		(pref, values);
	inherited2::FillProps		(pref, values);
}

//-------------------------------------------------------------------------
//
//void CSE_ALifeAnomalousZoneMoving::UPDATE_Write(NET_Packet &tNetPacket)
//{
//	inherited::UPDATE_Write		(tNetPacket);
//
//	tNetPacket.w_u32			(timestamp		);
//	tNetPacket.w_vec3			(o_Position		);
//	tNetPacket.w_vec3			(o_Angle		);	
//	tNetPacket.w_u32			(m_tNodeID		);
//};
//
//void CSE_ALifeAnomalousZoneMoving::UPDATE_Read	(NET_Packet &tNetPacket)
//{
//	inherited::UPDATE_Read		(tNetPacket);
//
//	tNetPacket.w_u32			(timestamp		);
//	tNetPacket.w_vec3			(o_Position		);
//	tNetPacket.w_vec3			(o_Angle		);	
//	tNetPacket.w_u32			(m_tNodeID		);
//};
//
//-------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeCreatureAbstract
////////////////////////////////////////////////////////////////////////////
CSE_ALifeCreatureAbstract::CSE_ALifeCreatureAbstract(LPCSTR caSection)	: CSE_ALifeDynamicObjectVisual(caSection)
{
	s_team = s_squad = s_group	= 0;
	fHealth						= 100;
	m_bDeathIsProcessed			= false;
	m_fAccuracy					= 25.f;
	m_fIntelligence				= 25.f;
	m_fMorale					= 100.f;
}

CSE_ALifeCreatureAbstract::~CSE_ALifeCreatureAbstract()
{
}

void CSE_ALifeCreatureAbstract::STATE_Write	(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u8				(s_team	);
	tNetPacket.w_u8				(s_squad);
	tNetPacket.w_u8				(s_group);
	tNetPacket.w_float			(fHealth);
}

void CSE_ALifeCreatureAbstract::STATE_Read	(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
	tNetPacket.r_u8				(s_team	);
	tNetPacket.r_u8				(s_squad);
	tNetPacket.r_u8				(s_group);
	if (m_wVersion > 18)
		tNetPacket.r_float		(fHealth);
	if (m_wVersion < 32)
		visual_read				(tNetPacket);
	o_model						= o_torso.yaw;
}

void CSE_ALifeCreatureAbstract::UPDATE_Write(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	
	tNetPacket.w_float_q16		(fHealth,	-500,1000);
	
	tNetPacket.w_u32			(timestamp		);
	tNetPacket.w_u8				(flags			);
	tNetPacket.w_vec3			(o_Position		);
	tNetPacket.w_angle8			(o_model		);
	tNetPacket.w_angle8			(o_torso.yaw	);
	tNetPacket.w_angle8			(o_torso.pitch	);
	tNetPacket.w_u8				(s_team);
	tNetPacket.w_u8				(s_squad);
	tNetPacket.w_u8				(s_group);
};

void CSE_ALifeCreatureAbstract::UPDATE_Read	(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	
	tNetPacket.r_float_q16		(fHealth,	-500,1000);
	
	tNetPacket.r_u32			(timestamp		);
	tNetPacket.r_u8				(flags			);
	tNetPacket.r_vec3			(o_Position		);
	tNetPacket.r_angle8			(o_model		);
	tNetPacket.r_angle8			(o_torso.yaw	);
	tNetPacket.r_angle8			(o_torso.pitch	);
	tNetPacket.r_u8				(s_team);
	tNetPacket.r_u8				(s_squad);
	tNetPacket.r_u8				(s_group);
};

u8 CSE_ALifeCreatureAbstract::g_team		()
{
	return s_team;
}

u8 CSE_ALifeCreatureAbstract::g_squad		()
{
	return s_squad;
}

u8 CSE_ALifeCreatureAbstract::g_group		()
{
	return s_group;
}

void CSE_ALifeCreatureAbstract::FillProps	(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProps			(pref,items);
    PHelper().CreateU8			(items,PrepareKey(pref,s_name, "Team"),		&s_team, 	0,64,1);
    PHelper().CreateU8			(items,PrepareKey(pref,s_name, "Squad"),	&s_squad, 	0,64,1);
    PHelper().CreateU8			(items,PrepareKey(pref,s_name, "Group"),	&s_group, 	0,64,1);
   	PHelper().CreateFloat			(items,PrepareKey(pref,s_name,"Personal",	"Health" 				),&fHealth,							0,200,5);
}

bool CSE_ALifeCreatureAbstract::used_ai_locations	() const
{
	return						(true);
}

bool CSE_ALifeCreatureAbstract::can_switch_online	() const
{
	return						(inherited::can_switch_online());
}

bool CSE_ALifeCreatureAbstract::can_switch_offline	() const
{
	return						(inherited::can_switch_offline() && (g_Health() > 0.f));
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeMonsterAbstract
////////////////////////////////////////////////////////////////////////////
CSE_ALifeMonsterAbstract::CSE_ALifeMonsterAbstract(LPCSTR caSection)	: CSE_ALifeCreatureAbstract(caSection), CSE_ALifeSchedulable(caSection)
{
	m_tNextGraphID				= m_tGraphID;
	m_tPrevGraphID				= m_tGraphID;
	m_fCurSpeed					= 0.0f;
	m_fDistanceFromPoint		= 0.0f;
	m_fDistanceToPoint			= 0.0f;
	m_tpaTerrain.clear			();

	m_fMaxHealthValue	 		= pSettings->r_float	(caSection,"MaxHealthValue");
	if (pSettings->line_exist(caSection,"hit_power")) {
		m_fHitPower				= pSettings->r_float(caSection,"hit_power");
		m_tHitType				= ALife::g_tfString2HitType(pSettings->r_string(caSection,"hit_type"));
	}
	else {
		m_fHitPower				= 0;
		m_tHitType				= ALife::eHitTypeMax;
	}

	{
		string64					S;
		m_fpImmunityFactors.resize	(ALife::eHitTypeMax);
		svector<float,ALife::eHitTypeMax>::iterator	B = m_fpImmunityFactors.begin(), I = B;
		svector<float,ALife::eHitTypeMax>::iterator	E = m_fpImmunityFactors.end();
		for ( ; I != E; ++I)
			*I					= pSettings->r_float(caSection,strcat(strcpy(S,ALife::g_cafHitType2String(ALife::EHitType(I - B))),"_immunity"));
	}

	if (pSettings->line_exist(caSection,"retreat_threshold"))
		m_fRetreatThreshold		= pSettings->r_float(caSection,"retreat_threshold");
	else
		m_fRetreatThreshold		= 0.2f;
	m_fEyeRange					= pSettings->r_float(caSection,"eye_range");

	m_fGoingSpeed				= pSettings->r_float(caSection, "going_speed");
	LPCSTR						S = pSettings->r_string(caSection,"terrain");
	u32							N = _GetItemCount(S);
	R_ASSERT					(((N % (LOCATION_TYPE_COUNT + 2)) == 0) && (N));
	ALife::STerrainPlace		tTerrainPlace;
	tTerrainPlace.tMask.resize	(LOCATION_TYPE_COUNT);
	string16					I;
	for (u32 i=0; i<N;) {
		for (u32 j=0; j<LOCATION_TYPE_COUNT; ++j, ++i)
			tTerrainPlace.tMask[j] = ALife::_LOCATION_ID(atoi(_GetItem(S,i,I)));
		tTerrainPlace.dwMinTime	= atoi(_GetItem(S,i++,I))*1000;
		tTerrainPlace.dwMaxTime	= atoi(_GetItem(S,i++,I))*1000;
		m_tpaTerrain.push_back	(tTerrainPlace);
	}
	m_tpBestDetector			= this;
}

CSE_ALifeMonsterAbstract::~CSE_ALifeMonsterAbstract()
{
}

CSE_Abstract *CSE_ALifeMonsterAbstract::init			()
{
	inherited1::init			();
	inherited2::init			();
	return						(base());
}

CSE_Abstract *CSE_ALifeMonsterAbstract::base			()
{
	return						(inherited1::base());
}

const CSE_Abstract *CSE_ALifeMonsterAbstract::base	() const
{
	return						(inherited1::base());
}

void CSE_ALifeMonsterAbstract::STATE_Write	(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	tNetPacket.w_stringZ		(m_out_space_restrictors);
	tNetPacket.w_stringZ		(m_in_space_restrictors);
}

void CSE_ALifeMonsterAbstract::STATE_Read	(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
	if (m_wVersion > 72) {
		tNetPacket.r_stringZ	(m_out_space_restrictors);
		if (m_wVersion > 73)
			tNetPacket.r_stringZ(m_in_space_restrictors);
	}
}

void CSE_ALifeMonsterAbstract::UPDATE_Write	(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	tNetPacket.w				(&m_tNextGraphID,			sizeof(m_tNextGraphID));
	tNetPacket.w				(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
	tNetPacket.w				(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	tNetPacket.w				(&m_fCurSpeed,				sizeof(m_fCurSpeed));
	tNetPacket.w				(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
	tNetPacket.w				(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
};

void CSE_ALifeMonsterAbstract::UPDATE_Read	(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	tNetPacket.r				(&m_tNextGraphID,			sizeof(m_tNextGraphID));
	tNetPacket.r				(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
	tNetPacket.r				(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
	tNetPacket.r				(&m_fCurSpeed,				sizeof(m_fCurSpeed));
	tNetPacket.r				(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
	tNetPacket.r				(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
};

void CSE_ALifeMonsterAbstract::FillProps		(LPCSTR pref, PropItemVec& items)
{
  	inherited1::FillProps		(pref,items);
	if (pSettings->line_exist(s_name,"SpaceRestrictionSection")) {
		LPCSTR					gcs = pSettings->r_string(s_name,"SpaceRestrictionSection");
		PHelper().CreateChoose	(items, PrepareKey(pref,s_name,"out space restrictions"),&m_out_space_restrictors, smSpawnItem, 0, (void*)gcs, 16);
		PHelper().CreateChoose	(items, PrepareKey(pref,s_name,"in space restrictions"),&m_in_space_restrictors,  smSpawnItem, 0, (void*)gcs, 16);
	}
}

bool CSE_ALifeMonsterAbstract::need_update	(CSE_ALifeDynamicObject *object)
{
	return						(CSE_ALifeSchedulable::need_update(object) && (fHealth > EPS_L));
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeCreatureActor
////////////////////////////////////////////////////////////////////////////
CSE_ALifeCreatureActor::CSE_ALifeCreatureActor	(LPCSTR caSection) : CSE_ALifeCreatureAbstract(caSection), CSE_ALifeTraderAbstract(caSection)
{
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
		set_visual				(pSettings->r_string(caSection,"visual"));
	m_u16NumItems				= 0;
	fArmor						= 0.f;
}

CSE_ALifeCreatureActor::~CSE_ALifeCreatureActor()
{
}

CSE_Abstract *CSE_ALifeCreatureActor::init			()
{
	inherited1::init			();
	inherited2::init			();
	return						(inherited1::base());
}

CSE_Abstract *CSE_ALifeCreatureActor::base			()
{
	return						(inherited1::base());
}

const CSE_Abstract *CSE_ALifeCreatureActor::base	() const
{
	return						(inherited1::base());
}

void CSE_ALifeCreatureActor::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion < 21) {
		CSE_ALifeDynamicObject::STATE_Read(tNetPacket,size);
		tNetPacket.r_u8			(s_team	);
		tNetPacket.r_u8			(s_squad);
		tNetPacket.r_u8			(s_group);
		if (m_wVersion > 18)
			tNetPacket.r_float	(fHealth);
		if (m_wVersion >= 3)
			visual_read			(tNetPacket);
	}
	else {
		inherited1::STATE_Read	(tNetPacket,size);
		inherited2::STATE_Read	(tNetPacket,size);
		if (m_wVersion < 32)
			visual_read			(tNetPacket);
	}
};

void CSE_ALifeCreatureActor::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
};

void CSE_ALifeCreatureActor::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
	tNetPacket.r_u16			(mstate		);
	tNetPacket.r_sdir			(accel		);
	tNetPacket.r_sdir			(velocity	);
	tNetPacket.r_float_q16		(fArmor,	-500,1000);
	tNetPacket.r_u8				(weapon		);
	////////////////////////////////////////////////////
	tNetPacket.r_u16			(m_u16NumItems);

	if (!m_u16NumItems) return;

	if (m_u16NumItems == 1)
	{
		tNetPacket.r_u8					( *((u8*)&(m_AliveState.enabled)) );

		tNetPacket.r_vec3				( m_AliveState.angular_vel );
		tNetPacket.r_vec3				( m_AliveState.linear_vel );

		tNetPacket.r_vec3				( m_AliveState.force );
		tNetPacket.r_vec3				( m_AliveState.torque );

		tNetPacket.r_vec3				( m_AliveState.position );

		tNetPacket.r_float				( m_AliveState.quaternion.x );
		tNetPacket.r_float				( m_AliveState.quaternion.y );
		tNetPacket.r_float				( m_AliveState.quaternion.z );
		tNetPacket.r_float				( m_AliveState.quaternion.w );

		return;
	};	
	////////////// Import dead body ////////////////////
	{
		m_BoneDataSize = tNetPacket.r_u8();
		u32 BodyDataSize = 24 + m_BoneDataSize*m_u16NumItems;
		tNetPacket.r(m_DeadBodyData, BodyDataSize);
	};
};
void CSE_ALifeCreatureActor::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);
	tNetPacket.w_u16			(mstate		);
	tNetPacket.w_sdir			(accel		);
	tNetPacket.w_sdir			(velocity	);
	tNetPacket.w_float_q16		(fArmor,	-500,1000);
	tNetPacket.w_u8				(weapon		);
	////////////////////////////////////////////////////
	tNetPacket.w_u16			(m_u16NumItems);
	if (!m_u16NumItems) return;	

	if (m_u16NumItems == 1)
	{
		tNetPacket.w_u8					( m_AliveState.enabled );

		tNetPacket.w_vec3				( m_AliveState.angular_vel );
		tNetPacket.w_vec3				( m_AliveState.linear_vel );

		tNetPacket.w_vec3				( m_AliveState.force );
		tNetPacket.w_vec3				( m_AliveState.torque );

		tNetPacket.w_vec3				( m_AliveState.position );

		tNetPacket.w_float				( m_AliveState.quaternion.x );
		tNetPacket.w_float				( m_AliveState.quaternion.y );
		tNetPacket.w_float				( m_AliveState.quaternion.z );
		tNetPacket.w_float				( m_AliveState.quaternion.w );	

		return;
	};
	////////////// Export dead body ////////////////////
	{
		tNetPacket.w_u8(m_BoneDataSize);
		u32 BodyDataSize = 24 + m_BoneDataSize*m_u16NumItems;
		tNetPacket.w(m_DeadBodyData, BodyDataSize);
	};
}

void CSE_ALifeCreatureActor::FillProps		(LPCSTR pref, PropItemVec& items)
{
  	inherited1::FillProps		(pref,items);
  	inherited2::FillProps		(pref,items);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeCreatureCrow
////////////////////////////////////////////////////////////////////////////
CSE_ALifeCreatureCrow::CSE_ALifeCreatureCrow(LPCSTR caSection) : CSE_ALifeCreatureAbstract(caSection)
{
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
		set_visual				(pSettings->r_string(caSection,"visual"));
	m_flags.set					(flUseSwitches,FALSE);
	m_flags.set					(flSwitchOffline,FALSE);
}

CSE_ALifeCreatureCrow::~CSE_ALifeCreatureCrow()
{
}

void CSE_ALifeCreatureCrow::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion > 20) {
		inherited::STATE_Read	(tNetPacket,size);
		if (m_wVersion < 32)
			visual_read			(tNetPacket);
	}
}

void CSE_ALifeCreatureCrow::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeCreatureCrow::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeCreatureCrow::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

void CSE_ALifeCreatureCrow::FillProps			(LPCSTR pref, PropItemVec& values)
{
  	inherited::FillProps			(pref,values);
}

bool CSE_ALifeCreatureCrow::used_ai_locations	() const
{
	return						(false);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeMonsterRat
////////////////////////////////////////////////////////////////////////////
CSE_ALifeMonsterRat::CSE_ALifeMonsterRat	(LPCSTR caSection) : CSE_ALifeMonsterAbstract(caSection), CSE_ALifeInventoryItem(caSection)
{
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
		set_visual				(pSettings->r_string(caSection,"visual"));
	// personal charactersitics
	fEyeFov						= 120;
	fEyeRange					= 10;
	fHealth						= 5;
	fMinSpeed					= .5;
	fMaxSpeed					= 1.5;
	fAttackSpeed				= 4.0;
	fMaxPursuitRadius			= 100;
	fMaxHomeRadius				= 10;
	// morale
	fMoraleSuccessAttackQuant	= 20;
	fMoraleDeathQuant			= -10;
	fMoraleFearQuant			= -20;
	fMoraleRestoreQuant			=  10;
	u16MoraleRestoreTimeInterval= 3000;
	fMoraleMinValue				= 0;
	fMoraleMaxValue				= 100;
	fMoraleNormalValue			= 66;
	// attack
	fHitPower					= 10.0;
	u16HitInterval				= 1500;
	fAttackDistance				= 0.7f;
	fAttackAngle				= 45;
	fAttackSuccessProbability	= 0.5f;
}

CSE_ALifeMonsterRat::~CSE_ALifeMonsterRat	()
{
}

void CSE_ALifeMonsterRat::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket,size);
	tNetPacket.r_float			(fEyeFov);
	tNetPacket.r_float			(fEyeRange);
	if (m_wVersion <= 5)
		tNetPacket.r_float		(fHealth);
	tNetPacket.r_float			(fMinSpeed);
	tNetPacket.r_float			(fMaxSpeed);
	tNetPacket.r_float			(fAttackSpeed);
	tNetPacket.r_float			(fMaxPursuitRadius);
	tNetPacket.r_float			(fMaxHomeRadius);
	// morale
	tNetPacket.r_float			(fMoraleSuccessAttackQuant);
	tNetPacket.r_float			(fMoraleDeathQuant);
	tNetPacket.r_float			(fMoraleFearQuant);
	tNetPacket.r_float			(fMoraleRestoreQuant);
	tNetPacket.r_u16			(u16MoraleRestoreTimeInterval);
	tNetPacket.r_float			(fMoraleMinValue);
	tNetPacket.r_float			(fMoraleMaxValue);
	tNetPacket.r_float			(fMoraleNormalValue);
	// attack
	tNetPacket.r_float			(fHitPower);
	tNetPacket.r_u16			(u16HitInterval);
	tNetPacket.r_float			(fAttackDistance);
	tNetPacket.r_float			(fAttackAngle);
	tNetPacket.r_float			(fAttackSuccessProbability);
	inherited2::STATE_Read		(tNetPacket,size);
}

void CSE_ALifeMonsterRat::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	tNetPacket.w_float			(fEyeFov);
	tNetPacket.w_float			(fEyeRange);
	tNetPacket.w_float			(fMinSpeed);
	tNetPacket.w_float			(fMaxSpeed);
	tNetPacket.w_float			(fAttackSpeed);
	tNetPacket.w_float			(fMaxPursuitRadius);
	tNetPacket.w_float			(fMaxHomeRadius);
	// morale
	tNetPacket.w_float			(fMoraleSuccessAttackQuant);
	tNetPacket.w_float			(fMoraleDeathQuant);
	tNetPacket.w_float			(fMoraleFearQuant);
	tNetPacket.w_float			(fMoraleRestoreQuant);
	tNetPacket.w_u16			(u16MoraleRestoreTimeInterval);
	tNetPacket.w_float			(fMoraleMinValue);
	tNetPacket.w_float			(fMoraleMaxValue);
	tNetPacket.w_float			(fMoraleNormalValue);
	// attack
	tNetPacket.w_float			(fHitPower);
	tNetPacket.w_u16			(u16HitInterval);
	tNetPacket.w_float			(fAttackDistance);
	tNetPacket.w_float			(fAttackAngle);
	tNetPacket.w_float			(fAttackSuccessProbability);
	inherited2::STATE_Write		(tNetPacket);
}

void CSE_ALifeMonsterRat::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeMonsterRat::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);
}

CSE_Abstract *CSE_ALifeMonsterRat::init			()
{
	inherited1::init			();
	inherited2::init			();
	return						(base());
}

CSE_Abstract *CSE_ALifeMonsterRat::base			()
{
	return						(inherited1::base());
}

const CSE_Abstract *CSE_ALifeMonsterRat::base	() const
{
	return						(inherited1::base());
}

void CSE_ALifeMonsterRat::FillProps			(LPCSTR pref, PropItemVec& items)
{
	inherited1::FillProps		(pref, items);
	inherited2::FillProps		(pref, items);
	// personal characteristics
   	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Personal",	"Field of view" 		),&fEyeFov,							0,170,10);
   	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Personal",	"Eye range" 			),&fEyeRange,						0,300,10);
   	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Personal",	"Minimum speed" 		),&fMinSpeed,						0,10,0.1f);
   	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Personal",	"Maximum speed" 		),&fMaxSpeed,						0,10,0.1f);
   	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Personal",	"Attack speed" 			),&fAttackSpeed,					0,10,0.1f);
   	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Personal",	"Pursuit distance" 		),&fMaxPursuitRadius,				0,300,10);
   	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Personal",	"Home distance" 		),&fMaxHomeRadius,					0,300,10);
	// morale																			
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Morale",		"Success attack quant" 	),&fMoraleSuccessAttackQuant,		-100,100,5);
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Morale",		"Death quant" 			),&fMoraleDeathQuant,				-100,100,5);
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Morale",		"Fear quant" 			),&fMoraleFearQuant,				-100,100,5);
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Morale",		"Restore quant" 		),&fMoraleRestoreQuant,				-100,100,5);
	PHelper().CreateU16  			(items, PrepareKey(pref,s_name,"Morale",		"Restore time interval" ),&u16MoraleRestoreTimeInterval,	0,65535,500);
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Morale",		"Minimum value" 		),&fMoraleMinValue,					-100,100,5);
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Morale",		"Maximum value" 		),&fMoraleMaxValue,					-100,100,5);
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Morale",		"Normal value" 			),&fMoraleNormalValue,				-100,100,5);
	// attack																			 	
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Attack",		"Hit power" 			),&fHitPower,						0,200,5);
	PHelper().CreateU16  			(items, PrepareKey(pref,s_name,"Attack",		"Hit interval" 			),&u16HitInterval,					0,65535,500);
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Attack",		"Distance" 				),&fAttackDistance,					0,300,10);
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Attack",		"Maximum angle" 		),&fAttackAngle,					0,180,10);
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Attack",		"Success probability" 	),&fAttackSuccessProbability,		0,100,1);
}	

bool CSE_ALifeMonsterRat::bfUseful		()
{
	return						(!smart_cast<CSE_ALifeGroupAbstract*>(this) && (fHealth <= EPS_L));
}


////////////////////////////////////////////////////////////////////////////
// CSE_ALifeMonsterZombie
////////////////////////////////////////////////////////////////////////////
CSE_ALifeMonsterZombie::CSE_ALifeMonsterZombie	(LPCSTR caSection) : CSE_ALifeMonsterAbstract(caSection)
{
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
		set_visual				(pSettings->r_string(caSection,"visual"));
	// personal charactersitics
	fEyeFov						= 120;
	fEyeRange					= 30;
	fHealth						= 200;
	fMinSpeed					= 1.5;
	fMaxSpeed					= 1.75;
	fAttackSpeed				= 2.0;
	fMaxPursuitRadius			= 100;
	fMaxHomeRadius				= 30;
	// attack
	fHitPower					= 20.0;
	u16HitInterval				= 1000;
	fAttackDistance				= 1.0f;
	fAttackAngle				= 15;
}

CSE_ALifeMonsterZombie::~CSE_ALifeMonsterZombie()
{
}

void CSE_ALifeMonsterZombie::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	// inherited properties
	inherited::STATE_Read		(tNetPacket,size);
	// personal characteristics
	tNetPacket.r_float			(fEyeFov);
	tNetPacket.r_float			(fEyeRange);
	if (m_wVersion <= 5)
		tNetPacket.r_float		(fHealth);
	tNetPacket.r_float			(fMinSpeed);
	tNetPacket.r_float			(fMaxSpeed);
	tNetPacket.r_float			(fAttackSpeed);
	tNetPacket.r_float			(fMaxPursuitRadius);
	tNetPacket.r_float			(fMaxHomeRadius);
	// attack
	tNetPacket.r_float			(fHitPower);
	tNetPacket.r_u16			(u16HitInterval);
	tNetPacket.r_float			(fAttackDistance);
	tNetPacket.r_float			(fAttackAngle);
}

void CSE_ALifeMonsterZombie::STATE_Write	(NET_Packet	&tNetPacket)
{
	// inherited properties
	inherited::STATE_Write		(tNetPacket);
	// personal characteristics
	tNetPacket.w_float			(fEyeFov);
	tNetPacket.w_float			(fEyeRange);
	tNetPacket.w_float			(fMinSpeed);
	tNetPacket.w_float			(fMaxSpeed);
	tNetPacket.w_float			(fAttackSpeed);
	tNetPacket.w_float			(fMaxPursuitRadius);
	tNetPacket.w_float			(fMaxHomeRadius);
	// attack
	tNetPacket.w_float			(fHitPower);
	tNetPacket.w_u16			(u16HitInterval);
	tNetPacket.w_float			(fAttackDistance);
	tNetPacket.w_float			(fAttackAngle);
}

void CSE_ALifeMonsterZombie::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeMonsterZombie::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

void CSE_ALifeMonsterZombie::FillProps		(LPCSTR pref, PropItemVec& items)
{
   	inherited::FillProps			(pref, items);
	// personal characteristics
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Personal",	"Field of view" 		),&fEyeFov,							0,170,10);
   	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Personal",	"Eye range" 			),&fEyeRange,						0,300,10);
   	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Personal",	"Minimum speed" 		),&fMinSpeed,						0,10,0.1f);
   	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Personal",	"Maximum speed" 		),&fMaxSpeed,						0,10,0.1f);
   	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Personal",	"Attack speed" 			),&fAttackSpeed,					0,10,0.1f);
   	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Personal",	"Pursuit distance" 		),&fMaxPursuitRadius,				0,300,10);
   	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Personal",	"Home distance" 		),&fMaxHomeRadius,					0,300,10);
	// attack																			 	
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Attack",		"Hit power" 			),&fHitPower,						0,200,5);
	PHelper().CreateU16  			(items, PrepareKey(pref,s_name,"Attack",		"Hit interval" 			),&u16HitInterval,					0,65535,500);
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Attack",		"Distance" 				),&fAttackDistance,					0,300,10);
	PHelper().CreateFloat			(items, PrepareKey(pref,s_name,"Attack",		"Maximum angle" 		),&fAttackAngle,					0,100,1);
}

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeMonsterBase
//////////////////////////////////////////////////////////////////////////
CSE_ALifeMonsterBase::CSE_ALifeMonsterBase	(LPCSTR caSection) : CSE_ALifeMonsterAbstract(caSection),CSE_PHSkeleton(caSection)
{
    set_visual					(pSettings->r_string(caSection,"visual"));
}

CSE_ALifeMonsterBase::~CSE_ALifeMonsterBase()
{
}

void CSE_ALifeMonsterBase::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket,size);
	if(m_wVersion>=68)
		inherited2::STATE_Read		(tNetPacket,size);
}

void CSE_ALifeMonsterBase::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
}

void CSE_ALifeMonsterBase::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeMonsterBase::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write		(tNetPacket);
	inherited2::UPDATE_Write		(tNetPacket);
}

void CSE_ALifeMonsterBase::load(NET_Packet &tNetPacket)
{
	inherited1::load(tNetPacket);
	inherited2::load(tNetPacket);
}

void CSE_ALifeMonsterBase::FillProps	(LPCSTR pref, PropItemVec& values)
{
	inherited1::FillProps			(pref,values);
	inherited2::FillProps			(pref,values);
}

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeHumanAbstract
//////////////////////////////////////////////////////////////////////////
CSE_ALifeHumanAbstract::CSE_ALifeHumanAbstract(LPCSTR caSection) : CSE_ALifeTraderAbstract(caSection), CSE_ALifeMonsterAbstract(caSection)
{
	m_tpPath.clear				();
	m_baVisitedVertices.clear	();
	m_dwCurTaskID				= ALife::_TASK_ID(-1);
	m_tTaskState				= ALife::eTaskStateChooseTask;
	m_dwCurTaskLocation			= u32(-1);
	m_fSearchSpeed				= pSettings->r_float(caSection, "search_speed");
	m_dwCurNode					= u32(-1);
	m_caKnownCustomers			= "m_trader0000";
	m_tpKnownCustomers.clear	();
	m_cpEquipmentPreferences.resize(5);
	m_cpMainWeaponPreferences.resize(4);
#ifdef XRGAME_EXPORTS
	m_cpEquipmentPreferences.resize(iFloor(ai().ef_storage().m_pfEquipmentType->ffGetMaxResultValue() + .5f));
	m_cpMainWeaponPreferences.resize(iFloor(ai().ef_storage().m_pfMainWeaponType->ffGetMaxResultValue() + .5f));
	R_ASSERT2					((iFloor(ai().ef_storage().m_pfEquipmentType->ffGetMaxResultValue() + .5f) == 5) && (iFloor(ai().ef_storage().m_pfMainWeaponType->ffGetMaxResultValue() + .5f) == 4),"Recompile Level Editor and xrAI and rebuild file \"game.spawn\"!");
#endif
	{
		for (int i=0, n=m_cpEquipmentPreferences.size(); i<n; ++i)
			m_cpEquipmentPreferences[i] = u8(::Random.randI(3));
	}
	{
		for (int i=0, n=m_cpMainWeaponPreferences.size(); i<n; ++i)
			m_cpMainWeaponPreferences[i] = u8(::Random.randI(3));
	}
	m_fGoingSuccessProbability	= pSettings->r_float(caSection, "going_item_detect_probability");
	m_fSearchSuccessProbability	= pSettings->r_float(caSection, "search_item_detect_probability");
}

CSE_ALifeHumanAbstract::~CSE_ALifeHumanAbstract()
{
}

CSE_Abstract *CSE_ALifeHumanAbstract::init			()
{
	inherited1::init			();
	inherited2::init			();
	return						(base());
}

CSE_Abstract *CSE_ALifeHumanAbstract::base			()
{
	return						(inherited2::base());
}

const CSE_Abstract *CSE_ALifeHumanAbstract::base	() const
{
	return						(inherited2::base());
}

void CSE_ALifeHumanAbstract::STATE_Write	(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
	save_data					(m_tpPath,tNetPacket);
	save_data					(m_baVisitedVertices,tNetPacket);
	tNetPacket.w_stringZ		(*m_caKnownCustomers?*m_caKnownCustomers:"");
	save_data					(m_tpKnownCustomers,tNetPacket);
	save_data					(m_cpEquipmentPreferences,tNetPacket);
	save_data					(m_cpMainWeaponPreferences,tNetPacket);
}

void CSE_ALifeHumanAbstract::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
	inherited2::STATE_Read		(tNetPacket, size);
	if (m_wVersion > 19) {
		load_data				(m_tpPath,tNetPacket);
		load_data				(m_baVisitedVertices,tNetPacket);
		if (m_wVersion > 35) {
			tNetPacket.r_stringZ(m_caKnownCustomers);
			load_data			(m_tpKnownCustomers,tNetPacket);
			if (m_wVersion > 69) {
				load_data		(m_cpEquipmentPreferences,tNetPacket);
				load_data		(m_cpMainWeaponPreferences,tNetPacket);
			}
		}
	}
	m_tpPath.clear				();
	m_baVisitedVertices.clear	();
}

void CSE_ALifeHumanAbstract::UPDATE_Write	(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);
	tNetPacket.w				(&m_tTaskState,sizeof(m_tTaskState));
	tNetPacket.w_u32			(m_dwCurTaskLocation);
	tNetPacket.w_u32			(m_dwCurTaskID);
};

void CSE_ALifeHumanAbstract::UPDATE_Read	(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
	tNetPacket.r				(&m_tTaskState,sizeof(m_tTaskState));
	tNetPacket.r_u32			(m_dwCurTaskLocation);
	tNetPacket.r_u32			(m_dwCurTaskID);
};

void CSE_ALifeHumanAbstract::FillProps		(LPCSTR pref, PropItemVec& items)
{
  	inherited1::FillProps		(pref,items);
  	inherited2::FillProps		(pref,items);
}

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeHumanStalker
//////////////////////////////////////////////////////////////////////////
CSE_ALifeHumanStalker::CSE_ALifeHumanStalker(LPCSTR caSection) : CSE_ALifeHumanAbstract(caSection),CSE_PHSkeleton(caSection)
{
	m_dwTotalMoney				= 0;
	m_trader_flags.set			(eTraderFlagInfiniteAmmo,TRUE);
}

CSE_ALifeHumanStalker::~CSE_ALifeHumanStalker()
{
}

void CSE_ALifeHumanStalker::STATE_Write		(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
}

void CSE_ALifeHumanStalker::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
	if(m_wVersion>=68)
		inherited2::STATE_Read		(tNetPacket, size);
}

void CSE_ALifeHumanStalker::UPDATE_Write	(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write		(tNetPacket);
	inherited2::UPDATE_Write		(tNetPacket);
};

void CSE_ALifeHumanStalker::UPDATE_Read		(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
};
void CSE_ALifeHumanStalker::load(NET_Packet &tNetPacket)
{
	inherited1::load(tNetPacket);
	inherited2::load(tNetPacket);
}

void CSE_ALifeHumanStalker::FillProps		(LPCSTR pref, PropItemVec& values)
{
	inherited1::FillProps			(pref,values);
	inherited2::FillProps			(pref,values);
}

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeObjectIdol
//////////////////////////////////////////////////////////////////////////
CSE_ALifeObjectIdol::CSE_ALifeObjectIdol	(LPCSTR caSection) : CSE_ALifeHumanAbstract(caSection)
{
	m_dwAniPlayType				= 0;
}

CSE_ALifeObjectIdol::~CSE_ALifeObjectIdol	()
{
}

void CSE_ALifeObjectIdol::STATE_Read		(NET_Packet& tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
	tNetPacket.r_stringZ		(m_caAnimations);
	tNetPacket.r_u32			(m_dwAniPlayType);
}

void CSE_ALifeObjectIdol::STATE_Write		(NET_Packet& tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_stringZ			(m_caAnimations);
	tNetPacket.w_u32			(m_dwAniPlayType);
}

void CSE_ALifeObjectIdol::UPDATE_Read		(NET_Packet& tNetPacket)
{
}

void CSE_ALifeObjectIdol::UPDATE_Write		(NET_Packet& tNetPacket)
{
}

void CSE_ALifeObjectIdol::FillProps			(LPCSTR pref, PropItemVec& items)
{
   	inherited::FillProps			(pref, items);
	PHelper().CreateRText		(items, PrepareKey(pref,s_name,"Idol", "Animations"),&m_caAnimations);
   	PHelper().CreateU32			(items, PrepareKey(pref,s_name,"Idol", "Animation playing type"),&m_dwAniPlayType,0,2,1);
}	
