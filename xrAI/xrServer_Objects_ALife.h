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

#pragma warning(push)
#pragma warning(disable:4005)

#ifdef XRGAME_EXPORTS
	class 	CALifeSimulator;
#endif

class CSE_ALifeItemWeapon;
class CSE_ALifeDynamicObject;

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeSchedulable,IPureSchedulableObject)
	CSE_ALifeItemWeapon				*m_tpCurrentBestWeapon;
	CSE_ALifeDynamicObject			*m_tpBestDetector;
	u64								m_schedule_counter;

									CSE_ALifeSchedulable	(LPCSTR caSection);
	virtual							~CSE_ALifeSchedulable	();
	// we need this to prevent virtual inheritance :-(
	virtual CSE_Abstract			*base					() = 0;
	virtual const CSE_Abstract		*base					() const = 0;
	virtual CSE_Abstract			*init					();
	// end of the virtual inheritance dependant code
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
add_to_type_list(CSE_ALifeSchedulable)
#define script_type_list save_type_list(CSE_ALifeSchedulable)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeGraphPoint,CSE_Abstract)
public:
	ref_str							m_caConnectionLevelName;
	ref_str							m_caConnectionPointName;
	u8								m_tLocations[LOCATION_TYPE_COUNT];

									CSE_ALifeGraphPoint(LPCSTR caSection);
	virtual							~CSE_ALifeGraphPoint();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeGraphPoint)
#define script_type_list save_type_list(CSE_ALifeGraphPoint)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeObject,CSE_Abstract,CRandom)
	enum {
		flUseSwitches	= u32(1) << 0,
		flSwitchOnline	= u32(1) << 1,
		flSwitchOffline	= u32(1) << 2,
		flInteractive	= u32(1) << 3,
		flVisibleForAI	= u32(1) << 4,
		flUsefulForAI	= u32(1) << 5,
	};

public:
	typedef CSE_Abstract inherited;
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
add_to_type_list(CSE_ALifeObject)
#define script_type_list save_type_list(CSE_ALifeObject)

SERVER_ENTITY_DECLARE_BEGIN0(CSE_ALifeGroupAbstract)
	ALife::OBJECT_VECTOR			m_tpMembers;
	bool							m_bCreateSpawnPositions;
	u16								m_wCount;
	ALife::_TIME_ID					m_tNextBirthTime;

									CSE_ALifeGroupAbstract	(LPCSTR caSection);
	virtual							~CSE_ALifeGroupAbstract	();
	virtual	CSE_Abstract			*init					();
	virtual CSE_Abstract			*base					() = 0;
	virtual const CSE_Abstract		*base					() const = 0;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeGroupAbstract)
#define script_type_list save_type_list(CSE_ALifeGroupAbstract)

template<class __A> class CSE_ALifeGroupTemplate : public __A, public CSE_ALifeGroupAbstract {
	typedef __A					inherited1;
	typedef CSE_ALifeGroupAbstract inherited2;
public:
									CSE_ALifeGroupTemplate(LPCSTR caSection) : __A(pSettings->line_exist(caSection,"monster_section") ? pSettings->r_string(caSection,"monster_section") : caSection), CSE_ALifeGroupAbstract(caSection)
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

	virtual CSE_Abstract *init		()
	{
		inherited1::init			();
		inherited2::init			();
		return						(base());
	}

	virtual CSE_Abstract *base		()
	{
		return						(inherited1::base());
	}

	virtual const CSE_Abstract *base() const
	{
		return						(inherited1::base());
	}

	virtual void		FillProps	(LPCSTR pref, PropItemVec& items)
	{
   		inherited1::FillProps		(pref, items);
   		inherited2::FillProps		(pref, items);
	};	
};

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeDynamicObject,CSE_ALifeObject)
	ALife::_TIME_ID					m_tTimeID;
	u64								m_switch_counter;
	
									CSE_ALifeDynamicObject(LPCSTR caSection);
	virtual							~CSE_ALifeDynamicObject();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeDynamicObject)
#define script_type_list save_type_list(CSE_ALifeDynamicObject)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeDynamicObjectVisual,CSE_ALifeDynamicObject,CSE_Visual)
									CSE_ALifeDynamicObjectVisual(LPCSTR caSection);
	virtual							~CSE_ALifeDynamicObjectVisual();
	virtual CSE_Visual* __stdcall	visual					();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeDynamicObjectVisual)
#define script_type_list save_type_list(CSE_ALifeDynamicObjectVisual)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifePHSkeletonObject,CSE_ALifeDynamicObjectVisual,CSE_PHSkeleton)
									CSE_ALifePHSkeletonObject(LPCSTR caSection);
	virtual							~CSE_ALifePHSkeletonObject();
	virtual bool					can_save				() const;
	virtual bool					used_ai_locations		() const;
	virtual	void					load					(NET_Packet &tNetPacket);
