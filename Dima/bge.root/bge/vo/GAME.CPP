//////////////////////////////////////////////////////////////////////////////////
//	Module			:	Game routines											//
//	File			:	game.cpp												//
//	Creation date	:	04.04.2002												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
// Used modules
//////////////////////////////////////////////////////////

#include "stdafx.h"

#include "board.h"
#include "eg_pvs.h"
#include "eval.h"
#include "game.h"
#include "mobil.h"
#include "mg_pvs.h"
#include "misc.h"
#include "mpc.h"
#include "tt.h"
#include "_math.h"
#include "_time.h"
#include "_mmx.h"
#include "interface.h"

//////////////////////////////////////////////////////////
// Variables declaration
//////////////////////////////////////////////////////////

//#define FIXED_DEPTH
#ifdef FIXED_DEPTH
	#define DEPTH_SEARCH 17
#endif

u64				qwStart, qwFinish, qwMoveStart, qwMoveFinish, qwStartThink, qwStopThink;
TBoardCell		tLastBest;
TBoardCell		tCurBest;
u8				ucLastDepth			= 0;
u8				ucBugLastDepth		= 0;
SFeature		tLastValue			= 0;
u32				dwCurrentMode    	= 0;
u32				dwCurrentTest    	= 0;
f64				dCurTimeLock     	= 0;
f64				dTotalWLDLock    	= 0;
f64				dTotalWLDTime    	= 0;
f64				dTotalExactLock  	= 0;
f64				dStartTime       	= DEFAULT_TIME;
f64				dTimeLeft        	= dStartTime;
bool			bTotalWLD        	= false;
bool			bExactScore      	= false;
bool			bStartedToSearch 	= true;
char			cCurMoveNumber   	= 0;
char			cCurMoveCount		= 1;
bool			bUpdated	     	= false;
bool			bChanging			= false;
HANDLE			hThreadHandle		= INVALID_HANDLE_VALUE;
TGame			tGame;
TSearchGameInfo tGlobalSearchParameters;

//////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////

void vfLoadQFParameters()
{
	vfDualPrintF("Loading qdwescence search parameters...");
	vfDualPrintF("not implemented yet\n");
}

void vfPrepareForGame()
{
	if (hThreadHandle != INVALID_HANDLE_VALUE) {
		while (bInHash)
			WaitForSingleObject(hThreadHandle,1);
		TerminateThread(hThreadHandle,1);
		CloseHandle(hThreadHandle);
		hThreadHandle = INVALID_HANDLE_VALUE;
	}
    memset(taHash,0,sizeof(taHash));
	memset(caNotationBoard,0,sizeof(caNotationBoard));

	//vfInitializeMoveList();
}

void vfLoadOpeningBook()
{
	vfDualPrintF("Loading opening book...");
	vfDualPrintF("not implemented yet\n");
}

void vfInitAll()
{
	CPU::vfInitHardware();
	vfInitalizeHash(6);
	vfLoadEFParameters();
	vfLoadMPCParameters();
	vfLoadQFParameters();
	vfLoadOpeningBook();
	vfLoadMobilityPatterns();
//	InitializeCriticalSection(&tCriticalSection);
}

char *cafPrintPrincipalVariation(TBoardCell *taBoard, char tBestMove, u8 ucDepth, TBoardCell tColor, SFeature tValue)
{
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	static char caString[80];
	sprintf(caString,"%c%c xx xx xx xx xx xx",(tBestMove - BOARD_START) % 9 + 'A',(tBestMove - BOARD_START) / 9 + '1');
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	return(caString);
}

void vfPrintPrincipalVariationX(TBoardCell *taBoard, TBoardCell tBestMove, u8 ucDepth, TBoardCell tColor, SFeature tValue, bool bWLD, bool bOverTime = false)
{
	bUpdated = false;
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	qwMoveFinish = qwfClock();
    
	int iPercent = 100;
	if (cCurMoveCount)
		iPercent = 100*cCurMoveNumber / cCurMoveCount;
	f64 dLock = (f64)(s64(qwMoveFinish - qwStart))/TICKS_PER_SECOND;
    u64 qwVC = qwVariationCount;

	bool bPVSChanged = false;
	if (tBestMove != tLastBest) {
        tLastBest = tBestMove;
        dCurTimeLock = dLock;
		bPVSChanged = true;
    }

	//tLastValue = tValue;

	/**
	if ((bOverTime) && (!tLastBest)) {
		
		THashValue tHashValue;
		u8 ucLevel;
		
		if (tColor == BLACK)
			vfCreateHashValue(taBoard,&tHashValue,WHITE);
		else
			vfCreateHashValue(taBoard,&tHashValue,BLACK);
		
		int iIndex = ifGetTTNode(tHashValue,&ucLevel);
		if (iIndex != -1) {
			tLastBest = tBestMove = taHash[iIndex].taCells[ucLevel].ucMove;
			tValue = taHash[iIndex].taCells[ucLevel].tScore;
			ucDepth = taHash[iIndex].taCells[ucLevel].ucDepth;
		}
		else {
			vfDualPrintF("Time is over... :-(\n");
			return;
		}
	}
	/**/

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);

	//ucLastDepth = ucDepth;
	//tLastValue = tValue;
    
	//bTotalWLD = bWLD;

	/**/
	if ((!bPVSChanged) && (!bStartedToSearch)) {
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		for (int i=0; i<78; i++) {
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
			printf("\b \b");
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		}
	}
	else /**/{
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (!bStartedToSearch)
			printf("\n");
	}

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	if (!bWLD) {
		vfDualPrintF("|%3d|%3d| %1c%5.2f| %s |%11I64u |%8.0f | %s |",
				ucDepth,
				iPercent,
				(tValue>0 ? '+' : (tValue<0 ? '-' : ' ')),
				fabs((f64)tValue/128.0),
				cafTimeToString(dLock),
				qwVC,
				dLock>0 ? (dLock ? qwVC/dLock : 0) : 0,
				cafPrintPrincipalVariation(taBoard,tBestMove,ucDepth,tColor,tValue));
	}
	else
		if (!bExactScore)
			if (tValue > 0)
				vfDualPrintF("|WLD|%3d|  Win  | %s |%11I64u |%8.0f | %s |",
						iPercent,
						cafTimeToString(dLock),
						qwVC,
						dLock>0 ? (dLock ? qwVC/dLock : 0) : 0,
						cafPrintPrincipalVariation(taBoard,tBestMove,ucDepth,tColor,tValue));
			else
				if (tValue < 0)
					vfDualPrintF("|WLD|%3d|  Loss | %s |%11I64u |%8.0f | %s |",
							iPercent,
							cafTimeToString(dLock),
							qwVC,
							dLock>0 ? (dLock ? qwVC/dLock : 0) : 0,
							cafPrintPrincipalVariation(taBoard,tBestMove,ucDepth,tColor,tValue));
				else
					vfDualPrintF("|WLD|%3d|  Draw | %s |%11I64u |%8.0f | %s |",
							iPercent,
							cafTimeToString(dLock),
							qwVC,
							dLock>0 ? (dLock ? qwVC/dLock : 0) : 0,
							cafPrintPrincipalVariation(taBoard,tBestMove,ucDepth,tColor,tValue));
		else
			vfDualPrintF("| ES|%3d| %1c%5.2f| %s |%11I64u |%8.0f | %s |",
					iPercent,
					(tValue>0 ? '+' : (tValue<0 ? '-' : ' ')),
					fabs((f64)tValue/128.0),
					cafTimeToString(dLock),
					qwVC,
					dLock>0 ? (dLock ? qwVC/dLock : 0) : 0,
					cafPrintPrincipalVariation(taBoard,tBestMove,ucDepth,tColor,tValue));

