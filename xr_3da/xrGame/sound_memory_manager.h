////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_memory_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Sound memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../feel_sound.h"
#include "memory_space.h"

class CSoundMemoryManager : public Feel::Sound {
private:
	// sound objects
	xr_vector<CSoundObject>	*m_sounds;
	xr_map<ESoundTypes,u32>	m_priorities;
	u32						m_max_sound_count;
	
	// sound threshold
	u32						m_last_sound_time;
	u32						m_sound_decrease_quant;
	float					m_decrease_factor;
	float					m_min_sound_threshold;
	float					m_sound_threshold;

	// selected sound
	const CSoundObject		*m_selected_sound;

	IC		void	update_sound_threshold		();
	IC		u32		get_priority				(const CSoundObject &sound) const;
protected:
	IC		void	set_sound_type_priority		(ESoundTypes sound_type, u32 priority);
public:
					CSoundMemoryManager			();
	virtual			~CSoundMemoryManager		();
			void	Init						();
	virtual	void	Load						(LPCSTR section);
	virtual	void	reinit						();
	virtual	void	reload						(LPCSTR section);
	virtual void	feel_sound_new				(CObject* who, int eType, const Fvector &Position, float power);
	virtual	void	update						();
			void	add_sound_object			(const CObject *object, int sound_type, const Fvector &position, float sound_power);
	IC		const xr_vector<CSoundObject>		&sound_objects	() const;
	IC		const CSoundObject					*sound			() const;
	IC		void	set_squad_objects			(xr_vector<CSoundObject> *squad_objects);
};

#include "sound_memory_manager_inline.h"