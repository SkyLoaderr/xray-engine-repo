//////////////////////////////////////////////////////////////////////////////////
//	Module			:	midgame solver											//
//	File			:	mg_pvs.cpp												//
//	Creation date	:	14.02.2001												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "board.h"
#include "eval.h"
#include "game.h"
#include "mg_pvs.h"
#include "mobil.h"
#include "moves.h"
#include "mpc.h"
#include "tt.h"
#include "_time.h"
#include "_mmx.h"

#define MIN_HASH_STORE_DEPTH        1
#define MIN_KILLER_HEURISTIC_DEPTH  1
#define MIN_ENHANCED_TRANSPOSITION_CUT_OFF_DEPTH 5
#define FASTEST_FIRST				4

//#define __OLD_UNDO__

#define MAX_RESTORABLE_FLIPS		2

u64 qwCount;
#define EVALUATE(a,b,c,d) {\
	TIMER_START(EvaluationFunction)\
	c = d*tfEvaluate(a,b);\
	TIMER_STOP(EvaluationFunction)\
}

TSearchGameInfo tParameters;

/**
IC u8 ucfSortMovesFastestFirst(TBoardCell *tpStartMove, int iIndex, TBoardCell tColor, TBoardCell tOpponentColor)
{
	TIMER_START(FastestFirst)
	TBoardCell *tpCurMove0 = tpStartMove, *tpCurMove1;
	SHashValue tOldHashValue = tGlobalSearchParameters.tHashValue;
	char cCurrentValue = 0;
	u32 dwFlipCount = 0;
	u8 caMobility[MAX_MOVE_COUNT];
	memset(caMobility,0x00,sizeof(caMobility));
	
	u8 ucMoveCount = 0;
	if (iIndex >= 0) {
		tpCurMove0++;
		caMobility[0] = 255;
		ucMoveCount++;
	}

	for ( ; tpCurMove0 - tpStartMove < MAX_MOVE_COUNT; tpCurMove0++)
		if (bfMakeMoveIfAvailable(tColor,*tpCurMove0,&cCurrentValue,&dwFlipCount)) {
			int iCurMove = tpCurMove0 - tpStartMove;
			for (tpCurMove1 = tpStartMove; tpCurMove1 - tpStartMove < MAX_MOVE_COUNT; tpCurMove1++)
				if (bfCheckIfAvailable(tOpponentColor,*tpCurMove1))
					caMobility[iCurMove]++;
			vfUndo(dwFlipCount, tOpponentColor);
			tGlobalSearchParameters.tHashValue = tOldHashValue;
			cCurrentValue = 0;
			ucMoveCount++;
		}

	bool bOk;
	do {
		bOk = true;
		for (int i=0; i < MAX_MOVE_COUNT - 1; i++)
			if (caMobility[i] < caMobility[i + 1]) {
				u8 ucTemp		= caMobility[i];
				caMobility[i]		= caMobility[i + 1];
				caMobility[i + 1]	= ucTemp;
				ucTemp				= tpStartMove[i];
				tpStartMove[i]		= tpStartMove[i + 1];
				tpStartMove[i + 1]	= ucTemp;
				bOk = false;
			}
	}
	while (!bOk);
	
	TIMER_STOP(FastestFirst)
	return(ucMoveCount);
}
/**/

int tfSearchOnePly(u8 ucEmpties, int tAlpha, int tBeta, TBoardCell tColor, char cCurrentValue, char cGameStage)
{
	__ASSERT(ucBugLastDepth == ucLastDepth);
	qwVariationCount++;

	TBoardCell tOpponentColor = (tColor == BLACK) ? WHITE : BLACK;

	int tBest = -INFINITY;
	int tValue;
	int tStartAlpha = tAlpha;

	SHashValue tOldHashValue = tGlobalSearchParameters.tHashValue;
	
#ifndef __MPI__
	u32 dwFlipCount;
#endif
	char cSafeValue = cCurrentValue;
	bool bFound = false;

	int iIndex = -1;
	u8 ucBestMove;

	u8 ucMoveCount = ucEmpties;
	TBoardCell *tpCurMove = taGlobalMoveStack;
	
#ifdef __MPI__
	ALIGN(16) u16	waTmp[MP_COUNT];
	COPY_EF_INDEXES(waTmp,waMPIndexes);
#endif

	for (int i=0; (tBest < tBeta) && (i<MAX_MOVE_COUNT); i++,tpCurMove++) {
		
		if (tGlobalSearchParameters.taBoard[*tpCurMove] != EMPTY)
			continue;

#ifdef __MPI__
		if (bfChangeEFIndexesIfAvailableFast(tColor,*tpCurMove)) {
#else
		if (bfMakeMoveIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {
#endif
			if (tBest > tAlpha) 
				tAlpha = tBest;

			if (tColor == WHITE)
				EVALUATE(0,cGameStage + 1,tValue,-1)
			else
				EVALUATE(1,cGameStage + 1,tValue,1)

			qwVariationCount++;

			bFound = true;
			
			if (tValue > tBest) {
				ucBestMove = *tpCurMove;
				tBest = tValue;
				vfMoveToFrontByIndex(i,taGlobalMoveStack,ucBestMove);
			}
			

#ifndef __MPI__
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
			vfUndo(dwFlipCount, tOpponentColor);
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
			cCurrentValue = cSafeValue;
			tGlobalSearchParameters.tHashValue = tOldHashValue;
#else
			COPY_EF_INDEXES(waMPIndexes,waTmp);
#endif
		}
	}
	
	// check for leaf or to pass
	if (!bFound) {
		tpCurMove = taGlobalMoveStack;//tpStartMove;
		for ( i=0; i<MAX_MOVE_COUNT; i++, tpCurMove++) {
			if (bfCheckIfAvailable(tOpponentColor,*tpCurMove)) {
				tGlobalSearchParameters.tHashValue ^= tChangeColor;
				tBest = -tfSearchOnePly(
					ucEmpties,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue,
					cGameStage);

				tGlobalSearchParameters.tHashValue = tOldHashValue;
				
				return(tBest);
			}
		}

		tBest = cCurrentValue > 0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties;
		
		tBest *=128;
		
		qwVariationCount++;

		return(tBest);
		/**
		if (tBest >= tBeta)
			return(tBeta);
		else
			if (tBest <= tAlpha)
				return(tAlpha);
			else
				return(tBest);
		/**/
	}
	else {
		return(tBest);
		/**
		if (tBest >= tBeta)
			return(tBeta);
		else
			if (tBest <= tAlpha)
				return(tAlpha);
			else
				return(tBest);
		/**/
	}
}

int tfSearchGameShallow(u8 ucDepth, u8 ucEmpties, int tAlpha, int tBeta, TBoardCell tColor, char cCurrentValue, char cGameStage)
{
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);

	qwVariationCount++;

	TBoardCell tOpponentColor = (tColor == BLACK) ? WHITE : BLACK;

	int tBest = -INFINITY;
	int tValue;
	int tStartAlpha = tAlpha;

	SHashValue tOldHashValue = tGlobalSearchParameters.tHashValue;
	
	u32 dwFlipCount;
	char cSafeValue = cCurrentValue;
	bool bFound = false;

	int iIndex = -1;
	u8 ucLevel;
	u8 ucBestMove;

	//////////////////////////////////////////////////////////
	//  if (bfNodeInHash(...))
	//		if (bmOutOfBounds(...))
	//			return(...);
	//		else
	//			vfUpdateBounds(...);
	//////////////////////////////////////////////////////////

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	if (ucDepth >= MIN_HASH_STORE_DEPTH) {
		
		iIndex = ifGetTTNode(tGlobalSearchParameters.tHashValue,&ucLevel);
		
		if (iIndex >= 0) {
			
			ucBestMove = taHash[iIndex].taCells[ucLevel].ucMove;

			if (ucBestMove != MOVE_PASS)
				vfMoveToFrontByValue(taGlobalMoveStack,ucBestMove);

			if (taHash[iIndex].taCells[ucLevel].ucDepth == ucDepth) {
				if(tBeta > taHash[iIndex].taCells[ucLevel].tBeta){
					tBeta = taHash[iIndex].taCells[ucLevel].tBeta;
					if(tBeta <= tAlpha){
						return(tBeta);
					}
				}
				if(tAlpha < taHash[iIndex].taCells[ucLevel].tAlpha){
					tAlpha = taHash[iIndex].taCells[ucLevel].tAlpha;
					if(tBeta <= tAlpha){
						return(tAlpha);
					}
				}
			}
		}
	}
	
	//////////////////////////////////////////////////////////
	//  if (bmOrderMovesCondition(...)) {
	//		vfOrderMoves(...)
	//////////////////////////////////////////////////////////

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	u8 ucMoveCount = ucEmpties;
	TBoardCell *tpCurMove = tpfGetAvailableMoves();
	TBoardCell *tpStartMove = tpCurMove;
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	
	/**/
	//////////////////////////////////////////////////////////
	//	Enhanced transposition cut-off
	//////////////////////////////////////////////////////////

	//if ((iIndex >= 0) && (taHash[iIndex].taCells[ucLevel].ucDepth <= ucDepth + 2))
	//if (iIndex >= 0)		
		if (ucDepth >= MIN_ENHANCED_TRANSPOSITION_CUT_OFF_DEPTH) {
			SHashValue tHashValue = tGlobalSearchParameters.tHashValue;
			for (int i=0, iMoveIndex = 0; (i<MAX_MOVE_COUNT); i++,tpCurMove++) {
				if (bfChangeHashIfAvailable(tColor,*tpCurMove)) {
					qwVariationCount++;
					iMoveIndex++;
					u32 dwIndex = (u32)(tGlobalSearchParameters.tHashValue & (((u32)1 << MAX_TT_HASH_BITS) - 1));
					u32 dwHashValue = (u32)((tGlobalSearchParameters.tHashValue >> 32) & 0xffffffff);
					tGlobalSearchParameters.tHashValue = tHashValue;
					if ((taHash[dwIndex].taCells[0].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[0].ucDepth == ucDepth - 1) && 
						(taHash[dwIndex].taCells[0].ucMove) && 
						(taHash[dwIndex].taCells[0].tBeta <= -tBeta)) {
						ucBestMove = *tpCurMove;
						//if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
							vfMoveToFrontByIndex(i,taGlobalMoveStack,ucBestMove);
						//if (ucDepth >= MIN_HASH_STORE_DEPTH)
							vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,-taHash[dwIndex].taCells[0].tBeta,ucBestMove);
						tpMoveStack = tpStartMove;
						return(tBeta);
					}
					if ((taHash[dwIndex].taCells[1].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[1].ucDepth == ucDepth - 1) && 
						(taHash[dwIndex].taCells[1].ucMove) && 
						(taHash[dwIndex].taCells[1].tBeta <= -tBeta)) {
						ucBestMove = *tpCurMove;
						//if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
							vfMoveToFrontByIndex(i,taGlobalMoveStack,ucBestMove);
						//if (ucDepth >= MIN_HASH_STORE_DEPTH)
							vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,-taHash[dwIndex].taCells[1].tBeta,ucBestMove);
						tpMoveStack = tpStartMove;
						return(tBeta);
					}
				}
			}
			
			tpCurMove = tpStartMove;

		}
	/**/

