#include "stdafx.h"
#pragma hdrstop

CStatTimer::CStatTimer()
:T(true)
{
	accum	= 0;
	result	= 0.f;
	count	= 0;
}

void	CStatTimer::FrameStart	()
{
	accum	= 0;
	count	= 0;
}
void	CStatTimer::FrameEnd	()
{
	float _time			= float(accum)*CPU::cycles2milisec;
	if (_time > result)	result	=	_time;
	else				result	=	0.99f*result + 0.01f*_time;
}

XRCORE_API pauseMngr	g_pauseMngr;


void pauseMngr::Pause(bool b){
	if(m_paused == b)return;

	xr_vector<CTimer*>::iterator it = m_timers.begin();
	for(;it!=m_timers.end();++it)
		(*it)->Pause(b);

	m_paused = b;
}

void pauseMngr::Register (CTimer* t){
		m_timers.push_back(t);
}

void pauseMngr::UnRegister (CTimer* t){
	xr_vector<CTimer*>::iterator it = std::find(m_timers.begin(),m_timers.end(),t);
	if( it!=m_timers.end() )
		m_timers.erase(it);
}
