////////////////////////////////////////////////////////////////////////////
//	Module 		: smart_cast.h
//	Created 	: 17.09.2004
//  Modified 	: 17.09.2004
//	Author		: Dmitriy Iassenev
//	Description : Smart dynamic cast
////////////////////////////////////////////////////////////////////////////

#ifndef SMART_CAST_H
#define SMART_CAST_H

#define  TL_FAST_COMPILATION
#undef   STATIC_CHECK
#include <typelist.h>

//#define PURE_DYNAMIC_CAST
#define PURE_DYNAMIC_CAST_COMPATIBILITY_CHECK

#ifdef PURE_DYNAMIC_CAST
#	define smart_cast dynamic_cast
#else
#	ifndef DECLARE_SPECIALIZATION
#		include "smart_cast_impl0.h"
#	else
#		include "smart_cast_impl2.h"
#		define DO_NOT_DECLARE_TYPE_LIST
#	endif

#	ifdef XRGAME_EXPORTS

		DECLARE_SPECIALIZATION			(CKinematics,		IRender_Visual,		dcast_PKinematics);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CKinematics,		IRender_Visual)

		DECLARE_SPECIALIZATION			(CSkeletonRigid,	IRender_Visual,		dcast_PSkeletonRigid);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CSkeletonRigid,	IRender_Visual)

		DECLARE_SPECIALIZATION			(CSkeletonAnimated,	IRender_Visual,		dcast_PSkeletonAnimated);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CSkeletonAnimated,	IRender_Visual)

		DECLARE_SPECIALIZATION			(IParticleCustom,	IRender_Visual,		dcast_ParticleCustom);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(IParticleCustom,	IRender_Visual)

#		ifndef DO_NOT_DECLARE_TYPE_LIST
			class ENGINE_API ISpatial;
			namespace Feel { class ENGINE_API Sound; }
			typedef Feel::Sound Feel__Sound;
			template <> extern\
			Feel::Sound* SmartDynamicCast::smart_cast<Feel::Sound,ISpatial>(ISpatial *p);\
			add_to_cast_list(Feel__Sound,ISpatial);
#			undef cast_type_list
#			define cast_type_list save_cast_list	(Feel__Sound,		ISpatial)
#		endif

		DECLARE_SPECIALIZATION			(IRenderable,		ISpatial,			dcast_Renderable);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(IRenderable,		ISpatial)

		DECLARE_SPECIALIZATION			(IRender_Light,		ISpatial,			dcast_Light);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(IRender_Light,		ISpatial)

		DECLARE_SPECIALIZATION			(CObject,			ISpatial,			dcast_CObject);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CObject,			ISpatial)

#		ifndef DO_NOT_DECLARE_TYPE_LIST
			class CObject;
			class CGameObject;
			add_to_cast_list						(CGameObject,		CObject);
#			undef cast_type_list
#			define cast_type_list save_cast_list	(CGameObject,		CObject)
#		endif

		DECLARE_SPECIALIZATION	(CEntity,			CGameObject,		cast_entity);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CEntity,			CGameObject)

		DECLARE_SPECIALIZATION	(CEntityAlive,		CGameObject,		cast_entity_alive);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CEntityAlive,		CGameObject)

		DECLARE_SPECIALIZATION	(CInventoryItem,	CGameObject,		cast_inventory_item);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CInventoryItem,	CGameObject)

		DECLARE_SPECIALIZATION	(CInventoryOwner,	CGameObject,		cast_inventory_owner);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CInventoryOwner,	CGameObject)

		DECLARE_SPECIALIZATION	(CActor,			CGameObject,		cast_actor);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CActor,			CGameObject)

		DECLARE_SPECIALIZATION	(CGameObject,		CInventoryOwner,	cast_game_object);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CGameObject,		CInventoryOwner)

		DECLARE_SPECIALIZATION	(CAI_Rat,			CCustomMonster,		dcast_Rat);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CAI_Rat,			CCustomMonster)

		DECLARE_SPECIALIZATION	(CWeapon,			CInventoryItem,			cast_weapon);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CWeapon,			CInventoryItem)

		DECLARE_SPECIALIZATION	(CWeapon,			CGameObject,			cast_weapon);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CWeapon,			CGameObject)

		DECLARE_SPECIALIZATION	(CFoodItem,			CInventoryItem,			cast_food_item);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CFoodItem,			CInventoryItem)

		DECLARE_SPECIALIZATION	(CMissile,			CInventoryItem,			cast_missile);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CMissile,			CInventoryItem)

		DECLARE_SPECIALIZATION	(CUIDragDropItem,	CUIWindow,				cast_drag_drop_item);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CUIDragDropItem,	CUIWindow)

		DECLARE_SPECIALIZATION	(CCustomZone,		CGameObject,			cast_custom_zone);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CCustomZone,		CGameObject)

		DECLARE_SPECIALIZATION	(CWeaponMagazined,	CWeapon,				cast_weapon_magazined);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CWeaponMagazined,	CWeapon)

		DECLARE_SPECIALIZATION	(CHudItem,			CInventoryItem,			cast_hud_item);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CHudItem,			CInventoryItem)

		DECLARE_SPECIALIZATION	(CPhysicsShellHolder,CGameObject,			cast_physics_shell_holder);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CPhysicsShellHolder,CGameObject)

		DECLARE_SPECIALIZATION	(IInputReceiver,	CGameObject,			cast_input_receiver);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(IInputReceiver,	CGameObject)

		DECLARE_SPECIALIZATION	(CWeaponAmmo,		CInventoryItem,			cast_weapon_ammo);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CWeaponAmmo,		CInventoryItem)

		DECLARE_SPECIALIZATION	(CEffectorShot,		CCameraEffector,		cast_effector_shot);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CEffectorShot,		CCameraEffector)

		DECLARE_SPECIALIZATION	(CEffectorZoomInertion,	CCameraEffector,	cast_effector_zoom_inertion);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CEffectorZoomInertion,	CCameraEffector)

		DECLARE_SPECIALIZATION	(CParticlesPlayer,	CGameObject,	cast_particles_player);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CParticlesPlayer,	CGameObject)

		DECLARE_SPECIALIZATION	(CArtefact,			CGameObject,	cast_artefact);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CArtefact,			CGameObject)

		DECLARE_SPECIALIZATION	(CCustomMonster,	CGameObject,	cast_custom_monster);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CCustomMonster,	CGameObject)

		DECLARE_SPECIALIZATION	(CAI_Stalker,		CGameObject,	cast_stalker);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CAI_Stalker,		CGameObject)

		DECLARE_SPECIALIZATION	(CScriptEntity,	CGameObject,		cast_script_entity);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CScriptEntity,	CGameObject)

		DECLARE_SPECIALIZATION	(CSpaceRestrictor,	CGameObject,	cast_restrictor);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CSpaceRestrictor,	CGameObject)

		DECLARE_SPECIALIZATION	(CExplosive,	CGameObject,		cast_explosive);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CExplosive,	CGameObject)

		DECLARE_SPECIALIZATION	(CGameObject,	CAttachmentOwner,	cast_game_object);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CGameObject,	CAttachmentOwner)

		DECLARE_SPECIALIZATION	(CGameObject,	CInventoryItem,		cast_game_object);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CGameObject,	CInventoryItem)

		DECLARE_SPECIALIZATION	(CAttachableItem,	CGameObject,	cast_attachable_item);
