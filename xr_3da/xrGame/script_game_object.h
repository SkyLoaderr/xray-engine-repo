////////////////////////////////////////////////////////////////////////////
//	Module 		: script_game_object.h
//	Created 	: 25.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script game object class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_space_forward.h"
#include "script_bind_macroses.h"
#include "script_export_space.h"
#include "xr_time.h"
#include "character_info_defs.h"

enum EPdaMsg;
enum ESoundTypes;
enum ETaskState;

namespace ALife {enum ERelationType;};
namespace ScriptEntity {enum EActionType;};
namespace MovementManager { enum EPathType;};
namespace DetailPathManager { enum EDetailPathType;};
namespace SightManager {enum ESightType;};

class NET_Packet;

namespace PatrolPathManager { 
	enum EPatrolStartType;
	enum EPatrolRouteType;
};

namespace MemorySpace {
	struct CMemoryInfo;
	struct CVisibleObject;
	struct CSoundObject;
	struct CHitObject;
	struct CNotYetVisibleObject;
};

namespace MonsterSpace {
	enum EBodyState;
	enum EMovementType;
	enum EMovementDirection;
	enum EDirectionType;
	enum EPathState;
	enum EObjectAction;
	enum EMentalState;
	enum EScriptMonsterMoveAction;
	enum EScriptMonsterSpeedParam;
	enum EScriptMonsterAnimAction;
	enum EScriptMonsterGlobalAction;
	enum EScriptSoundAnim;
	enum EMonsterSounds;
	enum EMonsterHeadAnimType;
	struct SBoneRotation;
};

namespace GameObject {
	enum ECallbackType;
};

class CGameObject;
class CScriptHit;
class CScriptEntityAction;
class CScriptTask;
class CScriptSoundInfo;
class CScriptMonsterHitInfo;
class CScriptBinderObject;
class CAbstractVertexEvaluator;
class CCoverPoint;
class CScriptIniFile;
class CPhysicsShell;
class CHelicopter;
class CHangingLamp;
class CHolderCustom;
struct ScriptCallbackInfo;

class CScriptGameObject {
	mutable CGameObject		*m_game_object;
public:

							CScriptGameObject		(CGameObject *tpGameObject);
	virtual					~CScriptGameObject		();
							operator CObject*		();

	IC		CGameObject			&object				() const;
			CScriptGameObject	*Parent				() const;
			void				Hit					(CScriptHit *tLuaHit);
			int					clsid				() const;
			void				play_cycle			(LPCSTR anim);
	_DECLARE_FUNCTION10	(Position	,	Fvector		);
	_DECLARE_FUNCTION10	(Direction	,	Fvector		);
	_DECLARE_FUNCTION10	(Mass		,	float		);
	_DECLARE_FUNCTION10	(ID			,	u32			);
	_DECLARE_FUNCTION10	(getVisible	,	BOOL		);
	_DECLARE_FUNCTION11	(setVisible	,	void, BOOL	);
	_DECLARE_FUNCTION10	(getEnabled	,	BOOL		);
	_DECLARE_FUNCTION11	(setEnabled	,	void, BOOL	);
	
			LPCSTR				Name				() const;
			shared_str			cName				() const;
			LPCSTR				Section				() const;
	// CInventoryItem
			u32					Cost				() const;
			float				GetCondition		() const;

	// CEntity
	_DECLARE_FUNCTION10	(DeathTime	,	u32		);
	_DECLARE_FUNCTION10	(Armor		,	float	);
	_DECLARE_FUNCTION10	(MaxHealth	,	float	);
	_DECLARE_FUNCTION10	(Accuracy	,	float	);
	_DECLARE_FUNCTION10	(Team		,	int		);
	_DECLARE_FUNCTION10	(Squad		,	int		);
	_DECLARE_FUNCTION10	(Group		,	int		);

			void				Kill				(CScriptGameObject* who);

