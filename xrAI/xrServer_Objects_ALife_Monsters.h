////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Retsky and Dmitriy Iassenev
//	Description : Server objects monsters for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALife_MonstersH
#define xrServer_Objects_ALife_MonstersH

#include "xrServer_Objects_ALife.h"

class CSE_ALifeTraderAbstract : virtual public CSE_Abstract {
public:
	float							m_fCumulativeItemMass;
	u32								m_dwMoney;
	EStalkerRank					m_tRank;
	float							m_fMaxItemMass;
	PERSONAL_EVENT_P_VECTOR			m_tpEvents;
	TASK_VECTOR						m_tpTaskIDs;
	
									CSE_ALifeTraderAbstract(LPCSTR caSection) : CSE_Abstract(caSection)
	{
		m_fCumulativeItemMass		= 0.0f;
		m_dwMoney					= 0;
		if (pSettings->line_exist(caSection, "money"))
			m_dwMoney 				= pSettings->r_u32(caSection, "money");
		m_tRank						= EStalkerRank(pSettings->r_u32(caSection, "rank"));
		m_fMaxItemMass				= pSettings->r_float(caSection, "max_item_mass");
		m_tpEvents.clear			();
		m_tpTaskIDs.clear			();
	};

	virtual							~CSE_ALifeTraderAbstract();

	virtual void					STATE_Write		(NET_Packet &tNetPacket);
	virtual void					STATE_Read		(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write	(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read		(NET_Packet &tNetPacket);
#ifdef _EDITOR
	virtual void					FillProp		(LPCSTR pref, PropItemVec& items);
#endif
};

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeTrader,CSE_ALifeDynamicObjectVisual,CSE_ALifeTraderAbstract)
									CSE_ALifeTrader	(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeCreatureAbstract,CSE_ALifeDynamicObjectVisual)
	u8								s_team;
	u8								s_squad;
	u8								s_group;
	float							fHealth;

	u32								timestamp;				// server(game) timestamp
	u8								flags;
	float							o_model;				// model yaw
	SRotation						o_torso;				// torso in world coords
									
									CSE_ALifeCreatureAbstract(LPCSTR caSection)	: CSE_ALifeDynamicObjectVisual(caSection), CSE_Abstract(caSection)
	{
		s_team = s_squad = s_group	= 0;
		fHealth						= 100;
	};
	
	virtual u8						g_team			()
	{
		return s_team;
	}

	virtual u8						g_squad			()
	{
		return s_squad;
	}