#ifdef WRITE_LOG
	fprintf(fVOOutput,"\n");
#endif

	bStartedToSearch = false;
	qwMoveStart = qwMoveFinish;
}

void vfPrintPrincipalVariation(TBoardCell tBestMove, u8 ucDepth, TBoardCell tColor, SFeature tValue, bool bWLD, char cCurrentMove, bool bOverTime)
{
	TIMER_START(PrintPrincipalVariation)
	/**/
	//if (bStopThink)
	//	return;

//	EnterCriticalSection(&tCriticalSection);
	//bChanging = true;
	//bInHash = true;
	cCurMoveNumber = cCurrentMove;
	__ASSERT(tBestMove);
	tCurBest = tBestMove;
	//ucLastDepth = ucDepth;
	tLastValue = tValue;
	bTotalWLD = bWLD;
	bUpdated = true;
	
	//vfPrintPrincipalVariationX(tGlobalSearchParameters.taBoard,tCurBest,ucDepth,tColor,tValue,bWLD,bOverTime);

#ifdef FIXED_DEPTH 
	//vfPrintPrincipalVariationX(tGlobalSearchParameters.taBoard,tCurBest,ucDepth,tColor,tValue,bWLD,bOverTime);
#endif
	//bChanging = false;
	//bInHash = false;
//	LeaveCriticalSection(&tCriticalSection);
	TIMER_STOP(PrintPrincipalVariation)
	return;
	/**

	if (bStopThink)
		return;

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	qwMoveFinish = GetTickCount();
    
	cCurMoveNumber = cCurrentMove;
	cCurMoveCount  = cMoveCount;
	int iPercent = 100;
	if (cMoveCount)
		iPercent = 100*cCurrentMove / cMoveCount;
	f64 dLock = (f64)(qwMoveFinish - qwStart)/TICKS_PER_SECOND;
    u64 qwVC = qwVariationCount;
    
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	bool bPVSChanged = false;
	if (tBestMove != tLastBest) {
        tLastBest = tBestMove;
        dCurTimeLock = dLock;
		bPVSChanged = true;
    }

	tLastValue = tValue;

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	if ((bOverTime) && (!tLastBest)) {
		
		THashValue tHashValue;
		u8 ucLevel;
		
		if (tColor == BLACK)
			vfCreateHashValue(taBoard,&tHashValue,WHITE);
		else
			vfCreateHashValue(taBoard,&tHashValue,BLACK);
		
		int iIndex = ifGetTTNode(&tHashValue,&ucLevel);
		if (iIndex != -1) {
			tBestMove = tLastBest = taHash[iIndex].taCells[ucLevel].ucMove;
			tValue = taHash[iIndex].taCells[ucLevel].tScore;
			ucDepth = taHash[iIndex].taCells[ucLevel].ucDepth;
		}
		else {
			vfDualPrintF("Time is over... :-(\n");
			return;
		}
	}

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);

	ucLastDepth = ucDepth;
	tLastValue = tValue;
    
	bTotalWLD = bWLD;

	if ((!bPVSChanged) && (!bStartedToSearch)) {
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		for (int i=0; i<78; i++) {
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
			printf("\b \b");
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		}
	}
	else {
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (!bStartedToSearch)
			printf("\n");
	}

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	if (!bWLD) {
		vfDualPrintF("|%3d|%3d| %1c%5.2f| %s |%11d |%8.0f | %s |",
				ucDepth,
				iPercent,
				(tValue>0 ? '+' : (tValue<0 ? '-' : ' ')),
				fabs((f64)tValue/128.0),
				cafTimeToString(dLock),
				qwVC,
				dLock>0 ? (dLock ? qwVC/dLock : 0) : 0,
				cafPrintPrincipalVariation(taBoard,tBestMove,ucDepth,tColor,tValue));
	}
	else
		if (!bExactScore)
			if (tValue > 0)
				vfDualPrintF("|WLD|%3d|  Win  | %s |%11d |%8.0f | %s |",
						iPercent,
						cafTimeToString(dLock),
						qwVC,
						dLock>0 ? (dLock ? qwVC/dLock : 0) : 0,
						cafPrintPrincipalVariation(taBoard,tBestMove,ucDepth,tColor,tValue));
			else
				if (tValue < 0)
					vfDualPrintF("|WLD|%3d|  Loss | %s |%11d |%8.0f | %s |",
							iPercent,
							cafTimeToString(dLock),
							qwVC,
							dLock>0 ? (dLock ? qwVC/dLock : 0) : 0,
							cafPrintPrincipalVariation(taBoard,tBestMove,ucDepth,tColor,tValue));
				else
					vfDualPrintF("|WLD|%3d|  Draw | %s |%11d |%8.0f | %s |",
							iPercent,
							cafTimeToString(dLock),
							qwVC,
							dLock>0 ? (dLock ? qwVC/dLock : 0) : 0,
							cafPrintPrincipalVariation(taBoard,tBestMove,ucDepth,tColor,tValue));
		else
			vfDualPrintF("| ES|%3d| %1c%5.2f| %s |%11d |%8.0f | %s |",
					iPercent,
					(tValue>0 ? '+' : (tValue<0 ? '-' : ' ')),
					fabs((f64)tValue/128.0),
					cafTimeToString(dLock),
					qwVC,
					dLock>0 ? (dLock ? qwVC/dLock : 0) : 0,
					cafPrintPrincipalVariation(taBoard,tBestMove,ucDepth,tColor,tValue));

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);

	fprintf(fVOOutput,"\n");

	bStartedToSearch = false;
	qwMoveStart = qwMoveFinish;
	/**/
}