	// CEntityAlive
	_DECLARE_FUNCTION10	(GetFOV				,			float);
	_DECLARE_FUNCTION10	(GetRange			,			float);
	_DECLARE_FUNCTION10	(GetHealth			,			float);
	_DECLARE_FUNCTION10	(GetPower			,			float);
	_DECLARE_FUNCTION10	(GetSatiety			,			float);
	_DECLARE_FUNCTION10	(GetRadiation		,			float);
	_DECLARE_FUNCTION10	(GetCircumspection	,			float);
	_DECLARE_FUNCTION10	(GetMorale			,			float);

	_DECLARE_FUNCTION11	(SetHealth,			void, float);
	_DECLARE_FUNCTION11	(SetPower,			void, float);
	_DECLARE_FUNCTION11	(SetSatiety,		void, float);
	_DECLARE_FUNCTION11	(SetRadiation,		void, float);
	_DECLARE_FUNCTION11	(SetCircumspection,	void, float);
	_DECLARE_FUNCTION11	(SetMorale,			void, float);

			void				set_fov				(float new_fov);
			void				set_range			(float new_range);
			bool				Alive				() const;
			ALife::ERelationType	GetRelationType	(CScriptGameObject* who);

	// CScriptEntity
	
	_DECLARE_FUNCTION12	(SetScriptControl,	void, bool,				LPCSTR);
	_DECLARE_FUNCTION10	(GetScriptControl	,			bool	);
	_DECLARE_FUNCTION10	(GetScriptControlName,			LPCSTR	);
//	_DECLARE_FUNCTION12	(AddAction,			void,const CScriptEntityAction *,				bool);
//	_DECLARE_FUNCTION10	(GetCurrentAction,const CScriptEntityAction *);
	_DECLARE_FUNCTION10	(GetEnemyStrength, int);
	_DECLARE_FUNCTION10	(can_script_capture, bool);
	

			CScriptEntityAction	*GetCurrentAction	() const;
			void				AddAction			(const CScriptEntityAction *tpEntityAction, bool bHighPriority = false);
			void				ResetActionQueue	();
	// Actor only
			void				SetActorPosition	(Fvector pos);
			void				SetActorDirection	(float dir);
	// CCustomMonster
			bool				CheckObjectVisibility(const CScriptGameObject *tpLuaGameObject);
			bool				CheckTypeVisibility	(const char *section_name);
			LPCSTR				WhoHitName			();
			LPCSTR				WhoHitSectionName	();

			void				ChangeTeam			(u8 team, u8 squad, u8 group);

	// CAI_Stalker
			void				UseObject			(const CScriptGameObject *tpLuaGameObject);
			CScriptGameObject	*GetCurrentWeapon	() const;
			CScriptGameObject	*GetFood			() const;
			CScriptGameObject	*GetMedikit			() const;

	// CAI_Bloodsucker
	
			void				set_invisible			(bool val);
			bool				get_invisible			();
			void				set_manual_invisibility (bool val);
			bool				get_manual_invisibility ();


			template <typename T>
			IC		T	*motivation_action_manager();

	// CProjector
			Fvector				GetCurrentDirection		();

	//////////////////////////////////////////////////////////////////////////
	// CInventoryOwner
	
	//передача порции информации InventoryOwner
			bool				GiveInfoPortion		(LPCSTR info_id);
			bool				DisableInfoPortion	(LPCSTR info_id);
			bool				GiveInfoPortionViaPda	(LPCSTR info_id, CScriptGameObject* pFromWho);
			bool				GiveGameNews		(LPCSTR news, LPCSTR texture_name, int x1, int y1, int x2, int y2, u32 delay);
			void				SetNewsShowTime		(LPCSTR news, int show_time);
	//предикаты наличия/отсутствия порции информации у персонажа
			bool				HasInfo				(LPCSTR info_id);
			bool				DontHasInfo			(LPCSTR info_id);
			xrTime				GetInfoTime			(LPCSTR info_id);
	//работа с заданиями
			ETaskState			GetGameTaskState	(LPCSTR task_id, int objective_num);
			void				SetGameTaskState	(ETaskState state, LPCSTR task_id, int objective_num);