#ifdef __MPI__
	ALIGN(16) u16	waTmp[MP_COUNT];
	COPY_EF_INDEXES(waTmp,waMPIndexes);
#endif
/**
	// sorting moves
	u8 ucMaxMoveCount = 60; 
	if (ucDepth > MIN_FASTEST_FIRST_DEPTH)
		ucMaxMoveCount = ucfSortMovesFastestFirst(tpCurMove,iIndex,tColor,tOpponentColor);
/**/
	for (int i=0; (tBest < tBeta) && (i<60); i++,tpCurMove++) {
		//__ASSERT((tpMoveStack - taGlobalMoveStack) % 60 == 0);
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (tGlobalSearchParameters.taBoard[*tpCurMove] != EMPTY)
			continue;
#ifdef __MPI__
		if (bfMakeMoveMobilityIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {
#else
		if (bfMakeMoveIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {
#endif
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);

			// if first time - do not use PVS
			/**/
			if (!bFound) {
				ucBestMove = *tpCurMove;
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				if (ucDepth > 2)
					tBest = -tfSearchGameShallow(
						ucDepth - 1,
						ucEmpties - 1,
						-tBeta,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1);
				else 
					tBest = -tfSearchOnePly(
						ucEmpties - 1,
						-tBeta,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1);

				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				bFound = true;
				vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
			}
			// otherwise - PVS
			else {
				if (tBest > tAlpha) 
					tAlpha = tBest;

				if (ucDepth > 2)
					tValue = -tfSearchGameShallow(
						ucDepth - 1,
						ucEmpties - 1,
						-tAlpha - 1,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1);
				else 
					tValue = -tfSearchOnePly(
						ucEmpties - 1,
						-tAlpha - 1,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1);
				
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				if ((tValue > tAlpha) && (tValue < tBeta)) {
						
					if (ucDepth > 2)
						tBest = -tfSearchGameShallow(
							ucDepth - 1,
							ucEmpties - 1,
							-tBeta,
							-tValue,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1);
					else
						tBest = -tfSearchOnePly(
							ucEmpties - 1,
							-tBeta,
							-tValue,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1);

					__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
					ucBestMove = *tpCurMove;
					vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
				}
				else
					if (tValue > tBest) {
						tBest = tValue;
						ucBestMove = *tpCurMove;
						vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
					}
			}
			/**
			if (tBest > tAlpha) 
				tAlpha = tBest;

			if (ucDepth > 2)
				tValue = -tfSearchGameShallow(
					ucDepth - 1,
					ucEmpties - 1,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue,
					cGameStage + 1);
			else
				tValue = -tfSearchOnePly(
					ucEmpties - 1,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue,
					cGameStage + 1);

			bFound = true;
			
			if (tValue > tBest) {
				ucBestMove = *tpCurMove;
				tBest = tValue;
				vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
			}
			/**/
			
			// restoring undo data
#ifdef __MPI__
	#ifdef __OLD_UNDO__
			vfUndo(dwFlipCount, tOpponentColor);
			COPY_EF_INDEXES(waMPIndexes,waTmp);
	#else
			if (dwFlipCount > MAX_RESTORABLE_FLIPS) {
				vfUndo(dwFlipCount, tOpponentColor);
				COPY_EF_INDEXES(waMPIndexes,waTmp);
			}
			else
				vfUndoAndRestore(dwFlipCount, tOpponentColor);
	#endif
#else
			vfUndo(dwFlipCount, tOpponentColor);
#endif
			cCurrentValue = cSafeValue;
			tGlobalSearchParameters.tHashValue = tOldHashValue;
		}
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	}
	
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	// check for leaf or to pass
	if (!bFound) {
		tpCurMove = tpStartMove;
		for ( i=0; i<MAX_MOVE_COUNT; i++, tpCurMove++) {
			if (bfCheckIfAvailable(tOpponentColor,*tpCurMove)) {
				tGlobalSearchParameters.tHashValue ^= tChangeColor;
				tBest = -tfSearchGameShallow(
					ucDepth,
					ucEmpties,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue,
					cGameStage);

				tGlobalSearchParameters.tHashValue = tOldHashValue;
				
				if (ucDepth >= MIN_HASH_STORE_DEPTH)
					vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,tBest,MOVE_PASS);

				tpMoveStack = tpStartMove;

				return(tBest);
			}
		}
		tpMoveStack = tpStartMove;

		tBest = cCurrentValue > 0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties;
		
		tBest *=128;
		
		qwVariationCount++;

		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (ucDepth >= MIN_HASH_STORE_DEPTH)
			vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,-INFINITY,+INFINITY,tBest,MOVE_PASS);
		
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		return(tBest);
	}
	else {
		if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
			COPY_MOVES(taGlobalMoveStack,tpStartMove);

		tpMoveStack = tpStartMove;

		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (ucDepth >= MIN_HASH_STORE_DEPTH)
			vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,tBest,ucBestMove);
		
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		return(tBest);
	}
}

