////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrMessages.h"
#include "xrServer_Object_Base.h"
#include "phnetstate.h"

#pragma warning(push)
#pragma warning(disable:4005)

//------------------------------------------------------------------------------
// Version history
//------------------------------------------------------------------------------
// 10 - xrSE_ALifeObjectPhysic			appended with property 'fixed_bone'
// 11 - xrSE_ALifeObjectHangingLamp		appended with property 'spot_brightness'
// 12 - xrSE_ALifeObjectHangingLamp		appended with property 'flags'
// 13 - xrSE_ALifeObjectHangingLamp		appended with property 'mass'
// 14 - xrSE_ALifeObjectPhysic			inherited from CSE_ALifeObject
// 15 - xrSE_ALifeAnomalousZone			inherited calls from CSE_ALifeDynamicObject
// 16 - xrSE_ALifeObjectPhysic			inherited from CSE_ALifeDynamicObject
// 17 - xrSE_...						inherited from CSE_Visual for smart Level Editor
// 18 - xrSE_ALifeObjectHangingLamp		appended with 'startup_animation'
// 19 - xrSE_Teamed						didn't save health parameter
// 20 - xrSE_ALife...					saving vectors in UPDATE_Read/UPDATE_Write changed to STATE_Read/STATE_Write
// 21 -	GLOBAL CLASS HIERARCHY UPDATE
// 22 - CSE_AnomalousZone				appended with a artefact spawns
// 23 - CSE_ALifeObject					appended with a spawn ID
// 24 - CSE_ALifeObject					appended with a group control
// 25 - CSE_ALifeObject					changed type of the property probability from u8 to float
// 26 - CSE_AnomalousZone				appended with artefact spawn information
// 27 - CSE_AnomalousZone				weights changed type from u32 to float
// 28 - CSE_AnomalousZone				appended with an anomalous zone type
// 29 - CSE_ALifeObjectPhysic			appended with an animation property
// 30 - CSE_ALifeTrader					appended with an ordered artefacts property
// 31 - CSE_ALifeTrader					appended with a supplies property
// 32 - CSE_ALifeDynamicObjectVisual	the only this class saves and loads visual object
// 33 - CSE_ALifeGraphPoint and CSE_ALifeLevelChanger	level id changed to level name
// 34 - CSE_ALifeLevelPoint and CSE_ALifeLevelChanger	appended several _new properties
// 35 - CSE_ALifeTrader					artefact order structures changed
// 36 - CSE_ALifeTrader					appended with the organization identifier property
//		and CSE_ALifeHumanAbstract		appended with known_traders property
//		and CSE_ALifeTask				appended with try count property
//		and CSE_ALifePersonalTask		no more exists
// 37 - бинокль имеет ServerEntity не CSE_ALifeItemWeapon, а CSE_ALifeItem
// 38 - CSE_ALifeHumanAbstract			appended with the equipment and weapon preferences
// 39 - CSE_ALifeAnomalousZone			appended with the start power property
// 40 - CSE_ALifeObjectPhysic			appended with activate flag
//		and CSE_ALifeItemWeapon			appended with weapon addons state flag
// 41 - CSE_ALifeItemTorch				appended glow
// 42 - CSE_ALifeItemTorch				appended guid_bone
// 43 - CSE_ALifeObjectHangingLamp		appended glow_texture and glow_radius
// 44 - xrSE_ALifeObjectHangingLamp		appended with property 'fixed bones'
// 45 - xrSE_ALifeObjectHangingLamp		appended with property 'health'
// 46 - xrSE_ALifeObjectSearchLight		appended with property 'guid_bone', 
//										appended with property 'rotation_bone'
// 47 - CSE_ALifeItemWeapon				appended with ammo type index
// 48 - CSE_ALifeObjectSearchlight		appended with property 'cone_bone'
// 49 - CSE_ALifeObjectHangingLamp		remove some property
// 50 - CSE_ALifeObject					appended with ALife flags property
// 51 - CSE_ALifeItemBolt				new class based on CSE_ALifeDynamicObject and CSE_AlifeInventoryItem
// 52 - CSE_ALifeItemExplosive			new class based on CSE_ALifeItem
// 53 - CSE_AlifeInventoryItem			appended with property 'condition'
// 54 - CSE_AlifeLevelChanger			property 'angles' changed from float to fvector
// 55 - CSE_AlifeItemCar->CSE_ALifeCar	heritage changed
// 56 - CSE_ALifeObjectHangingLamp		restore removed props + appended some new
// 57 - CSE_ALifeObjectPhysic			appended with source_id
// 58 - CSE_ALifeObject					appended with ini_string
// 59 - CSE_ALifeItemPDA				appended with m_original_owner
// 60 - CSE_AlifeInventoryItem			appended with property m_eItemPlace
// 61 - CSE_AlifeObjectPhysic			appended with bones_mask, root_bone
// 62 - CSE_AlifeObject					appended with m_story_id
// 63 - CSE_AlifeTraderAbstract			remove bug with money
// 64 - CSE_ALifeObjectPhysic			flags, source_id, saved_bones moved to CSE_AlifePHSkeletonObject
// 65 - CSE_ALifeObjectPhysic			startup_anim moved to CSE_AlifePHSkeletonObject
// 66 - CSE_ALifeObjectPhysic			CSE_ALifeCar - heritage changed
// 67 - CSE_ALifeCustomZone				new class appended, heritage changed
// 68 - CSE_ALifeHumanStalker,				
//		CSE_ALifeMonsterBase			new class appended, heritage changed
// 69 -	object broker changed from this version
//		CSE_ALifeObjectHangingLamp,				
//		CSE_ALifeHelicopter				heritage changed
// 70 -	CSE_Abstract					appended with m_script_version, script version support
// 71 -	CSE_Abstract					appended with m_client_data, ability to save/load client data
// 72 -	CSE_AlifeInventoryItem			removed property m_eItemPlace, added flag
// 73 -	CSE_AlifeMonsterAbstract		appended with property m_space_restrictors
// 74 -	CSE_AlifeMonsterAbstract		appended with property m_in_space_restrictors
// 75 -	CSE_AlifeSpaceRestrictor		new class added, hierarchy changed
// 76 - CSE_AlifeTraderAbstract			appended with m_iSpecificCharacter
// 77 - CSE_AlifeTraderAbstract			added CSE_ObjectClimable
// 78 - CSE_AlifeTraderAbstract			appended with flags for infinite ammo
// 79 - CSE_AlifeAnomalousZone			appended with power properties (3)
// 80 - CSE_Abstract					m_tSpawnID property moved from CSE_ALifeObject to CSE_Abstract
// 81 - CSE_SpawnGroup					appended with 2 properties
//------------------------------------------------------------------------------
#define SPAWN_VERSION	u16(81)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_Shape,ISE_Shape,CShapeData)
public:
	void							cform_read		(NET_Packet& P);
	void							cform_write		(NET_Packet& P);
									CSE_Shape		();
	virtual							~CSE_Shape		();
	virtual ISE_Shape*  __stdcall	shape			() = 0;
	virtual void __stdcall			assign_shapes	(CShapeData::shape_def* shapes, u32 cnt);
};
add_to_type_list(CSE_Shape)
#define script_type_list save_type_list(CSE_Shape)