public:
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifePHSkeletonObject)
#define script_type_list save_type_list(CSE_ALifePHSkeletonObject)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeSpaceRestrictor,CSE_ALifeDynamicObject,CSE_Shape)
	u8								m_space_restrictor_type;

									CSE_ALifeSpaceRestrictor	(LPCSTR caSection);
	virtual							~CSE_ALifeSpaceRestrictor	();
	virtual ISE_Shape*  __stdcall	shape						();
	virtual bool					can_switch_offline			() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeSpaceRestrictor)
#define script_type_list save_type_list(CSE_ALifeSpaceRestrictor)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeLevelChanger,CSE_ALifeSpaceRestrictor)
	ALife::_GRAPH_ID				m_tNextGraphID;
	u32								m_dwNextNodeID;
	Fvector							m_tNextPosition;
	Fvector							m_tAngles;
	ref_str							m_caLevelToChange;
	ref_str							m_caLevelPointToChange;

									CSE_ALifeLevelChanger	(LPCSTR caSection);
	virtual							~CSE_ALifeLevelChanger	();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeLevelChanger)
#define script_type_list save_type_list(CSE_ALifeLevelChanger)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeObjectPhysic,CSE_ALifeDynamicObjectVisual,CSE_PHSkeleton)
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
add_to_type_list(CSE_ALifeObjectPhysic)
#define script_type_list save_type_list(CSE_ALifeObjectPhysic)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeObjectHangingLamp,CSE_ALifeDynamicObjectVisual,CSE_PHSkeleton)

    void __stdcall 					OnChangeFlag	(PropValue* sender);
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
	virtual	void					load						(NET_Packet &tNetPacket);
	virtual bool					used_ai_locations	() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeObjectHangingLamp)
#define script_type_list save_type_list(CSE_ALifeObjectHangingLamp)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectProjector,CSE_ALifeDynamicObjectVisual)
									CSE_ALifeObjectProjector	(LPCSTR caSection);
	virtual							~CSE_ALifeObjectProjector	();
	virtual bool					used_ai_locations	() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeObjectProjector)
#define script_type_list save_type_list(CSE_ALifeObjectProjector)

SERVER_ENTITY_DECLARE_BEGIN3(CSE_ALifeHelicopter,CSE_ALifeDynamicObjectVisual,CSE_Motion,CSE_PHSkeleton)
	ref_str							engine_sound;
									CSE_ALifeHelicopter			(LPCSTR caSection);
	virtual							~CSE_ALifeHelicopter		();
	virtual	void					load						(NET_Packet &tNetPacket);
	virtual bool					used_ai_locations			() const;
	virtual CSE_Motion*	__stdcall	motion						();

SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeHelicopter)
#define script_type_list save_type_list(CSE_ALifeHelicopter)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeCar,CSE_ALifeDynamicObjectVisual,CSE_PHSkeleton)
	xr_vector<u8>					door_states;
									CSE_ALifeCar		(LPCSTR caSection);
	virtual							~CSE_ALifeCar		();
	virtual bool					used_ai_locations	() const;
	virtual	void					load					(NET_Packet &tNetPacket);
protected:
	virtual void					data_load				(NET_Packet &tNetPacket);
	virtual void					data_save				(NET_Packet &tNetPacket);
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeCar)
#define script_type_list save_type_list(CSE_ALifeCar)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectBreakable,CSE_ALifeDynamicObjectVisual)
    float							m_health;
									CSE_ALifeObjectBreakable	(LPCSTR caSection);
	virtual							~CSE_ALifeObjectBreakable	();
	virtual bool					used_ai_locations	() const;
	virtual bool					can_switch_offline	() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeObjectBreakable)
#define script_type_list save_type_list(CSE_ALifeObjectBreakable)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeObjectClimable,CSE_Shape,CSE_Abstract)
CSE_ALifeObjectClimable	(LPCSTR caSection);
virtual							~CSE_ALifeObjectClimable	();
virtual bool					used_ai_locations	() const;
virtual bool					can_switch_offline	() const;
virtual ISE_Shape*  __stdcall	shape				();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeObjectClimable)
#define script_type_list save_type_list(CSE_ALifeObjectClimable)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeMountedWeapon,CSE_ALifeDynamicObjectVisual)
									CSE_ALifeMountedWeapon	(LPCSTR caSection);
	virtual							~CSE_ALifeMountedWeapon	();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeMountedWeapon)
#define script_type_list save_type_list(CSE_ALifeMountedWeapon)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeTeamBaseZone,CSE_ALifeSpaceRestrictor)
									CSE_ALifeTeamBaseZone	(LPCSTR caSection);
	virtual							~CSE_ALifeTeamBaseZone	();

	u8								m_team;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeTeamBaseZone)
#define script_type_list save_type_list(CSE_ALifeTeamBaseZone)

#pragma warning(pop)

#endif