			bool				SendPdaMessage		(EPdaMsg pda_msg, CScriptGameObject* pForWho);
			
			bool				IsTalking			();
			void				StopTalk			();
			void				EnableTalk			();	
			void				DisableTalk			();
			bool				IsTalkEnabled		();

			void				DropItem			(CScriptGameObject* pItem, Fvector pos);
			void				ForEachInventoryItems(const luabind::functor<void> &functor);
			void				TransferItem		(CScriptGameObject* pItem, CScriptGameObject* pForWho);
			void				TransferMoney		(int money, CScriptGameObject* pForWho);
			u32					Money				();
			
			void				SetRelation			(ALife::ERelationType relation, CScriptGameObject* pWhoToSet);
	
			int					GetAttitude			(CScriptGameObject* pToWho);

			int					GetGoodwill			(CScriptGameObject* pToWho);
			void				SetGoodwill			(int goodwill, CScriptGameObject* pWhoToSet);
			void				ChangeGoodwill		(int delta_goodwill, CScriptGameObject* pWhoToSet);

			int					GetCommunityGoodwill	(CScriptGameObject* pToWho);
			void				SetCommunityGoodwill	(int goodwill, CScriptGameObject* pWhoToSet);
			void				ChangeCommunityGoodwill	(int delta_goodwill, CScriptGameObject* pWhoToSet);


			void				SetStartDialog		(LPCSTR dialog_id);
			void				GetStartDialog		();
			void				RestoreDefaultStartDialog();

			void				SwitchToTrade		();
			void				SwitchToTalk		();	
			void				RunTalkDialog		(CScriptGameObject* pToWho);
			void				ActorSleep			(int hours, int minutes);

			void				HideWeapon			();
			void				RestoreWeapon		();


			LPCSTR				ProfileName			();
			LPCSTR				CharacterName		();
			LPCSTR				CharacterCommunity	();
			int					CharacterRank		();
			int					CharacterReputation	();


			void SetCharacterRank			(int);
			void ChangeCharacterRank		(int);
			void SetCharacterReputation		(int);
			void ChangeCharacterReputation	(int);
			void SetCharacterCommunity		(LPCSTR);
		

			u32					GetInventoryObjectCount() const;

			CScriptGameObject	*GetActiveItem		();

			CScriptGameObject	*GetObjectByName	(LPCSTR caObjectName) const;
			CScriptGameObject	*GetObjectByIndex	(int iIndex) const;

			
	// Callbacks			
			void				SetCallback			(GameObject::ECallbackType type, const luabind::functor<void> &functor);
			void				SetCallback			(GameObject::ECallbackType type, const luabind::functor<void> &functor, const luabind::object &object);
			void				SetCallback			(GameObject::ECallbackType type);

			void				set_patrol_extrapolate_callback(const luabind::functor<bool> &functor);
			void				set_patrol_extrapolate_callback(const luabind::functor<bool> &functor, const luabind::object &object);
			void				set_patrol_extrapolate_callback();

			void				set_enemy_callback	(const luabind::functor<bool> &functor);
			void				set_enemy_callback	(const luabind::functor<bool> &functor, const luabind::object &object);
			void				set_enemy_callback	();
	
	//////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////use calback///////////////////////////////////////////////
			void				SetTipText			(LPCSTR tip_text);
			void				SetTipTextDefault	();
			void				SetNonscriptUsable	(bool nonscript_usable);
///////////////////////////////////////////////////////////////////////////////////////////
			void				set_fastcall		(const luabind::functor<bool> &functor, const luabind::object &object);
//////////////////////////////////////////////////////////////////////////

			LPCSTR				GetPatrolPathName	();
			u32					GetAmmoElapsed		();
			void				SetAmmoElapsed		(int ammo_elapsed);
			u32					GetAmmoCurrent		() const;
			void				SetQueueSize		(u32 queue_size);
			const MemorySpace::CHitObject	*GetBestHit			() const;
			const MemorySpace::CSoundObject	*GetBestSound		() const;
			CScriptGameObject	*GetBestEnemy		();
			CScriptGameObject	*GetBestItem		();