int tfSearchGame(u8 ucDepth, u8 ucEmpties, int tAlpha, int tBeta, TBoardCell tColor, char cCurrentValue, char cGameStage, char cUseProbCut)
{
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);

	qwVariationCount++;

	//if (bStopThink)
	//	return(-INFINITY);

	TBoardCell tOpponentColor = (tColor == BLACK) ? WHITE : BLACK;

	int tBest = -INFINITY;
	int tValue;
	int tStartAlpha = tAlpha;

	SHashValue tOldHashValue = tGlobalSearchParameters.tHashValue;
	
	u32 dwFlipCount;
	char cSafeValue = cCurrentValue;
	bool bFound = false;

	int iIndex = -1;
	u8 ucLevel;
	u8 ucBestMove;

	//////////////////////////////////////////////////////////
	//  if (bfNodeInHash(...))
	//		if (bmOutOfBounds(...))
	//			return(...);
	//		else
	//			vfUpdateBounds(...);
	//////////////////////////////////////////////////////////

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	if (ucDepth >= MIN_HASH_STORE_DEPTH) {
		
		iIndex = ifGetTTNode(tGlobalSearchParameters.tHashValue,&ucLevel);
		
		if (iIndex >= 0) {
			
			ucBestMove = taHash[iIndex].taCells[ucLevel].ucMove;

			if (ucBestMove != MOVE_PASS)
				vfMoveToFrontByValue(taGlobalMoveStack,ucBestMove);

			if (taHash[iIndex].taCells[ucLevel].ucDepth == ucDepth) {
				if(tBeta > taHash[iIndex].taCells[ucLevel].tBeta){
					tBeta = taHash[iIndex].taCells[ucLevel].tBeta;
					if(tBeta <= tAlpha){
						return(tBeta);
					}
				}
				if(tAlpha < taHash[iIndex].taCells[ucLevel].tAlpha){
					tAlpha = taHash[iIndex].taCells[ucLevel].tAlpha;
					if(tBeta <= tAlpha){
						return(tAlpha);
					}
				}
			}
		}
	}
	
	//////////////////////////////////////////////////////////
	//  if (bmOrderMovesCondition(...)) {
	//		vfOrderMoves(...)
	//////////////////////////////////////////////////////////

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	u8 ucMoveCount = ucEmpties;
	TBoardCell *tpCurMove = tpfGetAvailableMoves();
	TBoardCell *tpStartMove = tpCurMove;
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	
	//////////////////////////////////////////////////////////
	//	Enhanced transposition cut-off
	//////////////////////////////////////////////////////////

	if ((iIndex >= 0) && (taHash[iIndex].taCells[ucLevel].ucDepth == ucDepth))
		if (ucDepth >= MIN_ENHANCED_TRANSPOSITION_CUT_OFF_DEPTH) {
			SHashValue tHashValue = tGlobalSearchParameters.tHashValue;
			for (int i=0, iMoveIndex = 0; (i<MAX_MOVE_COUNT); i++,tpCurMove++) {
				if (bfChangeHashIfAvailable(tColor,*tpCurMove)) {
					qwVariationCount++;
					iMoveIndex++;
					u32 dwIndex = (u32)(tGlobalSearchParameters.tHashValue & (((u32)1 << MAX_TT_HASH_BITS) - 1));
					u32 dwHashValue = (u32)((tGlobalSearchParameters.tHashValue >> 32) & 0xffffffff);
					tGlobalSearchParameters.tHashValue = tHashValue;
					if ((taHash[dwIndex].taCells[0].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[0].ucDepth == ucDepth - 1) && 
						(taHash[dwIndex].taCells[0].ucMove) && 
						(taHash[dwIndex].taCells[0].tBeta <= -tBeta)) {
						ucBestMove = *tpCurMove;
						//if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
							vfMoveToFrontByIndex(i,taGlobalMoveStack,ucBestMove);
						//if (ucDepth >= MIN_HASH_STORE_DEPTH)
							vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,-taHash[dwIndex].taCells[0].tBeta,ucBestMove);
						tpMoveStack = tpStartMove;
						return(tBeta);
					}
					if ((taHash[dwIndex].taCells[1].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[1].ucDepth == ucDepth - 1) && 
						(taHash[dwIndex].taCells[1].ucMove) && 
						(taHash[dwIndex].taCells[1].tBeta <= -tBeta)) {
						ucBestMove = *tpCurMove;
						//if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
							vfMoveToFrontByIndex(i,taGlobalMoveStack,ucBestMove);
						//if (ucDepth >= MIN_HASH_STORE_DEPTH)
							vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,-taHash[dwIndex].taCells[1].tBeta,ucBestMove);
						tpMoveStack = tpStartMove;
						return(tBeta);
					}
				}
			}
			//ucMoveCount = iMoveIndex;
			tpCurMove = tpStartMove;
		}


	//////////////////////////////////////////////////////////
	//  if (bmCorrelationExists(...)) {
	//		if (bmCheckAlphaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//		
	//		if (bmCheckBetaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//	}
	//////////////////////////////////////////////////////////

	if ((cUseProbCut) && (ucDepth < MAX_PLY) && (ucDepth > MIN_PLY) && (cGameStage >= 0)) {
		
		if (taMPC[cGameStage][ucDepth].bCorrelationExists) {

			/**
			f64 A = taMPC[cGameStage][ucDepth].dB;
			f64 B = taMPC[cGameStage][ucDepth].dA;
			f64 SIGMA = taMPC[cGameStage][ucDepth].dSigma;
			char   cDepth = taMPC[cGameStage][ucDepth].cDepth;

			int tBound = (int)floor((+PERCENTILE * SIGMA + tBeta - B)/A);

			int tPCValue = tfSearchGameShallow(
									cDepth,
									ucEmpties,
									tBound,
									tBound+1,
									tColor,
									cCurrentValue,
									cGameStage);
			
			if (tPCValue >= tBound+1) {
				tpMoveStack = tpStartMove;
				return(tBeta);
			}

			tBound = (int)floor((-PERCENTILE * SIGMA + tAlpha - B)/A);
			tPCValue = tfSearchGameShallow(
									cDepth,
									ucEmpties,
									tBound-1,
									tBound,
									tColor,
									cCurrentValue,
									cGameStage);
			if (tPCValue <= tBound-1) {
				tpMoveStack = tpStartMove;
				return(tAlpha);
			}
			/**/
			MMX_FLUSH
			f64 A = taMPC[cGameStage][ucDepth].dB;
			f64 B = taMPC[cGameStage][ucDepth].dA;
			f64 SIGMA = taMPC[cGameStage][ucDepth].dSigma;
			char   cDepth = taMPC[cGameStage][ucDepth].cDepth;

			int tUpperBound = (int)floor((+PERCENTILE * SIGMA + tBeta - B)/A);
			int tLowerBound = (int)floor((-PERCENTILE * SIGMA + tAlpha - B)/A);

			int tPCValue = tfSearchGameShallow(
									cDepth,
									ucEmpties,
									tLowerBound - 1,
									tUpperBound + 1,
									tColor,
									cCurrentValue,
									cGameStage);
			if (tPCValue <= tLowerBound - 1) {
				tpMoveStack = tpStartMove;
				return(tAlpha);
			}
			else 
				if (tPCValue >= tUpperBound + 1) {
					tpMoveStack = tpStartMove;
					return(tBeta);
				}
			/**/
		}
	}

#ifdef __MPI__
	ALIGN(16) u16	waTmp[MP_COUNT];
	COPY_EF_INDEXES(waTmp,waMPIndexes);
#endif
/**
	// sorting moves
	u8 ucMaxMoveCount = 60; 
	if (ucDepth > MIN_FASTEST_FIRST_DEPTH)
		ucMaxMoveCount = ucfSortMovesFastestFirst(tpCurMove,iIndex,tColor,tOpponentColor);
/**/
	for (int i=0; (tBest < tBeta) && (i<60); i++,tpCurMove++) {
		//__ASSERT((tpMoveStack - taGlobalMoveStack) % 60 == 0);
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (tGlobalSearchParameters.taBoard[*tpCurMove] != EMPTY)
			continue;
#ifdef __MPI__
		if (bfMakeMoveMobilityIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {
#else
		if (bfMakeMoveIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {
#endif
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);

			// if first time - do not use PVS
			if (!bFound) {
				ucBestMove = *tpCurMove;
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				if (ucDepth > 2)
					tBest = -tfSearchGame(
						ucDepth - 1,
						ucEmpties - 1,
						-tBeta,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1,
						cUseProbCut);
				else 
					tBest = -tfSearchOnePly(
						ucEmpties - 1,
						-tBeta,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1);

				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				bFound = true;
				vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
			}
			// otherwise - PVS
			else {
				if (tBest > tAlpha) 
					tAlpha = tBest;

				if (ucDepth > 2)
					tValue = -tfSearchGame(
						ucDepth - 1,
						ucEmpties - 1,
						-tAlpha - 1,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1,
						cUseProbCut);
				else 
					tValue = -tfSearchOnePly(
						ucEmpties - 1,
						-tAlpha - 1,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1);
				
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				if ((tValue > tAlpha) && (tValue < tBeta)) {
						
					if (ucDepth > 2)
						tBest = -tfSearchGame(
							ucDepth - 1,
							ucEmpties - 1,
							-tBeta,
							-tValue,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1,
							cUseProbCut);
					else
						tBest = -tfSearchOnePly(
							ucEmpties - 1,
							-tBeta,
							-tValue,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1);

					__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
					ucBestMove = *tpCurMove;
					vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
				}
				else
					if (tValue > tBest) {
						tBest = tValue;
						ucBestMove = *tpCurMove;
						vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
					}
			}
			
			// restoring undo data
#ifdef __MPI__
	#ifdef __OLD_UNDO__
			vfUndo(dwFlipCount, tOpponentColor);
			COPY_EF_INDEXES(waMPIndexes,waTmp);
	#else
			if (dwFlipCount > MAX_RESTORABLE_FLIPS) {
				vfUndo(dwFlipCount, tOpponentColor);
				COPY_EF_INDEXES(waMPIndexes,waTmp);
			}
			else
				vfUndoAndRestore(dwFlipCount, tOpponentColor);
	#endif
#else
			vfUndo(dwFlipCount, tOpponentColor);
#endif
			cCurrentValue = cSafeValue;
			tGlobalSearchParameters.tHashValue = tOldHashValue;
		}
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	}
	
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	// check for leaf or to pass
	if (!bFound) {
		tpCurMove = tpStartMove;
		for ( i=0; i<MAX_MOVE_COUNT; i++, tpCurMove++) {
			if (bfCheckIfAvailable(tOpponentColor,*tpCurMove)) {
				tGlobalSearchParameters.tHashValue ^= tChangeColor;
				tBest = -tfSearchGame(
					ucDepth,
					ucEmpties,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue,
					cGameStage,
					cUseProbCut);

				tGlobalSearchParameters.tHashValue = tOldHashValue;
				
				if (ucDepth >= MIN_HASH_STORE_DEPTH)
					vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,tBest,MOVE_PASS);

				tpMoveStack = tpStartMove;

				return(tBest);
			}
		}
		tpMoveStack = tpStartMove;

		tBest = cCurrentValue > 0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties;
		
		tBest *=128;
		
		qwVariationCount++;

		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (ucDepth >= MIN_HASH_STORE_DEPTH)
			vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,-INFINITY,+INFINITY,tBest,MOVE_PASS);
		
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		return(tBest);
	}
	else {
		if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
			COPY_MOVES(taGlobalMoveStack,tpStartMove);

		tpMoveStack = tpStartMove;

		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (ucDepth >= MIN_HASH_STORE_DEPTH)
			vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,tBest,ucBestMove);
		
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		return(tBest);
	}
}

