////////////////////////////////////////////////////////////////////////////
//	Module 		: object_manager.h
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Object manager
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename T
>
class CObjectManager {
protected:
	xr_set<T*>		m_objects;
	T				*m_selected;
public:
					CObjectManager				();
	virtual			~CObjectManager				();
			void	Init						();
	virtual void	Load						(LPCSTR section);
	virtual void	reinit						();
	virtual void	reload						(LPCSTR section);
	virtual void	update						();
			bool	add							(T *object);
	virtual bool	useful						(T *object) const;
	virtual	float	evaluate					(T *object) const;
	virtual	void	reset						();
	IC		const T *selected					() const;
	IC		const xr_set<T*> &objects		() const;
};

#include "object_manager_inline.h"