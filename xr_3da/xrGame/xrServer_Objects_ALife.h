////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Retsky and Dmitriy Iassenev
//	Description : Server objects for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALifeH
#define xrServer_Objects_ALifeH

#include "xrServer_Objects.h"
#include "ai_alife_space.h"
using namespace ALife;

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeEvent,IPureServerObject)
	_EVENT_ID						m_tEventID;
	_TIME_ID						m_tTimeID;
	_GRAPH_ID						m_tGraphID;
	EBattleResult					m_tBattleResult;
	CSE_ALifeEventGroup				*m_tpMonsterGroup1;
	CSE_ALifeEventGroup				*m_tpMonsterGroup2;

									CSE_ALifeEvent	(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifePersonalEvent,IPureServerObject)
	_EVENT_ID						m_tEventID;
	_TIME_ID						m_tTimeID;
	_TASK_ID						m_tTaskID;
	int								m_iHealth;
	ERelation						m_tRelation;
	OBJECT_VECTOR					m_tpItemIDs;

									CSE_ALifePersonalEvent()
	{
	};
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeTask,IPureServerObject)
	_TASK_ID						m_tTaskID;
	_TIME_ID						m_tTimeID;
	_OBJECT_ID						m_tCustomerID;
	float							m_fCost;
	ETaskType						m_tTaskType;
	union {
		_CLASS_ID					m_tClassID;
		_OBJECT_ID					m_tObjectID;
	};
	union {
		_LOCATION_ID				m_tLocationID[LOCATION_TYPE_COUNT];
		_GRAPH_ID					m_tGraphID;
	};

									CSE_ALifeTask	()
	{
	};
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifePersonalTask,CSE_ALifeTask)
	u32								m_dwTryCount;

									CSE_ALifePersonalTask()
	{
		m_dwTryCount				= 0;
	};
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeGraphPoint,CSE_Abstract)
public:
	string32						m_caConnectionPointName;
	u32								m_tLevelID;
	u8								m_tLocations[LOCATION_TYPE_COUNT];
	CSE_ALifeGraphPoint(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

class CSE_ALifeObject : virtual public CSE_Abstract {
public:
	typedef CSE_Abstract inherited;
	_CLASS_ID						m_tClassID;
	_GRAPH_ID						m_tGraphID;
	_SPAWN_ID						m_tSpawnID;
	float							m_fDistance;
	bool							m_bOnline;
	float							m_fProbability;
	u32								m_dwSpawnGroup;
	bool							m_bDirectControl;
	u32								m_tNodeID;
	string64						m_caGroupControl;

									CSE_ALifeObject	(LPCSTR caSection);
	virtual void					STATE_Write		(NET_Packet &tNetPacket);
	virtual void					STATE_Read		(NET_Packet &tNetPacket, u16 size);
	virtual void					UPDATE_Write	(NET_Packet &tNetPacket);
	virtual void					UPDATE_Read		(NET_Packet &tNetPacket);
#ifdef _EDITOR
    virtual void					FillProp		(LPCSTR pref, PropItemVec &items);
#endif
};

class CSE_ALifeAbstractGroup : virtual public CSE_Abstract {
public:
	OBJECT_VECTOR					m_tpMembers;
	bool							m_bCreateSpawnPositions;
	u16								m_wCount;

									CSE_ALifeAbstractGroup(LPCSTR caSection) : CSE_Abstract(caSection)
	{
		m_tpMembers.clear			();
		m_bCreateSpawnPositions		= true;
		m_wCount					= 1;
	};

	virtual							~CSE_ALifeAbstractGroup()
	{
	};
	
	virtual void STATE_Read			(NET_Packet	&tNetPacket, u16 size);
	virtual void STATE_Write		(NET_Packet	&tNetPacket);
	virtual void UPDATE_Read		(NET_Packet	&tNetPacket);
	virtual void UPDATE_Write		(NET_Packet	&tNetPacket);
	#ifdef _EDITOR
	virtual void FillProp			(LPCSTR pref, PropItemVec& items);
	#endif
};

template<class __A> class CSE_ALifeGroupTemplate : public __A, public CSE_ALifeAbstractGroup {
	typedef __A					inherited1;
	typedef CSE_ALifeAbstractGroup inherited2;
public:
									CSE_ALifeGroupTemplate(LPCSTR caSection) : __A(caSection), CSE_ALifeAbstractGroup(caSection), CSE_Abstract(caSection)
	{
	};
	
	virtual							~CSE_ALifeGroupTemplate()
	{
	};
	
	virtual void STATE_Read			(NET_Packet	&tNetPacket, u16 size)
	{
		inherited1::STATE_Read		(tNetPacket,size);
		inherited2::STATE_Read		(tNetPacket,size);
	};

	virtual void STATE_Write		(NET_Packet	&tNetPacket)
	{
		inherited1::STATE_Write		(tNetPacket);
		inherited2::STATE_Write		(tNetPacket);
	};

	virtual void UPDATE_Read		(NET_Packet	&tNetPacket)
	{
		inherited1::UPDATE_Read		(tNetPacket);
		inherited2::UPDATE_Read		(tNetPacket);
	};

	virtual void UPDATE_Write		(NET_Packet	&tNetPacket)
	{
		inherited1::UPDATE_Write	(tNetPacket);
		inherited2::UPDATE_Write	(tNetPacket);
	};

	#ifdef _EDITOR
	virtual void FillProp			(LPCSTR pref, PropItemVec& items)
	{
   		inherited1::FillProp		(pref, items);
   		inherited2::FillProp		(pref, items);
	};	
	#endif
};

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeEventGroup,CSE_ALifeObject)
	u16								m_wCountBefore;
	u16								m_wCountAfter;
	
									CSE_ALifeEventGroup(LPCSTR caSection) : CSE_ALifeObject(caSection), CSE_Abstract(caSection)
	{
		m_wCountAfter				= m_wCountBefore;
	};
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeDynamicObject,CSE_ALifeObject)
	_TIME_ID						m_tTimeID;
	u32								m_dwLastSwitchTime;
	
									CSE_ALifeDynamicObject(LPCSTR caSection) : CSE_ALifeObject(caSection), CSE_Abstract(caSection)
	{
		m_tTimeID					= 0;
		m_dwLastSwitchTime			= 1;
	}
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeDynamicObjectVisual,CSE_ALifeDynamicObject,CSE_Visual)
									CSE_ALifeDynamicObjectVisual(LPCSTR caSection) : CSE_ALifeDynamicObject(caSection), CSE_Visual(), CSE_Abstract(caSection)
	{
	}
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeAnomalousZone,CSE_ALifeDynamicObject,CSE_Shape)
	f32								m_maxPower;
	f32								m_attn;
	u32								m_period;
	float							m_fRadius;
	float							m_fBirthProbability;
	u16								m_wItemCount;
	float							*m_faWeights;
	string64						*m_cppArtefactSections;
	u16								m_wArtefactSpawnCount;
	u32								m_dwStartIndex;
	EAnomalousZoneType				m_tAnomalyType;

									CSE_ALifeAnomalousZone	(LPCSTR caSection);
	virtual							~CSE_ALifeAnomalousZone	();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectPhysic,CSE_ALifeDynamicObjectVisual)
#ifdef _EDITOR
	void __fastcall					OnChangeAnim	(PropValue* sender);
#endif
	u32 							type;
	f32 							mass;
    string32 						fixed_bone;
	string32						startup_animation;
									CSE_ALifeObjectPhysic	(LPCSTR caSection);
    virtual 						~CSE_ALifeObjectPhysic	();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectHangingLamp,CSE_ALifeDynamicObjectVisual)
#ifdef _EDITOR
	void __fastcall					OnChangeAnim	(PropValue* sender);
#endif
	enum{
    	flPhysic					= (1<<0)
    };
	Flags16							flags;
    float							mass;
	u32								color;
	string32						startup_animation;
	string64						color_animator;
	string64						spot_texture;
	string32						spot_bone;
	float							spot_range;
	float							spot_cone_angle;
    float							spot_brightness;
									CSE_ALifeObjectHangingLamp	(LPCSTR caSection);
    virtual							~CSE_ALifeObjectHangingLamp	();
SERVER_ENTITY_DECLARE_END

#endif