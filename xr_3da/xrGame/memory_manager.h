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

class CMemoryManager : 
	public CVisualMemoryManager,
	public CSoundMemoryManager,
	public CHitMemoryManager,
	public CEnemyManager,
	public CItemManager
{
public:
							CMemoryManager			();
	virtual					~CMemoryManager			();
			void			Init					();
	virtual	void			Load					(LPCSTR section);
	virtual	void			reinit					();
	virtual	void			reload					(LPCSTR section);
	virtual	void			update					();
	IC		ERelationType	get_relation			(CEntityAlive *tpEntityAlive);
	template <typename T>
			void			update					(const xr_vector<T> &objects);
	IC		bool			visible					(const CObject *object) const;
	IC		const CEntityAlive *enemy				() const;
	IC		const CObject	*item					() const;
			const CMemoryObject<const CGameObject>	memory(const CObject *object) const;
};

#include "memory_manager_inline.h"