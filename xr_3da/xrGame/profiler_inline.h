////////////////////////////////////////////////////////////////////////////
//	Module 		: profiler_inline.h
//	Created 	: 23.07.2004
//  Modified 	: 23.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Profiler inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CProfiler::CProfiler				()
{
	m_actual					= true;
}

IC	CProfilePortion::CProfilePortion	(LPCSTR timer_id)
{
	m_timer_id							= timer_id;
	m_timer.Start						();
}

IC	CProfilePortion::~CProfilePortion	()
{
	m_time								= m_timer.GetElapsed_ticks();
	if (psAI_Flags.test(aiStats) && psDeviceFlags.test(rsStatistic))
		profiler().add_profile_portion	(*this);
}

IC	void CProfiler::add_profile_portion	(const CProfileResultPortion &profile_portion)
{
	m_section.Enter						();
	m_portions.push_back				(profile_portion);
	m_section.Leave						();
}

IC	CProfiler&	profiler				()
{
	return			(*g_profiler);
}

IC	CProfileStats::CProfileStats		()
{
	m_update_time	= 0;
	m_name			= shared_str("");
	m_time			= 0.f;
	m_min_time		= 0.f;
	m_max_time		= 0.f;
	m_total_time	= 0.f;
	m_count			= 0;
	m_call_count	= 0;
}
