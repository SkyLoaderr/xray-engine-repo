#include "stdafx.h"
#include "ai_monster_sound.h"


void CMonsterSound::Init(CCustomMonster *pM)
{
	pMonster			= pM;
	
	current				= 0;
	bkgnd				= 0;
	cur_type			= prev_type = SND_TYPE_IDLE;
	delay				= 0;
	time_next_play		= 0;
	play_bkgnd			= false;
	cur_priority		= new_priority = SND_PRIORITY_NORMAL;
}

void CMonsterSound::LoadSounds(LPCTSTR section)
{
	g_vfLoadSounds(sndIdle,			pSettings->r_string(section,"sound_idle"),			16,		SOUND_TYPE_MONSTER_TALKING);
	g_vfLoadSounds(sndEat,			pSettings->r_string(section,"sound_eat"),			16,		SOUND_TYPE_MONSTER_TALKING);
	g_vfLoadSounds(sndAttack,		pSettings->r_string(section,"sound_attack"),		16,		SOUND_TYPE_MONSTER_ATTACKING);
	g_vfLoadSounds(sndAttackHit,	pSettings->r_string(section,"sound_attack_hit"),	16,		SOUND_TYPE_MONSTER_ATTACKING);
	g_vfLoadSounds(sndTakeDamage,	pSettings->r_string(section,"sound_take_damage"),	16,		SOUND_TYPE_MONSTER_INJURING);
	g_vfLoadSounds(sndDie,			pSettings->r_string(section,"sound_die"),			16,		SOUND_TYPE_MONSTER_DYING);
	g_vfLoadSounds(sndBkground,		pSettings->r_string(section,"sound_bkgnd"),			16,		SOUND_TYPE_MONSTER_WALKING);
}

void CMonsterSound::ControlSound(TTime time)
{
	bool b_cur_playing		= current && current->feedback;
	bool b_bkgnd_playing	= bkgnd && bkgnd->feedback;
	
	// process bkground sound
	if (play_bkgnd && !b_bkgnd_playing) PlayBkgndSound();
	play_bkgnd = false;
	
	if ((prev_type != cur_type) && (cur_priority >= new_priority)) {
		// if sound is playing now - stop sound and start _new one
		if (b_cur_playing) current->stop();			
		PlaySound(cur_type);
		return;
	};

	// update sound position
	if (b_bkgnd_playing) bkgnd->feedback->set_position(pMonster->Position());
	if (b_cur_playing) { 
		current->feedback->set_position(pMonster->Position());
		return;
	}

	// there is no sound to play
	if (0 == delay) return;

	if (0 == time_next_play) time_next_play = time + ::Random.randI(delay);
	
	// check if it's time to play sound
	if (time_next_play < time) PlaySound(cur_type);
}

void CMonsterSound::PlaySound(ESoundType sound_type)
{
	SOUND_VECTOR *sv = &sndIdle;

	switch(sound_type) {
		case SND_TYPE_IDLE:			sv = &sndIdle;			break;
		case SND_TYPE_EAT:			sv = &sndEat;			break;
		case SND_TYPE_ATTACK:		sv = &sndAttack;		break;
		case SND_TYPE_ATTACK_HIT:	sv = &sndAttackHit;		break;
		case SND_TYPE_TAKE_DAMAGE:	sv = &sndTakeDamage;	break;
		case SND_TYPE_DIE:			sv = &sndDie;			break;
	}

	current = &sv->at(::Random.randI((int)sv->size()));
	current->play_at_pos(pMonster,pMonster->eye_matrix.c);
	//current->feedback->set_volume(1.f);

	delay			= 0;
	time_next_play	= 0;
	prev_type		= cur_type;
	cur_priority	= new_priority;
		
	OnSoundPlay();
}

void CMonsterSound::PlayBkgndSound()
{
	bkgnd = &sndBkground.at(::Random.randI((int)sndBkground.size()));
	bkgnd->play_at_pos(Level().CurrentEntity(),Level().CurrentEntity()->Position());
	///bkgnd->feedback->set_volume(1.f);
}

