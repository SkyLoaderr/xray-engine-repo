////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALifeH
#define xrServer_Objects_ALifeH

#include "xrServer_Objects.h"
#include "alife_space.h"
#include "phnetstate.h"

#ifdef XRGAME_EXPORTS
	class 	CALifeSimulator;
#endif

#ifdef _EDITOR
	class CSE_ALifeItemWeapon;
#endif

class CSE_ALifeDynamicObject;

class CSE_ALifeSchedulable : public IPureSchedulableObject, virtual public CSE_Abstract {
public:
	CSE_ALifeItemWeapon				*m_tpCurrentBestWeapon;
	CSE_ALifeDynamicObject			*m_tpBestDetector;
	u64								m_schedule_counter;

									CSE_ALifeSchedulable	(LPCSTR caSection);
	virtual							~CSE_ALifeSchedulable	();
	virtual bool					need_update				(CSE_ALifeDynamicObject *object);
#ifdef XRGAME_EXPORTS
	virtual	CSE_ALifeItemWeapon		*tpfGetBestWeapon		(ALife::EHitType		&tHitType,			float		&fHitPower) = 0;
	virtual bool					bfPerformAttack			()											{return(true);};
	virtual	void					vfUpdateWeaponAmmo		()											{};
	virtual	void					vfProcessItems			()											{};
	virtual	void					vfAttachItems			(ALife::ETakeType		tTakeType = ALife::eTakeTypeAll)		{};
	virtual	ALife::EMeetActionType	tfGetActionType			(CSE_ALifeSchedulable	*tpALifeSchedulable,int			iGroupIndex, bool bMutualDetection) = 0;
	virtual bool					bfActive				()															= 0;
	virtual CSE_ALifeDynamicObject	*tpfGetBestDetector		()															= 0;
#endif
};

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

class CSE_ALifeObject : virtual public CSE_Abstract, public CRandom {
public:
	enum {
		flUseSwitches	= u32(1) << 0,
		flSwitchOnline	= u32(1) << 1,
		flSwitchOffline	= u32(1) << 2,
		flInteractive	= u32(1) << 3,
		flVisibleForAI	= u32(1) << 4,
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
	ALife::_STORY_ID				m_story_id;
#ifdef XRGAME_EXPORTS
	CALifeSimulator					*m_alife_simulator;
#endif

#ifdef _EDITOR
	void __fastcall					OnChooseGroupControl(ChooseItemVec& lst);
#endif
									CSE_ALifeObject		(LPCSTR caSection);
	virtual							~CSE_ALifeObject	();
	virtual bool					used_ai_locations	() const;
	virtual bool					can_save			() const;
	virtual bool					can_switch_online	() const;
	virtual bool					can_switch_offline	() const;
	virtual bool					interactive			() const;
			void					can_switch_online	(bool value);
			void					can_switch_offline	(bool value);
			void					interactive			(bool value);
#ifdef XRGAME_EXPORTS
	virtual void					spawn_supplies		();
			CALifeSimulator			&alife				() const;
#endif
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

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeDynamicObject,CSE_ALifeObject)
	ALife::_TIME_ID					m_tTimeID;
	u64								m_switch_counter;
	
									CSE_ALifeDynamicObject(LPCSTR caSection);
	virtual							~CSE_ALifeDynamicObject();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeDynamicObjectVisual,CSE_ALifeDynamicObject,CSE_Visual)
									CSE_ALifeDynamicObjectVisual(LPCSTR caSection);
	virtual							~CSE_ALifeDynamicObjectVisual();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifePHSkeletonObject,CSE_ALifeDynamicObjectVisual)
									CSE_ALifePHSkeletonObject(LPCSTR caSection);
	virtual							~CSE_ALifePHSkeletonObject();

	enum{
		flActive					= (1<<0),
		flSpawnCopy					= (1<<1),
		flSavedData					= (1<<2),
		flNotSave					= (1<<3)
	};
	ref_str							startup_animation;
	Flags8							flags;
	SPHBonesData					saved_bones;
	u16								source_id;//for break only
	
	virtual	void					load					(NET_Packet &tNetPacket);
protected:
	void							data_load				(NET_Packet &tNetPacket);
public:
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

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectPhysic,CSE_ALifePHSkeletonObject)
#ifdef _EDITOR
	void __fastcall					OnChangeAnim	(PropValue* sender);
	void __fastcall					OnChooseAnim	(ChooseItemVec& lst);
	void __fastcall					OnChooseBone	(ChooseItemVec& lst);
#endif
	u32 							type;
	f32 							mass;
    ref_str 						fixed_bones;
									CSE_ALifeObjectPhysic	(LPCSTR caSection);
    virtual 						~CSE_ALifeObjectPhysic	();
	virtual bool					used_ai_locations		() const;
	virtual bool					can_save				() const;
	virtual	void					load					(NET_Packet &tNetPacket);
//	virtual	void					load					(IReader& r){inherited::load(r);}
//	using inherited::load(IReader&);

SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectHangingLamp,CSE_ALifeDynamicObjectVisual)

#ifdef _EDITOR
    void __fastcall					OnChangeAnim	(PropValue* sender);
    void __fastcall					OnChooseAnim	(ChooseItemVec& lst);
    void __fastcall					OnChooseBone	(ChooseItemVec& lst);
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
	void __fastcall					OnChooseAnim				(ChooseItemVec& lst);
#endif
	ref_str							engine_sound;
	ref_str							startup_animation;
									CSE_ALifeHelicopter			(LPCSTR caSection);
	virtual							~CSE_ALifeHelicopter		();
	virtual bool					used_ai_locations			() const;
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
	virtual bool					used_ai_locations	() const;
	virtual bool					can_switch_offline	() const;
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeMountedWeapon,CSE_ALifeDynamicObjectVisual)
									CSE_ALifeMountedWeapon	(LPCSTR caSection);
	virtual							~CSE_ALifeMountedWeapon	();
SERVER_ENTITY_DECLARE_END

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeTeamBaseZone,CSE_ALifeScriptZone)
									CSE_ALifeTeamBaseZone	(LPCSTR caSection);
	virtual							~CSE_ALifeTeamBaseZone	();

	u8								m_team;
SERVER_ENTITY_DECLARE_END

#endif