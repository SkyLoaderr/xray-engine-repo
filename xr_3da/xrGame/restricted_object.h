////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object.h
//	Created 	: 18.08.2004
//  Modified 	: 18.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "gameobject.h"

class CRestrictedObject : virtual public CGameObject {
private:
	typedef CGameObject inherited;

public:
	virtual BOOL	net_Spawn			(LPVOID data);
			void	add_border			() const;
			void	remove_border		() const;
			bool	accessible			(const Fvector &position) const;
			bool	accessible			(u32 vertex_id) const;
			u32		accessible_nearest	(const Fvector &position, Fvector &result) const;
};