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
	virtual							~CSE_ALifeSchedulable	();
#ifndef _EDITOR
#ifndef AI_COMPILER
	virtual	CSE_ALifeItemWeapon		*tpfGetBestWeapon		(ALife::EHitType		&tHitType,			float		&fHitPower) = 0;
	virtual bool					bfPerformAttack			()											{return(true);};
	virtual	void					vfUpdateWeaponAmmo		()											{};
	virtual	void					vfProcessItems			()											{};
	virtual	void					vfAttachItems			(ALife::ETakeType		tTakeType = ALife::eTakeTypeAll)		{};
	virtual	ALife::EMeetActionType	tfGetActionType			(CSE_ALifeSchedulable	*tpALifeSchedulable,int			iGroupIndex, bool bMutualDetection) = 0;
	virtual bool					bfActive				()															= 0;
	virtual CSE_ALifeDynamicObject	*tpfGetBestDetector		()															= 0;
#endif
#endif
};

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeEvent,CPureServerObject)
	ALife::_EVENT_ID				m_tEventID;
	ALife::_TIME_ID					m_tTimeID;
	ALife::_GRAPH_ID				m_tGraphID;
	ALife::ECombatResult			m_tCombatResult;
	CSE_ALifeEventGroup				*m_tpMonsterGroup1;
	CSE_ALifeEventGroup				*m_tpMonsterGroup2;

									CSE_ALifeEvent	();
	virtual							~CSE_ALifeEvent	();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifePersonalEvent,CPureServerObject)
	ALife::_EVENT_ID				m_tEventID;
	ALife::_TIME_ID					m_tTimeID;
	ALife::_TASK_ID					m_tTaskID;
	int								m_iHealth;
	ALife::EEventRelationType		m_tEventRelationType;
	ALife::OBJECT_VECTOR			m_tpItemIDs;

									CSE_ALifePersonalEvent();
	virtual							~CSE_ALifePersonalEvent();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeTask,CPureServerObject)
	ALife::_TASK_ID					m_tTaskID;
	ALife::_TIME_ID					m_tTimeID;
	ALife::_OBJECT_ID				m_tCustomerID;
	float							m_fCost;
	ALife::ETaskType				m_tTaskType;
	u32								m_dwTryCount;
	union {
		string32					m_caSection;
		ALife::_OBJECT_ID			m_tObjectID;
	};
	union {
		ALife::_LOCATION_ID			m_tLocationID[LOCATION_TYPE_COUNT];
		ALife::_GRAPH_ID			m_tGraphID;
	};

									CSE_ALifeTask	();
	virtual							~CSE_ALifeTask	();
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
	virtual							~CSE_ALifeGraphPoint();
SERVER_ENTITY_DECLARE_END

class CSE_ALifeObject : virtual public CSE_Abstract {
protected:
	enum {
		flUseSwitches	= u32(1) << 0,
		flSwitchOnline  = u32(1) << 1,
		flSwitchOffline = u32(1) << 2,
	};
public:
	typedef CSE_Abstract inherited;
	ALife::_CLASS_ID				m_tClassID;
	ALife::_GRAPH_ID				m_tGraphID;
	ALife::_SPAWN_ID				m_tSpawnID;
	float							m_fDistance;
	bool							m_bOnline;
	float							m_fProbability;
	u32								m_dwSpawnGroup;
	bool							m_bDirectControl;
	u32								m_tNodeID;
	ref_str							m_caGroupControl;
	flags32							m_flags;							

									CSE_ALifeObject		(LPCSTR caSection);
	virtual							~CSE_ALifeObject	();
	virtual bool					used_ai_locations	() const;
	virtual bool					can_switch_online	() const;
	virtual bool					can_switch_offline	() const;
	virtual bool					can_save			() const;
SERVER_ENTITY_DECLARE_END

class CSE_ALifeGroupAbstract : virtual public CSE_Abstract {
public:
	ALife::OBJECT_VECTOR			m_tpMembers;
	bool							m_bCreateSpawnPositions;
	u16								m_wCount;
	ALife::_TIME_ID					m_tNextBirthTime;

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
	
									CSE_ALifeEventGroup(LPCSTR caSection);
	virtual							~CSE_ALifeEventGroup();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeDynamicObject,CSE_ALifeObject)
	ALife::_TIME_ID					m_tTimeID;
	u64								m_qwSwitchCounter;
	
