////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_player.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Sound player
////////////////////////////////////////////////////////////////////////////

#pragma once

class CSoundPlayer {
public:
	struct CSoundParams {
		u32										m_priority;
		u32										m_synchro_mask;
		ref_str									m_bone_name;
	};

	struct CSoundCollection : public CSoundParams {
		xr_vector<ref_sound*>					m_sounds;

						~CSoundCollection		()
		{
			xr_vector<ref_sound*>::iterator		I = m_sounds.begin();
			xr_vector<ref_sound*>::iterator		E = m_sounds.end();
			for ( ; I != E; ++I) {
				VERIFY							(*I);
				if ((*I)->feedback)
					(*I)->stop					();
				xr_delete						(*I);
			}
		}
	};

	struct CSoundSingle : public CSoundParams {
		ref_sound								*m_sound;
		u32										m_start_time;
		u32										m_stop_time;
		bool									m_started;
		u16										m_bone_id;

				CSoundSingle					()
		{
			m_started							= false;
		}

				void	destroy					()
		{
			VERIFY								(m_sound);
			if (m_sound->feedback)
				m_sound->stop					();
		}

				void	play_at_pos				(CObject *object, const Fvector &position)
		{
			m_sound->play_at_pos				(object,position);
			m_started							= true;
		}

		IC		bool	started					() const
		{
			return								(m_started);
		}
	};

	struct CInappropriateSoundPredicate {
		u32										m_sound_mask;

						CInappropriateSoundPredicate	(u32 sound_mask) : m_sound_mask(sound_mask)
		{
		}

		bool			operator()				(const CSoundSingle &sound) const
		{
			return		(
				(sound.m_synchro_mask & m_sound_mask) || 
				(
					!sound.m_sound->feedback && 
					(sound.m_stop_time <= Level().timeServer())
				)
			);
		}
	};


private:
	xr_map<u32,CSoundCollection>				m_sounds;
	xr_vector<CSoundSingle>						m_playing_sounds;
	u32											m_sound_mask;

			void	load					(xr_vector<ref_sound*> &sounds, LPCSTR	prefix, u32 max_count, ESoundTypes type);
	IC		Fvector compute_sound_point		(const CSoundSingle &sound);
			void	remove_inappropriate_sounds(u32 sound_mask);
			void	update_playing_sounds	();
			bool	check_sound_legacy		(u32 internal_type) const;
public:
					CSoundPlayer			();
	virtual			~CSoundPlayer			();
			void	Init					();
	virtual	void	reinit					();
			void	add						(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type, LPCSTR bone_name);
			void	remove					(u32 internal_type);
			void	play					(u32 internal_type, u32 max_start_time = 0, u32 min_start_time = 0, u32 max_stop_time = 0, u32 min_stop_time = 0, u32 id = u32(-1));
			void	update					(float time_delta);
	IC		void	set_sound_mask			(u32 sound_mask);
	IC		const xr_vector<CSoundSingle>	&playing_sounds() const;
	IC		u32		active_sound_count		(bool only_playing = false) const;
};

#include "sound_player_inline.h"
