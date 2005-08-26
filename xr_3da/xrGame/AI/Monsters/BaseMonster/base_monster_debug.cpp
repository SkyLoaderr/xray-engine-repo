#include "stdafx.h"
#include "base_monster.h"
#include "../../../level.h"
#include "../../../level_debug.h"
#include "../../../entitycondition.h"
#include "../../../ai_debug.h"
#include "../state_defs.h"
#include "../state_manager.h"

#ifdef DEBUG
CBaseMonster::SDebugInfo CBaseMonster::show_debug_info()
{
	if (m_show_debug_info == 0) {
		DBG().text(this).clear();
		return SDebugInfo();
	}

	float y				= 200;
	float x				= (m_show_debug_info == 1) ? 40.f : float(::Render->getTarget()->get_width() / 2) + 40.f;
	const float delta_y	= 12;

	string256	text;

	u32			color			= D3DCOLOR_XRGB(0,255,0);
	u32			delimiter_color	= D3DCOLOR_XRGB(0,0,255);

	DBG().text(this).clear	 ();

	DBG().text(this).add_item("---------------------------------------", x, y+=delta_y, delimiter_color);

	sprintf(text, "-- Monster : [%s]  Current Time = [%u]", *cName(), Device.dwTimeGlobal);
	DBG().text(this).add_item(text, x, y+=delta_y, color);
	DBG().text(this).add_item("-----------   PROPERTIES   ------------", x, y+=delta_y, delimiter_color);

	sprintf(text, "Health = [%f]", conditions().GetHealth());
	DBG().text(this).add_item(text,										 x, y+=delta_y, color);

	sprintf(text, "Morale = [%f]", Morale.get_morale());
	DBG().text(this).add_item(text,										 x, y+=delta_y, color);


	DBG().text(this).add_item("-----------   MEMORY   ----------------", x, y+=delta_y, delimiter_color);

	if (EnemyMan.get_enemy()) {
		sprintf(text, "Current Enemy = [%s] Time Seen [%u]", *EnemyMan.get_enemy()->cName(), EnemyMan.get_enemy_time_last_seen());
	} else 
		sprintf(text, "Current Enemy = [NONE]");

	DBG().text(this).add_item(text,										 x, y+=delta_y, color);

	if (CorpseMan.get_corpse()) {
		sprintf(text, "Current Corpse = [%s]", *CorpseMan.get_corpse()->cName());
	} else 
		sprintf(text, "Current Corpse = [NONE]");

	DBG().text(this).add_item(text,										 x, y+=delta_y, color);

	// Sound
	if (SoundMemory.IsRememberSound()) {
		SoundElem	sound_elem;
		bool		dangerous_sound;
		SoundMemory.GetSound(sound_elem, dangerous_sound);

		string128	s_type;

		switch(sound_elem.type){
			case WEAPON_SHOOTING:			strcpy(s_type,"WEAPON_SHOOTING"); break;
			case MONSTER_ATTACKING:			strcpy(s_type,"MONSTER_ATTACKING"); break;
			case WEAPON_BULLET_RICOCHET:	strcpy(s_type,"WEAPON_BULLET_RICOCHET"); break;
			case WEAPON_RECHARGING:			strcpy(s_type,"WEAPON_RECHARGING"); break;

			case WEAPON_TAKING:				strcpy(s_type,"WEAPON_TAKING"); break;
			case WEAPON_HIDING:				strcpy(s_type,"WEAPON_HIDING"); break;
			case WEAPON_CHANGING:			strcpy(s_type,"WEAPON_CHANGING"); break;
			case WEAPON_EMPTY_CLICKING:		strcpy(s_type,"WEAPON_EMPTY_CLICKING"); break;

			case MONSTER_DYING:				strcpy(s_type,"MONSTER_DYING"); break;
			case MONSTER_INJURING:			strcpy(s_type,"MONSTER_INJURING"); break;
			case MONSTER_WALKING:			strcpy(s_type,"MONSTER_WALKING"); break;
			case MONSTER_JUMPING:			strcpy(s_type,"MONSTER_JUMPING"); break;
			case MONSTER_FALLING:			strcpy(s_type,"MONSTER_FALLING"); break;
			case MONSTER_TALKING:			strcpy(s_type,"MONSTER_TALKING"); break;

			case DOOR_OPENING:				strcpy(s_type,"DOOR_OPENING"); break;
			case DOOR_CLOSING:				strcpy(s_type,"DOOR_CLOSING"); break;
			case OBJECT_BREAKING:			strcpy(s_type,"OBJECT_BREAKING"); break;
			case OBJECT_FALLING:			strcpy(s_type,"OBJECT_FALLING"); break;
			case NONE_DANGEROUS_SOUND:		strcpy(s_type,"NONE_DANGEROUS_SOUND"); break;
		}

		sprintf(text,"Sound: type=[%s] time=[%u] power=[%.3f] val=[%i]", s_type, sound_elem.time, sound_elem.power, sound_elem.value);
	} else 
		sprintf(text, "Sound: NONE");

	DBG().text(this).add_item(text,										 x, y+=delta_y, color);

	// Hit
	if (HitMemory.is_hit()) {
		if (HitMemory.get_last_hit_object()) {
			sprintf(text,"Hit Info: object=[%s] time=[%u]", *(HitMemory.get_last_hit_object()->cName()), HitMemory.get_last_hit_time());
		} else {
			sprintf(text,"Hit Info: object=[NONE] time=[%u]", HitMemory.get_last_hit_time());
		}
	} else 
		sprintf(text, "Hit Info: NONE");

	DBG().text(this).add_item(text,										 x, y+=delta_y, color);

	DBG().text(this).add_item("-----------   MOVEMENT   ------------", x, y+=delta_y, delimiter_color);

	sprintf(text, "Actual = [%u] Enabled = [%u]",			 control().path_builder().actual(), control().path_builder().enabled());
	DBG().text(this).add_item(text,										x, y+=delta_y, color);
	
	sprintf(text, "Speed: Linear = [%.3f] Angular = [%.3f]", control().movement().velocity_current(), 0.f);
	DBG().text(this).add_item(text,										x, y+=delta_y, color);
	
	DBG().text(this).add_item("---------------------------------------", x, y+=delta_y, delimiter_color);

	return SDebugInfo(x, y, delta_y, color, delimiter_color);
}