SERVER_ENTITY_DECLARE_BEGIN(CSE_Spectator,CSE_Abstract)
									CSE_Spectator	(LPCSTR caSection);
	virtual							~CSE_Spectator	();
	virtual u8						g_team			();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_Spectator)
#define script_type_list save_type_list(CSE_Spectator)

SERVER_ENTITY_DECLARE_BEGIN(CSE_Target,CSE_Abstract)
									CSE_Target		(LPCSTR caSection);
	virtual							~CSE_Target		();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_Target)
#define script_type_list save_type_list(CSE_Target)

SERVER_ENTITY_DECLARE_BEGIN(CSE_TargetAssault,CSE_Target)
									CSE_TargetAssault(LPCSTR caSection);
	virtual							~CSE_TargetAssault();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_TargetAssault)
#define script_type_list save_type_list(CSE_TargetAssault)

SERVER_ENTITY_DECLARE_BEGIN(CSE_Target_CS_Base,CSE_Target)
	float							radius;
	u8								s_team;
									CSE_Target_CS_Base(LPCSTR caSection);
	virtual							~CSE_Target_CS_Base();
	virtual u8						g_team			();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_Target_CS_Base)
#define script_type_list save_type_list(CSE_Target_CS_Base)

SERVER_ENTITY_DECLARE_BEGIN(CSE_Target_CS_Cask,CSE_Target)
	shared_str							s_Model;
									CSE_Target_CS_Cask(LPCSTR caSection);
	virtual							~CSE_Target_CS_Cask();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_Target_CS_Cask)