int tfSearchGameRoot(u8 ucDepth, u8 ucEmpties, int tAlpha, int tBeta, TBoardCell tColor, char cCurrentValue, char cGameStage, char cUseProbCut)
{
	//return(tfSearchSelectiveEndgameRoot(ucDepth,ucEmpties,tAlpha,tBeta,tColor,cCurrentValue,cGameStage,cUseProbCut));
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);

	qwVariationCount++;

	//////////////////////////////////////////////////////////
	//  if (bmLeafNode(...))
	//		if (bmQdwescenceCondition(...))
	//			return(tfSearchGame(...));		
	//		else
	//			return(EVALUATE(...));
	//////////////////////////////////////////////////////////

	if (!ucDepth) {
		
		SFeature tBest;
		
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (tColor == WHITE)
			EVALUATE(0,cGameStage,tBest,-1)
		else
			EVALUATE(1,cGameStage,tBest,1)

		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		
		return(tBest);
	}
	
	
	TBoardCell tOpponentColor = (tColor == BLACK) ? WHITE : BLACK;

	int tBest = -INFINITY;
	int tValue;
	int tStartAlpha = tAlpha;

	SHashValue tOldHashValue = tGlobalSearchParameters.tHashValue;
	
	u32 dwFlipCount;
	char cSafeValue = cCurrentValue;
	bool bFound = false;

	int iIndex = -1;
	u8 ucLevel;
	u8 ucBestMove = MOVE_PASS;

	//////////////////////////////////////////////////////////
	//  if (bfNodeInHash(...))
	//		if (bmOutOfBounds(...))
	//			return(...);
	//		else
	//			vfUpdateBounds(...);
	//////////////////////////////////////////////////////////

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	if (ucDepth >= MIN_HASH_STORE_DEPTH) {
		
		iIndex = ifGetTTNode(tGlobalSearchParameters.tHashValue,&ucLevel);
		
		if (iIndex >= 0) {
			
			ucBestMove = taHash[iIndex].taCells[ucLevel].ucMove;

			if (ucBestMove != MOVE_PASS)
				vfMoveToFrontByValue(taGlobalMoveStack,ucBestMove);

			if (taHash[iIndex].taCells[ucLevel].ucDepth == ucDepth) {
				if(tBeta > taHash[iIndex].taCells[ucLevel].tBeta){
					tBeta = taHash[iIndex].taCells[ucLevel].tBeta;
					if(tBeta <= tAlpha){
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,cCurMoveCount,1);
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,1,1);
						return(tBeta);
					}
				}
				if(tAlpha < taHash[iIndex].taCells[ucLevel].tAlpha){
					tAlpha = taHash[iIndex].taCells[ucLevel].tAlpha;
					if(tBeta <= tAlpha){
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tAlpha,false,cCurMoveCount,1);
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tAlpha,false,1,1);
						return(tAlpha);
					}
				}
			}
		}
	}
	
	//////////////////////////////////////////////////////////
	//  if (bmOrderMovesCondition(...)) {
	//		vfOrderMoves(...)
	//////////////////////////////////////////////////////////

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	u8 ucMoveCount = ucEmpties;
	TBoardCell *tpCurMove = tpfGetAvailableMoves();
	TBoardCell *tpStartMove = tpCurMove;
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	
	//////////////////////////////////////////////////////////
	//	Enhanced transposition cut-off
	//////////////////////////////////////////////////////////

	if ((iIndex >= 0) && (taHash[iIndex].taCells[ucLevel].ucDepth == ucDepth))
		if (ucDepth >= MIN_ENHANCED_TRANSPOSITION_CUT_OFF_DEPTH) {
			SHashValue tHashValue = tGlobalSearchParameters.tHashValue;
			for (int i=0, iMoveIndex = 0; (i<MAX_MOVE_COUNT); i++,tpCurMove++) {
				if (bfChangeHashIfAvailable(tColor,*tpCurMove)) {
					qwVariationCount++;
					iMoveIndex++;
					u32 dwIndex = (u32)(tGlobalSearchParameters.tHashValue & (((u32)1 << MAX_TT_HASH_BITS) - 1));
					u32 dwHashValue = (u32)((tGlobalSearchParameters.tHashValue >> 32) & 0xffffffff);
					tGlobalSearchParameters.tHashValue = tHashValue;
					if ((taHash[dwIndex].taCells[0].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[0].ucDepth == ucDepth - 1) && 
						(taHash[dwIndex].taCells[0].ucMove) && 
						(taHash[dwIndex].taCells[0].tBeta <= -tBeta)) {
						ucBestMove = *tpCurMove;
						//if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
							vfMoveToFrontByIndex(i,taGlobalMoveStack,ucBestMove);
						//if (ucDepth >= MIN_HASH_STORE_DEPTH)
							vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,-taHash[dwIndex].taCells[0].tBeta,ucBestMove);
						tpMoveStack = tpStartMove;
						vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,iMoveIndex);
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,iMoveIndex,1);
						return(tBeta);
					}
					if ((taHash[dwIndex].taCells[1].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[1].ucDepth == ucDepth - 1) && 
						(taHash[dwIndex].taCells[1].ucMove) && 
						(taHash[dwIndex].taCells[1].tBeta <= -tBeta)) {
						ucBestMove = *tpCurMove;
						//if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
							vfMoveToFrontByIndex(i,taGlobalMoveStack,ucBestMove);
						//if (ucDepth >= MIN_HASH_STORE_DEPTH)
							vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,-taHash[dwIndex].taCells[1].tBeta,ucBestMove);
						tpMoveStack = tpStartMove;
						vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,iMoveIndex);
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,iMoveIndex,1);
						return(tBeta);
					}
				}
			}
			//ucMoveCount = iMoveIndex;
			tpCurMove = tpStartMove;
		}


	//////////////////////////////////////////////////////////
	//  if (bmCorrelationExists(...)) {
	//		if (bmCheckAlphaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//		
	//		if (bmCheckBetaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//	}
	//////////////////////////////////////////////////////////

	if ((cUseProbCut) && (ucDepth < MAX_PLY) && (ucDepth > MIN_PLY) && (cGameStage > 0)) {
		
		if (taMPC[cGameStage][ucDepth].bCorrelationExists) {

			/**
			f64 A = taMPC[cGameStage][ucDepth].dB;
			f64 B = taMPC[cGameStage][ucDepth].dA;
			f64 SIGMA = taMPC[cGameStage][ucDepth].dSigma;
			char   cDepth = taMPC[cGameStage][ucDepth].cDepth;

			int tBound = (int)floor((+PERCENTILE * SIGMA + tBeta - B)/A);

			int tPCValue = tfSearchGameShallow(
									cDepth,
									ucEmpties,
									tBound,
									tBound+1,
									tColor,
									cCurrentValue,
									cGameStage);
			
			if (tPCValue >= tBound+1) {
				tpMoveStack = tpStartMove;
				vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,1,1);
				return(tBeta);
			}

			tBound = (int)floor((-PERCENTILE * SIGMA + tAlpha - B)/A);
			tPCValue = tfSearchGameShallow(
									cDepth,
									ucEmpties,
									tBound-1,
									tBound,
									tColor,
									cCurrentValue,
									cGameStage);
			if (tPCValue <= tBound-1) {
				tpMoveStack = tpStartMove;
				vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,1,1);
				return(tAlpha);
			}
			/**/
			MMX_FLUSH
			f64 A = taMPC[cGameStage][ucDepth].dB;
			f64 B = taMPC[cGameStage][ucDepth].dA;
			f64 SIGMA = taMPC[cGameStage][ucDepth].dSigma;
			char   cDepth = taMPC[cGameStage][ucDepth].cDepth;

			int tUpperBound = (int)floor((+PERCENTILE * SIGMA + tBeta - B)/A);
			int tLowerBound = (int)floor((-PERCENTILE * SIGMA + tAlpha - B)/A);

			int tPCValue = tfSearchGameShallow(
									cDepth,
									ucEmpties,
									tLowerBound - 1,
									tUpperBound + 1,
									tColor,
									cCurrentValue,
									cGameStage);
			if (tPCValue <= tLowerBound - 1) {
				tpMoveStack = tpStartMove;
				return(tAlpha);
			}
			else 
				if (tPCValue >= tUpperBound + 1) {
					tpMoveStack = tpStartMove;
					return(tBeta);
				}
			/**/
		}
	}