void vfPrepareForMove(TBoardCell *taBoard, TBoardCell tColor, char *cCurrentValue, u8 *ucEmpties)
{
	memcpy(tGlobalSearchParameters.taBoard,taBoard,BOARD_SIZE*sizeof(TBoardCell));

	if (tColor == BLACK)
		vfCreateHashValue(tGlobalSearchParameters.taBoard,&tGlobalSearchParameters.tHashValue,WHITE);
	else
		vfCreateHashValue(tGlobalSearchParameters.taBoard,&tGlobalSearchParameters.tHashValue,BLACK);

	*cCurrentValue = tColor==BLACK ? cfGetDiskDifference() : -cfGetDiskDifference();
	
	*ucEmpties = ucfGetEmptiesCount();

	cBugTimeStamp = cGlobalTimeStamp = 60 - *ucEmpties;

	cCurMoveCount = cfGetMoveCount(tColor);
    
	vfInitializeMoveList();
	vfPrepareMoveListForBoard();
	vfInitMPIndexes();

	qwMoveStart			  = qwStart;
	qwVariationCount	  = 0;
    tLastBest			  = 0;
    dCurTimeLock		  = 0;
	bInHash				  = 0;
    tppFlipStack		  = tpaGlobalFlipStack;
	//tpMoveStack			  = taGlobalMoveStack + MAX_MOVE_COUNT;
#ifdef _DEBUG
    dwTTUseCount		  = 0;
    dwTTAddCount		  = 0;
    dwTTCollision		  = 0;
#endif
}

