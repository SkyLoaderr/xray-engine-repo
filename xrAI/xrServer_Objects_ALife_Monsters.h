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

class CALifeTraderAbstract : virtual public CAbstractServerObject {
public:
	float							m_fCumulativeItemMass;
	u32								m_dwMoney;
	EStalkerRank					m_tRank;
	float							m_fMaxItemMass;
	PERSONAL_EVENT_P_VECTOR			m_tpEvents;
	TASK_VECTOR						m_tpTaskIDs;
	
									CALifeTraderAbstract(LPCSTR caSection) : CAbstractServerObject(caSection)
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

	virtual							~CALifeTraderAbstract()
	{
		free_vector					(m_tpEvents);
	};

	virtual void					STATE_Write		(NET_Packet &tNetPacket);
	virtual void					STATE_Read		(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write	(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read		(NET_Packet &tNetPacket);
#ifdef _EDITOR
	virtual void					FillProp		(LPCSTR pref, PropItemVec& items);
#endif
};

SERVER_OBJECT_DECLARE_BEGIN2(CALifeTrader,CALifeDynamicObjectVisual,CALifeTraderAbstract)
									CALifeTrader	(LPCSTR caSection) : CALifeDynamicObjectVisual(caSection), CALifeTraderAbstract(caSection), CAbstractServerObject(caSection)
	{
	};
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN(CALifeCreatureAbstract,CALifeDynamicObjectVisual)
	u8								s_team;
	u8								s_squad;
	u8								s_group;
	float							fHealth;

	u32								timestamp;				// server(game) timestamp
	u8								flags;
	float							o_model;				// model yaw
	SRotation						o_torso;				// torso in world coords
									
									CALifeCreatureAbstract(LPCSTR caSection)	: CALifeDynamicObjectVisual(caSection), CAbstractServerObject(caSection)
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
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN(CALifeMonsterAbstract,CALifeCreatureAbstract)
	_GRAPH_ID						m_tNextGraphID;
	_GRAPH_ID						m_tPrevGraphID;
	float							m_fGoingSpeed;
	float							m_fCurSpeed;
	float							m_fDistanceFromPoint;
	float							m_fDistanceToPoint;
	TERRAIN_VECTOR					m_tpaTerrain;
	
									CALifeMonsterAbstract(LPCSTR caSection)	: CALifeCreatureAbstract(caSection), CAbstractServerObject(caSection)
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
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN2(CALifeCreatureActor,CALifeCreatureAbstract,CALifeTraderAbstract)
	u16								mstate;
	Fvector							accel;
	Fvector							velocity;
	float							fArmor;
	u8								weapon;
									CALifeCreatureActor		(LPCSTR caSection);
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN(CALifeMonsterCrow,CALifeCreatureAbstract)
									CALifeMonsterCrow		(LPCSTR caSection) : CALifeCreatureAbstract(caSection), CAbstractServerObject(caSection)
	{
		if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
    	    set_visual				(pSettings->r_string(caSection,"visual"));
	};
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN(CALifeMonsterRat,CALifeMonsterAbstract)
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

									CALifeMonsterRat	(LPCSTR caSection);				// constructor for variable initialization
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN(CALifeMonsterZombie,CALifeMonsterAbstract)
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

									CALifeMonsterZombie	(LPCSTR caSection);				// constructor for variable initialization
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN(CALifeMonsterBiting,CALifeMonsterAbstract)
									CALifeMonsterBiting	(LPCSTR caSection);				// constructor for variable initialization
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN2(CALifeHumanAbstract,CALifeTraderAbstract,CALifeMonsterAbstract)
	DWORD_VECTOR					m_tpaVertices;
	BOOL_VECTOR						m_baVisitedVertices;
	PERSONAL_TASK_P_VECTOR			m_tpTasks;
	ETaskState						m_tTaskState;
	u32								m_dwCurTaskLocation;
	u32								m_dwCurTask;
	float							m_fSearchSpeed;

									CALifeHumanAbstract(LPCSTR caSection) : CALifeTraderAbstract(caSection), CALifeMonsterAbstract(caSection), CAbstractServerObject(caSection)
	{
		m_tpaVertices.clear			();
		m_baVisitedVertices.clear	();
		m_tpTasks.clear				();
		m_dwCurTask					= u32(-1);
		m_tTaskState				= eTaskStateNoTask;
		m_dwCurTaskLocation			= u32(-1);
		m_fSearchSpeed				= pSettings->r_float(caSection, "search_speed");
	};

	virtual							~CALifeHumanAbstract()
	{
		free_vector					(m_tpTasks);
	};
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN(CALifeHumanStalker,CALifeHumanAbstract)
									CALifeHumanStalker	(LPCSTR caSection) : CALifeHumanAbstract(caSection), CAbstractServerObject(caSection)
	{
	};
SERVER_OBJECT_DECLARE_END

SERVER_OBJECT_DECLARE_BEGIN(CALifeObjectIdol,CALifeHumanAbstract)
	string256						m_caAnimations;
	u32								m_dwAniPlayType;
									CALifeObjectIdol	(LPCSTR caSection);
SERVER_OBJECT_DECLARE_END

#endif