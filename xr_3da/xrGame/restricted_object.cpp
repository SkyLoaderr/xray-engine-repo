////////////////////////////////////////////////////////////////////////////
//	Module 		: restricted_object.cpp
//	Created 	: 18.08.2004
//  Modified 	: 18.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Restricted object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "restricted_object.h"
#include "level.h"
#include "space_restrictor_manager.h"
#include "xrServer_Objects_ALife_Monsters.h"

BOOL CRestrictedObject::net_Spawn		(LPVOID data)
{
	if (!inherited::net_Spawn(data))
		return					(FALSE);

	CSE_Abstract				*abstract	= (CSE_Abstract*)(data);
	CSE_ALifeMonsterAbstract	*monster	= dynamic_cast<CSE_ALifeMonsterAbstract*>(abstract);
	VERIFY						(monster);
	
	Level().space_restrictor_manager().associate		(ID(),monster->m_space_restrictors);

	return						(TRUE);
}

void CRestrictedObject::add_border		() const
{
	if (accessible(Position()))
		Level().space_restrictor_manager().add_border	(ID());
}

void CRestrictedObject::remove_border	() const
{
	if (accessible(Position()))
		Level().space_restrictor_manager().remove_border(ID());
}

bool CRestrictedObject::accessible		(const Fvector &position) const
{
	return	(Level().space_restrictor_manager().accessible(ID(),position));
}

bool CRestrictedObject::accessible		(u32 vertex_id) const
{
	return	(Level().space_restrictor_manager().accessible(ID(),ai().level_graph().vertex_position(vertex_id)));
}