#		undef cast_type_list
#		define cast_type_list save_cast_list	(CAttachableItem,	CGameObject)

#	endif
	
	DECLARE_SPECIALIZATION	(CSE_Abstract,		CSE_ALifeInventoryItem,	cast_abstract);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_Abstract,		CSE_ALifeInventoryItem)

	DECLARE_SPECIALIZATION	(CSE_Abstract,		CSE_ALifeTraderAbstract,	cast_abstract);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_Abstract,		CSE_ALifeTraderAbstract)

	DECLARE_SPECIALIZATION	(CSE_Abstract, CSE_ALifeGroupAbstract,	cast_abstract);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_Abstract, CSE_ALifeGroupAbstract)

	DECLARE_SPECIALIZATION	(CSE_Abstract,	CSE_ALifeSchedulable,	cast_abstract);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_Abstract,	CSE_ALifeSchedulable)

	DECLARE_SPECIALIZATION	(CSE_ALifeGroupAbstract, CSE_Abstract,		cast_group_abstract);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_ALifeGroupAbstract, CSE_Abstract)

	DECLARE_SPECIALIZATION	(CSE_ALifeSchedulable,	CSE_Abstract,		cast_schedulable);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_ALifeSchedulable,	 CSE_Abstract)

	DECLARE_SPECIALIZATION	(CSE_ALifeInventoryItem,	CSE_Abstract,	cast_inventory_item);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_ALifeInventoryItem,	CSE_Abstract)

	DECLARE_SPECIALIZATION	(CSE_ALifeTraderAbstract,	CSE_Abstract,	cast_trader_abstract);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_ALifeTraderAbstract,	CSE_Abstract)

	DECLARE_SPECIALIZATION	(CSE_ALifeObject,	CSE_Abstract,			cast_alife_object);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_ALifeObject,	CSE_Abstract)

	DECLARE_SPECIALIZATION	(CSE_ALifeDynamicObject,	CSE_Abstract,	cast_alife_dynamic_object);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_ALifeDynamicObject,	CSE_Abstract)

	DECLARE_SPECIALIZATION	(CSE_ALifeItemAmmo, CSE_Abstract,			cast_item_ammo);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_ALifeItemAmmo, CSE_Abstract)

	DECLARE_SPECIALIZATION	(CSE_ALifeItemWeapon, CSE_Abstract,			cast_item_weapon);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_ALifeItemWeapon, CSE_Abstract)

	DECLARE_SPECIALIZATION	(CSE_ALifeItemDetector, CSE_Abstract,			cast_item_detector);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_ALifeItemDetector, CSE_Abstract)

	DECLARE_SPECIALIZATION	(CSE_ALifeMonsterAbstract,	CSE_Abstract,	cast_monster_abstract);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_ALifeMonsterAbstract,	CSE_Abstract)

	DECLARE_SPECIALIZATION	(CSE_ALifeHumanAbstract,	CSE_Abstract,	cast_human_abstract);
#	undef cast_type_list
#	define cast_type_list save_cast_list	(CSE_ALifeHumanAbstract,	CSE_Abstract)

#	ifndef DO_NOT_DECLARE_TYPE_LIST
#		include "smart_cast_impl1.h"
#	endif
#endif

#endif //SMART_CAST_H