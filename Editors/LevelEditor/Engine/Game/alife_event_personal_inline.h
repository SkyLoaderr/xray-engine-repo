////////////////////////////////////////////////////////////////////////////
//	Module 		: alife_event_personal_inline.h
//	Created 	: 02.06.2004
//  Modified 	: 02.06.2004
//	Author		: Dmitriy Iassenev
//	Description : ALife personal event class inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename T>
IC	void CALifeEventPersonal::load_data	(T &stream)
{
	stream.r					(&m_tEventID,		sizeof(m_tEventID));
	stream.r					(&m_tTimeID,		sizeof(m_tTimeID));
	stream.r					(&m_tTaskID,		sizeof(m_tTaskID));
	stream.r					(&m_iHealth,		sizeof(m_iHealth));
	stream.r					(&m_tEventRelationType,sizeof(m_tEventRelationType));
}

template <typename T>
IC	void CALifeEventPersonal::save_data	(T &stream)
{
	stream.w					(&m_tEventID,		sizeof(m_tEventID));
	stream.w					(&m_tTimeID,		sizeof(m_tTimeID));
	stream.w					(&m_tTaskID,		sizeof(m_tTaskID));
	stream.w					(&m_iHealth,		sizeof(m_iHealth));
	stream.w					(&m_tEventRelationType,sizeof(m_tEventRelationType));
}