#ifdef __MPI__
	ALIGN(16) u16	waTmp[MP_COUNT];
	COPY_EF_INDEXES(waTmp,waMPIndexes);
#endif
/**
	// sorting moves
	u8 ucMaxMoveCount = 60; 
	if (ucDepth > MIN_FASTEST_FIRST_DEPTH)
		ucMaxMoveCount = ucfSortMovesFastestFirst(tpCurMove,iIndex,tColor,tOpponentColor);
/**/
	for (int i=0, iMoveIndex=0; (tBest < tBeta) && (i<60); i++,tpCurMove++) {
//		__ASSERT((tpMoveStack - taGlobalMoveStack) % 60 == 0);
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (tGlobalSearchParameters.taBoard[*tpCurMove] != EMPTY)
			continue;
#ifdef __MPI__
		if (bfMakeMoveMobilityIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {
#else
		if (bfMakeMoveIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {
#endif
			iMoveIndex++;
			// if first time - do not use PVS
			if (!bFound) {
				ucBestMove = *tpCurMove;
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				if (ucDepth > 2)
					tBest = -tfSearchGame(
						ucDepth - 1,
						ucEmpties - 1,
						-tBeta,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1,
						cUseProbCut);
				else 
					tBest = -tfSearchOnePly(
						ucEmpties - 1,
						-tBeta,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1);

				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				bFound = true;
				vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);

				//vfUndo(dwFlipCount, tOpponentColor);
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				//cCurrentValue = cSafeValue;
				//tGlobalSearchParameters.tHashValue = tOldHashValue;
				vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,iMoveIndex);
				//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,iMoveIndex,1);
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				//bfMakeMoveIfAvailableFast(tColor,ucBestMove,&cCurrentValue,&dwFlipCount);
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
			}
			// otherwise - PVS
			else {
				if (tBest > tAlpha) 
					tAlpha = tBest;

				if (ucDepth > 2)
					tValue = -tfSearchGame(
						ucDepth - 1,
						ucEmpties - 1,
						-tAlpha - 1,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1,
						cUseProbCut);
				else 
					tValue = -tfSearchOnePly(
						ucEmpties - 1,
						-tAlpha - 1,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1);
				
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				if ((tValue > tAlpha) && (tValue < tBeta)) {
						
					ucBestMove = *tpCurMove;
					vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
					vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,iMoveIndex);
					if (ucDepth > 2)
						tBest = -tfSearchGame(
							ucDepth - 1,
							ucEmpties - 1,
							-tBeta,
							-tValue,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1,
							cUseProbCut);
					else
						tBest = -tfSearchOnePly(
							ucEmpties - 1,
							-tBeta,
							-tValue,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1);

					__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
						
					//vfUndo(dwFlipCount, tOpponentColor);
					__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
					//cCurrentValue = cSafeValue;
					//tGlobalSearchParameters.tHashValue = tOldHashValue;
					vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,iMoveIndex);
					//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,1,1);
					__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
					//bfMakeMoveIfAvailableFast(tColor,ucBestMove,&cCurrentValue,&dwFlipCount);
					__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				}
				else
					if (tValue > tBest) {
						tBest = tValue;
						ucBestMove = *tpCurMove;
						vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);

						//vfUndo(dwFlipCount, tOpponentColor);
						__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
						//cCurrentValue = cSafeValue;
						//tGlobalSearchParameters.tHashValue = tOldHashValue;
						vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,iMoveIndex);
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,1,1);
						__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
						//bfMakeMoveIfAvailableFast(tColor,ucBestMove,&cCurrentValue,&dwFlipCount);
						__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
					}
#ifdef EXTENDED_INTERFACE
					else {
						//vfUndo(dwFlipCount, tOpponentColor);
						__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
						//cCurrentValue = cSafeValue;
						//tGlobalSearchParameters.tHashValue = tOldHashValue;
						vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,iMoveIndex);
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,1,1);
						__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
						//bfMakeMoveIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount);
						__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
					}
#endif
			}
			
			// restoring undo data
#ifdef __MPI__
	#ifdef __OLD_UNDO__
			vfUndo(dwFlipCount, tOpponentColor);
			COPY_EF_INDEXES(waMPIndexes,waTmp);
	#else
			if (dwFlipCount > MAX_RESTORABLE_FLIPS) {
				vfUndo(dwFlipCount, tOpponentColor);
				COPY_EF_INDEXES(waMPIndexes,waTmp);
			}
			else
				vfUndoAndRestore(dwFlipCount, tOpponentColor);
	#endif
#else
			vfUndo(dwFlipCount, tOpponentColor);
#endif
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
			cCurrentValue = cSafeValue;
			tGlobalSearchParameters.tHashValue = tOldHashValue;
		}
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	}
	
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	// check for leaf or to pass
	if (!bFound) {
		tpCurMove = tpStartMove;
		for ( i=0; i<MAX_MOVE_COUNT; i++, tpCurMove++) {
			if (bfCheckIfAvailable(tOpponentColor,*tpCurMove)) {
				tGlobalSearchParameters.tHashValue ^= tChangeColor;
				tBest = -tfSearchGame(
					ucDepth,
					ucEmpties,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue,
					cGameStage,
					cUseProbCut);

				tGlobalSearchParameters.tHashValue = tOldHashValue;
				
				if (ucDepth >= MIN_HASH_STORE_DEPTH)
					vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,tBest,MOVE_PASS);

				tpMoveStack = tpStartMove;

				return(tBest);
			}
		}
		tpMoveStack = tpStartMove;

		tBest = cCurrentValue > 0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties;
		
		tBest *=128;
		
		qwVariationCount++;

		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (ucDepth >= MIN_HASH_STORE_DEPTH)
			vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,-INFINITY,+INFINITY,tBest,MOVE_PASS);
		
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		return(tBest);
	}
	else {
		if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
			COPY_MOVES(taGlobalMoveStack,tpStartMove);

		tpMoveStack = tpStartMove;

		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (ucDepth >= MIN_HASH_STORE_DEPTH)
			vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,tBest,ucBestMove);
		
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		return(tBest);
	}
}