void CBaseMonster::debug_fsm()
{
	if (!psAI_Flags.test(aiMonsterDebug)) {
		DBG().object_info(this,this).clear ();
		return;
	}
	
	EMonsterState state = StateMan->get_state_type();
	
	string128 st;

	switch (state) {
		case eStateRest_WalkGraphPoint:					sprintf(st,"Rest :: Walk Graph");			break;
		case eStateRest_Idle:							sprintf(st,"Rest :: Idle");					break;
		case eStateRest_Fun:							sprintf(st,"Rest :: Fun");					break;
		case eStateRest_Sleep:							sprintf(st,"Rest :: Sleep");				break;
		case eStateEat_CorpseApproachRun:				sprintf(st,"Eat :: Corpse Approach Run");	break;
		case eStateEat_CorpseApproachWalk:				sprintf(st,"Eat :: Corpse Approach Walk");	break;
		case eStateEat_CheckCorpse:						sprintf(st,"Eat :: Check Corpse");			break;
		case eStateEat_Eat:								sprintf(st,"Eat :: Eating");				break;
		case eStateEat_WalkAway:						sprintf(st,"Eat :: Walk Away");				break;
		case eStateEat_Rest:							sprintf(st,"Eat :: Rest After Meal");		break;
		case eStateEat_Drag:							sprintf(st,"Eat :: Drag");					break;
		case eStateAttack_Run:							sprintf(st,"Attack :: Run");				break;
		case eStateAttack_Melee:						sprintf(st,"Attack :: Melee");				break;
		case eStateAttack_RunAttack:					sprintf(st,"Attack :: Run Attack");			break;
		case eStateAttack_RunAway:						sprintf(st,"Attack :: Run Away");			break;
		case eStateAttack_FindEnemy:					sprintf(st,"Attack :: Find Enemy");			break;
		case eStateAttack_Steal:						sprintf(st,"Attack :: Steal");				break;
		case eStateAttack_AttackHidden:					sprintf(st,"Attack :: Attack Hidden");		break;
		case eStateAttack_AttackRat:					sprintf(st,"Attack :: Attack Rat");			break;
		
		case eStateAttack_HideInCover:					sprintf(st,"Attack :: Hide In Cover");		break;
		case eStateAttack_MoveOut:						sprintf(st,"Attack :: Move Out From Cover");break;
		case eStateAttack_CampInCover:					sprintf(st,"Attack :: Camp In Cover");		break;

		case eStatePanic_Run:							sprintf(st,"Panic :: Run Away");				break;
		case eStatePanic_FaceUnprotectedArea:			sprintf(st,"Panic :: Face Unprotected Area");	break;
		case eStateHitted_Hide:							sprintf(st,"Hitted :: Hide");					break;
		case eStateHitted_MoveOut:						sprintf(st,"Hitted :: MoveOut");				break;
		case eStateHearDangerousSound_Hide:				sprintf(st,"Dangerous Snd :: Hide");			break;
		case eStateHearDangerousSound_FaceOpenPlace:	sprintf(st,"Dangerous Snd :: FaceOpenPlace");	break;
		case eStateHearDangerousSound_StandScared:		sprintf(st,"Dangerous Snd :: StandScared");		break;
		case eStateHearInterestingSound_MoveToDest:		sprintf(st,"Interesting Snd :: MoveToDest");	break;
		case eStateHearInterestingSound_LookAround:		sprintf(st,"Interesting Snd :: LookAround");	break;
		case eStateControlled_Follow_Wait:				sprintf(st,"Controlled :: Follow : Wait");			break;
		case eStateControlled_Follow_WalkToObject:		sprintf(st,"Controlled :: Follow : WalkToObject");	break;
		case eStateControlled_Attack:					sprintf(st,"Controlled :: Attack");					break;
		case eStateThreaten:							sprintf(st,"Threaten :: ");							break;
		case eStateFindEnemy_Run:						sprintf(st,"Find Enemy :: Run");							break;
		case eStateFindEnemy_LookAround_MoveToPoint:	sprintf(st,"Find Enemy :: Look Around : Move To Point");	break;
		case eStateFindEnemy_LookAround_LookAround:		sprintf(st,"Find Enemy :: Look Around : Look Around");		break;
		case eStateFindEnemy_LookAround_TurnToPoint:	sprintf(st,"Find Enemy :: Look Around : Turn To Point");	break;
		case eStateFindEnemy_Angry:						sprintf(st,"Find Enemy :: Angry");							break;
		case eStateFindEnemy_WalkAround:				sprintf(st,"Find Enemy :: Walk Around");					break;
		case eStateSquad_Rest_Idle:						sprintf(st,"Squad :: Rest : Idle");					break;
		case eStateSquad_Rest_WalkAroundLeader:			sprintf(st,"Squad :: Rest : WalkAroundLeader");		break;
		case eStateSquad_RestFollow_Idle:				sprintf(st,"Squad :: Follow Leader : Idle");		break;
		case eStateSquad_RestFollow_WalkToPoint:		sprintf(st,"Squad :: Follow Leader : WalkToPoint");	break;
		case eStateCustom_Vampire:						sprintf(st,"Attack :: Vampire");			break;
		case eStateBurerAttack_Tele:					sprintf(st,"Attack :: Telekinesis");			break;
		case eStateBurerAttack_Gravi:					sprintf(st,"Attack :: Gravi Wave");				break;
		case eStateBurerAttack_RunAround:				sprintf(st,"Attack :: Run Around");			break;
		case eStateBurerAttack_FaceEnemy:				sprintf(st,"Attack :: Face Enemy");			break;
		case eStateBurerAttack_Melee:					sprintf(st,"Attack :: Melee");				break;
		case eStateBurerScanning:						sprintf(st,"Attack :: Scanning");			break;
		case eStateCustomMoveToRestrictor:				sprintf(st,"Moving To Restrictor :: Position not accessible");	break;
		case eStateUnknown:								sprintf(st,"Unknown State :: ");			break;
		default:										sprintf(st,"Undefined State ::");			break;
	}
	
	DBG().object_info(this,this).remove_item (u32(0));
	DBG().object_info(this,this).remove_item (u32(1));
	DBG().object_info(this,this).add_item	 (*cName(), D3DCOLOR_XRGB(255,0,0), 0);
	DBG().object_info(this,this).add_item	 (st, D3DCOLOR_XRGB(255,0,0), 1);
}


#endif
