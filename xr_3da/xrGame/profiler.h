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
	shared_str			m_name;
	u32				m_update_time;

	IC				CProfileStats		();
};

class CProfiler {
private:
	struct pred_rstr {
		IC	bool operator()	(const shared_str &_1, const shared_str &_2) const
		{
			return	(xr_strcmp(*_1,*_2) < 0);
		}
	};
protected:
	typedef xr_vector<CProfileResultPortion>		PORTIONS;
	typedef xr_map<shared_str,CProfileStats,pred_rstr>	TIMERS;

protected:
	PORTIONS		m_portions;
	TIMERS			m_timers;
	bool			m_actual;
	string256		m_temp;

protected:
			void	setup_timer			(LPCSTR timer_id, u64 timer_time);
	IC		void	convert_string		(LPCSTR str, shared_str &out, u32 max_string_size);

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