int tfSearchSelectiveEndgame(u8 ucDepth, u8 ucEmpties, int tAlpha, int tBeta, TBoardCell tColor, char cCurrentValue, char cGameStage, char cUseProbCut)
{
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);

	qwVariationCount++;

	//if (bStopThink)
	//	return(-INFINITY);

	TBoardCell tOpponentColor = (tColor == BLACK) ? WHITE : BLACK;

	int tBest = -INFINITY;
	int tValue;
	int tStartAlpha = tAlpha;

	SHashValue tOldHashValue = tGlobalSearchParameters.tHashValue;
	
	u32 dwFlipCount;
	char cSafeValue = cCurrentValue;
	bool bFound = false;

	int iIndex = -1;
	u8 ucLevel;
	u8 ucBestMove;

	//////////////////////////////////////////////////////////
	//  if (bfNodeInHash(...))
	//		if (bmOutOfBounds(...))
	//			return(...);
	//		else
	//			vfUpdateBounds(...);
	//////////////////////////////////////////////////////////

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	if (ucDepth >= MIN_HASH_STORE_DEPTH) {
		
		iIndex = ifGetTTNode(tGlobalSearchParameters.tHashValue,&ucLevel);
		
		if (iIndex >= 0) {
			
			ucBestMove = taHash[iIndex].taCells[ucLevel].ucMove;

			if (ucBestMove != MOVE_PASS)
				vfMoveToFrontByValue(taGlobalMoveStack,ucBestMove);

			if (taHash[iIndex].taCells[ucLevel].ucDepth == ucDepth) {
				if(tBeta > taHash[iIndex].taCells[ucLevel].tBeta){
					tBeta = taHash[iIndex].taCells[ucLevel].tBeta;
					if(tBeta <= tAlpha){
						return(tBeta);
					}
				}
				if(tAlpha < taHash[iIndex].taCells[ucLevel].tAlpha){
					tAlpha = taHash[iIndex].taCells[ucLevel].tAlpha;
					if(tBeta <= tAlpha){
						return(tAlpha);
					}
				}
			}
		}
	}
	
	//////////////////////////////////////////////////////////
	//  if (bmOrderMovesCondition(...)) {
	//		vfOrderMoves(...)
	//////////////////////////////////////////////////////////

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	u8 ucMoveCount = ucEmpties;
	int _iMoveIndex;
	bool bETCFound = false;
	TBoardCell *tpCurMove = tpfGetAvailableMovesAndFastestFirstX(ucBestMove, ucEmpties, &ucMoveCount, tColor,tOpponentColor, &bETCFound, &ucBestMove, &_iMoveIndex, tBeta);
	if (bETCFound) {
		vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,tBest,ucBestMove);
		return(tBeta);
	}
	TBoardCell *tpStartMove = tpCurMove;
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	
	//////////////////////////////////////////////////////////
	//	Enhanced transposition cut-off
	//////////////////////////////////////////////////////////

	/**
	if ((iIndex >= 0) && (taHash[iIndex].taCells[ucLevel].ucDepth == ucDepth))
		if (ucDepth >= MIN_ENHANCED_TRANSPOSITION_CUT_OFF_DEPTH) {
			SHashValue tHashValue = tGlobalSearchParameters.tHashValue;
			for (int i=0, iMoveIndex = 0; (i<MAX_MOVE_COUNT); i++,tpCurMove++) {
				if (bfChangeHashIfAvailable(tColor,*tpCurMove)) {
					qwVariationCount++;
					iMoveIndex++;
					u32 dwIndex = (u32)(tGlobalSearchParameters.tHashValue & (((u32)1 << MAX_TT_HASH_BITS) - 1));
					u32 dwHashValue = (u32)((tGlobalSearchParameters.tHashValue >> 32) & 0xffffffff);
					tGlobalSearchParameters.tHashValue = tHashValue;
					if ((taHash[dwIndex].taCells[0].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[0].ucDepth == ucDepth - 1) && 
						(taHash[dwIndex].taCells[0].ucMove) && 
						(taHash[dwIndex].taCells[0].tBeta <= -tBeta)) {
						ucBestMove = *tpCurMove;
						//if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
							vfMoveToFrontByIndex(i,taGlobalMoveStack,ucBestMove);
						//if (ucDepth >= MIN_HASH_STORE_DEPTH)
							vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,-taHash[dwIndex].taCells[0].tBeta,ucBestMove);
						tpMoveStack = tpStartMove;
						return(tBeta);
					}
					if ((taHash[dwIndex].taCells[1].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[1].ucDepth == ucDepth - 1) && 
						(taHash[dwIndex].taCells[1].ucMove) && 
						(taHash[dwIndex].taCells[1].tBeta <= -tBeta)) {
						ucBestMove = *tpCurMove;
						//if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
							vfMoveToFrontByIndex(i,taGlobalMoveStack,ucBestMove);
						//if (ucDepth >= MIN_HASH_STORE_DEPTH)
							vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,-taHash[dwIndex].taCells[1].tBeta,ucBestMove);
						tpMoveStack = tpStartMove;
						return(tBeta);
					}
				}
			}
			//ucMoveCount = iMoveIndex;
			tpCurMove = tpStartMove;
		}
	/**/


	//////////////////////////////////////////////////////////
	//  if (bmCorrelationExists(...)) {
	//		if (bmCheckAlphaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//		
	//		if (bmCheckBetaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//	}
	//////////////////////////////////////////////////////////

	if ((cUseProbCut) && (ucDepth < MAX_PLY) && (ucDepth > MIN_PLY) && (cGameStage >= 0)) {
		
		if (taMPC[cGameStage][ucDepth].bCorrelationExists) {

			/**
			f64 A = taMPC[cGameStage][ucDepth].dB;
			f64 B = taMPC[cGameStage][ucDepth].dA;
			f64 SIGMA = taMPC[cGameStage][ucDepth].dSigma;
			char   cDepth = taMPC[cGameStage][ucDepth].cDepth;

			int tBound = (int)floor((+PERCENTILE * SIGMA + tBeta - B)/A);

			int tPCValue = tfSearchGameShallow(
									cDepth,
									ucEmpties,
									tBound,
									tBound+1,
									tColor,
									cCurrentValue,
									cGameStage);
			
			if (tPCValue >= tBound+1) {
				tpMoveStack = tpStartMove;
				return(tBeta);
			}

			tBound = (int)floor((-PERCENTILE * SIGMA + tAlpha - B)/A);
			tPCValue = tfSearchGameShallow(
									cDepth,
									ucEmpties,
									tBound-1,
									tBound,
									tColor,
									cCurrentValue,
									cGameStage);
			if (tPCValue <= tBound-1) {
				tpMoveStack = tpStartMove;
				return(tAlpha);
			}
			/**/
			MMX_FLUSH
			f64 A = taMPC[cGameStage][ucDepth].dB;
			f64 B = taMPC[cGameStage][ucDepth].dA;
			f64 SIGMA = taMPC[cGameStage][ucDepth].dSigma;
			char   cDepth = taMPC[cGameStage][ucDepth].cDepth;

			int tUpperBound = (int)floor((+PERCENTILE * SIGMA + tBeta - B)/A);
			int tLowerBound = (int)floor((-PERCENTILE * SIGMA + tAlpha - B)/A);

			int tPCValue = tfSearchGameShallow(
									cDepth,
									ucEmpties,
									tLowerBound - 1,
									tUpperBound + 1,
									tColor,
									cCurrentValue,
									cGameStage);
			if (tPCValue <= tLowerBound - 1) {
				tpMoveStack = tpStartMove;
				return(tAlpha);
			}
			else 
				if (tPCValue >= tUpperBound + 1) {
					tpMoveStack = tpStartMove;
					return(tBeta);
				}
			/**/
		}
	}

#ifdef __MPI__
	ALIGN(16) u16	waTmp[MP_COUNT];
	COPY_EF_INDEXES(waTmp,waMPIndexes);
#endif
/**
	// sorting moves
	u8 ucMaxMoveCount = 60; 
	if (ucDepth > MIN_FASTEST_FIRST_DEPTH)
		ucMaxMoveCount = ucfSortMovesFastestFirst(tpCurMove,iIndex,tColor,tOpponentColor);
/**/
	for (int i=0; (tBest < tBeta) && (i<ucMoveCount); i++,tpCurMove++) {
		//__ASSERT((tpMoveStack - taGlobalMoveStack) % 60 == 0);
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (tGlobalSearchParameters.taBoard[*tpCurMove] != EMPTY)
			continue;
#ifdef __MPI__
		if (bfMakeMoveMobilityIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {
#else
		if (bfMakeMoveIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {
#endif
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);

			// if first time - do not use PVS
			if (!bFound) {
				ucBestMove = *tpCurMove;
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				
				if (ucDepth > FASTEST_FIRST)
					tBest = -tfSearchSelectiveEndgame(
						ucDepth - 1,
						ucEmpties - 1,
						-tBeta,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1,
						cUseProbCut);
				else 
					if (ucDepth > 2)
						tBest = -tfSearchGame(
							ucDepth - 1,
							ucEmpties - 1,
							-tBeta,
							-tAlpha,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1,
							cUseProbCut);
					else 
						tBest = -tfSearchOnePly(
							ucEmpties - 1,
							-tBeta,
							-tAlpha,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1);

				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				bFound = true;
				vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
			}
			// otherwise - PVS
			else {
				if (tBest > tAlpha) 
					tAlpha = tBest;

				if (ucDepth > FASTEST_FIRST)
					tValue = -tfSearchSelectiveEndgame(
						ucDepth - 1,
						ucEmpties - 1,
						-tAlpha - 1,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1,
						cUseProbCut);
				else
					if (ucDepth > 2)
						tValue = -tfSearchGame(
							ucDepth - 1,
							ucEmpties - 1,
							-tAlpha - 1,
							-tAlpha,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1,
							cUseProbCut);
					else 
						tValue = -tfSearchOnePly(
							ucEmpties - 1,
							-tAlpha - 1,
							-tAlpha,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1);
				
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				if ((tValue > tAlpha) && (tValue < tBeta)) {
						
					if (ucDepth > 2)
						tBest = -tfSearchGame(
							ucDepth - 1,
							ucEmpties - 1,
							-tBeta,
							-tValue,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1,
							cUseProbCut);
					else
						tBest = -tfSearchOnePly(
							ucEmpties - 1,
							-tBeta,
							-tValue,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1);

					__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
					ucBestMove = *tpCurMove;
					vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
				}
				else
					if (tValue > tBest) {
						tBest = tValue;
						ucBestMove = *tpCurMove;
						vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
					}
			}
			
			// restoring undo data
#ifdef __MPI__
	#ifdef __OLD_UNDO__
			vfUndo(dwFlipCount, tOpponentColor);
			COPY_EF_INDEXES(waMPIndexes,waTmp);
	#else
			if (dwFlipCount > MAX_RESTORABLE_FLIPS) {
				vfUndo(dwFlipCount, tOpponentColor);
				COPY_EF_INDEXES(waMPIndexes,waTmp);
			}
			else
				vfUndoAndRestore(dwFlipCount, tOpponentColor);
	#endif
#else
			vfUndo(dwFlipCount, tOpponentColor);
#endif
			cCurrentValue = cSafeValue;
			tGlobalSearchParameters.tHashValue = tOldHashValue;
		}
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	}
	
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	// check for leaf or to pass
	if (!bFound) {
		tpCurMove = tpStartMove;
		for ( i=0; i<MAX_MOVE_COUNT; i++, tpCurMove++) {
			if (bfCheckIfAvailable(tOpponentColor,*tpCurMove)) {
				tGlobalSearchParameters.tHashValue ^= tChangeColor;
				tBest = -tfSearchSelectiveEndgame(
					ucDepth,
					ucEmpties,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue,
					cGameStage,
					cUseProbCut);

				tGlobalSearchParameters.tHashValue = tOldHashValue;
				
				if (ucDepth >= MIN_HASH_STORE_DEPTH)
					vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,tBest,MOVE_PASS);

				tpMoveStack = tpStartMove;

				return(tBest);
			}
		}
		tpMoveStack = tpStartMove;

		tBest = cCurrentValue > 0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties;
		
		tBest *=128;
		
		qwVariationCount++;

		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (ucDepth >= MIN_HASH_STORE_DEPTH)
			vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,-INFINITY,+INFINITY,tBest,MOVE_PASS);
		
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		return(tBest);
	}
	else {
		if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
			COPY_MOVES(taGlobalMoveStack,tpStartMove);

		tpMoveStack = tpStartMove;

		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (ucDepth >= MIN_HASH_STORE_DEPTH)
			vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,tBest,ucBestMove);
		
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		return(tBest);
	}
}

