#include "stdafx.h"
#include "base_monster.h"
#include "../../../level.h"
#include "../../../level_debug.h"
#include "../../../entitycondition.h"

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
		sprintf(text, "Current Enemy = [%s]", *EnemyMan.get_enemy()->cName());
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

	DBG().text(this).add_item("---------------------------------------", x, y+=delta_y, delimiter_color);

	return SDebugInfo(x, y, delta_y, color, delimiter_color);
}
#endif