									CSE_ALifeDynamicObject(LPCSTR caSection);
	virtual							~CSE_ALifeDynamicObject();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeDynamicObjectVisual,CSE_ALifeDynamicObject,CSE_Visual)
									CSE_ALifeDynamicObjectVisual(LPCSTR caSection);
	virtual							~CSE_ALifeDynamicObjectVisual();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeScriptZone,CSE_ALifeDynamicObject,CSE_Shape)
									CSE_ALifeScriptZone		(LPCSTR caSection);
	virtual							~CSE_ALifeScriptZone	();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeLevelChanger,CSE_ALifeScriptZone)
	ALife::_GRAPH_ID				m_tNextGraphID;
	u32								m_dwNextNodeID;
	Fvector							m_tNextPosition;
	Fvector							m_tAngles;
	string32						m_caLevelToChange;
	ref_str							m_caLevelPointToChange;

									CSE_ALifeLevelChanger	(LPCSTR caSection);
	virtual							~CSE_ALifeLevelChanger	();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectPhysic,CSE_ALifeDynamicObjectVisual)
#ifdef _EDITOR
	void __fastcall					OnChangeAnim	(PropValue* sender);
	void __fastcall					OnChooseAnim	(PropValue* sender, AStringVec& lst);
	void __fastcall					OnChooseBone	(PropValue* sender, AStringVec& lst);
#endif
	u32 							type;
	f32 							mass;
    ref_str 						fixed_bones;
	ref_str							startup_animation;
    enum{
    	flActive					= (1<<0),
		flSpawnCopy					= (1<<1),
    };
    Flags8							flags;
									CSE_ALifeObjectPhysic	(LPCSTR caSection);
    virtual 						~CSE_ALifeObjectPhysic	();
	virtual bool					used_ai_locations	() const;
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectHangingLamp,CSE_ALifeDynamicObjectVisual)

#ifdef _EDITOR
    void __fastcall					OnChangeAnim	(PropValue* sender);
    void __fastcall					OnChooseAnim	(PropValue* sender, AStringVec& lst);
    void __fastcall					OnChooseBone	(PropValue* sender, AStringVec& lst);
    void __fastcall					OnChangeFlag	(PropValue* sender);
#endif
    enum{
        flPhysic					= (1<<0),
		flCastShadow				= (1<<1),
		flR1						= (1<<2),
		flR2						= (1<<3),
		flTypeSpot					= (1<<4),
        flPointAmbient				= (1<<5),
    };

    Flags16							flags;
// light color    
    u32								color;
    float							brightness;
    ref_str							color_animator;
// light texture    
	ref_str							light_texture;
// range
    float							range;
	float							m_virtual_size;
// bones&motions
	ref_str							guid_bone;
    ref_str							fixed_bones;
    ref_str							startup_animation;
// spot
	float							spot_cone_angle;
// ambient    
    float							m_ambient_radius;
    float							m_ambient_power;
	ref_str							m_ambient_texture;
// glow    
	ref_str							glow_texture;
	float							glow_radius;
// game
    float							m_health;
	
                                    CSE_ALifeObjectHangingLamp	(LPCSTR caSection);
    virtual							~CSE_ALifeObjectHangingLamp	();
	virtual bool					used_ai_locations	() const;
SERVER_ENTITY_DECLARE_END


SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectProjector,CSE_ALifeDynamicObjectVisual)
									CSE_ALifeObjectProjector	(LPCSTR caSection);
	virtual							~CSE_ALifeObjectProjector	();
	virtual bool					used_ai_locations	() const;
SERVER_ENTITY_DECLARE_END


SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeHelicopter,CSE_ALifeDynamicObjectVisual,CSE_Motion)
#ifdef _EDITOR
	void __fastcall					OnChangeAnim				(PropValue* sender);
	void __fastcall					OnChooseAnim				(PropValue* sender, AStringVec& lst);
#endif
	ref_str							engine_sound;
	ref_str							startup_animation;
									CSE_ALifeHelicopter			(LPCSTR caSection);
	virtual							~CSE_ALifeHelicopter		();
	virtual bool					used_ai_locations	() const;
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeCar,CSE_ALifeDynamicObjectVisual)
									CSE_ALifeCar		(LPCSTR caSection);
	virtual							~CSE_ALifeCar		();
	virtual bool					used_ai_locations	() const;
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectBreakable,CSE_ALifeDynamicObjectVisual)
    float							m_health;
									CSE_ALifeObjectBreakable	(LPCSTR caSection);
	virtual							~CSE_ALifeObjectBreakable	();
SERVER_ENTITY_DECLARE_END

#endif