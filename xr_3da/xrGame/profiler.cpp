////////////////////////////////////////////////////////////////////////////
//	Module 		: profiler.cpp
//	Created 	: 23.07.2004
//  Modified 	: 23.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Profiler
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "profiler.h"
#include "../gamefont.h"

CProfiler	*g_profiler			= 0;
LPCSTR		indent				= "  ";
char		white_character		= '.';

struct CProfilePortionPredicate {
	IC		bool operator()	(const CProfileResultPortion &_1, const CProfileResultPortion &_2) const
	{
		return					(xr_strcmp(_1.m_timer_id,_2.m_timer_id) < 0);
	}
};

IC	u32 compute_string_length(LPCSTR str)
{
	LPCSTR						i, j = str;
	u32							count = 0;
	while ((i = strchr(j,'/')) != 0) {
		j = i					= i + 1;
		++count;
	}
	return						(count*xr_strlen(indent) + xr_strlen(j));
}

CProfiler::CProfiler		()
{
	m_portions.reserve			(1000);
	m_actual					= true;
}

IC	void CProfiler::convert_string(LPCSTR str, shared_str &out, u32 max_string_size)
{
	LPCSTR						i, j = str;
	u32							count = 0;
	while ((i = strchr(j,'/')) != 0) {
		j = i					= i + 1;
		++count;
	}
	strcpy						(m_temp,"");
	for (u32 k = 0; k<count; ++k)
		strcat					(m_temp,indent);
	strcat						(m_temp,j);
	count						= xr_strlen(m_temp);
	for ( ; count < max_string_size; ++count)
		m_temp[count]			= white_character;
	m_temp[max_string_size]		= 0;
	out							= m_temp;
}

void CProfiler::setup_timer	(LPCSTR timer_id, u64 timer_time)
{
	float						_time = float(timer_time)*CPU::cycles2milisec;
	TIMERS::iterator			i = m_timers.find(timer_id);
	if (i == m_timers.end()) {
		strcpy					(m_temp,timer_id);
		LPSTR					j,k = m_temp;
		while ((j = strchr(k,'/')) != 0) {
			*j					= 0;
			TIMERS::iterator	m = m_timers.find(m_temp);
			if (m == m_timers.end())
				m_timers.insert	(std::make_pair(shared_str(m_temp),CProfileStats()));
			*j					= '/';
			k					= j + 1;
		}
		i						= m_timers.insert(std::make_pair(shared_str(timer_id),CProfileStats())).first;
		m_actual				= false;
	}
	if (_time > (*i).second.m_time)
		(*i).second.m_time		= _time;
	else
		(*i).second.m_time		= .01f*_time + .99f*(*i).second.m_time;

	(*i).second.m_update_time	= Device.dwTimeGlobal;
}

void CProfiler::show_stats	(CGameFont *game_font, bool show)
{
	if (!show) {
		m_portions.clear		();
		return;
	}
		
	if (!m_portions.empty()) {
		std::sort				(m_portions.begin(),m_portions.end(),CProfilePortionPredicate());
		u64						timer_time = 0;

		PORTIONS::const_iterator	I = m_portions.begin(), J = I;
		PORTIONS::const_iterator	E = m_portions.end();
		for ( ; I != E; ++I) {
			if (xr_strcmp((*I).m_timer_id,(*J).m_timer_id)) {
				setup_timer		((*J).m_timer_id,timer_time);
				timer_time		= 0;
				J				= I;
			}
			timer_time			+= (*I).m_stop - (*I).m_start;
		}
		setup_timer				((*J).m_timer_id,timer_time);

		m_portions.clear		();

		if (!m_actual) {
			u32					max_string_size = 0;
			TIMERS::iterator	I = m_timers.begin();
			TIMERS::iterator	E = m_timers.end();
			for ( ; I != E; ++I)
				max_string_size	= _max(max_string_size,compute_string_length(*(*I).first));
			I					= m_timers.begin();
			for ( ; I != E; ++I)
				convert_string	(*(*I).first,(*I).second.m_name,max_string_size);

			m_actual			= true;
		}
	}
	
	TIMERS::iterator			I = m_timers.begin();
	TIMERS::iterator			E = m_timers.end();
	for ( ; I != E; ++I) {
		if ((*I).second.m_update_time != Device.dwTimeGlobal)
			(*I).second.m_time	*= .99f;
		game_font->OutNext		("%s.. %.3f",*(*I).second.m_name,(*I).second.m_time);
	}
}