	_DECLARE_FUNCTION10			(GetActionCount,u32);
	
			const				CScriptEntityAction	*GetActionByIndex(u32 action_index = 0);

//////////////////////////////////////////////////////////////////////////
// Inventory Owner
//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			Flags32				get_actor_relation_flags	()			const;
			void 				set_actor_relation_flags	(Flags32);
			LPCSTR				snd_character_profile_sect	()			const;

			//////////////////////////////////////////////////////////////////////////
			MemorySpace::CMemoryInfo *memory		(const CScriptGameObject &lua_game_object);
			CScriptGameObject		*best_weapon	();
			void					explode			(u32 level_time);
			CScriptGameObject		*GetEnemy		() const;
			CScriptGameObject		*GetCorpse		() const;
			CScriptSoundInfo		GetSoundInfo	();
			CScriptMonsterHitInfo	GetMonsterHitInfo();
			void					bind_object		(CScriptBinderObject *object);
			CScriptGameObject		*GetCurrentOutfit() const;
			

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
			void				set_body_state		(MonsterSpace::EBodyState body_state);
			void				set_movement_type	(MonsterSpace::EMovementType movement_type);
			void				set_mental_state	(MonsterSpace::EMentalState mental_state);
			void				set_path_type		(MovementManager::EPathType path_type);
			void				set_detail_path_type(DetailPathManager::EDetailPathType detail_path_type);

	MonsterSpace::EBodyState			body_state			() const;
	MonsterSpace::EMovementType			movement_type		() const;
	MonsterSpace::EMovementType			target_movement_type() const;
	MonsterSpace::EMentalState			mental_state		() const;
	MovementManager::EPathType			path_type			() const;
	DetailPathManager::EDetailPathType	detail_path_type	() const;

			u32					add_sound			(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type, LPCSTR bone_name);
			u32					add_sound			(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type);
			u32					add_sound			(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type, LPCSTR bone_name, LPCSTR head_anim);
			void				remove_sound		(u32 internal_type);
			void				set_sound_mask		(u32 sound_mask);
			void				set_sight			(SightManager::ESightType sight_type, const Fvector *vector3d, u32 dwLookOverDelay);
			void				set_sight			(SightManager::ESightType sight_type, bool torso_look, bool path);
			void				set_sight			(SightManager::ESightType sight_type, const Fvector &vector3d, bool torso_look);
			void 				set_sight			(SightManager::ESightType sight_type, const Fvector *vector3d);
			void 				set_sight			(CScriptGameObject *object_to_look, bool torso_look);
			void 				set_sight			(CScriptGameObject *object_to_look, bool torso_look, LPCSTR bone_name);
			void 				set_sight			(const MemorySpace::CMemoryInfo *memory_object, bool	torso_look);
			CHARACTER_RANK_VALUE GetRank			();
			void				play_sound				(u32 internal_type);
			void				play_sound				(u32 internal_type, u32 max_start_time);
			void				play_sound				(u32 internal_type, u32 max_start_time, u32 min_start_time);
			void				play_sound				(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time);
			void				play_sound				(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time);
			void				play_sound				(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time, u32 id);