DWORD WINAPI vpfAnalyzeGame(VOID)
{
	//f64 taAlphaWindow[] = {0,3,3,2,2,1.9,1.9,1.7,1.7,1.6,1.5,1.5,1.3,1.3};
	//f64 taBetaWindow[] = {0,3,3,2,2,1.9,1.9,1.7,1.7,1.6,1.5,1.5,1.3,1.3};
//	EnterCriticalSection(&tCriticalSection);
	TBoardCell tColor = tGame.tColor;
	char       cCurrentValue = 0;
	u8      ucEmpties = 0;

	vfPrepareForMove(tGame.taBoard,tColor,&cCurrentValue,&ucEmpties);
	
	if (ucEmpties < 4) {
		bExactScore = true;
		tfSolveForMate(ucEmpties,-INFINITY,+INFINITY,tColor,cCurrentValue);
		tCurBest = taGlobalMoveStack[0];
//		LeaveCriticalSection(&tCriticalSection);
		return(ucEmpties);
	}
	
	u8      ucPly = tGame.ucPly;
	SFeature   tValue = 0, tValueX = 0;
	SFeature   tOldValue = -INFINITY;
	
	tOldValue = tLastValue;
	bExactScore = false;

//	LeaveCriticalSection(&tCriticalSection);
	PERCENTILE = THRESHOLD_90_PERCENTS;
	if (ucEmpties > 2) {
		u8 ucLevel;
		int iIndex = ifGetTTNode(tGlobalSearchParameters.tHashValue,&ucLevel);
		if (iIndex >= 0) {
			if (taHash[iIndex].taCells[ucLevel].ucDepth == ucEmpties) {
				taHash[iIndex].taCells[ucLevel].ucDepth = 0;
			}
		}
		for (u8 ucDepth = ucEmpties % 2 == 0 ? 2 : 1; ucDepth <= ucPly; ucDepth+=2) {
			
			bInHash = true;
			cCurMoveNumber = 0;
			ucBugLastDepth = ucLastDepth = ucDepth;
			bInHash = false;

			tValue = tfSearchGameRoot(ucDepth,ucEmpties,-INFINITY,+INFINITY,tColor,cCurrentValue,cGlobalTimeStamp,USE_PROB_CUT);
			/**
			SFeature	tDelta = 192;
			tValueX = tValue;
			tValue = tfSearchGameRoot(ucDepth,ucEmpties,tValueX - tDelta,tValueX + tDelta,tColor,cCurrentValue,cGlobalTimeStamp,USE_PROB_CUT);
			
			/**
			if (tValue <= tValueX - tDelta)
				while (tValue <= tValueX - tDelta) {
					tValueX = tValue - tDelta;
					tValue = tfSearchGameRoot(ucDepth,ucEmpties,tValueX - tDelta,tValueX + tDelta,tColor,cCurrentValue,cGlobalTimeStamp,USE_PROB_CUT);
				}
			else
				if (tValue >= tValueX + tDelta)
					while (tValue >= tValueX + tDelta) {
						tValueX = tValue + tDelta;
						tValue = tfSearchGameRoot(ucDepth,ucEmpties,tValueX - tDelta,tValueX + tDelta,tColor,cCurrentValue,cGlobalTimeStamp,USE_PROB_CUT);
					}
			/**
			if (tValue <= tValueX - tDelta)
				tValue = tfSearchGameRoot(ucDepth,ucEmpties,-INFINITY,tValueX - tDelta,tColor,cCurrentValue,cGlobalTimeStamp,USE_PROB_CUT);
			else
				if (tValue >= tValueX + tDelta)
					tValue = tfSearchGameRoot(ucDepth,ucEmpties,tValueX + tDelta,+INFINITY,tColor,cCurrentValue,cGlobalTimeStamp,USE_PROB_CUT);
			/**/


			MMX_FLUSH;

			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
			vfPrintPrincipalVariation(tCurBest,ucDepth,tColor,tValue,false,cCurMoveCount);
			//vfPrintPrincipalVariationX(tGame.taBoard,tCurBest,ucDepth,tColor,tValue,false,true);
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
			__ASSERT(ucBugLastDepth == ucLastDepth);
		}
		tOldValue = tValue;
	}
	/**/
	//if (bStopThink)
	//	return(0);
	
#ifdef FIXED_DEPTH 
	//if (ucEmpties > 22)
	//	return(0);
#endif
	if (ucEmpties >= 18) {
		bInHash = true;
		cCurMoveNumber = 0;
		ucBugLastDepth = ucLastDepth = ucEmpties-2;
		bInHash = false;
		tOldValue = tValue = tfSearchGameRoot(ucLastDepth,ucEmpties,-64*128,+64*128,tColor,cCurrentValue,cGlobalTimeStamp,USE_PROB_CUT);
//		tOldValue = tValue = tfSearchSelectiveEndgameRoot(ucLastDepth,ucEmpties,-64*128,+64*128,tColor,cCurrentValue,cGlobalTimeStamp,USE_PROB_CUT);
		MMX_FLUSH
		vfPrintPrincipalVariation(tCurBest,ucLastDepth,tColor,tValue,false,cCurMoveCount);
		//vfPrintPrincipalVariationX(tGame.taBoard,tCurBest,ucEmpties-2,tColor,tValue,false,true);
		if (ucEmpties >= 22) {
			bInHash = true;
			cCurMoveNumber = 0;
			ucBugLastDepth = ucLastDepth = ucEmpties;
			PERCENTILE = THRESHOLD_99_PERCENTS;
			bInHash = false;
			tOldValue = tValue = tfSearchGameRoot(ucLastDepth,ucEmpties,-64*128,+64*128,tColor,cCurrentValue,cGlobalTimeStamp,USE_PROB_CUT);
//			tOldValue = tValue = tfSearchSelectiveEndgameRoot(ucLastDepth,ucEmpties,-64*128,+64*128,tColor,cCurrentValue,cGlobalTimeStamp,USE_PROB_CUT);
			MMX_FLUSH
			vfPrintPrincipalVariation(tCurBest,ucLastDepth,tColor,tValue,false,cCurMoveCount);
			//vfPrintPrincipalVariationX(tGame.taBoard,tCurBest,ucEmpties,tColor,tValue,false,true);
		}
	}
	//tfSearchEndgameRoot(ucEmpties,-2*128,2*128,tColor,cCurrentValue,cGlobalTimeStamp,USE_PROB_CUT);
	
	cBugTimeStamp = cGlobalTimeStamp = 121 - ucEmpties;

	//tValue = tOldValue > 0 ? 1 : tOldValue < 0 ? -1 : 0;
	/////////////////////////////////////////////////////////////////
	RESET_ALL_TIMERS;
	/////////////////////////////////////////////////////////////////
	
	
	/**/
	tValue = tfSolveGameRoot(ucEmpties,-2*128,0,tColor,cCurrentValue);
	MMX_FLUSH
	if (tValue >= 0) {
		tValue = tfSolveGameRoot(ucEmpties,0,2*128,tColor,cCurrentValue);
		MMX_FLUSH
	/**
		bExactScore = true;
		tValue = tfSolveGameRoot(ucEmpties,0*128,+64*128,tColor,cCurrentValue);
	}
	else {
		bExactScore = true;
		tValue = tfSolveGameRoot(ucEmpties,-64*128,0*128,tColor,cCurrentValue);
	/**/
	}
	
	
	/**/
	vfPrintPrincipalVariation(tCurBest,ucEmpties,tColor,tValue,true,cCurMoveCount);

#ifdef FIXED_DEPTH 
	//if (ucEmpties > 22)
	//	return(0);
#endif
	bExactScore = true;
	//tOldValue = tValue;
	/**
	if (tValue < 0)
		for (int i=0; i>=-64; i-=2) {
			tValue = tfSolveGameRoot(ucEmpties,(i-2)*128,i*128,tColor,cCurrentValue);
			MMX_FLUSH
			if (tValue == i*128)
				break;
		}
	else
		for (int i=2; i<=64; i+=2) {
			tValue = tfSolveGameRoot(ucEmpties,i*128,(i+2)*128,tColor,cCurrentValue);
			MMX_FLUSH
			if (tValue == i*128)
				break;
		}
	/**/
	if (tValue < 0)
		if (tOldValue >= 0)
			for (int i=0; i>=-64; i-=2) {
				tValue = tfSolveGameRoot(ucEmpties,(i-2)*128,i*128,tColor,cCurrentValue);
				if (tValue == i*128)
					break;
			}
		else {
			tOldValue = -((((((-tOldValue) / 128) - 1) / 2) + 1) * 2);
			if (tOldValue > -64 )
				tValue = tfSolveGameRoot(ucEmpties,(tOldValue - 2)*128,(tOldValue + 0)*128,tColor,cCurrentValue);
			else
				tValue = tfSolveGameRoot(ucEmpties,(tOldValue - 0)*128,(tOldValue + 2)*128,tColor,cCurrentValue);
			if (tValue >= tOldValue*128)
				for (int i=tOldValue; i<0; i+=2) {
					tValue = tfSolveGameRoot(ucEmpties,i*128,(i+2)*128,tColor,cCurrentValue);
					if (tValue == i*128)
						break;
				}
			else
				for (int i=tOldValue - 2; i>=-64; i-=2) {
					tValue = tfSolveGameRoot(ucEmpties,(i-2)*128,i*128,tColor,cCurrentValue);
					if (tValue == i*128)
						break;
				}
		}
	else
		if (tValue > 0)
			if ((tOldValue <= 0) || (false))
				for (int i=0; i<64; i+=2) {
					tValue = tfSolveGameRoot(ucEmpties,i*128,(i+2)*128,tColor,cCurrentValue);
					if (tValue == i*128)
						break;
				}
			else {
				tOldValue = (((((tOldValue / 128) - 1) / 2) + 1) * 2);
				if (tOldValue < 64)
					tValue = tfSolveGameRoot(ucEmpties,(tOldValue - 0)*128,(tOldValue + 2)*128,tColor,cCurrentValue);
				else
					tValue = tfSolveGameRoot(ucEmpties,(tOldValue - 2)*128,(tOldValue + 0)*128,tColor,cCurrentValue);
				if (tValue <= tOldValue*128)
					for (int i=tOldValue; i>0; i-=2) {
						tValue = tfSolveGameRoot(ucEmpties,(i-2)*128,i*128,tColor,cCurrentValue);
						if (tValue == i*128)
							break;
					}
				else
					for (int i=tOldValue + 2; i<64; i+=2) {
						tValue = tfSolveGameRoot(ucEmpties,i*128,(i+2)*128,tColor,cCurrentValue);
						if (tValue == i*128)
							break;
					}
			}
	/**
	if (tValue < 0)
		if (tOldValue >= 0)
			for (int i=0; i>=-64; i-=2) {
				tValue = tfSolveGameRoot(ucEmpties,(i-2)*128,0,tColor,cCurrentValue);
				if (tValue > (i-2)*128)
					break;
			}
		else {
			tOldValue = -((((((-tOldValue) / 128) - 1) / 2) + 1) * 2);
			tValue = tfSolveGameRoot(ucEmpties,(tOldValue - 2)*128,tOldValue*128,tColor,cCurrentValue);
			if (tValue == tOldValue*128)
				for (int i=tOldValue; i<0; i+=2) {
					tValue = tfSolveGameRoot(ucEmpties,tOldValue*128,(i+2)*128,tColor,cCurrentValue);
					if (tValue < (i+2)*128)
						break;
				}
			else
				for (int i=tOldValue - 2; i>=-64; i-=2) {
					tValue = tfSolveGameRoot(ucEmpties,(i-2)*128,tOldValue*128,tColor,cCurrentValue);
					if (tValue > (i-2)*128)
						break;
				}
		}
	else
		if (tValue > 0)
			if ((tOldValue <= 0) || (false))
				for (int i=0; i<=64; i+=2) {
					tValue = tfSolveGameRoot(ucEmpties,0,(i+2)*128,tColor,cCurrentValue);
					if (tValue < (i+2)*128)
						break;
				}
			else {
				tOldValue = (((((tOldValue / 128) - 1) / 2) + 1) * 2);
				tValue = tfSolveGameRoot(ucEmpties,tOldValue*128,(tOldValue + 2)*128,tColor,cCurrentValue);
				if (tValue == tOldValue*128)
					for (int i=tOldValue; i>0; i-=2) {
						tValue = tfSolveGameRoot(ucEmpties,(i-2)*128,tOldValue*128,tColor,cCurrentValue);
						if (tValue > (i-2)*128)
							break;
					}
				else
					for (int i=tOldValue + 2; i<64; i+=2) {
						tValue = tfSolveGameRoot(ucEmpties,tOldValue*128,(i+2)*128,tColor,cCurrentValue);
						if (tValue < (i+2)*128)
							break;
					}
			}
	/**/

	vfPrintPrincipalVariation(tCurBest,ucEmpties,tColor,tValue,true,cCurMoveCount);

	bInHash = false;
	return(0);
}

