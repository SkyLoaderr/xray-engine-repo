//////////////////////////////////////////////////////////////////////////////////
//	Module			:	endgame solver											//
//	File			:	eg_pvs.cpp												//
//	Creation date	:	14.02.2001												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "board.h"
#include "eg_pvs.h"
#include "game.h"
#include "mobil.h"
#include "moves.h"
#include "tt.h"
#include "_time.h"
#include "_mmx.h"

#define MIN_ENDGAME_MOVE_ORDERING_DEPTH		6
#define MIN_HASH_STORE_DEPTH				6
#define MIN_KILLER_HEURISTIC_DEPTH			4
#define FASTEST_FIRST						7
#define	MIN_USE_MPC_DEPTH					12
//#define MIN_ENHANCED_TRANSPOSITION_CUT_OFF_DEPTH 11

IC void vfMakeSavedMove(SSavedMove &tSavedMove, TBoardCell tColor, char &cCurrentValue)
{
	TIMER_START(MakeSavedMove)
	tGlobalSearchParameters.tHashValue = tSavedMove.tHashValue;
	u32 dwFlipCount = tSavedMove.ucFlipCount;
	cCurrentValue = tSavedMove.cCurrentValue;
	
	TBoardCell **tppFlipStackX = tSavedMove.tppFlipStack;
	*(*(--tppFlipStackX)) = tColor;
	dwFlipCount--;

	while (dwFlipCount) {
		dwFlipCount--;
        *(*(--tppFlipStackX)) = tColor;
    }
	TIMER_STOP(MakeSavedMove)
}

IC void vfUndoSavedMove(SSavedMove &tSavedMove, TBoardCell tColor)
{
	TIMER_START(UndoSavedMove)
	u32 dwFlipCount = tSavedMove.ucFlipCount;
	
	TBoardCell **tppFlipStackX = tSavedMove.tppFlipStack;
	*(*(--tppFlipStackX)) = EMPTY;
	dwFlipCount--;

	while (dwFlipCount) {
		dwFlipCount--;
        *(*(--tppFlipStackX)) = tColor;
    }
	TIMER_STOP(UndoSavedMove)
}

SFeature tfSolveForMate(u8 ucEmpties, SFeature tAlpha, SFeature tBeta, TBoardCell tColor, char cCurrentValue, bool bRoot)
{
	qwVariationCount++;

	//////////////////////////////////////////////////////////
	//  if (bmLeafNode(...))
	//		if (bmQdwescenceCondition(...))
	//			return(tfSearchGame(...));		
	//		else
	//			return(tfEvaluate(...));
	//////////////////////////////////////////////////////////

	u8 ucBestMove = 0;
	
	SFeature tBest = -INFINITY;
	SFeature tValue;
	SFeature tStartAlpha = tAlpha;
	u32 dwFlipCount;

	TBoardCell tOpponentColor = (tColor == BLACK) ? WHITE : BLACK;

	if (ucEmpties == 1) {
		
		ucBestMove = *taGlobalMoveStack;
		
		if (bfCountIfAvailable(tColor,ucBestMove,&cCurrentValue))
			tBest = cCurrentValue;
		else {
			cCurrentValue = -cCurrentValue;
			if (bfCountIfAvailable(tOpponentColor,ucBestMove,&cCurrentValue))
				tBest = -cCurrentValue;
			else 
				tBest = cCurrentValue < 0 ? -cCurrentValue + ucEmpties : -cCurrentValue - ucEmpties;
		}
		
		qwVariationCount+=2;
		tBest *=128;
		
		return(tBest);
	}

	SHashValue tOldHashValue = tGlobalSearchParameters.tHashValue;
	
	char cSafeValue = cCurrentValue;
	bool bFound = false;

	//////////////////////////////////////////////////////////
	//  if (bmOrderMovesCondition(...)) {
	//		vfOrderMoves(...)
	//////////////////////////////////////////////////////////

	u8 ucMoveCount = ucEmpties;
	TBoardCell *tpCurMove;
	tpCurMove = tpfGetAvailableMoves(ucEmpties);
	TBoardCell *tpStartMove	= tpCurMove;

	//////////////////////////////////////////////////////////
	//  if (bmOrderMovesCondition(...)) {
	//		if (bmCheckAlphaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//		
	//		if (bmCheckBetaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//	}
	//////////////////////////////////////////////////////////

	for (int i=0, iMoveIndex=0; (tBest < tBeta) && (i < ucMoveCount); i++, tpCurMove++ ) {
		
		if (bfMakeMoveIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {

			vfMoveToBackByValue(*tpCurMove,ucEmpties);
			
			iMoveIndex++;
			
			if (tBest > tAlpha) 
				tAlpha = tBest;

			tValue = -tfSolveForMate(
				ucEmpties - 1,
				-tBeta,
				-tAlpha,
				tOpponentColor,
				-cCurrentValue,
				false);
			
			
			if (tValue > tBest) {
				ucBestMove = *tpCurMove;
				tBest = tValue;
				if (i)
					vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);

				if (bRoot) {
					vfUndo(dwFlipCount, tOpponentColor);
					cCurrentValue = cSafeValue;
					tGlobalSearchParameters.tHashValue = tOldHashValue;
					vfPrintPrincipalVariation(ucBestMove,ucEmpties,tColor,tBest,true,iMoveIndex);
					bfMakeMoveIfAvailable(tColor,ucBestMove,&cCurrentValue,&dwFlipCount);
				}
			}

			bFound = true;
			// restoring undo data
			vfUndo(dwFlipCount, tOpponentColor);
			cCurrentValue = cSafeValue;
			tGlobalSearchParameters.tHashValue = tOldHashValue;
		}
	}
	
	// check for leaf or to pass
	if (!bFound) {
		tpCurMove = tpStartMove;
		for ( i=0; i<ucEmpties; i++, tpCurMove++) {
			if (bfCheckIfAvailable(tOpponentColor,*tpCurMove)) {
				tGlobalSearchParameters.tHashValue ^= tChangeColor;
				tBest = -tfSolveForMate(
					ucEmpties,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue,
					false);

				tGlobalSearchParameters.tHashValue = tOldHashValue;
				
				tpMoveStack -= ucEmpties;

				return(tBest);
			}
		}
		
		tpMoveStack = tpStartMove;

		tBest = cCurrentValue > 0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties;
		
		tBest *=128;

		qwVariationCount++;

		return(tBest);
	}
	else {
		COPY_MEMORY(taGlobalMoveStack,tpStartMove,ucEmpties*sizeof(TBoardCell));

		tpMoveStack = tpStartMove;

		return(tBest);
	}
}

