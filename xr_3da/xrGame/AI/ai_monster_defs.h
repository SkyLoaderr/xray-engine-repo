#pragma once

typedef u32 TTime;


// Logging
// Using multiparam macros: 
// e.g. OUTPUT_M("X = [%u], vector = [%f,%f,%f]", *"*/ m_dwCurrentTime, VPUSH(Position())  /*"*);

#define ENABLE_WRITE_LOG_EX

#undef	OUTPUT
#undef	OUTPUT_M

#ifndef ENABLE_WRITE_LOG_EX
	
	#define OUTPUT(str) ;
	#define OUTPUT_M(str,params) ;
	
#else

	#define OUTPUT(str) Msg(str);
	#define OUTPUT_M(str,params) Msg(str,/params/);

#endif