int tfSearchSelectiveEndgameRoot(u8 ucDepth, u8 ucEmpties, int tAlpha, int tBeta, TBoardCell tColor, char cCurrentValue, char cGameStage, char cUseProbCut)
{
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);

	qwVariationCount++;

	//////////////////////////////////////////////////////////
	//  if (bmLeafNode(...))
	//		if (bmQdwescenceCondition(...))
	//			return(tfSearchGame(...));		
	//		else
	//			return(EVALUATE(...));
	//////////////////////////////////////////////////////////

	if (!ucDepth) {
		
		SFeature tBest;
		
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (tColor == WHITE)
			EVALUATE(0,cGameStage,tBest,-1)
		else
			EVALUATE(1,cGameStage,tBest,1)

		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		
		return(tBest);
	}
	
	
	TBoardCell tOpponentColor = (tColor == BLACK) ? WHITE : BLACK;

	int tBest = -INFINITY;
	int tValue;
	int tStartAlpha = tAlpha;

	SHashValue tOldHashValue = tGlobalSearchParameters.tHashValue;
	
	u32 dwFlipCount;
	char cSafeValue = cCurrentValue;
	bool bFound = false;

	int iIndex = -1;
	u8 ucLevel;
	u8 ucBestMove = MOVE_PASS;

	//////////////////////////////////////////////////////////
	//  if (bfNodeInHash(...))
	//		if (bmOutOfBounds(...))
	//			return(...);
	//		else
	//			vfUpdateBounds(...);
	//////////////////////////////////////////////////////////

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	if (ucDepth >= MIN_HASH_STORE_DEPTH) {
		
		iIndex = ifGetTTNode(tGlobalSearchParameters.tHashValue,&ucLevel);
		
		if (iIndex >= 0) {
			
			ucBestMove = taHash[iIndex].taCells[ucLevel].ucMove;

			if (ucBestMove != MOVE_PASS)
				vfMoveToFrontByValue(taGlobalMoveStack,ucBestMove);

			if (taHash[iIndex].taCells[ucLevel].ucDepth == ucDepth) {
				if(tBeta > taHash[iIndex].taCells[ucLevel].tBeta){
					tBeta = taHash[iIndex].taCells[ucLevel].tBeta;
					if(tBeta <= tAlpha){
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,cCurMoveCount,1);
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,1,1);
						return(tBeta);
					}
				}
				if(tAlpha < taHash[iIndex].taCells[ucLevel].tAlpha){
					tAlpha = taHash[iIndex].taCells[ucLevel].tAlpha;
					if(tBeta <= tAlpha){
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tAlpha,false,cCurMoveCount,1);
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tAlpha,false,1,1);
						return(tAlpha);
					}
				}
			}
		}
	}
	
	//////////////////////////////////////////////////////////
	//  if (bmOrderMovesCondition(...)) {
	//		vfOrderMoves(...)
	//////////////////////////////////////////////////////////

	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	u8 ucMoveCount = ucEmpties;
	TBoardCell *tpCurMove = tpfGetAvailableMoves();
	TBoardCell *tpStartMove = tpCurMove;
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	
	//////////////////////////////////////////////////////////
	//	Enhanced transposition cut-off
	//////////////////////////////////////////////////////////

	if ((iIndex >= 0) && (taHash[iIndex].taCells[ucLevel].ucDepth == ucDepth))
		if (ucDepth >= MIN_ENHANCED_TRANSPOSITION_CUT_OFF_DEPTH) {
			SHashValue tHashValue = tGlobalSearchParameters.tHashValue;
			for (int i=0, iMoveIndex = 0; (i<MAX_MOVE_COUNT); i++,tpCurMove++) {
				if (bfChangeHashIfAvailable(tColor,*tpCurMove)) {
					qwVariationCount++;
					iMoveIndex++;
					u32 dwIndex = (u32)(tGlobalSearchParameters.tHashValue & (((u32)1 << MAX_TT_HASH_BITS) - 1));
					u32 dwHashValue = (u32)((tGlobalSearchParameters.tHashValue >> 32) & 0xffffffff);
					tGlobalSearchParameters.tHashValue = tHashValue;
					if ((taHash[dwIndex].taCells[0].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[0].ucDepth == ucDepth - 1) && 
						(taHash[dwIndex].taCells[0].ucMove) && 
						(taHash[dwIndex].taCells[0].tBeta <= -tBeta)) {
						ucBestMove = *tpCurMove;
						//if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
							vfMoveToFrontByIndex(i,taGlobalMoveStack,ucBestMove);
						//if (ucDepth >= MIN_HASH_STORE_DEPTH)
							vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,-taHash[dwIndex].taCells[0].tBeta,ucBestMove);
						tpMoveStack = tpStartMove;
						vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,iMoveIndex);
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,iMoveIndex,1);
						return(tBeta);
					}
					if ((taHash[dwIndex].taCells[1].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[1].ucDepth == ucDepth - 1) && 
						(taHash[dwIndex].taCells[1].ucMove) && 
						(taHash[dwIndex].taCells[1].tBeta <= -tBeta)) {
						ucBestMove = *tpCurMove;
						//if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
							vfMoveToFrontByIndex(i,taGlobalMoveStack,ucBestMove);
						//if (ucDepth >= MIN_HASH_STORE_DEPTH)
							vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,-taHash[dwIndex].taCells[1].tBeta,ucBestMove);
						tpMoveStack = tpStartMove;
						vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,iMoveIndex);
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,iMoveIndex,1);
						return(tBeta);
					}
				}
			}
			//ucMoveCount = iMoveIndex;
			tpCurMove = tpStartMove;
		}


	//////////////////////////////////////////////////////////
	//  if (bmCorrelationExists(...)) {
	//		if (bmCheckAlphaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//		
	//		if (bmCheckBetaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//	}
	//////////////////////////////////////////////////////////

	if ((cUseProbCut) && (ucDepth < MAX_PLY) && (ucDepth > MIN_PLY) && (cGameStage > 0)) {
		
		if (taMPC[cGameStage][ucDepth].bCorrelationExists) {

			/**
			f64 A = taMPC[cGameStage][ucDepth].dB;
			f64 B = taMPC[cGameStage][ucDepth].dA;
			f64 SIGMA = taMPC[cGameStage][ucDepth].dSigma;
			char   cDepth = taMPC[cGameStage][ucDepth].cDepth;

			int tBound = (int)floor((+PERCENTILE * SIGMA + tBeta - B)/A);

			int tPCValue = tfSearchGameShallow(
									cDepth,
									ucEmpties,
									tBound,
									tBound+1,
									tColor,
									cCurrentValue,
									cGameStage);
			
			if (tPCValue >= tBound+1) {
				tpMoveStack = tpStartMove;
				vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,1,1);
				return(tBeta);
			}

			tBound = (int)floor((-PERCENTILE * SIGMA + tAlpha - B)/A);
			tPCValue = tfSearchGameShallow(
									cDepth,
									ucEmpties,
									tBound-1,
									tBound,
									tColor,
									cCurrentValue,
									cGameStage);
			if (tPCValue <= tBound-1) {
				tpMoveStack = tpStartMove;
				vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBeta,false,1,1);
				return(tAlpha);
			}
			/**/
			MMX_FLUSH
			f64 A = taMPC[cGameStage][ucDepth].dB;
			f64 B = taMPC[cGameStage][ucDepth].dA;
			f64 SIGMA = taMPC[cGameStage][ucDepth].dSigma;
			char   cDepth = taMPC[cGameStage][ucDepth].cDepth;

			int tUpperBound = (int)floor((+PERCENTILE * SIGMA + tBeta - B)/A);
			int tLowerBound = (int)floor((-PERCENTILE * SIGMA + tAlpha - B)/A);

			int tPCValue = tfSearchGameShallow(
									cDepth,
									ucEmpties,
									tLowerBound - 1,
									tUpperBound + 1,
									tColor,
									cCurrentValue,
									cGameStage);
			if (tPCValue <= tLowerBound - 1) {
				tpMoveStack = tpStartMove;
				return(tAlpha);
			}
			else 
				if (tPCValue >= tUpperBound + 1) {
					tpMoveStack = tpStartMove;
					return(tBeta);
				}
			/**/
		}
	}