	virtual u8						g_group			()
	{
		return s_group;
	}
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeMonsterAbstract,CSE_ALifeCreatureAbstract)
	_GRAPH_ID						m_tNextGraphID;
	_GRAPH_ID						m_tPrevGraphID;
	float							m_fGoingSpeed;
	float							m_fCurSpeed;
	float							m_fDistanceFromPoint;
	float							m_fDistanceToPoint;
	TERRAIN_VECTOR					m_tpaTerrain;
	
									CSE_ALifeMonsterAbstract(LPCSTR caSection)	: CSE_ALifeCreatureAbstract(caSection), CSE_Abstract(caSection)
	{
		m_tNextGraphID				= m_tGraphID;
		m_tPrevGraphID				= m_tGraphID;
		m_fCurSpeed					= 0.0f;
		m_fDistanceFromPoint		= 0.0f;
		m_fDistanceToPoint			= 0.0f;
		m_tpaTerrain.clear			();
		LPCSTR						S;
		if (pSettings->line_exist(caSection,"monster_section")) {
			S						= pSettings->r_string	(pSettings->r_string(caSection,"monster_section"),"terrain");
			m_fGoingSpeed			= pSettings->r_float	(pSettings->r_string(caSection,"monster_section"), "going_speed");
		}
		else {
			S						= pSettings->r_string	(caSection,"terrain");
			m_fGoingSpeed			= pSettings->r_float	(caSection, "going_speed");
		}
		u32							N = _GetItemCount(S);
		R_ASSERT					(((N % (LOCATION_TYPE_COUNT + 2)) == 0) && (N));
		STerrainPlace				tTerrainPlace;
		tTerrainPlace.tMask.resize	(LOCATION_TYPE_COUNT);
		string16					I;
		for (u32 i=0; i<N;) {
			for (u32 j=0; j<LOCATION_TYPE_COUNT; j++, i++)
				tTerrainPlace.tMask[j] = _LOCATION_ID(atoi(_GetItem(S,i,I)));
			tTerrainPlace.dwMinTime	= atoi(_GetItem(S,i++,I))*1000;
			tTerrainPlace.dwMaxTime	= atoi(_GetItem(S,i++,I))*1000;
			m_tpaTerrain.push_back	(tTerrainPlace);
		}
	};
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeCreatureActor,CSE_ALifeCreatureAbstract,CSE_ALifeTraderAbstract)
	u16								mstate;
	Fvector							accel;
	Fvector							velocity;
	float							fArmor;
	u8								weapon;
									CSE_ALifeCreatureActor		(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeMonsterCrow,CSE_ALifeCreatureAbstract)
									CSE_ALifeMonsterCrow		(LPCSTR caSection) : CSE_ALifeCreatureAbstract(caSection), CSE_Abstract(caSection)
	{
		if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
    	    set_visual				(pSettings->r_string(caSection,"visual"));
	};
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeMonsterRat,CSE_ALifeMonsterAbstract)
	// Personal characteristics:
	float							fEyeFov;
	float							fEyeRange;
	float							fMinSpeed;
	float							fMaxSpeed;
	float							fAttackSpeed;
	float							fMaxPursuitRadius;
	float							fMaxHomeRadius;
	// morale
	float							fMoraleSuccessAttackQuant;
	float							fMoraleDeathQuant;
	float							fMoraleFearQuant;
	float							fMoraleRestoreQuant;
	u16								u16MoraleRestoreTimeInterval;
	float							fMoraleMinValue;
	float							fMoraleMaxValue;
	float							fMoraleNormalValue;
	// attack
	float							fHitPower;
	u16								u16HitInterval;
	float							fAttackDistance;
	float							fAttackAngle;
	float							fAttackSuccessProbability;

									CSE_ALifeMonsterRat	(LPCSTR caSection);				// constructor for variable initialization
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeMonsterZombie,CSE_ALifeMonsterAbstract)
	// Personal characteristics:
	float							fEyeFov;
	float							fEyeRange;
	float							fMinSpeed;
	float							fMaxSpeed;
	float							fAttackSpeed;
	float							fMaxPursuitRadius;
	float							fMaxHomeRadius;
	// attack
	float							fHitPower;
	u16								u16HitInterval;
	float							fAttackDistance;
	float							fAttackAngle;

									CSE_ALifeMonsterZombie	(LPCSTR caSection);				// constructor for variable initialization
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeMonsterBiting,CSE_ALifeMonsterAbstract)
									CSE_ALifeMonsterBiting	(LPCSTR caSection);				// constructor for variable initialization
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeHumanAbstract,CSE_ALifeTraderAbstract,CSE_ALifeMonsterAbstract)
	DWORD_VECTOR					m_tpaVertices;
	xr_vector<bool>					m_baVisitedVertices;
	PERSONAL_TASK_P_VECTOR			m_tpTasks;
	ETaskState						m_tTaskState;
	u32								m_dwCurTaskLocation;
	u32								m_dwCurTask;
	float							m_fSearchSpeed;

									CSE_ALifeHumanAbstract(LPCSTR caSection) : CSE_ALifeTraderAbstract(caSection), CSE_ALifeMonsterAbstract(caSection), CSE_Abstract(caSection)
	{
		m_tpaVertices.clear			();
		m_baVisitedVertices.clear	();
		m_tpTasks.clear				();
		m_dwCurTask					= u32(-1);
		m_tTaskState				= eTaskStateNoTask;
		m_dwCurTaskLocation			= u32(-1);
		m_fSearchSpeed				= pSettings->r_float(caSection, "search_speed");
	};

	virtual							~CSE_ALifeHumanAbstract();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeHumanStalker,CSE_ALifeHumanAbstract)
									CSE_ALifeHumanStalker	(LPCSTR caSection) : CSE_ALifeHumanAbstract(caSection), CSE_Abstract(caSection)
	{
	};
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectIdol,CSE_ALifeHumanAbstract)
	string256						m_caAnimations;
	u32								m_dwAniPlayType;
									CSE_ALifeObjectIdol(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

#endif