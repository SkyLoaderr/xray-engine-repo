////////////////////////////////////////////////////////////////////////////
//	Module 		: level_spawn_constructor_inline.h
//	Created 	: 16.10.2004
//  Modified 	: 16.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Level spawn constructor inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CLevelSpawnConstructor::CLevelSpawnConstructor			(const CGameGraph::SLevel &level, CGameSpawnConstructor *game_spawn_constructor) :
	CThread						(level.id())
{
	m_level						= level;
	m_game_spawn_constructor	= game_spawn_constructor;
	thDestroyOnComplete			= FALSE;
	m_actor						= 0;
}

IC	CSE_ALifeCreatureActor *CLevelSpawnConstructor::actor	() const
{
	return						(m_actor);
}

IC	const CGameGraph::SLevel &CLevelSpawnConstructor::level	() const
{
	return						(m_level);
}
