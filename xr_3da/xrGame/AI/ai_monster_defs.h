#pragma once

typedef u32 TTime;


// Logging
// Using multiparam macros: 
// e.g. LOG_EX2("X = [%u], vector = [%f,%f,%f]", *"*/ m_dwCurrentTime, VPUSH(Position())  /*"*);

#define ENABLE_WRITE_LOG_EX

#undef	LOG_EX
#undef	LOG_EX2

#ifndef ENABLE_WRITE_LOG_EX
	
	#define LOG_EX(str) ;
	#define LOG_EX2(str,params) ;
	
#else

	#define LOG_EX(str) Msg(str);
	#define LOG_EX2(str,params) Msg(str,/params/);

#endif

