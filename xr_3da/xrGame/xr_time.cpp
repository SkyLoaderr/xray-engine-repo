#include "stdafx.h"
#include "xr_time.h"
#include "ui/UIInventoryUtilities.h"
#include "level.h"


#define sec2ms		1000
#define min2ms		60*sec2ms
#define hour2ms		60*min2ms
#define day2ms		24*hour2ms


u32 get_time()
{
	return u32(Level().GetGameTime() & u32(-1));
}

xrTime get_time_struct()
{
	return xrTime( Level().GetGameTime() );
}

LPCSTR	xrTime::dateToString	(int mode)								
{ 
	return *InventoryUtilities::GetDateAsString(m_time,(InventoryUtilities::EDatePrecision)mode);
}
LPCSTR	xrTime::timeToString	(int mode)								
{ 
	return *InventoryUtilities::GetTimeAsString(m_time,(InventoryUtilities::ETimePrecision)mode);
}

void	xrTime::add				(const xrTime& other)					
{  
	m_time += other.m_time;				
}
void	xrTime::sub				(const xrTime& other)					
{  
	if(*this>other)
		m_time -= other.m_time; 
	else 
		m_time=0;	
}

void	xrTime::setHMS			(int h, int m, int s)					
{ 
	m_time=0; 
	m_time+=(h*hour2ms+m*min2ms+s*sec2ms);
}

void	xrTime::setHMSms		(int h, int m, int s, int ms)			
{ 
	m_time=0; 
	m_time+=(h*hour2ms+m*min2ms+s*sec2ms+ms);
}

float	xrTime::diffSec			(const xrTime& other)					
{ 
	if(*this>other) 
		return (m_time-other.m_time)/(float)sec2ms; 
	return ((other.m_time-m_time)/(float)sec2ms)*(-1.0f);	
}