#ifdef __MPI__
	ALIGN(16) u16	waTmp[MP_COUNT];
	COPY_EF_INDEXES(waTmp,waMPIndexes);
#endif
/**
	// sorting moves
	u8 ucMaxMoveCount = 60; 
	if (ucDepth > MIN_FASTEST_FIRST_DEPTH)
		ucMaxMoveCount = ucfSortMovesFastestFirst(tpCurMove,iIndex,tColor,tOpponentColor);
/**/
	for (int i=0, iMoveIndex=0; (tBest < tBeta) && (i<60); i++,tpCurMove++) {
//		__ASSERT((tpMoveStack - taGlobalMoveStack) % 60 == 0);
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (tGlobalSearchParameters.taBoard[*tpCurMove] != EMPTY)
			continue;
#ifdef __MPI__
		if (bfMakeMoveMobilityIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {
#else
		if (bfMakeMoveIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {
#endif
			iMoveIndex++;
			// if first time - do not use PVS
			if (!bFound) {
				ucBestMove = *tpCurMove;
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				if (ucDepth > FASTEST_FIRST)
					tBest = -tfSearchSelectiveEndgame(
						ucDepth - 1,
						ucEmpties - 1,
						-tBeta,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1,
						cUseProbCut);
				else 
					if (ucDepth > 2)
						tBest = -tfSearchGame(
							ucDepth - 1,
							ucEmpties - 1,
							-tBeta,
							-tAlpha,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1,
							cUseProbCut);
					else 
						tBest = -tfSearchOnePly(
							ucEmpties - 1,
							-tBeta,
							-tAlpha,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1);

				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				bFound = true;
				vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);

				//vfUndo(dwFlipCount, tOpponentColor);
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				//cCurrentValue = cSafeValue;
				//tGlobalSearchParameters.tHashValue = tOldHashValue;
				vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,iMoveIndex);
				//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,iMoveIndex,1);
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				//bfMakeMoveIfAvailableFast(tColor,ucBestMove,&cCurrentValue,&dwFlipCount);
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
			}
			// otherwise - PVS
			else {
				if (tBest > tAlpha) 
					tAlpha = tBest;

				if (ucDepth > FASTEST_FIRST)
					tValue = -tfSearchSelectiveEndgame(
						ucDepth - 1,
						ucEmpties - 1,
						-tAlpha - 1,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue,
						cGameStage + 1,
						cUseProbCut);
				else 
					if (ucDepth > 2)
						tValue = -tfSearchGame(
							ucDepth - 1,
							ucEmpties - 1,
							-tAlpha - 1,
							-tAlpha,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1,
							cUseProbCut);
					else 
						tValue = -tfSearchOnePly(
							ucEmpties - 1,
							-tAlpha - 1,
							-tAlpha,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1);
				
				__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				if ((tValue > tAlpha) && (tValue < tBeta)) {
						
					ucBestMove = *tpCurMove;
					vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
					vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,iMoveIndex);
					if (ucDepth > 2)
						tBest = -tfSearchGame(
							ucDepth - 1,
							ucEmpties - 1,
							-tBeta,
							-tValue,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1,
							cUseProbCut);
					else
						tBest = -tfSearchOnePly(
							ucEmpties - 1,
							-tBeta,
							-tValue,
							tOpponentColor,
							-cCurrentValue,
							cGameStage + 1);

					__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
						
					//vfUndo(dwFlipCount, tOpponentColor);
					__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
					//cCurrentValue = cSafeValue;
					//tGlobalSearchParameters.tHashValue = tOldHashValue;
					vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,iMoveIndex);
					//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,1,1);
					__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
					//bfMakeMoveIfAvailableFast(tColor,ucBestMove,&cCurrentValue,&dwFlipCount);
					__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
				}
				else
					if (tValue > tBest) {
						tBest = tValue;
						ucBestMove = *tpCurMove;
						vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);

						//vfUndo(dwFlipCount, tOpponentColor);
						__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
						//cCurrentValue = cSafeValue;
						//tGlobalSearchParameters.tHashValue = tOldHashValue;
						vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,iMoveIndex);
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,1,1);
						__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
						//bfMakeMoveIfAvailableFast(tColor,ucBestMove,&cCurrentValue,&dwFlipCount);
						__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
					}
#ifdef EXTENDED_INTERFACE
					else {
						//vfUndo(dwFlipCount, tOpponentColor);
						__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
						//cCurrentValue = cSafeValue;
						//tGlobalSearchParameters.tHashValue = tOldHashValue;
						vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,iMoveIndex);
						//vfPrintPrincipalVariation(ucBestMove,ucDepth,tColor,tBest,false,1,1);
						__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
						//bfMakeMoveIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount);
						__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
					}
#endif
			}
			
			// restoring undo data
#ifdef __MPI__
	#ifdef __OLD_UNDO__
			vfUndo(dwFlipCount, tOpponentColor);
			COPY_EF_INDEXES(waMPIndexes,waTmp);
	#else
			if (dwFlipCount > MAX_RESTORABLE_FLIPS) {
				vfUndo(dwFlipCount, tOpponentColor);
				COPY_EF_INDEXES(waMPIndexes,waTmp);
			}
			else
				vfUndoAndRestore(dwFlipCount, tOpponentColor);
	#endif
#else
			vfUndo(dwFlipCount, tOpponentColor);
#endif
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
			__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
			cCurrentValue = cSafeValue;
			tGlobalSearchParameters.tHashValue = tOldHashValue;
		}
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	}
	
	__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
	// check for leaf or to pass
	if (!bFound) {
		tpCurMove = tpStartMove;
		for ( i=0; i<MAX_MOVE_COUNT; i++, tpCurMove++) {
			if (bfCheckIfAvailable(tOpponentColor,*tpCurMove)) {
				tGlobalSearchParameters.tHashValue ^= tChangeColor;
				tBest = -tfSearchSelectiveEndgame(
					ucDepth,
					ucEmpties,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue,
					cGameStage,
					cUseProbCut);

				tGlobalSearchParameters.tHashValue = tOldHashValue;
				
				if (ucDepth >= MIN_HASH_STORE_DEPTH)
					vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,tBest,MOVE_PASS);

				tpMoveStack = tpStartMove;

				return(tBest);
			}
		}
		tpMoveStack = tpStartMove;

		tBest = cCurrentValue > 0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties;
		
		tBest *=128;
		
		qwVariationCount++;

		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (ucDepth >= MIN_HASH_STORE_DEPTH)
			vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,-INFINITY,+INFINITY,tBest,MOVE_PASS);
		
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		return(tBest);
	}
	else {
		if (ucDepth >= MIN_KILLER_HEURISTIC_DEPTH)
			COPY_MOVES(taGlobalMoveStack,tpStartMove);

		tpMoveStack = tpStartMove;

		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		if (ucDepth >= MIN_HASH_STORE_DEPTH)
			vfPutTTNode(tGlobalSearchParameters.tHashValue,ucDepth,tStartAlpha,tBeta,tBest,ucBestMove);
		
		__ASSERT(cGlobalTimeStamp == cBugTimeStamp);
		return(tBest);
	}
}
