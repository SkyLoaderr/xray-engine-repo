////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "visual_memory_manager.h"
#include "sound_memory_manager.h"
#include "hit_memory_manager.h"
#include "enemy_manager.h"
#include "item_manager.h"

class CAI_Stalker;

class CMemoryManager : 
	public CVisualMemoryManager,
	public CSoundMemoryManager,
	public CHitMemoryManager,
	public CEnemyManager,
	public CItemManager
{
protected:
	CAI_Stalker				*m_object;
	CEntityAlive			*m_self;

protected:
	template <typename T>
		void				update					(const xr_vector<T> &objects);
	template <typename T, typename _predicate>
		void				fill_enemies			(const xr_vector<T> &objects, const _predicate &predicate) const;

public:
							CMemoryManager			();
	virtual					~CMemoryManager			();
			void			init					();
	virtual	void			Load					(LPCSTR section);
	virtual	void			reinit					();
	virtual	void			reload					(LPCSTR section);
	virtual	void			update					(float time_delta);
	IC		ALife::ERelationType get_relation		(const CEntityAlive *tpEntityAlive) const;
	template <typename _predicate>
	IC		void			fill_enemies			(const _predicate &predicate) const;
	IC		bool			visible					(const CObject *object) const;
	IC		const CEntityAlive	*enemy				() const;
	IC		const CGameObject	*item				() const;
			const CMemoryInfo	memory				(const CObject *object) const;
	IC		void			enable					(const CObject *object, bool enable);
};

#include "memory_manager_inline.h"