////////////////////////////////////////////////////////////////////////////
//	Module 		: enemy_manager_inline.h
//	Created 	: 30.12.2003
//  Modified 	: 30.12.2003
//	Author		: Dmitriy Iassenev
//	Description : Enemy manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CEnemyManager::CEnemyManager							(CCustomMonster *object)
{
	VERIFY						(object);
	m_object					= object;
	m_ready_to_save				= true;
}