void vfGetBestMove(u8 ucEmpties, f64 &dTimeLeft, int *iRow, int *iColumn, f64 *dEval, f64 *dTime)
{
	f64 dWaitTime;
	qwStart = qwStartThink = qwfClock();
	u64 qwLastTickCount = qwStartThink;

	bUpdated = false;

	TIMER_INIT(MPIMemCpy);

#if USE_PROB_CUT
	tGame.ucPly = ucEmpties - 4;
#else
	tGame.ucPly = ucEmpties - 8;
#endif

	if (ucEmpties > 18) 
		dWaitTime = dTimeLeft/(f64)(((ucEmpties - 18 - 1) >> 1) + 1);
	else
		dWaitTime  = dTimeLeft;
	
	bInHash = false;
	bTotalWLD = false;
	tLastBest = 0;
	tLastValue = 0;
	
	vfDualPrintF("+---+---+-------+--------------+------------+---------+----------------------+\n");
    vfDualPrintF("|ply| %c | score |  time(s)     |    nodes   |   nps   |        variation     |\n",'%');
    vfDualPrintF("+---+---+-------+--------------+------------+---------+----------------------+\n");

	bStartedToSearch = true;

#ifdef FIXED_DEPTH 
	#if USE_PROB_CUT
		tGame.ucPly = ucEmpties - 4;
	#else
		tGame.ucPly = ucEmpties - 8;
	#endif
		tGame.ucPly = DEPTH_SEARCH < ucEmpties ? DEPTH_SEARCH : ucEmpties - 4;
		//SetPriorityClass	(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
		//SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
		//dw64Count = 0;
		vpfAnalyzeGame();
		SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
		SetPriorityClass	(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
		MMX_FLUSH;
		vfPrintPrincipalVariationX(tGame.taBoard,tCurBest,ucLastDepth,tGame.tColor,tLastValue,bTotalWLD,true);
		printf("\n");
		*iRow = (tLastBest - BOARD_START) / 9;
		*iColumn = (tLastBest - BOARD_START) % 9;
		*dEval = (f64)tLastValue / 128;
		*dTime = dWaitTime - dTimeLeft;
		vfDualPrintF("+---+---+-------+--------------+------------+---------+----------------------+\n");
		PRINT_TIMERS;
	#ifdef _DEBUG
		printf("BAD count       : %d\n",dwTTBADCollision);
		printf("Use count       : %d\n",dwTTUseCount);
		printf("Add count       : %d\n",dwTTAddCount);
		printf("Collision count : %d\n",dwTTCollision);
	#endif
		return;
#else

	u32 dwThreadID;
	//u32 ulThreadID;
	
	bChanging = false;
	//bStopThink = false;
	
	u8 ucLastCheckedDepth = 0;
	u8 ucCurMoveNumber = 0;
	bool bIncreased = false;

	//vpfAnalyzeGame();
	hThreadHandle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)vpfAnalyzeGame,NULL,0,(unsigned long *)&dwThreadID);
	//hThreadHandle = (HANDLE)_beginthreadex(0,0,&vpfAnalyzeGame,&tGame,0,&ulThreadID);

	do {
		qwStopThink = qwfClock();

		//if ((f64)(qwStopThink - qwStartThink)/TICKS_PER_SECOND >= dWaitTime)
		//	break;
		/**/
		//SuspendThread(hThreadHandle);

		/**/
		if (((qwStopThink - qwLastTickCount > 0) && (bUpdated)) || (qwStopThink - qwLastTickCount >= TICKS_PER_SECOND) && (!bInHash)) {
		//if (((qwStopThink - qwLastTickCount >= 100) && (bUpdated)) || (qwStopThink - qwLastTickCount >= TICKS_PER_SECOND)) {
		//if ((qwStopThink - qwLastTickCount >= TICKS_PER_SECOND) && (!bInHash)) {
			//while ((bInHash) || (bChanging))
			//while (bInHash)
			//	WaitForSingleObject(hThreadHandle,0);

//			EnterCriticalSection(&tCriticalSection);
			SuspendThread(hThreadHandle);
			
			qwLastTickCount = qwStopThink;
			/**/
			if (cGlobalTimeStamp < 60) {
				f64 dTemp = (f64)((s64)(qwStopThink - qwStartThink))/TICKS_PER_SECOND;
				if (ucLastCheckedDepth < ucLastDepth) {
					if (dTemp >= dWaitTime/3)
						break;
				}
				else 
					if (dTemp >= dWaitTime)
						if (ucLastCheckedDepth == ucLastDepth)
							if (!ucCurMoveNumber)
								break;
							else {
								if (!bIncreased) {
									dWaitTime *= 1.5;
									bIncreased = true;
								}
								else
									break;
							}
						else
							break;
					else 
						if (dTemp >= 0.8*dWaitTime) {
							if (ucLastCheckedDepth == ucLastDepth)
								if (!ucCurMoveNumber)
									break;
						}
			}
			else {
				dWaitTime = dTimeLeft/2;
				if ((f64)((s64)(qwStopThink - qwStartThink))/TICKS_PER_SECOND >= dWaitTime)
					break;
			}
			ucLastCheckedDepth = ucLastDepth;
			ucCurMoveNumber = cCurMoveNumber;
			/**/
			vfPrintPrincipalVariationX(tGame.taBoard,tCurBest,ucLastDepth,tGame.tColor,tLastValue,bTotalWLD,true);
			ResumeThread(hThreadHandle);
//			LeaveCriticalSection(&tCriticalSection);
		}
		/**/
	}
	while (WaitForSingleObject(hThreadHandle,50) != WAIT_OBJECT_0);
	
	ResumeThread(hThreadHandle);
