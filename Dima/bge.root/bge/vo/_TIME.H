//////////////////////////////////////////////////////////
//	Module			:	Time routines using RDTSC 		//
//	File			:	_time.h							//
//	Creation date	:	31.08.2002						//
//	Author			:	Dmitriy Iassenev				//
//////////////////////////////////////////////////////////

#ifndef __VO_TIME__
#define __VO_TIME__

#include "_math.h"
#include "_time.h"

#define	TICKS_PER_SECOND	1000

IC u64 qwfClock() 
{
	return CPU::qwfGetCycleCount()*TICKS_PER_SECOND/CPU::qwCyclesPerSecond;
};

#ifdef __PROFILE__
	#define MAX_TIMER_COUNT		  64

	extern u64	qwaTimerCalls[MAX_TIMER_COUNT];
	extern u64	qwaTimerTimes[MAX_TIMER_COUNT];
	extern u32	dwTimerCount;

	#define TIMER_EXTERN_DECLARE(a) \
		extern u64 qwTemp1##a, qwTemp2##a, &qwTime##a, &qwCount##a;

	#define TIMER_DECLARE(a) \
		u64 qwTemp1##a, qwTemp2##a, &qwTime##a = qwaTimerTimes[dwTimerCount] = 0, &qwCount##a = qwaTimerCalls[dwTimerCount++] = 0;
	#define TIMER_INIT(a) \
		qwTime##a = qwCount##a = 0;

	#define TIMER_START(a) \
		qwTemp1##a = CPU::qwfGetCycleCount();

	#define TIMER_STOP(a) \
		qwTemp2##a = CPU::qwfGetCycleCount();\
		qwTime##a += qwTemp2##a - qwTemp1##a - CPU::qwCyclesPerRDTSC;\
		qwCount##a++;

	#define TIMER_STOP_PREVIOUS(a,b) \
		qwTemp2##a = CPU::qwfGetCycleCount();\
		qwTime##a += qwTemp2##a - qwTemp1##b - CPU::qwCyclesPerRDTSC;\
		qwCount##a++;

	#define PRINT_TIMER_HEADER {\
		vfDualPrintF("+----------+-----------+-----------------------------------------------------+\n");\
		vfDualPrintF("|   time   |   count   |                        function                     |\n");\
		vfDualPrintF("+----------+-----------+-----------------------------------------------------+\n");\
	}

	#define TIMER_PRINT(a) \
		vfDualPrintF("|%10.5f|%11I64u|%52s |\n",f64(i64(qwTime##a))/f64(i64(CPU::cycles_per_second)),qwCount##a,#a);

	#define PRINT_PROFILE_TIME {\
		u64 qwTemp1 = 0, qwTemp2 = 0;\
		for (u32 _i_=0; _i_ < dwTimerCount; _i_++) {\
			qwTemp1 += qwaTimerCalls[_i_];\
			qwTemp2 += qwaTimerTimes[_i_];\
		}\
		vfDualPrintF("+----------+-----------+-----------------------------------------------------+\n");\
		vfDualPrintF("|%10.5f|%11I64u|                                               Total |\n",f64(i64(qwTemp2))/f64(i64(CPU::cycles_per_second)),qwTemp1);\
		vfDualPrintF("+==========+===========+=====================================================+\n");\
		vfDualPrintF("|%10.5f|%11I64u|                                           Profiling |\n",f64(i64(qwTemp1*CPU::timer_overhead))/f64(i64(CPU::cycles_per_second)),qwTemp1);\
		vfDualPrintF("+----------+-----------+-----------------------------------------------------+\n");\
	}

	#define PRINT_TIMERS {\
		PRINT_TIMER_HEADER\
		TIMER_PRINT(MPIMemCpy)\
		TIMER_PRINT(EvaluationFunction)\
		TIMER_PRINT(GetAvailableMoves)\
		TIMER_PRINT(MoveToFrontByValue)\
		TIMER_PRINT(CheckIfAvailable_FALSE)\
		TIMER_PRINT(CheckIfAvailable_TRUE)\
		TIMER_PRINT(MakeMoveMobilityIfAvailableFast_FALSE)\
		TIMER_PRINT(MakeMoveMobilityIfAvailableFast_TRUE)\
		TIMER_PRINT(MakeMoveIfAvailableFast_FALSE)\
		TIMER_PRINT(MakeMoveIfAvailableFast_TRUE)\
		TIMER_PRINT(ChangeEFIndexesIfAvailableFast_FALSE)\
		TIMER_PRINT(ChangeEFIndexesIfAvailableFast_TRUE)\
		TIMER_PRINT(ChangeHashIfAvailable_FALSE)\
		TIMER_PRINT(ChangeHashIfAvailable_TRUE)\
		TIMER_PRINT(Undo)\
		TIMER_PRINT(GlobalMoveStack)\
		TIMER_PRINT(GetTTNode)\
		TIMER_PRINT(PutTTNode)\
		TIMER_PRINT(MoveToFrontByIndex)\
		TIMER_PRINT(MoveToBackByValue)\
		TIMER_PRINT(GetEndgameTTNode)\
		TIMER_PRINT(PutEndgameTTNode)\
		TIMER_PRINT(GetAvailableMovesWithEmpties)\
		TIMER_PRINT(GetAvailableMovesAndFastestFirst)\
		TIMER_PRINT(PrintPrincipalVariation)\
		TIMER_PRINT(FastestFirst)\
		TIMER_PRINT(SwapMoves)\
		TIMER_PRINT(MakeSavedMove)\
		TIMER_PRINT(UndoSavedMove)\
		PRINT_PROFILE_TIME\
	}
	#define RESET_ALL_TIMERS {\
		memset(qwaTimerCalls,0,sizeof(qwaTimerCalls));\
		memset(qwaTimerTimes,0,sizeof(qwaTimerTimes));\
	}

