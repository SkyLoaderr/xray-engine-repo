////////////////////////////////////////////////////////////////////////////
//	Module 		: profiler_inline.h
//	Created 	: 23.07.2004
//  Modified 	: 23.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Profiler inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CProfilePortion::CProfilePortion	(LPCSTR timer_id)
{
	m_timer_id						= timer_id;
	m_start							= CPU::GetCycleCount();
}

IC	CProfilePortion::~CProfilePortion	()
{
	m_stop							= CPU::GetCycleCount();
	if (psAI_Flags.test(aiStats))
		profiler().add_profile_portion	(*this);
}

IC	void CProfiler::add_profile_portion	(const CProfileResultPortion &profile_portion)
{
	m_portions.push_back			(profile_portion);
}

IC	CProfiler&	profiler()
{
	return			(*g_profiler);
}

IC	CProfileStats::CProfileStats		()
{
	m_time			= 0.f;
	m_name			= shared_str("");
	m_update_time	= 0;
}