//	LeaveCriticalSection(&tCriticalSection);
	//bStopThink = true;
	
	//SetThreadPriority(hThreadHandle,THREAD_PRIORITY_IDLE);
	//while ((bInHash) || (bChanging))
	while (bInHash)
		WaitForSingleObject(hThreadHandle,0);
	//WaitForSingleObject(hThreadHandle,INFINITE);
	//SuspendThread(hThreadHandle);
	
	bool bGameIsSolved;
	if (WaitForSingleObject(hThreadHandle,0) != WAIT_OBJECT_0) {
		//SuspendThread(hThreadHandle);
		TerminateThread(hThreadHandle,0);
		//_endthreadex();
		//while (WaitForSingleObject(hThreadHandle,0) != WAIT_OBJECT_0) ;
		WaitForSingleObject(hThreadHandle,INFINITE);
		CloseHandle(hThreadHandle);
		hThreadHandle = INVALID_HANDLE_VALUE;
		bGameIsSolved = false;
	}
	else {
		bGameIsSolved = true;
	}
	
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);


	/**
	FILE *fBug = fopen("hash.bug","wb");
	fwrite(taHash,1,sizeof(taHash),fBug);
	fclose(fBug);
	/**/
	
	bInHash = false;
	
	dWaitTime = (f64)((s64)(qwStopThink - qwStartThink))/TICKS_PER_SECOND;
	dTimeLeft -= dWaitTime;
	
	//vfPrintPrincipalVariation(tCurBest,ucLastDepth,tGame.tColor,tLastValue,bTotalWLD,cCurMoveNumber);
	vfPrintPrincipalVariationX(tGame.taBoard,tCurBest,ucLastDepth,tGame.tColor,tLastValue,bTotalWLD,true);
	printf("\n");
    vfDualPrintF("+---+---+-------+--------------+------------+---------+----------------------+\n");
	
	*iRow = (tLastBest - BOARD_START) / 9;
	*iColumn = (tLastBest - BOARD_START) % 9;
	*dEval = (f64)tLastValue / 128;
	*dTime = dWaitTime;

#ifdef _DEBUG
	printf("BAD count       : %d\n",dwTTBADCollision);
	printf("Use count       : %d\n",dwTTUseCount);
	printf("Add count       : %d\n",dwTTAddCount);
	printf("Collision count : %d\n",dwTTCollision);