SFeature tfSolveGameShallow(u8 ucEmpties, SFeature tAlpha, SFeature tBeta, TBoardCell tColor, char cCurrentValue)
{
	qwVariationCount++;

	//////////////////////////////////////////////////////////
	//  if (bmLeafNode(...))
	//		if (bmQdwescenceCondition(...))
	//			return(tfSearchGame(...));		
	//		else
	//			return(tfEvaluate(...));
	//////////////////////////////////////////////////////////

	u8 ucBestMove = 0;
	
	SFeature tBest = -INFINITY;
	SFeature tValue;
	SFeature tStartAlpha = tAlpha;
	u32 dwFlipCount;

	TBoardCell tOpponentColor = (tColor == BLACK) ? WHITE : BLACK;

	if (ucEmpties == 2) {
		
			  ucBestMove  = taGlobalMoveStack[0];
		u8 ucBestMove1 = taGlobalMoveStack[1];
		char  cCurrentValueOld = cCurrentValue;

		if (bfMakeMoveIfAvailableFast(tColor,ucBestMove,&cCurrentValue,&dwFlipCount)) {
			
			cCurrentValue *= -1;
			
			if (bfCountIfAvailable(tOpponentColor,ucBestMove1,&cCurrentValue))
				tBest = -cCurrentValue;
			else {
				cCurrentValue *= -1;
				if (bfCountIfAvailable(tColor,ucBestMove1,&cCurrentValue))
					tBest = cCurrentValue;
				else {
					tBest = cCurrentValue < 0 ? cCurrentValue - 1: cCurrentValue + 1;
					//qwVariationCount--;
				}
			}
			
			vfUndo(dwFlipCount,tOpponentColor);

			qwVariationCount+=2;

			if (tBest*128 < tBeta) 
			{

				cCurrentValue = cCurrentValueOld;

				if (bfMakeMoveIfAvailableFast(tColor,ucBestMove1,&cCurrentValue,&dwFlipCount)) {
					
					cCurrentValue *= -1;
					
					if (bfCountIfAvailable(tOpponentColor,ucBestMove,&cCurrentValue)) {
						if (-cCurrentValue > tBest)
							tBest =  -cCurrentValue;
					}
					else {
						
						cCurrentValue *= -1;
						
						if (bfCountIfAvailable(tColor,ucBestMove,&cCurrentValue)) {
							if (cCurrentValue > tBest) {
								tBest =  cCurrentValue;
							}
						}
						else {
							SFeature tBest1 = cCurrentValue < 0 ? cCurrentValue - 1: cCurrentValue + 1;
							if (tBest1 > tBest)
								tBest = tBest1;
							//qwVariationCount--;
						}
					}

					vfUndo(dwFlipCount,tOpponentColor);
		
					qwVariationCount+=2;

				}
			}
		}
		else
			if (bfMakeMoveIfAvailableFast(tColor,ucBestMove1,&cCurrentValue,&dwFlipCount)) {
				
				cCurrentValue *= -1;
				
				if (bfCountIfAvailable(tOpponentColor,ucBestMove,&cCurrentValue))
					tBest = -cCurrentValue;
				else {
					cCurrentValue *= -1;
					if (bfCountIfAvailable(tColor,ucBestMove,&cCurrentValue))
						tBest = cCurrentValue;
					else {
						tBest = cCurrentValue < 0 ? cCurrentValue - 1: cCurrentValue + 1;
						//qwVariationCount--;
					}
				}
				
				vfUndo(dwFlipCount,tOpponentColor);

				qwVariationCount+=2;

			}
			else {
				cCurrentValueOld = (cCurrentValue *= -1);
				TBoardCell tTmp = tColor;
				tColor = tOpponentColor;
				tOpponentColor = tTmp;
				if (bfMakeMoveIfAvailableFast(tColor,ucBestMove,&cCurrentValue,&dwFlipCount)) {
					
					cCurrentValue *= -1;
					
					if (bfCountIfAvailable(tOpponentColor,ucBestMove1,&cCurrentValue))
						tBest = -cCurrentValue;
					else {
						cCurrentValue *= -1;
						if (bfCountIfAvailable(tColor,ucBestMove1,&cCurrentValue))
							tBest = cCurrentValue;
						else {
							tBest = cCurrentValue < 0 ? cCurrentValue - 1: cCurrentValue + 1;
							//qwVariationCount--;
						}
					}
					
					vfUndo(dwFlipCount,tOpponentColor);
					
					qwVariationCount+=2;

					if (tBest*128 < -tAlpha) 
					{
						
						cCurrentValue = cCurrentValueOld;

						if (bfMakeMoveIfAvailableFast(tColor,ucBestMove1,&cCurrentValue,&dwFlipCount)) {
							
							cCurrentValue *= -1;
							
							if (bfCountIfAvailable(tOpponentColor,ucBestMove,&cCurrentValue)) {
								if (-cCurrentValue > tBest)
									tBest =  -cCurrentValue;
							}
							else {
								
								cCurrentValue *= -1;
								
								if (bfCountIfAvailable(tColor,ucBestMove,&cCurrentValue)) {
									if (cCurrentValue > tBest) {
										tBest =  cCurrentValue;
									}
								}
								else {
									SFeature tBest1 = cCurrentValue < 0 ? cCurrentValue - 1: cCurrentValue + 1;
									if (tBest1 > tBest)
										tBest = tBest1;
									//qwVariationCount--;
								}
							}

							vfUndo(dwFlipCount,tOpponentColor);
							qwVariationCount+=2;
						}
					}
					tBest *= -1;
				}
				else
					if (bfMakeMoveIfAvailableFast(tColor,ucBestMove1,&cCurrentValue,&dwFlipCount)) {
						
						cCurrentValue *= -1;
						
						if (bfCountIfAvailable(tOpponentColor,ucBestMove,&cCurrentValue))
							tBest = -cCurrentValue;
						else {
							cCurrentValue *= -1;
							if (bfCountIfAvailable(tColor,ucBestMove,&cCurrentValue))
								tBest = cCurrentValue;
							else {
								tBest = cCurrentValue < 0 ? cCurrentValue - 1: cCurrentValue + 1;
								//qwVariationCount--;
							}
						}
						
						vfUndo(dwFlipCount,tOpponentColor);
						tBest *= -1;

						qwVariationCount+=2;

					}
					else {
						tBest = -cCurrentValue < 0 ? -cCurrentValue - 2 : -cCurrentValue + 2;
						
						qwVariationCount++;

					}
			}

		tBest *=128;
		
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

	SHashValue tOldHashValue = tGlobalSearchParameters.tHashValue;
	
	char cSafeValue = cCurrentValue;
	bool bFound = false;

	//int iIndex = -1;
	//u8 ucLevel;
	STT_Structure tHashCell;

	//////////////////////////////////////////////////////////
	//  if (bfNodeInHash(...))
	//		if (bmOutOfBounds(...))
	//			return(...);
	//		else
	//			vfUpdateBounds(...);
	//////////////////////////////////////////////////////////

	if (ucEmpties >= MIN_HASH_STORE_DEPTH) {
		if (bfGetEndgameTTNode(tGlobalSearchParameters.tHashValue,tHashCell)) {
			
			/**
			if ((tHashCell.ucDepth == ucEmpties) || (tHashCell.ucDepth >= MIN_USE_MPC_DEPTH)) {
				ucBestMove = tHashCell.ucMove;

				if (ucBestMove != MOVE_PASS)
					vfMoveToFrontByValue(taGlobalMoveStack,ucBestMove);
			}
			/**/

			if (tHashCell.ucDepth > ucEmpties) {
				if(tBeta > tHashCell.tBeta){
					tBeta = tHashCell.tBeta;
					if(tBeta <= tAlpha){
						return(tBeta);
					}
				}
				if(tAlpha < tHashCell.tAlpha){
					tAlpha = tHashCell.tAlpha;
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

	u8 ucMoveCount = ucEmpties;
	TBoardCell *tpCurMove;
	tpCurMove = tpfGetAvailableMoves(ucEmpties);
	TBoardCell *tpStartMove	= tpCurMove;

	//////////////////////////////////////////////////////////
	//  if (bmOrderMovesCondition(...)) {
	//		if (bmCheckAlphaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//		
	//		if (bmCheckBetaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//	}
	//////////////////////////////////////////////////////////

	for (int i=0, iMoveIndex=0; (tBest < tBeta) && (i < ucMoveCount); i++, tpCurMove++ ) {
		
		if ((bfMakeMoveIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount))) {

			vfMoveToBackByValue(*tpCurMove,ucEmpties);
			
			iMoveIndex++;
			
			if (tBest > tAlpha) 
				tAlpha = tBest;

			tValue = -tfSolveGameShallow(
				ucEmpties - 1,
				-tBeta,
				-tAlpha,
				tOpponentColor,
				-cCurrentValue);
			
			
			if (tValue > tBest) {
				ucBestMove = *tpCurMove;
				tBest = tValue;
				//if (i)
					vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
			}
			bFound = true;
			// restoring undo data
			vfUndo(dwFlipCount, tOpponentColor);
			cCurrentValue = cSafeValue;
			tGlobalSearchParameters.tHashValue = tOldHashValue;
		}
	}
	
	// check for leaf or to pass
	if (!bFound) {
		tpCurMove = tpStartMove;
		for ( i=0; i<ucEmpties; i++, tpCurMove++) {
			if (bfCheckIfAvailable(tOpponentColor,*tpCurMove)) {
				tGlobalSearchParameters.tHashValue ^= tChangeColor;
				tBest = -tfSolveGameShallow(
					ucEmpties,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue);

				tGlobalSearchParameters.tHashValue = tOldHashValue;
				
				if (ucEmpties >= MIN_HASH_STORE_DEPTH)
					vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,tStartAlpha,tBeta,tBest,MOVE_PASS);

				tpMoveStack = tpStartMove;

				return(tBest);
			}
		}
		
		tpMoveStack = tpStartMove;

		tBest = cCurrentValue > 0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties;
		
		tBest *=128;

		if (ucEmpties >= MIN_HASH_STORE_DEPTH)
			vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,-INFINITY,+INFINITY,tBest,MOVE_PASS);
		
		qwVariationCount++;

		return(tBest);
	}
	else {
		//if (ucEmpties >= MIN_KILLER_HEURISTIC_DEPTH) {
		//	COPY_MEMORY(taGlobalMoveStack,tpStartMove,ucEmpties*sizeof(TBoardCell));
		//}

		tpMoveStack = tpStartMove;

		if (ucEmpties >= MIN_HASH_STORE_DEPTH)
			vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,tStartAlpha,tBeta,tBest,ucBestMove);
		
		return(tBest);
	}
}

SFeature tfSolveGame(u8 ucEmpties, SFeature tAlpha, SFeature tBeta, TBoardCell tColor, char cCurrentValue)
{
	qwVariationCount++;
//	if (qwVariationCount == 18406253) {
//		vfShowBoard(tColor);
//	}

	//////////////////////////////////////////////////////////
	//  if (bmLeafNode(...))
	//		if (bmQdwescenceCondition(...))
	//			return(tfSearchGame(...));		
	//		else
	//			return(tfEvaluate(...));
	//////////////////////////////////////////////////////////

	u8 ucBestMove = 0;
	
	SFeature tBest = -INFINITY;
	SFeature tValue;
	SFeature tStartAlpha = tAlpha;
	u32 dwFlipCount;

	TBoardCell tOpponentColor = (tColor == BLACK) ? WHITE : BLACK;

	SHashValue tOldHashValue = tGlobalSearchParameters.tHashValue;
	
	char cSafeValue = cCurrentValue;
	bool bFound = false;

	//int iIndex = -1;
	//u8 ucLevel;
	STT_Structure tHashCell;

	//////////////////////////////////////////////////////////
	//  if (bfNodeInHash(...))
	//		if (bmOutOfBounds(...))
	//			return(...);
	//		else
	//			vfUpdateBounds(...);
	//////////////////////////////////////////////////////////

	if (ucEmpties >= MIN_HASH_STORE_DEPTH)	{
		if (bfGetEndgameTTNode(tGlobalSearchParameters.tHashValue,tHashCell)) {
			
			if ((tHashCell.ucDepth > ucEmpties) || (tHashCell.ucDepth >= MIN_USE_MPC_DEPTH)) {
				ucBestMove = tHashCell.ucMove;

				if (ucBestMove != MOVE_PASS)
					vfMoveToFrontByValue(taGlobalMoveStack,ucBestMove);
			}

			if (tHashCell.ucDepth > ucEmpties) {
				if(tBeta > tHashCell.tBeta){
					tBeta = tHashCell.tBeta;
					if(tBeta <= tAlpha)
						return(tBeta);
				}
				if(tAlpha < tHashCell.tAlpha){
					tAlpha = tHashCell.tAlpha;
					if(tBeta <= tAlpha)
						return(tAlpha);
				}
			}
		}
	}
	
	//////////////////////////////////////////////////////////
	//  if (bmOrderMovesCondition(...)) {
	//		vfOrderMoves(...)
	//////////////////////////////////////////////////////////

	/**/
	int i = 0;
	if ((ucBestMove) && (ucBestMove != MOVE_PASS)) {
		if (bfMakeMoveIfAvailableFast(tColor,ucBestMove,&cCurrentValue,&dwFlipCount)) {

			vfMoveToBackByValue(ucBestMove,ucEmpties);
			
			if (tBest > tAlpha) 
				tAlpha = tBest;

			if (ucEmpties <= FASTEST_FIRST)
				tValue = -tfSolveGameShallow(
					ucEmpties - 1,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue);
			else
				tValue = -tfSolveGame(
					ucEmpties - 1,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue);
			
			
			if (tValue > tBest)
				tBest = tValue;
			bFound = true;
			
			// restoring undo data
			vfUndo(dwFlipCount, tOpponentColor);

			cCurrentValue = cSafeValue;
			tGlobalSearchParameters.tHashValue = tOldHashValue;
			i++;
		}
	}

	/**
	u8 ucMoveCount = ucEmpties;
	SSavedMove	taSavedMoves[32];
	TBoardCell **tppOldFlipStack = tppFlipStack;
	bool bETCFound;
	int _iMoveIndex;
	if (tBest < tBeta) {
		vfGetAvailableMovesAndFastestFirst(ucBestMove, ucEmpties, &ucMoveCount, tColor,tOpponentColor, &bETCFound, &ucBestMove, &_iMoveIndex, tBeta, cCurrentValue, taSavedMoves);
		if (bETCFound) {
			vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,tStartAlpha,tBeta,tBeta,ucBestMove);
			tppFlipStack = tppOldFlipStack;
			return(tBeta);
		}
	}

	for ( i=0 ; (tBest < tBeta) && (i < ucMoveCount); i++) {
		
		vfMakeSavedMove(taSavedMoves[i],tColor,cCurrentValue);
		
		vfMoveToBackByValue(taSavedMoves[i].tMove,ucEmpties);

		if (tBest > tAlpha) 
			tAlpha = tBest;

		if (ucEmpties <= FASTEST_FIRST)
			tValue = -tfSolveGameShallow(
				ucEmpties - 1,
				-tBeta,
				-tAlpha,
				tOpponentColor,
				-cCurrentValue);
		else
			tValue = -tfSolveGame(
				ucEmpties - 1,
				-tBeta,
				-tAlpha,
				tOpponentColor,
				-cCurrentValue);
		
		
		if (tValue > tBest) {
			ucBestMove = taSavedMoves[i].tMove;
			tBest = tValue;
		}
		bFound = true;
		// restoring undo data
		vfUndoSavedMove(taSavedMoves[i], tOpponentColor);
		cCurrentValue = cSafeValue;
		tGlobalSearchParameters.tHashValue = tOldHashValue;
	}
	
	tppFlipStack = tppOldFlipStack;
	//////////////////////////////////////////////////////////
	//  if (bmOrderMovesCondition(...)) {
	//		if (bmCheckAlphaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//		
	//		if (bmCheckBetaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//	}
	//////////////////////////////////////////////////////////
	
	// check for leaf or to pass
	if (!bFound) {
		//TBoardCell *tpCurMove = tpfGetAvailableMoves(ucEmpties), *tpStartMove = tpCurMove;
		TBoardCell *tpCurMove = tpfGetAvailableMoves(ucEmpties), *tpStartMove = tpCurMove;;
		for ( i=0; i<ucEmpties; i++, tpCurMove++) {
			if (bfCheckIfAvailable(tOpponentColor,*tpCurMove)) {
				tGlobalSearchParameters.tHashValue ^= tChangeColor;
				
				tBest = -tfSolveGame(
					ucEmpties,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue);

				tGlobalSearchParameters.tHashValue = tOldHashValue;
				
				if (ucEmpties >= MIN_HASH_STORE_DEPTH)
					vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,tStartAlpha,tBeta,tBest,MOVE_PASS);

				tpMoveStack = tpStartMove;

				return(tBest);
			}
		}
		
		tpMoveStack = tpStartMove;

		tBest = cCurrentValue > 0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties;
		
		tBest *=128;

		if (ucEmpties >= MIN_HASH_STORE_DEPTH)
			vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,-INFINITY,+INFINITY,tBest,MOVE_PASS);
		
		qwVariationCount++;

		return(tBest);
	}
	else {
		//if (ucEmpties >= MIN_KILLER_HEURISTIC_DEPTH && tpStartMove) {
		//if (ucEmpties >= MIN_KILLER_HEURISTIC_DEPTH) {
		//	COPY_MEMORY(taGlobalMoveStack,tpStartMove,ucEmpties*sizeof(TBoardCell));
		//}

		//if (tpStartMove)
		//	tpMoveStack = tpStartMove;

		if (ucEmpties >= MIN_HASH_STORE_DEPTH)
			vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,tStartAlpha,tBeta,tBest,ucBestMove);
		
		return(tBest);
	}
	/**/
	TBoardCell *tpCurMove=0, *tpStartMove=0;
	u8 ucMoveCount = ucEmpties;
	bool bETCFound;
	int _iMoveIndex;
	if (tBest < tBeta) {
		tpCurMove = tpfGetAvailableMovesAndFastestFirst(ucBestMove, ucEmpties, &ucMoveCount, tColor,tOpponentColor, &bETCFound, &ucBestMove, &_iMoveIndex, tBeta);
		if (bETCFound) {
			vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,tStartAlpha,tBeta,tBeta,ucBestMove);
			return(tBeta);
		}
		
		tpStartMove	= tpCurMove;
		//tpCurMove++;
		//i++;
	}

	for (i=0 ; (tBest < tBeta) && (i < ucMoveCount); i++, tpCurMove++ ) {
		
		if (bfMakeMoveIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {

			vfMoveToBackByValue(*tpCurMove,ucEmpties);
			
			if (tBest > tAlpha) 
				tAlpha = tBest;

			if (ucEmpties <= FASTEST_FIRST)
				tValue = -tfSolveGameShallow(
					ucEmpties - 1,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue);
			else
				tValue = -tfSolveGame(
					ucEmpties - 1,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue);
			
			
			if (tValue > tBest) {
				ucBestMove = *tpCurMove;
				tBest = tValue;
				vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
			}
			bFound = true;
			// restoring undo data
			vfUndo(dwFlipCount, tOpponentColor);
			cCurrentValue = cSafeValue;
			tGlobalSearchParameters.tHashValue = tOldHashValue;
		}
	}
	//////////////////////////////////////////////////////////
	//  if (bmOrderMovesCondition(...)) {
	//		if (bmCheckAlphaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//		
	//		if (bmCheckBetaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//	}
	//////////////////////////////////////////////////////////
	
	// check for leaf or to pass
	if (!bFound) {
		tpCurMove = tpStartMove;
		for ( i=0; i<ucEmpties; i++, tpCurMove++) {
			if (bfCheckIfAvailable(tOpponentColor,*tpCurMove)) {
				tGlobalSearchParameters.tHashValue ^= tChangeColor;
				
				tBest = -tfSolveGame(
					ucEmpties,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue);

				tGlobalSearchParameters.tHashValue = tOldHashValue;
				
				if (ucEmpties >= MIN_HASH_STORE_DEPTH)
					vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,tStartAlpha,tBeta,tBest,MOVE_PASS);

				tpMoveStack -= ucEmpties;

				return(tBest);
			}
		}
		
		tpMoveStack = tpStartMove;

		tBest = cCurrentValue > 0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties;
		
		tBest *=128;

		if (ucEmpties >= MIN_HASH_STORE_DEPTH)
			vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,-INFINITY,+INFINITY,tBest,MOVE_PASS);
		
		qwVariationCount++;

		return(tBest);
	}
	else {
		if (ucEmpties >= MIN_KILLER_HEURISTIC_DEPTH && tpStartMove) {
			COPY_MEMORY(taGlobalMoveStack,tpStartMove,ucEmpties*sizeof(TBoardCell));
		}

		if (tpStartMove)
			tpMoveStack = tpStartMove;

		if (ucEmpties >= MIN_HASH_STORE_DEPTH)
			vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,tStartAlpha,tBeta,tBest,ucBestMove);
		
		return(tBest);
	}
	/**/
}

