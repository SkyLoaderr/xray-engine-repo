////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_memory_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Sound memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace MemorySpace {
	struct CSoundObject;
};

enum ESoundTypes;

class CCustomMonster;

class CSoundMemoryManager {
public:
	typedef MemorySpace::CSoundObject			CSoundObject;
	typedef xr_vector<CSoundObject>				SOUNDS;
	typedef xr_map<ESoundTypes,u32>				PRIORITIES;

private:
	CCustomMonster				*m_object;
	CAI_Stalker					*m_stalker;
	CSoundUserDataVisitor		*m_visitor;

private:
	SOUNDS						*m_sounds;
	PRIORITIES					m_priorities;
	u32							m_max_sound_count;
	
private:
	u32							m_last_sound_time;
	u32							m_sound_decrease_quant;
	float						m_decrease_factor;
	float						m_min_sound_threshold;
	float						m_sound_threshold;
	float						m_self_sound_factor;

private:
	const CSoundObject			*m_selected_sound;

private:
	IC		void				update_sound_threshold	();
	IC		u32					priority				(const CSoundObject &sound) const;

protected:
	IC		void				priority				(const ESoundTypes &sound_type, u32 priority);

public:
	IC							CSoundMemoryManager		(CCustomMonster *object, CAI_Stalker *stalker, CSoundUserDataVisitor *visitor);
	virtual						~CSoundMemoryManager	();
	virtual	void				Load					(LPCSTR section);
	virtual	void				reinit					();
	virtual	void				reload					(LPCSTR section);
	virtual void				feel_sound_new			(CObject* who, int eType, CSoundUserDataPtr user_data, const Fvector &Position, float power);
	virtual	void				update					();
			void				remove_links			(CObject *object);

public:
			void				add						(const CObject *object, int sound_type, const Fvector &position, float sound_power);
			void				enable					(const CObject *object, bool enable);

public:
	IC		const SOUNDS		&objects				() const;
	IC		const CSoundObject	*sound					() const;
	IC		void				set_squad_objects		(SOUNDS *squad_objects);

public:
	IC		void				set_threshold			(float threshold);
	IC		void				restore_threshold		();
};

#include "sound_memory_manager_inline.h"