#else
	#define TIMER_EXTERN_DECLARE(a)
	#define TIMER_DECLARE(a)
	#define TIMER_INIT(a)
	#define TIMER_START(a)
	#define TIMER_STOP(a)
	#define TIMER_STOP_PREVIOUS(a,b)
	#define TIMER_PRINT(a)
	#define PRINT_PROFILE_TIME
	#define PRINT_TIMER_HEADER
	#define PRINT_TIMERS
	#define RESET_ALL_TIMERS
#endif

	TIMER_EXTERN_DECLARE(TimerOverhead)
	TIMER_EXTERN_DECLARE(MPIMemCpy)
	TIMER_EXTERN_DECLARE(EvaluationFunction)
	TIMER_EXTERN_DECLARE(GetAvailableMoves)
	TIMER_EXTERN_DECLARE(MoveToFrontByValue)
	TIMER_EXTERN_DECLARE(CheckIfAvailable_FALSE)
	TIMER_EXTERN_DECLARE(CheckIfAvailable_TRUE)
	TIMER_EXTERN_DECLARE(MakeMoveMobilityIfAvailableFast_FALSE)
	TIMER_EXTERN_DECLARE(MakeMoveMobilityIfAvailableFast_TRUE)
	TIMER_EXTERN_DECLARE(MakeMoveIfAvailableFast_FALSE)
	TIMER_EXTERN_DECLARE(MakeMoveIfAvailableFast_TRUE)
	TIMER_EXTERN_DECLARE(ChangeEFIndexesIfAvailableFast_FALSE)
	TIMER_EXTERN_DECLARE(ChangeEFIndexesIfAvailableFast_TRUE)
	TIMER_EXTERN_DECLARE(ChangeHashIfAvailable_FALSE)
	TIMER_EXTERN_DECLARE(ChangeHashIfAvailable_TRUE)
	TIMER_EXTERN_DECLARE(Undo)
	TIMER_EXTERN_DECLARE(PrintPrincipalVariation)
	TIMER_EXTERN_DECLARE(GlobalMoveStack)
	TIMER_EXTERN_DECLARE(GetTTNode)
	TIMER_EXTERN_DECLARE(GetEndgameTTNode)
	TIMER_EXTERN_DECLARE(PutTTNode)
	TIMER_EXTERN_DECLARE(PutEndgameTTNode)
	TIMER_EXTERN_DECLARE(MoveToFrontByIndex)
	TIMER_EXTERN_DECLARE(MoveToBackByValue)
	TIMER_EXTERN_DECLARE(GetAvailableMovesWithEmpties)
	TIMER_EXTERN_DECLARE(GetAvailableMovesAndFastestFirst)
	TIMER_EXTERN_DECLARE(FastestFirst)
	TIMER_EXTERN_DECLARE(SwapMoves)
	TIMER_EXTERN_DECLARE(MakeSavedMove)
	TIMER_EXTERN_DECLARE(UndoSavedMove)
#endif //__VO_TIME__
