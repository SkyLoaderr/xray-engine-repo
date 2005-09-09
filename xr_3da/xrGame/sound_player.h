////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_player.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Sound player
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "random32.h"
#include "ai_sounds.h"

class CSoundPlayer : public CRandom32 {
public:
	struct CSoundParams {
		u32										m_priority;
		u32										m_synchro_mask;
		shared_str								m_bone_name;
	};

	struct CSoundCollection : public CSoundParams, public CRandom32 {
		xr_vector<ref_sound*>					m_sounds;
		shared_str								m_sound_prefix;

		IC				CSoundCollection		()
		{
			seed								(u32(CPU::QPC() & 0xffffffff));
		}

						~CSoundCollection		()
		{
			xr_vector<ref_sound*>::iterator		I = m_sounds.begin();
			xr_vector<ref_sound*>::iterator		E = m_sounds.end();
			for ( ; I != E; ++I) {
				VERIFY							(*I);
				if ((*I)->_feedback())
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
			if (m_sound->_feedback())
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

		bool			operator()				(CSoundSingle &sound)
		{
			VERIFY		(sound.m_sound);
			bool		result = 
				(sound.m_synchro_mask & m_sound_mask) || 
				(
					!sound.m_sound->_feedback() && 
					(sound.m_stop_time <= Device.dwTimeGlobal)
				);
			if (result)
				sound.destroy					();
			return								(result);
		}
	};

public:
	typedef xr_map<u32,CSoundCollection>		SOUND_COLLECTIONS;

private:
	SOUND_COLLECTIONS							m_sounds;
	xr_vector<CSoundSingle>						m_playing_sounds;
	u32											m_sound_mask;
	CObject										*m_object;
	shared_str									m_sound_prefix;

			u32			load						(xr_vector<ref_sound*> &sounds, LPCSTR	prefix, u32 max_count, ESoundTypes type, CSound_UserDataPtr data);
	IC		Fvector		compute_sound_point			(const CSoundSingle &sound);
			void		remove_inappropriate_sounds	(u32 sound_mask);
			void		update_playing_sounds		();
			bool		check_sound_legacy			(u32 internal_type) const;
	IC		ref_sound	*add						(ESoundTypes type, LPCSTR name, CSound_UserDataPtr data) const;

public:
						CSoundPlayer				(CObject *object);
	virtual				~CSoundPlayer				();
	virtual	void		reinit						();
	virtual	void		reload						(LPCSTR section);
			void		unload						();
			u32			add							(LPCSTR prefix, u32 max_count, ESoundTypes type, u32 priority, u32 mask, u32 internal_type, LPCSTR bone_name, CSound_UserDataPtr data = 0);
			void		remove						(u32 internal_type);
			void		clear						();
			void		play						(u32 internal_type, u32 max_start_time = 0, u32 min_start_time = 0, u32 max_stop_time = 0, u32 min_stop_time = 0, u32 id = u32(-1));
			void		update						(float time_delta);
	IC		void		set_sound_mask				(u32 sound_mask);
	IC		void		remove_active_sounds		(u32 sound_mask);
	IC		const xr_vector<CSoundSingle> &playing_sounds() const;
	IC		u32			active_sound_count			(bool only_playing = false) const;
			bool		need_bone_data				() const;
	IC		const SOUND_COLLECTIONS &objects		() const;
	IC		bool		active_sound_type			(u32 synchro_mask) const;
	IC		void		sound_prefix				(const shared_str &sound_prefix);
};

#include "sound_player_inline.h"