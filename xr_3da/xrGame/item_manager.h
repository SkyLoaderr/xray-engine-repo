////////////////////////////////////////////////////////////////////////////
//	Module 		: item_manager.h
//	Created 	: 27.12.2003
//  Modified 	: 27.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Item manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "object_manager.h"

class CGameObject;
class CRestrictedObject;

class CItemManager : public CObjectManager<const CGameObject > {
private:
	typedef CObjectManager<const CGameObject > inherited;

private:
	CRestrictedObject	*m_restricted_object;

public:
	virtual void	reinit						();
	virtual bool	useful						(const CGameObject *object) const;
	virtual	float	evaluate					(const CGameObject *object) const;
	IC		const xr_vector<const CGameObject*> &items() const;
};

#include "item_manager_inline.h"