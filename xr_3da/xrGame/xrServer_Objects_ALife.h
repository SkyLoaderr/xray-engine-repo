////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shishkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALifeH
#define xrServer_Objects_ALifeH

#include "xrServer_Objects.h"
#include "ai_alife_space.h"
using namespace ALife;

#ifndef _EDITOR
	#ifndef AI_COMPILER
		class CSE_ALifeSimulator;
	#endif
#else
	class CSE_ALifeItemWeapon;
#endif

class CSE_ALifeSchedulable : public IPureSchedulableObject, virtual public CSE_Abstract {
public:
	CSE_ALifeItemWeapon				*m_tpCurrentBestWeapon;
#ifndef _EDITOR
#ifndef AI_COMPILER
	CSE_ALifeSimulator				*m_tpALife;
#endif
#endif
	CSE_ALifeDynamicObject			*m_tpBestDetector;
	u64								m_qwUpdateCounter;

									CSE_ALifeSchedulable	(LPCSTR caSection);
#ifndef _EDITOR
#ifndef AI_COMPILER
	virtual	CSE_ALifeItemWeapon		*tpfGetBestWeapon		(EHitType				&tHitType,			float		&fHitPower) = 0;
	virtual bool					bfPerformAttack			()											{return(true);};
	virtual	void					vfUpdateWeaponAmmo		()											{};
	virtual	void					vfProcessItems			()											{};
	virtual	void					vfAttachItems			(ETakeType				tTakeType = eTakeTypeAll)		{};
	virtual	EMeetActionType			tfGetActionType			(CSE_ALifeSchedulable	*tpALifeSchedulable,int			iGroupIndex, bool bMutualDetection) = 0;
	virtual bool					bfActive				()															= 0;
	virtual CSE_ALifeDynamicObject	*tpfGetBestDetector		()															= 0;
#endif
#endif
};

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeEvent,CPureServerObject)
	_EVENT_ID						m_tEventID;
	_TIME_ID						m_tTimeID;
	_GRAPH_ID						m_tGraphID;
	ECombatResult					m_tCombatResult;
	CSE_ALifeEventGroup				*m_tpMonsterGroup1;
	CSE_ALifeEventGroup				*m_tpMonsterGroup2;

									CSE_ALifeEvent	()
	{
	};
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifePersonalEvent,CPureServerObject)
	_EVENT_ID						m_tEventID;
	_TIME_ID						m_tTimeID;
	_TASK_ID						m_tTaskID;
	int								m_iHealth;
	EEventRelationType				m_tEventRelationType;
	OBJECT_VECTOR					m_tpItemIDs;

									CSE_ALifePersonalEvent()
	{
	};
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeTask,CPureServerObject)
	_TASK_ID						m_tTaskID;
	_TIME_ID						m_tTimeID;
	_OBJECT_ID						m_tCustomerID;
	float							m_fCost;
	ETaskType						m_tTaskType;
	u32								m_dwTryCount;
	union {
		string32					m_caSection;
		_OBJECT_ID					m_tObjectID;
	};
	union {
		_LOCATION_ID				m_tLocationID[LOCATION_TYPE_COUNT];
		_GRAPH_ID					m_tGraphID;
	};

									CSE_ALifeTask	();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_LevelPoint,CSE_Abstract)
									CSE_LevelPoint(LPCSTR caSection);
	virtual							~CSE_LevelPoint();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeGraphPoint,CSE_Abstract)
public:
	string32						m_caConnectionLevelName;
	string32						m_caConnectionPointName;
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
SERVER_ENTITY_DECLARE_END

class CSE_ALifeGroupAbstract : virtual public CSE_Abstract {
public:
	OBJECT_VECTOR					m_tpMembers;
	bool							m_bCreateSpawnPositions;
	u16								m_wCount;
	_TIME_ID						m_tNextBirthTime;

									CSE_ALifeGroupAbstract(LPCSTR caSection);
	virtual							~CSE_ALifeGroupAbstract();
SERVER_ENTITY_DECLARE_END

template<class __A> class CSE_ALifeGroupTemplate : public __A, public CSE_ALifeGroupAbstract {
	typedef __A					inherited1;
	typedef CSE_ALifeGroupAbstract inherited2;
public:
									CSE_ALifeGroupTemplate(LPCSTR caSection) : __A(pSettings->line_exist(caSection,"monster_section") ? pSettings->r_string(caSection,"monster_section") : caSection), CSE_ALifeGroupAbstract(caSection), CSE_Abstract(pSettings->line_exist(caSection,"monster_section") ? pSettings->r_string(caSection,"monster_section") : caSection)
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
	u64								m_qwSwitchCounter;
	
									CSE_ALifeDynamicObject(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeDynamicObjectVisual,CSE_ALifeDynamicObject,CSE_Visual)
									CSE_ALifeDynamicObjectVisual(LPCSTR caSection);
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeLevelChanger,CSE_ALifeDynamicObject,CSE_Shape)
	_GRAPH_ID						m_tNextGraphID;
	u32								m_dwNextNodeID;
	Fvector							m_tNextPosition;
	float							m_fAngle;
	string32						m_caLevelToChange;
	string32						m_caLevelPointToChange;

									CSE_ALifeLevelChanger	(LPCSTR caSection);
	virtual							~CSE_ALifeLevelChanger	();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectPhysic,CSE_ALifeDynamicObjectVisual)
#ifdef _EDITOR
	void __fastcall					OnChangeAnim	(PropValue* sender);
#endif
	u32 							type;
	f32 							mass;
    string32 						fixed_bone;
	string32						startup_animation;
	bool							activate;
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