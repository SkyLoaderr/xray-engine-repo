////////////////////////////////////////////////////////////////////////////
//	Module 		: agent_manager_inline.h
//	Created 	: 24.05.2004
//  Modified 	: 24.05.2004
//	Author		: Dmitriy Iassenev
//	Description : Agent manager inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	const CAgentManager::MEMBER_STORAGE	&CAgentManager::members	() const
{
	return				(m_members);
}

IC	CAgentManager::MEMBER_STORAGE	&CAgentManager::members	()
{
	return				(m_members);
}

IC	const CSetupAction &CAgentManager::action	(CAI_Stalker *object) const
{
	return				(member(object).action());
}

IC	const CMemberOrder &CAgentManager::member	(CAI_Stalker *object) const
{
	const_iterator		I = std::find_if(members().begin(), members().end(), CMemberPredicate(object));
	VERIFY				(I != members().end());
	return				(*I);
}

IC	MemorySpace::squad_mask_type CAgentManager::mask(const CAI_Stalker *object) const
{
	const_iterator		I = std::find_if(members().begin(),members().end(), CMemberPredicate(object));
	VERIFY				(I != members().end());
	return				(MemorySpace::squad_mask_type(1) << (I - members().begin()));
}

IC	void CAgentManager::set_squad_objects		(xr_vector<CVisibleObject> *objects)
{
	m_visible_objects	= objects;
}

IC	void CAgentManager::set_squad_objects		(xr_vector<CSoundObject> *objects)
{
	m_sound_objects		= objects;
}

IC	void CAgentManager::set_squad_objects		(xr_vector<CHitObject> *objects)
{
	m_hit_objects		= objects;
}

IC	xr_vector<CVisibleObject> &CAgentManager::visibles	() const
{
	VERIFY				(m_visible_objects);
	return				(*m_visible_objects);
}

IC	xr_vector<CSoundObject> &CAgentManager::sounds		() const
{
	VERIFY				(m_sound_objects);
	return				(*m_sound_objects);
}

IC	xr_vector<CHitObject> &CAgentManager::hits			() const
{
	VERIFY				(m_hit_objects);
	return				(*m_hit_objects);
}

IC	CAgentManager::iterator CAgentManager::member		(MemorySpace::squad_mask_type mask)
{
	iterator			I = m_members.begin();
	iterator			E = m_members.end();
	for ( ; I != E; ++I, mask >>= 1)
		if (mask == 1)
			return		(I);
	NODEFAULT;
#ifdef DEBUG
	return				(E);
#endif
}

template <typename T>
IC	void CAgentManager::reset_memory_masks				(xr_vector<T> &objects)
{
	xr_vector<CVisibleObject>::iterator	I = m_visible_objects->begin();
	xr_vector<CVisibleObject>::iterator	E = m_visible_objects->end();
	for ( ; I != E; ++I)
		(*I).m_squad_mask.one		();
}

IC	void CAgentManager::reset_memory_masks				()
{
	reset_memory_masks				(*m_visible_objects);
	reset_memory_masks				(*m_sound_objects);
	reset_memory_masks				(*m_hit_objects);
}

IC	bool CAgentManager::group_behaviour					() const
{
	return							(members().size() > 1);
}

IC	void CAgentManager::clear_danger_covers	()
{
	m_danger_covers.clear			();
}

IC	shared_str CAgentManager::cName	() const
{
	return							("agent_manager");
}

IC	CAgentManagerMotivationPlanner &CAgentManager::brain() const
{
	VERIFY							(m_brain);
	return							(*m_brain);
}

IC	void CAgentManager::register_corpse	(CAI_Stalker *corpse) const
{
	xr_vector<CMemberCorpse>::iterator	I = std::find(m_corpses.begin(),m_corpses.end(),corpse);
	VERIFY2								(I == m_corpses.end(),"Cannot register corpse more than a time!");
	m_corpses.push_back					(CMemberCorpse(corpse,0,Device.dwTimeGlobal));
}

IC	xr_vector<CAgentManager::CMemberCorpse> &CAgentManager::member_corpses	()
{
	return								(m_corpses);
}

IC	void CAgentManager::register_grenade	(const CExplosive *grenade, const CGameObject *game_object) const
{
	xr_vector<CMemberGrenade>::iterator	I = std::find(m_grenades.begin(),m_grenades.end(),grenade);
	if (I != m_grenades.end())
		return;
	m_grenades.push_back				(CMemberGrenade(grenade,game_object,0,Device.dwTimeGlobal));
}

IC	xr_vector<CAgentManager::CMemberGrenade> &CAgentManager::member_grenades	()
{
	return								(m_grenades);
}
