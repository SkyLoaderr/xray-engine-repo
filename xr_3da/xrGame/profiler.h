////////////////////////////////////////////////////////////////////////////
//	Module 		: profiler.h
//	Created 	: 23.07.2004
//  Modified 	: 23.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Profiler
////////////////////////////////////////////////////////////////////////////

#pragma once

struct CProfileResultPortion {
	LPCSTR			m_timer_id;
	u64				m_start;
	u64				m_stop;
};

struct CProfilePortion : public CProfileResultPortion {
	IC				CProfilePortion		(LPCSTR timer_id);
	IC				~CProfilePortion	();
};

struct CProfileStats {
	float			m_time;
};

class CProfiler {
protected:
	typedef xr_vector<CProfileResultPortion>		PORTIONS;
	typedef xr_map<LPCSTR,CProfileStats,pred_str>	TIMERS;

protected:
	PORTIONS		m_portions;
	TIMERS			m_timers;
	u32				m_max_string_size;
	bool			m_actual;

protected:
			void	setup_timer			(LPCSTR timer_id, u64 timer_time);
	IC		ref_str convert_string		(LPCSTR str);

public:
					CProfiler			();
			void	show_stats			(CGameFont *game_font, bool show);
	IC		void	add_profile_portion	(const CProfileResultPortion &profile_portion);
};

extern 	CProfiler *g_profiler;

IC	CProfiler&	profiler();
		
#define START_PROFILE(a) { CProfilePortion	__profile_portion__(a);
#define STOP_PROFILE     }

#include "profiler_inline.h"