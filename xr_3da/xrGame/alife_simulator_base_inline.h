////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_simulator_base_inline.h
//	Created 	: 25.12.2002
//  Modified 	: 12.05.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife Simulator base inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	bool								CALifeSimulatorBase::initialized			() const
{
	return						(m_initialized);
}

IC	const CALifeSimulatorHeader			&CALifeSimulatorBase::header				() const
{
	VERIFY						(initialized());
	VERIFY						(m_header);
	return						(*m_header);
}

IC	const CALifeTimeManager				&CALifeSimulatorBase::time					() const
{
	VERIFY						(initialized());
	VERIFY						(m_time_manager);
	return						(*m_time_manager);
}

IC	const CALifeSpawnRegistry			&CALifeSimulatorBase::spawns				() const
{
	VERIFY						(initialized());
	VERIFY						(m_spawns);
	return						(*m_spawns);
}

IC	const CALifeObjectRegistry			&CALifeSimulatorBase::objects				() const
{
	VERIFY						(initialized());
	VERIFY						(m_objects);
	return						(*m_objects);
}

IC	const CALifeEventRegistry			&CALifeSimulatorBase::events				() const
{
	VERIFY						(initialized());
	VERIFY						(m_events);
	return						(*m_events);
}

IC	const CALifeTaskRegistry			&CALifeSimulatorBase::tasks					() const
{
	VERIFY						(initialized());
	VERIFY						(m_tasks);
	return						(*m_tasks);
}

IC	const CALifeGraphRegistry			&CALifeSimulatorBase::graph					() const
{
	VERIFY						(initialized());
	VERIFY						(m_graph_objects);
	return						(*m_graph_objects);
}

IC	const CALifeTraderRegistry			&CALifeSimulatorBase::traders				() const
{
	VERIFY						(initialized());
	VERIFY						(m_traders);
	return						(*m_traders);
}

IC	const CALifeScheduleRegistry		&CALifeSimulatorBase::scheduled				() const
{
	VERIFY						(initialized());
	VERIFY						(m_scheduled);
	return						(*m_scheduled);
}

IC	const CALifeAnomalyRegistry			&CALifeSimulatorBase::anomalies				() const
{
	VERIFY						(initialized());
	VERIFY						(m_anomalies);
	return						(*m_anomalies);
}

IC	const CALifeOrganizationRegistry	&CALifeSimulatorBase::organizations			() const
{
	VERIFY						(initialized());
	VERIFY						(m_organizations);
	return						(*m_organizations);
}

IC	const CALifeNewsRegistry			&CALifeSimulatorBase::news					() const
{
	VERIFY						(initialized());
	VERIFY						(m_news);
	return						(*m_news);
}

IC	const CALifeStoryRegistry			&CALifeSimulatorBase::story_objects			() const
{
	VERIFY						(initialized());
	VERIFY						(m_story_objects);
	return						(*m_story_objects);
}

//IC	const CALifeRegistryContainer		&CALifeSimulatorBase::registry				() const
//{
//	VERIFY						(initialized());
//	VERIFY						(m_registry_container);
//	return						(*m_registry_container);
//}
//
IC	CALifeSimulatorHeader				&CALifeSimulatorBase::header				()
{
	VERIFY						(initialized());
	VERIFY						(m_header);
	return						(*m_header);
}

IC	CALifeTimeManager					&CALifeSimulatorBase::time					()
{
	VERIFY						(initialized());
	VERIFY						(m_time_manager);
	return						(*m_time_manager);
}

IC	CALifeSpawnRegistry					&CALifeSimulatorBase::spawns				()
{
	VERIFY						(initialized());
	VERIFY						(m_spawns);
	return						(*m_spawns);
}

IC	CALifeObjectRegistry				&CALifeSimulatorBase::objects				()
{
	VERIFY						(initialized());
	VERIFY						(m_objects);
	return						(*m_objects);
}

IC	CALifeEventRegistry					&CALifeSimulatorBase::events				()
{
	VERIFY						(initialized());
	VERIFY						(m_events);
	return						(*m_events);
}

IC	CALifeTaskRegistry					&CALifeSimulatorBase::tasks					()
{
	VERIFY						(initialized());
	VERIFY						(m_tasks);
	return						(*m_tasks);
}

IC	CALifeGraphRegistry					&CALifeSimulatorBase::graph					()
{
	VERIFY						(initialized());
	VERIFY						(m_graph_objects);
	return						(*m_graph_objects);
}

IC	CALifeTraderRegistry				&CALifeSimulatorBase::traders				()
{
	VERIFY						(initialized());
	VERIFY						(m_traders);
	return						(*m_traders);
}

IC	CALifeScheduleRegistry				&CALifeSimulatorBase::scheduled				()
{
	VERIFY						(initialized());
	VERIFY						(m_scheduled);
	return						(*m_scheduled);
}

IC	CALifeAnomalyRegistry				&CALifeSimulatorBase::anomalies				()
{
	VERIFY						(initialized());
	VERIFY						(m_anomalies);
	return						(*m_anomalies);
}

IC	CALifeOrganizationRegistry			&CALifeSimulatorBase::organizations			()
{
	VERIFY						(initialized());
	VERIFY						(m_organizations);
	return						(*m_organizations);
}

IC	CALifeNewsRegistry					&CALifeSimulatorBase::news					()
{
	VERIFY						(initialized());
	VERIFY						(m_news);
	return						(*m_news);
}

IC	CALifeStoryRegistry					&CALifeSimulatorBase::story_objects			()
{
	VERIFY						(initialized());
	VERIFY						(m_story_objects);
	return						(*m_story_objects);
}

IC	CALifeRegistryContainer		&CALifeSimulatorBase::registry						() const
{
	VERIFY						(initialized());
	VERIFY						(m_registry_container);
	return						(*m_registry_container);
}

IC	CRandom32							&CALifeSimulatorBase::random				()
{
	return						(m_random);
}

IC	void								CALifeSimulatorBase::setup_command_line		(shared_str &command_line)
{
	m_server_command_line		= &command_line;
}

IC	xrServer							&CALifeSimulatorBase::server				() const
{
	VERIFY						(m_server);
	return						(*m_server);
}

IC	CALifeTimeManager					&CALifeSimulatorBase::time_manager			()
{
	VERIFY						(initialized());
	VERIFY						(m_time_manager);
	return						(*m_time_manager);
}

IC	const CALifeTimeManager				&CALifeSimulatorBase::time_manager			() const
{
	VERIFY						(initialized());
	VERIFY						(m_time_manager);
	return						(*m_time_manager);
}

IC	shared_str								*CALifeSimulatorBase::server_command_line	() const
{
	VERIFY						(m_server_command_line);
	return						(m_server_command_line);
}