SFeature tfSolveGameRoot(u8 ucEmpties, SFeature tAlpha, SFeature tBeta, TBoardCell tColor, char cCurrentValue)
{
	//return(tfSolveGame(ucEmpties,tAlpha,tBeta,tColor,cCurrentValue));
	qwVariationCount++;

	//////////////////////////////////////////////////////////
	//  if (bmLeafNode(...))
	//		if (bmQdwescenceCondition(...))
	//			return(tfSearchGame(...));		
	//		else
	//			return(tfEvaluate(...));
	//////////////////////////////////////////////////////////

	u8 ucBestMove = 0;
	
	SFeature tBest = -INFINITY;
	SFeature tValue;
	SFeature tStartAlpha = tAlpha;
	u32 dwFlipCount;

	TBoardCell tOpponentColor = (tColor == BLACK) ? WHITE : BLACK;

	if (ucEmpties == 1) {
		
		ucBestMove = *taGlobalMoveStack;
		
		if (bfCountIfAvailable(tColor,ucBestMove,&cCurrentValue))
			tBest = cCurrentValue;
		else {
			cCurrentValue = -cCurrentValue;
			if (bfCountIfAvailable(tOpponentColor,ucBestMove,&cCurrentValue))
				tBest = -cCurrentValue;
			else 
				tBest = cCurrentValue < 0 ? -cCurrentValue + ucEmpties : -cCurrentValue - ucEmpties;
		}
		
		qwVariationCount+=2;
		tBest *=128;
		
		return(tBest);

	}

	SHashValue tOldHashValue = tGlobalSearchParameters.tHashValue;
	
	char cSafeValue = cCurrentValue;
	bool bFound = false;

	//int iIndex = -1;
	//u8 ucLevel;
	STT_Structure tHashCell;

	//////////////////////////////////////////////////////////
	//  if (bfNodeInHash(...))
	//		if (bmOutOfBounds(...))
	//			return(...);
	//		else
	//			vfUpdateBounds(...);
	//////////////////////////////////////////////////////////

	if (bfGetEndgameTTNode(tGlobalSearchParameters.tHashValue,tHashCell)) {
		
//		if ((tHashCell.ucDepth > ucEmpties) || (tHashCell.ucDepth >= MIN_USE_MPC_DEPTH)) {
			ucBestMove = tHashCell.ucMove;

			if (ucBestMove != MOVE_PASS)
				vfMoveToFrontByValue(taGlobalMoveStack,ucBestMove);
//		}

		if (tHashCell.ucDepth > ucEmpties) {
			if(tBeta > tHashCell.tBeta){
				tBeta = tHashCell.tBeta;
				if(tBeta <= tAlpha){
					vfPrintPrincipalVariation(ucBestMove,ucEmpties,tColor,tBeta,true,cCurMoveCount,1);
					return(tBeta);
				}
			}
			if(tAlpha < tHashCell.tAlpha){
				tAlpha = tHashCell.tAlpha;
				if(tBeta <= tAlpha){
					vfPrintPrincipalVariation(ucBestMove,ucEmpties,tColor,tAlpha,true,cCurMoveCount,1);
					return(tAlpha);
				}
			}
		}
	}
	
	//////////////////////////////////////////////////////////
	//  if (bmOrderMovesCondition(...)) {
	//		vfOrderMoves(...)
	//////////////////////////////////////////////////////////

	u8 ucMoveCount = ucEmpties;
	TBoardCell *tpCurMove;
	if (ucEmpties < FASTEST_FIRST)
		tpCurMove = tpfGetAvailableMoves(ucEmpties);
	else {
		bool bETCFound;
		int iMoveIndex;
		tpCurMove = tpfGetAvailableMovesAndFastestFirst(ucBestMove, ucEmpties, &ucMoveCount, tColor,tOpponentColor, &bETCFound, &ucBestMove, &iMoveIndex, tBeta);
		if (bETCFound) {
			vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,tStartAlpha,tBeta,tBeta,ucBestMove);
			vfPrintPrincipalVariation(ucBestMove,ucEmpties,tColor,tBeta,false,iMoveIndex);
			return(tBeta);
		}
	}
	TBoardCell *tpStartMove = tpCurMove;

	//////////////////////////////////////////////////////////
	//  if (bmOrderMovesCondition(...)) {
	//		if (bmCheckAlphaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//		
	//		if (bmCheckBetaBound(..,tAlpha,tfSearchGame(...),..))
	//			return(...);
	//	}
	//////////////////////////////////////////////////////////

	for (int i=0, iMoveIndex=0; (tBest < tBeta) && (i < ucMoveCount); i++, tpCurMove++ ) {
		
		if (bfMakeMoveIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {

			vfMoveToBackByValue(*tpCurMove,ucEmpties);
			
			iMoveIndex++;
			
			if (tBest > tAlpha) 
				tAlpha = tBest;

			if (ucEmpties <= FASTEST_FIRST)
				tValue = -tfSolveGameShallow(
					ucEmpties - 1,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue);
			else
				tValue = -tfSolveGame(
					ucEmpties - 1,
					-tBeta,
					-tAlpha,
					tOpponentColor,
					-cCurrentValue);
			
			
			if (tValue > tBest) {
				ucBestMove = *tpCurMove;
				tBest = tValue;
				//if (i)
					vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
					
				vfUndo(dwFlipCount, tOpponentColor);
				cCurrentValue = cSafeValue;
				tGlobalSearchParameters.tHashValue = tOldHashValue;
				vfPrintPrincipalVariation(ucBestMove,ucEmpties,tColor,tBest,true,iMoveIndex);
				bfMakeMoveIfAvailable(tColor,ucBestMove,&cCurrentValue,&dwFlipCount);
			}
#ifdef EXTENDED_INTERFACE
			else {
				vfUndo(dwFlipCount, tOpponentColor);
				cCurrentValue = cSafeValue;
				tGlobalSearchParameters.tHashValue = tOldHashValue;
				vfPrintPrincipalVariation(ucBestMove,ucEmpties,tColor,tBest,true,iMoveIndex);
				bfMakeMoveIfAvailable(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount);
			}
#endif
			bFound = true;
			// restoring undo data
			vfUndo(dwFlipCount, tOpponentColor);
			cCurrentValue = cSafeValue;
			tGlobalSearchParameters.tHashValue = tOldHashValue;
		}
	}
	
	// check for leaf or to pass
	if (!bFound) {
		tpCurMove = tpStartMove;
		for (int i=0; i<ucEmpties; i++, tpCurMove++) {
			if (bfCheckIfAvailable(tOpponentColor,*tpCurMove)) {
				tGlobalSearchParameters.tHashValue ^= tChangeColor;
				if (ucEmpties <= FASTEST_FIRST)
					tBest = -tfSolveGameShallow(
						ucEmpties,
						-tBeta,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue);
				else
					tBest = -tfSolveGame(
						ucEmpties,
						-tBeta,
						-tAlpha,
						tOpponentColor,
						-cCurrentValue);

				tGlobalSearchParameters.tHashValue = tOldHashValue;
				
				vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,tStartAlpha,tBeta,tBest,MOVE_PASS);

				tpMoveStack -= ucEmpties;

				return(tBest);
			}
		}
		
		tpMoveStack = tpStartMove;

		tBest = cCurrentValue > 0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties;
		
		tBest *=128;

		vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,-INFINITY,+INFINITY,tBest,MOVE_PASS);
		
		qwVariationCount++;

		return(tBest);

	}
	else {
		COPY_MEMORY(taGlobalMoveStack,tpStartMove,ucEmpties*sizeof(TBoardCell));

		tpMoveStack = tpStartMove;

		vfPutEndgameTTNode(tGlobalSearchParameters.tHashValue,ucEmpties,tStartAlpha,tBeta,tBest,ucBestMove);
		
		return(tBest);
	}
}
