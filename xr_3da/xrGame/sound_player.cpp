////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_player.cpp
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Sound player
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "sound_player.h"
#include "script_engine.h"
#include "ai/stalker/ai_stalker_space.h"
#include "ai_space.h"

CSoundPlayer::CSoundPlayer			()
{
	init							();
}

CSoundPlayer::~CSoundPlayer			()
{
	clear							();
}

void CSoundPlayer::clear			()
{
	while (!m_sounds.empty())
		remove						(m_sounds.begin()->first);
}

void CSoundPlayer::init				()
{
	seed							(u32(CPU::GetCycleCount() & 0xffffffff));
}

void CSoundPlayer::reinit			()
{
}

void CSoundPlayer::reload			(LPCSTR section)
{
	clear							();
	set_sound_mask					(u32(-1));
	set_sound_mask					(0);
	VERIFY							(m_playing_sounds.empty());
	m_object						= smart_cast<CObject*>(this);
	VERIFY							(m_object);
}

u32 CSoundPlayer::add				(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type, LPCSTR bone_name, LPCSTR head_anim)
{
	xr_map<u32,CSoundCollection>::iterator	I = m_sounds.find(internal_type);
	if (m_sounds.end() != I)
		return						(0);

	CSoundCollection				sound_collection;
	sound_collection.m_priority		= priority;
	sound_collection.m_synchro_mask	= mask;
	sound_collection.m_bone_name	= bone_name;
	sound_collection.m_sound_prefix	= prefix;
	m_sounds.insert					(std::make_pair(internal_type,sound_collection));
	
	I								= m_sounds.find(internal_type);
	VERIFY							(m_sounds.end() != I);
	return							(load((*I).second.m_sounds,prefix,max_count,type));
}

void CSoundPlayer::remove			(u32 internal_type)
{
	xr_map<u32,CSoundCollection>::iterator	I = m_sounds.find(internal_type);
	VERIFY							(m_sounds.end() != I);
	m_sounds.erase					(I);
}

u32 CSoundPlayer::load				(xr_vector<ref_sound*> &sounds, LPCSTR prefix, u32 max_count, ESoundTypes type)
{
	sounds.clear				();
	for (int j=0, N = _GetItemCount(prefix); j<N; ++j) {
		string256				fn, s;
		LPSTR					S = (LPSTR)&s;
		_GetItem				(prefix,j,S);
		if (FS.exist(fn,"$game_sounds$",S,".ogg")){
			sounds.push_back	(xr_new<ref_sound>());
			::Sound->create		(*sounds.back(),TRUE,prefix,type);
		}
		for (u32 i=0; i<max_count; ++i){
			string256			name;
			sprintf				(name,"%s%d",S,i);
			if (FS.exist(fn,"$game_sounds$",name,".ogg")){
				sounds.push_back(xr_new<ref_sound>());
				::Sound->create	(*sounds.back(),TRUE,name,type);
			}
		}
	}
	if (sounds.empty()) {
		Msg						("! There are no sounds with prefix %s",prefix);
	}
	return						((u32)sounds.size());
}

bool CSoundPlayer::check_sound_legacy(u32 internal_type) const
{
	xr_map<u32,CSoundCollection>::const_iterator	J = m_sounds.find(internal_type);
	if (m_sounds.end() == J) {
		ai().script_engine().script_log(eLuaMessageTypeError,"Can't find sound with internal type %d (sound_script = %d)",internal_type,StalkerSpace::eStalkerSoundScript);
		return						(false);
	}

	VERIFY							(m_sounds.end() != J);
	const CSoundCollection			&sound = (*J).second;
	if (sound.m_synchro_mask & m_sound_mask)
		return						(false);

	xr_vector<CSoundSingle>::const_iterator	I = m_playing_sounds.begin();
	xr_vector<CSoundSingle>::const_iterator	E = m_playing_sounds.end();
	for ( ; I != E; ++I)
		if ((*I).m_synchro_mask & sound.m_synchro_mask)
			if ((*I).m_priority <= sound.m_priority)
				return				(false);
	return							(true);
}

void CSoundPlayer::update			(float time_delta)
{
	remove_inappropriate_sounds		(m_sound_mask);
	update_playing_sounds			();
}

void CSoundPlayer::remove_inappropriate_sounds(u32 sound_mask)
{
	xr_vector<CSoundSingle>::iterator	I = remove_if(m_playing_sounds.begin(),m_playing_sounds.end(),CInappropriateSoundPredicate(sound_mask));
	m_playing_sounds.erase				(I,m_playing_sounds.end());
}

void CSoundPlayer::update_playing_sounds()
{
	xr_vector<CSoundSingle>::iterator	I = m_playing_sounds.begin();
	xr_vector<CSoundSingle>::iterator	E = m_playing_sounds.end();
	for ( ; I != E; ++I) {
		if ((*I).m_sound->feedback)
			(*I).m_sound->feedback->set_position(compute_sound_point(*I));
		else
			if (!(*I).started() && (Level().timeServer() >= (*I).m_start_time))
				(*I).play_at_pos			(m_object,compute_sound_point(*I));
	}
}

void CSoundPlayer::play				(u32 internal_type, u32 max_start_time, u32 min_start_time, u32 max_stop_time, u32 min_stop_time, u32 id)
{
	if (!check_sound_legacy(internal_type))
		return;
	
	xr_map<u32,CSoundCollection>::iterator	I = m_sounds.find(internal_type);
	VERIFY						(m_sounds.end() != I);
	CSoundCollection			&sound = (*I).second;
	if (sound.m_sounds.empty()) {
		Msg						("! There are no sounds in sound collection \"%s\" with internal type %d (sound_script = %d)",*sound.m_sound_prefix,internal_type,StalkerSpace::eStalkerSoundScript);
		return;
	}

	remove_inappropriate_sounds	(sound.m_synchro_mask);

	CSoundSingle				sound_single;
	(CSoundParams&)sound_single	= (CSoundParams&)sound;
	sound_single.m_bone_id		= smart_cast<CKinematics*>(m_object->Visual())->LL_BoneID(sound.m_bone_name);
	sound_single.m_sound		= sound.m_sounds[id == u32(-1) ? sound.random(sound.m_sounds.size()) : id];
	VERIFY						(sound_single.m_sound->handle);
	VERIFY						(max_start_time >= min_start_time);
	VERIFY						(max_stop_time >= min_stop_time);
	u32							random_time = 0;
	
	if (max_start_time)
		random_time				= random(max_start_time - min_start_time) + min_start_time;

	sound_single.m_start_time	= Level().timeServer() + random_time;
	
	random_time					= 0;
	if (max_stop_time)
		random_time				= random(max_stop_time - min_stop_time) + min_stop_time;

	sound_single.m_stop_time	= sound_single.m_start_time + sound_single.m_sound->handle->length_ms() + random_time;
	m_playing_sounds.push_back	(sound_single);
	
	if (Level().timeServer() >= m_playing_sounds.back().m_start_time)
		m_playing_sounds.back().play_at_pos(m_object,compute_sound_point(m_playing_sounds.back()));
}

IC	Fvector CSoundPlayer::compute_sound_point(const CSoundSingle &sound)
{
	Fmatrix								l_tMatrix;
	l_tMatrix.mul_43					(m_object->XFORM(),smart_cast<CKinematics*>(m_object->Visual())->LL_GetBoneInstance(sound.m_bone_id).mTransform);
	return								(l_tMatrix.c);
}