#endif
#ifdef THINK_OPPONENT_TIME
	if (!bGameIsSolved) {
		
		char cCurrentValue = 0;
		bfMakeMove(tGame.taBoard,tGame.tColor,tLastBest,&cCurrentValue);
		tGame.tColor = tGame.tColor == BLACK ? WHITE : BLACK;
		ucEmpties--;
		bInHash = false;

		qwStart = qwStartThink = qwfClock();
		u64 qwLastTickCount = qwStartThink;

		bUpdated = false;

#if USE_PROB_CUT
		tGame.ucPly = ucEmpties - 4;
#else
		tGame.ucPly = ucEmpties - 8;
#endif
		
		bInHash = false;
		bTotalWLD = false;
		tLastBest = 0;
		tLastValue = 0;
	
		/**
		vfDualPrintF("+---+---+-------+--------------+------------+---------+----------------------+\n");
		vfDualPrintF("|ply| %c | score |  time(s)     |    nodes   |   nps   |        variation     |\n",'%');
		vfDualPrintF("+---+---+-------+--------------+------------+---------+----------------------+\n");
		/**/

		bStartedToSearch = true;

		hThreadHandle = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)vpfAnalyzeGame,NULL,0,&ulThreadID);
	}
#endif
#endif
}

void vfFinishThread()
{
	while (bInHash)
		WaitForSingleObject(hThreadHandle,1);
	TerminateThread(hThreadHandle,1);
	CloseHandle(hThreadHandle);
	hThreadHandle = INVALID_HANDLE_VALUE;
	//vfPrintPrincipalVariationX(tGame.taBoard,tCurBest,ucLastDepth,tGame.tColor,tLastValue,bTotalWLD,true);
}

DWORD WINAPI vpfSolveGame(VOID)
{
	TBoardCell tColor = tGame.tColor;
	char       cCurrentValue = 0;
	u8      ucEmpties = 0;

	vfPrepareForMove(tGame.taBoard,tColor,&cCurrentValue,&ucEmpties);
	
	if (ucEmpties < 4) {
		bExactScore = true;
		tfSolveForMate(ucEmpties,-INFINITY,+INFINITY,tColor,cCurrentValue);
		tCurBest = taGlobalMoveStack[0];
		return(ucEmpties);
	}
	
	u8      ucPly = tGame.ucPly;
	SFeature   tValue = 0, tValueX = 0;
	SFeature   tOldValue = -INFINITY;
	
	tOldValue = tLastValue;
	bExactScore = false;

	PERCENTILE = THRESHOLD_90_PERCENTS;
	if (ucEmpties > 2) {
		u8 ucLevel;
		int iIndex = ifGetTTNode(tGlobalSearchParameters.tHashValue,&ucLevel);
		if (iIndex >= 0) {
			if (taHash[iIndex].taCells[ucLevel].ucDepth == ucEmpties) {
				taHash[iIndex].taCells[ucLevel].ucDepth = 0;
			}
		}
		for (u8 ucDepth = ucEmpties % 2 == 0 ? 2 : 1; ucDepth <= ucPly; ucDepth+=2) {
			
			bInHash = true;
			cCurMoveNumber = 0;
			ucBugLastDepth = ucLastDepth = ucDepth;
			bInHash = false;

			tValue = tfSearchGameRoot(ucDepth,ucEmpties,-INFINITY,+INFINITY,tColor,cCurrentValue,cGlobalTimeStamp,USE_PROB_CUT);
			
			MMX_FLUSH;

			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
			vfPrintPrincipalVariation(tCurBest,ucDepth,tColor,tValue,false,cCurMoveCount);
			//vfPrintPrincipalVariationX(tGame.taBoard,tCurBest,ucDepth,tColor,tValue,false,true);
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
			__ASSERT(ucBugLastDepth == ucLastDepth);
		}
		tOldValue = tValue;
	}
	if (ucEmpties >= 18) {
		bInHash = true;
		cCurMoveNumber = 0;
		ucBugLastDepth = ucLastDepth = ucEmpties-2;
		bInHash = false;
		tOldValue = tValue = tfSearchGameRoot(ucLastDepth,ucEmpties,-64*128,+64*128,tColor,cCurrentValue,cGlobalTimeStamp,USE_PROB_CUT);
		MMX_FLUSH
		vfPrintPrincipalVariation(tCurBest,ucLastDepth,tColor,tValue,false,cCurMoveCount);
		//vfPrintPrincipalVariationX(tGame.taBoard,tCurBest,ucEmpties-2,tColor,tValue,false,true);
		if (ucEmpties >= 22) {
			bInHash = true;
			cCurMoveNumber = 0;
			ucBugLastDepth = ucLastDepth = ucEmpties;
			PERCENTILE = THRESHOLD_99_PERCENTS;
			bInHash = false;
			tOldValue = tValue = tfSearchGameRoot(ucLastDepth,ucEmpties,-64*128,+64*128,tColor,cCurrentValue,cGlobalTimeStamp,USE_PROB_CUT);
			MMX_FLUSH
			vfPrintPrincipalVariation(tCurBest,ucLastDepth,tColor,tValue,false,cCurMoveCount);
			//vfPrintPrincipalVariationX(tGame.taBoard,tCurBest,ucEmpties,tColor,tValue,false,true);
		}
	}
	
	cBugTimeStamp = cGlobalTimeStamp = 121 - ucEmpties;

	tValue = tfSolveGameRoot(ucEmpties,-2*128,0,tColor,cCurrentValue);
	MMX_FLUSH
	if (tValue >= 0) {
		tValue = tfSolveGameRoot(ucEmpties,0,2*128,tColor,cCurrentValue);
		MMX_FLUSH
	}
	vfPrintPrincipalVariation(tCurBest,ucEmpties,tColor,tValue,true,cCurMoveCount);

	bExactScore = true;
	if (tValue < 0)
		if (tOldValue >= 0)
			for (int i=0; i>=-64; i-=2) {
				tValue = tfSolveGameRoot(ucEmpties,(i-2)*128,i*128,tColor,cCurrentValue);
				if (tValue == i*128)
					break;
			}
		else {
			tOldValue = -((((((-tOldValue) / 128) - 1) / 2) + 1) * 2);
			if (tOldValue > -64 )
				tValue = tfSolveGameRoot(ucEmpties,(tOldValue - 2)*128,(tOldValue + 0)*128,tColor,cCurrentValue);
			else
				tValue = tfSolveGameRoot(ucEmpties,(tOldValue - 0)*128,(tOldValue + 2)*128,tColor,cCurrentValue);
			if (tValue >= tOldValue*128)
				for (int i=tOldValue; i<0; i+=2) {
					tValue = tfSolveGameRoot(ucEmpties,i*128,(i+2)*128,tColor,cCurrentValue);
					if (tValue == i*128)
						break;
				}
			else
				for (int i=tOldValue - 2; i>=-64; i-=2) {
					tValue = tfSolveGameRoot(ucEmpties,(i-2)*128,i*128,tColor,cCurrentValue);
					if (tValue == i*128)
						break;
				}
		}
	else
		if (tValue > 0)
			if ((tOldValue <= 0) || (false))
				for (int i=0; i<64; i+=2) {
					tValue = tfSolveGameRoot(ucEmpties,i*128,(i+2)*128,tColor,cCurrentValue);
					if (tValue == i*128)
						break;
				}
			else {
				tOldValue = (((((tOldValue / 128) - 1) / 2) + 1) * 2);
				if (tOldValue < 64)
					tValue = tfSolveGameRoot(ucEmpties,(tOldValue - 0)*128,(tOldValue + 2)*128,tColor,cCurrentValue);
				else
					tValue = tfSolveGameRoot(ucEmpties,(tOldValue - 2)*128,(tOldValue + 0)*128,tColor,cCurrentValue);
				if (tValue <= tOldValue*128)
					for (int i=tOldValue; i>0; i-=2) {
						tValue = tfSolveGameRoot(ucEmpties,(i-2)*128,i*128,tColor,cCurrentValue);
						if (tValue == i*128)
							break;
					}
				else
					for (int i=tOldValue + 2; i<64; i+=2) {
						tValue = tfSolveGameRoot(ucEmpties,i*128,(i+2)*128,tColor,cCurrentValue);
						if (tValue == i*128)
							break;
					}
			}
	vfPrintPrincipalVariation(tCurBest,ucEmpties,tColor,tValue,true,cCurMoveCount);

	bInHash = false;
	return(0);
}

