//////////////////////////////////////////////////////////
//	Module			:	Time routines using RDTSC 		//
//	File			:	_time.cpp						//
//	Creation date	:	31.08.2002						//
//	Author			:	Dmitriy Iassenev				//
//////////////////////////////////////////////////////////

#include "stdafx.h"

#include "_time.h"

#ifdef __PROFILE__
	u64	qwaTimerCalls[MAX_TIMER_COUNT];
	u64	qwaTimerTimes[MAX_TIMER_COUNT];
	u32	dwTimerCount = 0;
#endif

TIMER_DECLARE(TimerOverhead)
TIMER_DECLARE(MPIMemCpy)
TIMER_DECLARE(EvaluationFunction)
TIMER_DECLARE(GetAvailableMoves)
TIMER_DECLARE(MoveToFrontByValue)
TIMER_DECLARE(CheckIfAvailable_FALSE)
TIMER_DECLARE(CheckIfAvailable_TRUE)
TIMER_DECLARE(MakeMoveMobilityIfAvailableFast_FALSE)
TIMER_DECLARE(MakeMoveMobilityIfAvailableFast_TRUE)
TIMER_DECLARE(MakeMoveIfAvailableFast_FALSE)
TIMER_DECLARE(MakeMoveIfAvailableFast_TRUE)
TIMER_DECLARE(ChangeEFIndexesIfAvailableFast_FALSE)
TIMER_DECLARE(ChangeEFIndexesIfAvailableFast_TRUE)
TIMER_DECLARE(ChangeHashIfAvailable_FALSE)
TIMER_DECLARE(ChangeHashIfAvailable_TRUE)
TIMER_DECLARE(Undo)
TIMER_DECLARE(PrintPrincipalVariation)
TIMER_DECLARE(GlobalMoveStack)
TIMER_DECLARE(GetTTNode)
TIMER_DECLARE(GetEndgameTTNode)
TIMER_DECLARE(PutTTNode)
TIMER_DECLARE(PutEndgameTTNode)
TIMER_DECLARE(MoveToFrontByIndex)
TIMER_DECLARE(MoveToBackByValue)
TIMER_DECLARE(GetAvailableMovesWithEmpties)
TIMER_DECLARE(GetAvailableMovesAndFastestFirst)
TIMER_DECLARE(GetAvailableMovesAndFastestFirstX)
TIMER_DECLARE(FastestFirst)
TIMER_DECLARE(SwapMoves)
TIMER_DECLARE(MakeSavedMove)
TIMER_DECLARE(UndoSavedMove)