#define script_type_list save_type_list(CSE_Target_CS_Cask)

SERVER_ENTITY_DECLARE_BEGIN(CSE_Target_CS,CSE_Target)
	shared_str							s_Model;
									CSE_Target_CS	(LPCSTR caSection);
	virtual							~CSE_Target_CS	();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_Target_CS)
#define script_type_list save_type_list(CSE_Target_CS)

SERVER_ENTITY_DECLARE_BEGIN(CSE_Temporary,CSE_Abstract)
	u32								m_tNodeID;
									CSE_Temporary	(LPCSTR caSection);
	virtual							~CSE_Temporary	();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_Temporary)
#define script_type_list save_type_list(CSE_Temporary)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_Event,CSE_Shape,CSE_Abstract)
	struct tAction
	{
		u8		type;
		u16		count;
		u64		cls;
		LPSTR	event;
	};
	xr_vector<tAction>				Actions;

									CSE_Event		(LPCSTR caSection);
	virtual							~CSE_Event		();
			void					Actions_clear	();
	virtual ISE_Shape*  __stdcall	shape			();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_Event)
#define script_type_list save_type_list(CSE_Event)

SERVER_ENTITY_DECLARE_BEGIN(CSE_SpawnGroup,CSE_Abstract)
public:
	enum ESpawnGroupFlags {
		flSpawnGroupActive				= u32(1 << 0),
		flSpawnGroupSpawnOnSurgeOnly	= u32(1 << 1),
		flSpawnGroupSequential			= u32(1 << 2),
		flSpawnGroupIfNotDestroyed		= u32(1 << 3),
	};

public:
	float								m_spawn_probability;
	u32									m_spawn_interval;
	Flags32								m_flags;

										CSE_SpawnGroup	(LPCSTR caSection);
	virtual								~CSE_SpawnGroup	();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_SpawnGroup)
#define script_type_list save_type_list(CSE_SpawnGroup)

SERVER_ENTITY_DECLARE_BEGIN0(CSE_PHSkeleton)
								CSE_PHSkeleton(LPCSTR caSection);
virtual							~CSE_PHSkeleton();

enum{
	flActive					= (1<<0),
	flSpawnCopy					= (1<<1),
	flSavedData					= (1<<2),
	flNotSave					= (1<<3),
};
	Flags8							_flags;
	SPHBonesData					saved_bones;
	u16								source_id;//for break only
	virtual	void					load					(NET_Packet &tNetPacket);
protected:
	virtual void					data_load				(NET_Packet &tNetPacket);
	virtual void					data_save				(NET_Packet &tNetPacket);
public:
	SERVER_ENTITY_DECLARE_END
		add_to_type_list(CSE_PHSkeleton)
#define script_type_list save_type_list(CSE_PHSkeleton)

#ifndef AI_COMPILER
extern CSE_Abstract	*F_entity_Create	(LPCSTR caSection);
#endif

#pragma warning(pop)