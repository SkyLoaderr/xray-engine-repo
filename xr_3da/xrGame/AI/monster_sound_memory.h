#pragma once

#include "ai_monster_defs.h"

class CAI_Biting;

typedef enum {
	WEAPON_SHOOTING = 0,
	MONSTER_ATTACKING,
	WEAPON_BULLET_RICOCHET,
	WEAPON_RECHARGING,

	WEAPON_TAKING,
	WEAPON_HIDING,			
	WEAPON_CHANGING,
	WEAPON_EMPTY_CLICKING,

	MONSTER_DYING,	
	MONSTER_INJURING,
	MONSTER_WALKING,			
	MONSTER_JUMPING,			
	MONSTER_FALLING,			
	MONSTER_TALKING,			

	DOOR_OPENING,		
	DOOR_CLOSING,		
	OBJECT_BREAKING,	
	OBJECT_FALLING,
	NONE_DANGEROUS_SOUND
} TSoundDangerValue;


#define FACTOR_SOUND_TYPE	int(8)
#define FACTOR_DISTANCE		int(1)
#define FACTOR_DELTA_TIME	int(2)
#define FACTOR_SOUND_POWER	int(50)


typedef struct tagSoundElement
{
	const CObject		*who;
	TSoundDangerValue	type;
	Fvector				position;		// ������� �����, �� �������, ��������� ����
	float				power;
	TTime				time;			// ����� ����������� �����

	int					value;			// ��������� �������� ������� �����		

	tagSoundElement() {  
		who = 0; type = NONE_DANGEROUS_SOUND; position.set(0,0,0); power = 0.f; time = 0; value = 0;
	}
	bool operator < (const tagSoundElement &s) const  { 
		return (value < s.value);
	}
	IC void SetConvert(const CObject* who, int eType, const Fvector &position, float power, TTime time) {
		this->who = who; type = ConvertSoundType((ESoundTypes)eType); this->position = position; this->power = power; this->time = time;
	}
	TSoundDangerValue ConvertSoundType(ESoundTypes stype);

	void CalcValue(TTime cur_time, Fvector cur_pos) {
		value = FACTOR_SOUND_TYPE * u32(NONE_DANGEROUS_SOUND - WEAPON_SHOOTING) - iFloor(FACTOR_DISTANCE * cur_pos.distance_to(position)) - FACTOR_DELTA_TIME * iFloor(float((cur_time - time) / 1000)) + FACTOR_SOUND_POWER * iFloor(power);
	}

} SoundElem;


class CMonsterSoundMemory {
	TTime					time_memory;				// ����� �������� ������
	xr_vector<SoundElem>	Sounds;

	CAI_Biting				*monster;

public:
				CMonsterSoundMemory		();
	virtual		~CMonsterSoundMemory	();

	void		init_external			(CAI_Biting *M, TTime mem_time);

	void		HearSound				(const SoundElem &s);
	void		HearSound				(const CObject* who, int eType, const Fvector &Position, float power, TTime time);
	IC	bool	IsRememberSound			() {return (!Sounds.empty());}		
	void		GetSound				(SoundElem &s, bool &bDangerous);	// ���������� ����� ������� ����
	SoundElem	&GetSound				();

	void		UpdateHearing			();

	bool		is_loud_sound			(float val);
	
	void		clear					() {Sounds.clear();}
};