void vfPrintPrincipalVariationX(TBoardCell *taBoard, TBoardCell tBestMove, u8 ucDepth, TBoardCell tColor, SFeature tValue, u8 ucEmpties, int iCount)
{
	MMX_FLUSH;
	bUpdated = false;
	qwMoveFinish = qwfClock();
    
	int iPercent = 100;
	if (cCurMoveCount)
		iPercent = 100*cCurMoveNumber / cCurMoveCount;
	f64 dLock = (f64)(s64(qwMoveFinish - qwStart))/TICKS_PER_SECOND;
    u64 qwVC = qwVariationCount;

	bool bPVSChanged = false;
	if (tBestMove != tLastBest) {
        tLastBest = tBestMove;
        dCurTimeLock = dLock;
		bPVSChanged = true;
    }

	vfDualPrintF("|%3d|%3d| %1c%5.2f| %s |%11I64u |%8.0f | %s |\n",
			iCount,
			ucEmpties,
			(tValue>0 ? '+' : (tValue<0 ? '-' : ' ')),
			fabs((f64)tValue/128.0),
			cafTimeToString(dLock),
			qwVC,
			dLock>0 ? (dLock ? qwVC/dLock : 0) : 0,
			cafPrintPrincipalVariation(taBoard,tBestMove,ucDepth,tColor,tValue));

	bStartedToSearch = false;
	qwMoveStart = qwMoveFinish;
}

void vfSolveGames(char *caFileName)
{
	FILE *fInput = fopen(caFileName,"rt");
	if (!fInput) {
		vfDualPrintF("Could not open file %s",caFileName);
		return;
	}
	vfDualPrintF("Solving positions from %s...\n",caFileName);
	vfDualPrintF("+---+---+-------+--------------+------------+---------+----------------------+\n");
    vfDualPrintF("| # |ply| score |  time(s)     |    nodes   |   nps   |        variation     |\n");
    vfDualPrintF("+---+---+-------+--------------+------------+---------+----------------------+\n");

	char caString[130], caStr2[2];
	u64 qwTotalSolveTime = 0;
	for (int i=1; !feof(fInput); i++) {
		if (fscanf(fInput,"%s%s",caString,caStr2) == -1)
			break;
		int j = strlen(caString);
		caString[j] = ' ';;
		caString[j + 1] = caStr2[0];
		caString[j + 2] = 0;
		
		u8 ucEmpties;
		vfLoadGame(tGame.taBoard,&tGame.tColor,&ucEmpties,caString);
		//memcpy(tGlobalSearchParameters.taBoard, tGame.taBoard, BOARD_SIZE*sizeof(TBoardCell));
		//vfShowBoard(tGame.tColor);

		tGame.ucPly = ucEmpties - 4;
		bUpdated = false;

		f64 dWaitTime  = dTimeLeft = 100000000.0;
		
		bInHash = false;
		bTotalWLD = false;
		tLastBest = 0;
		tLastValue = 0;

		bStartedToSearch = true;
		
		//SetPriorityClass	(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
		//SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
		u64 qwStartTimer = CPU::qwfGetCycleCount();
		u64 qwLastTickCount = qwStart = qwStartThink = qwStartTimer*TICKS_PER_SECOND/CPU::qwCyclesPerSecond;
		vpfSolveGame();
		qwTotalSolveTime += CPU::qwfGetCycleCount();
		qwTotalSolveTime -= qwStartTimer + CPU::qwCyclesPerRDTSC;
		SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
		SetPriorityClass	(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
		vfPrintPrincipalVariationX(tGame.taBoard,tCurBest,ucLastDepth,tGame.tColor,tLastValue,ucEmpties,i);
	}
	vfDualPrintF("+---+---+-------+--------------+------------+---------+----------------------+\n");
	vfDualPrintF("Total time : %.3fs\n",f64(s64(qwTotalSolveTime))/f64(s64(CPU::qwCyclesPerSecond)));
}