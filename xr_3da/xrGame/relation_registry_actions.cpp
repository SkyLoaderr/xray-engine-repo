//////////////////////////////////////////////////////////////////////////
// relation_registry_actions.cpp:	реестр для хранения данных об отношении персонажа к 
//									другим персонажам
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "relation_registry.h"
#include "alife_registry_wrappers.h"

#include "character_community.h"
#include "character_reputation.h"
#include "character_rank.h"

#include "actor.h"
#include "inventoryowner.h"
#include "entity_alive.h"
#include "ai/stalker/ai_stalker.h"
#include "ai/monsters/BaseMonster/base_monster.h"
#include "level.h"

#include "seniority_hierarchy_holder.h"
#include "team_hierarchy_holder.h"
#include "squad_hierarchy_holder.h"
#include "group_hierarchy_holder.h"

//#ifndef _DEBUG
#	define DEFAULT_HIT_BEHAVIOUR
#	define DEFAULT_KILL_BEHAVIOUR
//#endif

//////////////////////////////////////////////////////////////////////////

void RELATION_REGISTRY::Action (CEntityAlive* from, CEntityAlive* to, ERelationAction action)
{
	static CHARACTER_GOODWILL friend_attack_goodwill = pSettings->r_s32(ACTIONS_POINTS_SECT, "friend_attack_goodwill");
	static CHARACTER_GOODWILL neutral_attack_goodwill = pSettings->r_s32(ACTIONS_POINTS_SECT, "neutral_attack_goodwill");
	static CHARACTER_GOODWILL enemy_attack_goodwill = pSettings->r_s32(ACTIONS_POINTS_SECT, "enemy_attack_goodwill");

	static CHARACTER_GOODWILL friend_attack_reputation = pSettings->r_s32(ACTIONS_POINTS_SECT, "friend_attack_reputation");
	static CHARACTER_GOODWILL neutral_attack_reputation = pSettings->r_s32(ACTIONS_POINTS_SECT, "neutral_attack_reputation");
	static CHARACTER_GOODWILL enemy_attack_reputation = pSettings->r_s32(ACTIONS_POINTS_SECT, "enemy_attack_reputation");

	static CHARACTER_GOODWILL friend_kill_goodwill = pSettings->r_s32(ACTIONS_POINTS_SECT, "friend_kill_goodwill");
	static CHARACTER_GOODWILL neutral_kill_goodwill = pSettings->r_s32(ACTIONS_POINTS_SECT, "neutral_kill_goodwill");
	static CHARACTER_GOODWILL enemy_kill_goodwill = pSettings->r_s32(ACTIONS_POINTS_SECT, "enemy_kill_goodwill");
	static CHARACTER_GOODWILL community_member_kill_goodwill = pSettings->r_s32(ACTIONS_POINTS_SECT, "community_member_kill_goodwill");

	static CHARACTER_REPUTATION_VALUE friend_kill_reputation = pSettings->r_s32(ACTIONS_POINTS_SECT, "friend_kill_reputation");
	static CHARACTER_REPUTATION_VALUE neutral_kill_reputation = pSettings->r_s32(ACTIONS_POINTS_SECT, "neutral_kill_reputation");
	static CHARACTER_REPUTATION_VALUE enemy_kill_reputation = pSettings->r_s32(ACTIONS_POINTS_SECT, "enemy_kill_reputation");

	//порог величины хита, после которого регистрируется помощь актера персонажу
	static float help_hit_threshold		= pSettings->r_float(ACTIONS_POINTS_SECT, "help_hit_threshold");
	//(с) мин. время через которое снова будет зарегестрировано сообщение об атаке на персонажа
	static u32	 min_attack_delta_time	= u32(1000.f * pSettings->r_float(ACTIONS_POINTS_SECT, "min_attack_delta_time"));

	static CHARACTER_GOODWILL friend_fight_help_goodwill		= pSettings->r_s32(ACTIONS_POINTS_SECT, "friend_fight_help_goodwill");
	static CHARACTER_GOODWILL neutral_fight_help_goodwill		= pSettings->r_s32(ACTIONS_POINTS_SECT, "neutral_fight_help_goodwill");
	static CHARACTER_GOODWILL enemy_fight_help_goodwill			= pSettings->r_s32(ACTIONS_POINTS_SECT, "enemy_fight_help_goodwill");

	static CHARACTER_REPUTATION_VALUE friend_fight_help_reputation	= pSettings->r_s32(ACTIONS_POINTS_SECT, "friend_fight_help_reputation");
	static CHARACTER_REPUTATION_VALUE neutral_fight_help_reputation = pSettings->r_s32(ACTIONS_POINTS_SECT, "neutral_fight_help_reputation");
	static CHARACTER_REPUTATION_VALUE enemy_fight_help_reputation	= pSettings->r_s32(ACTIONS_POINTS_SECT, "enemy_fight_help_reputation");


	CActor*				actor			= smart_cast<CActor*>				(from);
	CInventoryOwner*	inv_owner_from	= smart_cast<CInventoryOwner*>		(from);
	CAI_Stalker*		stalker_from	= smart_cast<CAI_Stalker*>			(from);
	CAI_Stalker*		stalker			= smart_cast<CAI_Stalker*>			(to);
	//CBaseMonster*	monster	= smart_cast<CBaseMonster*>	(to);

	//вычисление изменения репутации и рейтинга пока ведется 
	//только для актера
	if(!inv_owner_from) return;
	
	ALife::ERelationType relation = ALife::eRelationTypeDummy;
	if(stalker)
	{
		stalker->m_actor_relation_flags.set(action, TRUE);
		relation = GetRelationType(smart_cast<CInventoryOwner*>(stalker), inv_owner_from);
	}

	switch(action)
	{
	case ATTACK:
		{
			if(actor)
			{
				//учитывать ATTACK и FIGHT_HELP, только если прошло время
				//min_attack_delta_time 
				FIGHT_DATA* fight_data_from = FindFight (from->ID());
				if(Device.dwTimeGlobal - fight_data_from->attack_time < min_attack_delta_time)
					break;

				fight_data_from->attack_time = Device.dwTimeGlobal;

				//если мы атаковали персонажа или монстра, который 
				//кого-то атаковал, то мы помогли тому, кто защищался
				FIGHT_DATA* fight_data = FindFight (to->ID());
				if(fight_data && fight_data->total_hit > help_hit_threshold)
				{
#pragma todo("убрать поиск Level().Objects.net_Find, так как могут быть тормоза")
					CAI_Stalker* defending_stalker = smart_cast<CAI_Stalker*>(Level().Objects.net_Find(fight_data->defender));
					CAI_Stalker* attacking_stalker = smart_cast<CAI_Stalker*>(Level().Objects.net_Find(fight_data->attacker));
					if(defending_stalker)
						Action(actor, defending_stalker, attacking_stalker?FIGHT_HELP_HUMAN:FIGHT_HELP_MONSTER);
				}
			}

			if(stalker)
			{
				CHARACTER_GOODWILL delta_goodwill = 0;
				CHARACTER_REPUTATION_VALUE	delta_reputation = 0;
				if(ALife::eRelationTypeEnemy == relation)
				{
					delta_goodwill = enemy_attack_goodwill;
					delta_reputation = enemy_attack_reputation;
				}
				else if(ALife::eRelationTypeNeutral == relation)
				{
					delta_goodwill = neutral_attack_goodwill;
					delta_reputation = neutral_attack_reputation;
				}
				else if(ALife::eRelationTypeFriend == relation)
				{
					delta_goodwill = friend_attack_goodwill;
					delta_reputation = friend_attack_reputation;
				}

				//сталкер при нападении на членов своей же группировки отношения не меняют
				//(считается, что такое нападение всегда случайно)
				bool stalker_attack_team_mate = stalker_from && (stalker_from->Community() == stalker->Community());
				if (delta_goodwill && !stalker_attack_team_mate) {
#ifdef DEFAULT_HIT_BEHAVIOUR
					//изменить отношение ко всем членам атакованой группы (если такая есть)
					//как к тому кого атаковали
					CGroupHierarchyHolder& group = Level().seniority_holder().team(stalker->g_Team()).squad(stalker->g_Squad()).group(stalker->g_Group());
					for(std::size_t i = 0;  i < group.members().size(); i++)
						ChangeGoodwill(group.members()[i]->ID(), from->ID(), delta_goodwill);
						
					ChangeCommunityGoodwill(stalker->Community(), from->ID(), 
						(CHARACTER_GOODWILL)(CHARACTER_COMMUNITY::sympathy(stalker->Community())*(float)delta_goodwill));
#else
					ChangeGoodwill(stalker->ID(), from->ID(), delta_goodwill);
#endif
				}
				if(delta_reputation)
					inv_owner_from->ChangeReputation(delta_reputation);
			}
		}
		break;
	case KILL:
		{
			if(stalker)
			{
				FIGHT_DATA* fight_data_from = FindFight (from->ID());	
				
				//мы помним то, какое отношение обороняющегося к атакующему
				//было перед началом драки
				ALife::ERelationType relation_before_attack = ALife::eRelationTypeDummy;
				if(fight_data_from)
					relation_before_attack = fight_data_from->defender_to_attacker;
				else
					relation_before_attack = relation;

				CHARACTER_GOODWILL			delta_goodwill = 0;
				CHARACTER_REPUTATION_VALUE	delta_reputation = 0;

				if(ALife::eRelationTypeEnemy == relation_before_attack)
				{
					delta_goodwill = enemy_kill_goodwill;
					delta_reputation = enemy_kill_reputation;
				}
				else if(ALife::eRelationTypeNeutral == relation_before_attack)
				{
					delta_goodwill = neutral_kill_goodwill;
					delta_reputation = neutral_kill_reputation;
				}
				else if(ALife::eRelationTypeFriend == relation_before_attack)
				{
					delta_goodwill = friend_kill_goodwill;
					delta_reputation = friend_kill_reputation;
				}

#ifdef DEFAULT_KILL_BEHAVIOUR
				CHARACTER_GOODWILL community_goodwill = 
#endif
					(CHARACTER_GOODWILL)(CHARACTER_COMMUNITY::sympathy(stalker->Community())*
					(float)(delta_goodwill+community_member_kill_goodwill));

				//сталкер при нападении на членов своей же группировки отношения не меняют
				//(считается, что такое нападение всегда случайно)
				bool stalker_kills_team_mate = stalker_from && (stalker_from->Community() == stalker->Community());

				if(delta_goodwill && !stalker_kills_team_mate)
				{
					//изменить отношение ко всем членам группы (если такая есть)
					//убитого, кроме него самого
#ifdef DEFAULT_KILL_BEHAVIOUR
					CGroupHierarchyHolder& group = Level().seniority_holder().team(stalker->g_Team()).squad(stalker->g_Squad()).group(stalker->g_Group());
					for(std::size_t i = 0;  i < group.members().size(); i++)
						if(stalker->ID() != group.members()[i]->ID())
							ChangeGoodwill(group.members()[i]->ID(), from->ID(), delta_goodwill);

					if (community_goodwill)
						ChangeCommunityGoodwill(stalker->Community(), from->ID(), community_goodwill);
#endif
				}

				if(delta_reputation)
					inv_owner_from->ChangeReputation(delta_reputation);

				CHARACTER_RANK_VALUE		delta_rank = 0;
				delta_rank = CHARACTER_RANK::rank_kill_points(CHARACTER_RANK::ValueToIndex(stalker->Rank()));
				if(delta_rank)
					inv_owner_from->ChangeRank(delta_rank);
			}
		}
		break;
	case FIGHT_HELP_HUMAN:
	case FIGHT_HELP_MONSTER:
		{
			if(stalker && stalker->g_Alive())
			{
				CHARACTER_GOODWILL			delta_goodwill = 0;
				CHARACTER_REPUTATION_VALUE	delta_reputation = 0;

				if(ALife::eRelationTypeEnemy == relation)
				{
					delta_goodwill = enemy_fight_help_goodwill;
					delta_reputation = enemy_fight_help_reputation;
				}
				else if(ALife::eRelationTypeNeutral == relation)
				{
					delta_goodwill = neutral_fight_help_goodwill;
					delta_reputation = neutral_fight_help_reputation;
				}
				else if(ALife::eRelationTypeFriend == relation)
				{
					delta_goodwill = friend_fight_help_goodwill;
					delta_reputation = friend_fight_help_reputation;
				}

				if(delta_goodwill)
				{
					//изменить отношение ко всем членам атакованой группы (если такая есть)
					//как к тому кого атаковали
					CGroupHierarchyHolder& group = Level().seniority_holder().team(stalker->g_Team()).squad(stalker->g_Squad()).group(stalker->g_Group());
					for(std::size_t i = 0;  i < group.members().size(); i++)
						ChangeGoodwill(group.members()[i]->ID(), from->ID(), delta_goodwill);

					ChangeCommunityGoodwill(stalker->Community(), from->ID(), 
						(CHARACTER_GOODWILL)(CHARACTER_COMMUNITY::sympathy(stalker->Community())*(float)delta_goodwill));
				}

				if(delta_reputation)
					inv_owner_from->ChangeReputation(delta_reputation);
			}
		}
		break;
	}
}