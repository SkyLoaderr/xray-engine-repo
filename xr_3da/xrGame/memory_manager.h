////////////////////////////////////////////////////////////////////////////
//	Module 		: memory_manager.h
//	Created 	: 02.10.2001
//  Modified 	: 19.11.2003
//	Author		: Dmitriy Iassenev
//	Description : Memory manager
////////////////////////////////////////////////////////////////////////////

#pragma once

class CVisualMemoryManager;
class CSoundMemoryManager;
class CHitMemoryManager;
class CEnemyManager;
class CItemManager;
class CGreetingManager;
class CDangerManager;
class CCustomMonster;
class CAI_Stalker;

namespace MemorySpace {
	struct CMemoryInfo;
};

class CMemoryManager {
public:
	typedef MemorySpace::CMemoryInfo CMemoryInfo;

protected:
	CVisualMemoryManager	*m_visual;
	CSoundMemoryManager		*m_sound;
	CHitMemoryManager		*m_hit;
	CEnemyManager			*m_enemy;
	CItemManager			*m_item;
	CGreetingManager		*m_greeting;
	CDangerManager			*m_danger;

protected:
	CCustomMonster			*m_object;
	CAI_Stalker				*m_stalker;

protected:
	template <typename T>
			void				update			(const xr_vector<T> &objects);
	template <typename T, typename _predicate>
			void				fill_enemies	(const xr_vector<T> &objects, const _predicate &predicate) const;

public:
								CMemoryManager	(CCustomMonster *object, CSoundUserDataVisitor *visitor);
	virtual						~CMemoryManager	();
	virtual	void				Load			(LPCSTR section);
	virtual	void				reinit			();
	virtual	void				reload			(LPCSTR section);
	virtual	void				update			(float time_delta);
			void				remove_links	(CObject *object);

public:
			void				enable			(const CObject *object, bool enable);
			const CMemoryInfo	memory			(const CObject *object) const;

public:
	template <typename _predicate>
	IC		void				fill_enemies	(const _predicate &predicate) const;

public:
	IC		CVisualMemoryManager&visual			() const;
	IC		CSoundMemoryManager	&sound			() const;
	IC		CHitMemoryManager	&hit			() const;
	IC		CEnemyManager		&enemy			() const;
	IC		CItemManager		&item			() const;
	IC		CGreetingManager	&greeting		() const;
	IC		CDangerManager		&danger			() const;
	IC		CCustomMonster		&object			() const;
	IC		CAI_Stalker			&stalker		() const;
};

#include "memory_manager_inline.h"