			void				set_item				(MonsterSpace::EObjectAction object_action);
			void				set_item				(MonsterSpace::EObjectAction object_action, CScriptGameObject *game_object);
			void				set_item				(MonsterSpace::EObjectAction object_action, CScriptGameObject *game_object, u32 queue_size);
			void				set_item				(MonsterSpace::EObjectAction object_action, CScriptGameObject *game_object, u32 queue_size, u32 queue_interval);
			void				set_desired_position	();
			void				set_desired_position	(const Fvector *desired_position);
			void				set_desired_direction	();
			void				set_desired_direction	(const Fvector *desired_direction);
			void				set_node_evaluator		();
			void				set_node_evaluator		(CAbstractVertexEvaluator *node_evaluator);
			void				set_path_evaluator		();
			void				set_path_evaluator		(CAbstractVertexEvaluator *path_evaluator);
			void				set_patrol_path			(LPCSTR path_name, const PatrolPathManager::EPatrolStartType patrol_start_type, const PatrolPathManager::EPatrolRouteType patrol_route_type, bool random);
			void				set_dest_level_vertex_id(u32 level_vertex_id);
			u32					level_vertex_id			() const;
			void				add_animation			(LPCSTR animation, bool hand_usage = true);
			void				clear_animations		();
			int					animation_count			() const;
			CScriptBinderObject	*binded_object			();
			void				set_previous_point		(int point_index);
			void				set_start_point			(int point_index);
			u32					get_current_patrol_point_index();
			bool				path_completed			() const;
			void				patrol_path_make_inactual();
			void				extrapolate_length		(float extrapolate_length);
			float				extrapolate_length		() const;
			void				enable_memory_object	(CScriptGameObject *object, bool enable);
			int					active_sound_count		();
			int					active_sound_count		(bool only_playing);
			const CCoverPoint	*best_cover				(const Fvector &position, const Fvector &enemy_position, float radius, float min_enemy_distance, float max_enemy_distance);
			const CCoverPoint	*safe_cover				(const Fvector &position, float radius, float min_distance);
			CScriptIniFile		*spawn_ini				() const;
			bool				active_zone_contact		(u16 id);

			///
			void				add_restrictions		(LPCSTR out, LPCSTR in);
			void				remove_restrictions		(LPCSTR out, LPCSTR in);
			void				remove_all_restrictions	();
			LPCSTR				in_restrictions			();
			LPCSTR				out_restrictions		();
			bool				accessible_position		(const Fvector &position);
			bool				accessible_vertex_id	(u32 level_vertex_id);
			u32					accessible_nearest		(const Fvector &position, Fvector &result);

			const xr_vector<MemorySpace::CVisibleObject>		&memory_visible_objects	() const;
			const xr_vector<MemorySpace::CSoundObject>			&memory_sound_objects	() const;
			const xr_vector<MemorySpace::CHitObject>			&memory_hit_objects		() const;
			const xr_vector<MemorySpace::CNotYetVisibleObject>	&not_yet_visible_objects() const;
			float				visibility_threshold	() const;
			void				enable_vision			(bool value);
			bool				vision_enabled			() const;
			void				set_sound_threshold		(float value);
			void				restore_sound_threshold	();
			//////////////////////////////////////////////////////////////////////////
			bool				limping					() const;
			//////////////////////////////////////////////////////////////////////////
			void				enable_attachable_item	(bool value);
			bool				attachable_item_enabled	() const;
			//////////////////////////////////////////////////////////////////////////
			// для подключения info_portion для pda
			void				set_character_pda_info (LPCSTR info_id);
			LPCSTR				get_character_pda_info ();
			void				set_pda_info		   (LPCSTR info_id);
			LPCSTR				get_pda_info		   ();
			//////////////////////////////////////////////////////////////////////////
			// CustomZone
			void				EnableAnomaly			();
			void				DisableAnomaly			();
			
	
			// HELICOPTER
			CHelicopter*		get_helicopter			();
			//LAMP
			CHangingLamp*		get_hanging_lamp		();
			CHolderCustom*		get_custom_holder		();
			Fvector				bone_position			(LPCSTR bone_name) const;
			bool				is_body_turning			() const;
			CPhysicsShell*		get_physics_shell		() const;
			bool				weapon_strapped			() const;
			bool				weapon_unstrapped		() const;
			void				eat						(CScriptGameObject *item);
			bool				inside					(const Fvector &position, float epsilon) const;
			bool				inside					(const Fvector &position) const;

			Fvector				head_orientation		() const;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CScriptGameObject)
#undef script_type_list
#define script_type_list save_type_list(CScriptGameObject)