//////////////////////////////////////////////////////////////////////////////////
//	Module			:	Move routines											//
//	File			:	moves.cpp												//
//	Creation date	:	22.01.2002												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "board.h"
#include "eval.h"
#include "game.h"
#include "mobil.h"
#include "moves.h"
#include "_time.h"
#include "_mmx.h"

const u8 ucaW2B[64] = {
/*B2*/ 20,25,65,70,
/*B1*/ 11,16,19,26,64,71,74,79,
/*C2*/ 21,24,29,34,56,61,66,69,
/*D2*/ 22,23,38,43,47,52,67,68,
/*D3*/ 31,32,39,42,48,51,58,59,
/*D1*/ 13,14,37,44,46,53,76,77,
/*C3*/ 30,33,57,60,
/*C1*/ 12,15,28,35,55,62,75,78,
/*A1*/ 10,17,73,80, 
/*D4*/ 40,41,49,50
};


ALIGN(16)	TBoardCell   taGlobalMoveStack[60*MAX_MOVE_COUNT];
TBoardCell  *tpMoveStack;
u8			caFastestFirstOrder[MAX_MOVE_COUNT];

void vfInitializeMoveList()
{
	tpMoveStack = taGlobalMoveStack;
	
	for (int i=MAX_MOVE_COUNT-1; i>=0; i--, tpMoveStack++)
		*tpMoveStack = ucaW2B[i];
	
	/**/
	bool bOk;
	do {
		bOk = true;
		tpMoveStack = taGlobalMoveStack;
		for ( i=0; i<MAX_MOVE_COUNT-1; i++, tpMoveStack++)
			if ((tGlobalSearchParameters.taBoard[*tpMoveStack] != EMPTY) && ((tGlobalSearchParameters.taBoard[*(tpMoveStack+1)] == EMPTY))) {
				TBoardCell tTmp = *tpMoveStack;
				*tpMoveStack = *(tpMoveStack + 1);
				*(tpMoveStack + 1) = tTmp;
				bOk = false;
			}
	}
	while (!bOk);
	/**/

	tpMoveStack = taGlobalMoveStack + MAX_MOVE_COUNT;
}

void vfPrepareMoveListForBoard()
{
	tpMoveStack = taGlobalMoveStack;
	
	bool bOk;
	do {
		bOk = true;
		tpMoveStack = taGlobalMoveStack;
		for (int i=0; i<MAX_MOVE_COUNT-1; i++, tpMoveStack++)
			if ((tGlobalSearchParameters.taBoard[*tpMoveStack] != EMPTY) && ((tGlobalSearchParameters.taBoard[*(tpMoveStack+1)] == EMPTY))) {
				TBoardCell tTmp = *tpMoveStack;
				*tpMoveStack = *(tpMoveStack + 1);
				*(tpMoveStack + 1) = tTmp;
				bOk = false;
			}
	}
	while (!bOk);

	tpMoveStack = taGlobalMoveStack + MAX_MOVE_COUNT;
}

char cfGetMoveCount(TBoardCell tMoveColor)
{
	char cCurMoveCount = 0;
	for (int i=BOARD_START; i<BOARD_SIZE; i++)
		cCurMoveCount += bfCheckIfAvailable(tMoveColor,i) ? 1 : 0;
	return(cCurMoveCount);
}

TBoardCell *tpfGetAvailableMoves()
{
	TIMER_START(GetAvailableMoves)
	TBoardCell *tpTmp = tpMoveStack;
	COPY_MOVES(tpTmp,taGlobalMoveStack);
	tpMoveStack += MAX_MOVE_COUNT;
	TIMER_STOP(GetAvailableMoves)
	return(tpTmp);
}

TBoardCell *tpfGetAvailableMoves(u8 ucEmpties)
{
	TIMER_START(GetAvailableMovesWithEmpties)
	TBoardCell *tpTmp = tpMoveStack;
	memcpy(tpTmp,taGlobalMoveStack,ucEmpties*sizeof(TBoardCell));
	tpMoveStack += ucEmpties;
//	COPY_MOVES(tpTmp,taGlobalMoveStack);
//	tpMoveStack += MAX_MOVE_COUNT;
	TIMER_STOP(GetAvailableMovesWithEmpties)
	return(tpTmp);
}

void vfMoveToFrontByValue(TBoardCell *taStack, TBoardCell tMove)
{
	TIMER_START(MoveToFrontByValue)
	for ( TBoardCell *tpTmp = taStack; ; tpTmp++)
		if (*tpTmp == tMove)
			break;
	
//	__ASSERT(tpTmp - taStack < 60);

	for (int i=tpTmp - taStack; i>0; i--, tpTmp--)
		*tpTmp = *(tpTmp - 1);
	
	*tpTmp = tMove;
	TIMER_STOP(MoveToFrontByValue)
}

void vfMoveToFrontByIndex(u32 ucIndex, TBoardCell *taStack, TBoardCell tMove)
{
	TIMER_START(MoveToFrontByIndex)
	TBoardCell *tpTmp = taStack + ucIndex;

	__ASSERT(*tpTmp == tMove);

	for (int i=ucIndex; i>0; i--, tpTmp--)
		*tpTmp = *(tpTmp - 1);
	
	*tpTmp = tMove;
	TIMER_STOP(MoveToFrontByIndex)
}

#ifdef _DEBUG
void vfPrintMoves()
{
	printf("\n");
	for (TBoardCell *tpTmp = taGlobalMoveStack; tpTmp - taGlobalMoveStack < MAX_MOVE_COUNT; tpTmp++)
		printf("%2d%c,",*tpTmp,tGlobalSearchParameters.taBoard[*tpTmp] == EMPTY ? '*' : ' ');
	printf("\n");
}

void vfValidateMoveList(u8 ucEmpties)
{
	for (TBoardCell *tpTmp = taGlobalMoveStack; tpTmp - taGlobalMoveStack < ucEmpties; tpTmp++)
		if (tGlobalSearchParameters.taBoard[*tpTmp] != EMPTY) {
			*tpTmp = *tpTmp;
			vfPrintMoves();
		}
}
#endif

void vfMoveToBackByValue(TBoardCell tMove, u8 ucEmpties)
{
	TIMER_START(MoveToBackByValue)
	for ( TBoardCell *tpTmp = taGlobalMoveStack; *tpTmp != tMove; tpTmp++);
	
	__ASSERT(tpTmp - taGlobalMoveStack < ucEmpties);

//	*tpTmp = taGlobalMoveStack[ucEmpties - 1];
//	taGlobalMoveStack[ucEmpties - 1] = tMove;
	COPY_MEMORY(tpTmp,tpTmp + 1, (ucEmpties - (tpTmp - taGlobalMoveStack) - 1)*sizeof(TBoardCell));
	
	taGlobalMoveStack[ucEmpties - 1] = tMove;
	TIMER_STOP(MoveToBackByValue)
}

//int iIndexes[38];

//TBoardCell *tpfGetAvailableMovesAndFastestFirst(u8 ucBestMove, u8 ucEmpties, u8 *ucMoveCount, TBoardCell tColor, TBoardCell tOpponentColor)
/**
TBoardCell *tpfGetAvailableMovesAndFastestFirst(u8 ucBestMove, u8 ucEmpties, u8 *ucMoveCount, TBoardCell tColor, TBoardCell tOpponentColor, bool *bpETCFound, u8 *ucpBestMove, int *ipMoveIndex, SFeature tBeta)
{
	TIMER_START(GetAvailableMovesAndFastestFirst)
	TBoardCell *tpTmp = tpMoveStack, *tpMoveStackOld = tpMoveStack;
	COPY_MEMORY(tpTmp,taGlobalMoveStack,ucEmpties*sizeof(TBoardCell));
	tpMoveStack += ucEmpties;
//	COPY_MOVES(tpTmp,taGlobalMoveStack);
//	tpMoveStack += MAX_MOVE_COUNT;
	
	*ucMoveCount = 0;
	*bpETCFound = false;
	if (true) {
		memset(caFastestFirstOrder,0xff,ucEmpties*sizeof(char));
		char cCurrentValue = 0;
		u32 dwFlipCount = 0;
		//if (*tpTmp == ucBestMove)
		//	tpTmp++;
		SHashValue tHashValue = tGlobalSearchParameters.tHashValue;
		for (; tpTmp - tpMoveStackOld < ucEmpties; tpTmp++)
			if ((tGlobalSearchParameters.taBoard[*tpTmp] == EMPTY) && (bfMakeMoveIfAvailableFast(tColor, *tpTmp,&cCurrentValue,&dwFlipCount))) {
				if (ucEmpties >= 9) {
					u32 dwIndex = (u32)(tGlobalSearchParameters.tHashValue & (((u32)1 << MAX_TT_HASH_BITS) - 1));
					u32 dwHashValue = (u32)((tGlobalSearchParameters.tHashValue >> 32) & 0xffffffff);
					if ((taHash[dwIndex].taCells[0].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[0].ucDepth == ucEmpties - 0) && 
						(taHash[dwIndex].taCells[0].ucMove) && 
						(taHash[dwIndex].taCells[0].tBeta <= -tBeta)) {
						
						*bpETCFound = true;
						*ucpBestMove = *tpTmp;
						*ipMoveIndex = *ucMoveCount;

						vfUndo(dwFlipCount,tOpponentColor);
						tGlobalSearchParameters.tHashValue = tHashValue;

						tpMoveStack -= ucEmpties;
						
						qwVariationCount++;
						return(0);
					}
					if ((taHash[dwIndex].taCells[1].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[1].ucDepth == ucEmpties - 0) && 
						(taHash[dwIndex].taCells[1].ucMove) && 
						(taHash[dwIndex].taCells[1].tBeta <= -tBeta)) {
						
						*bpETCFound = true;
						*ucpBestMove = *tpTmp;
						*ipMoveIndex = *ucMoveCount;

						vfUndo(dwFlipCount,tOpponentColor);
						tGlobalSearchParameters.tHashValue = tHashValue;

						tpMoveStack -= ucEmpties;
						qwVariationCount++;
						return(0);
					}
				}
				
				++*ucMoveCount;

				char cTmp = tpTmp - tpMoveStackOld;	

				if (*tpTmp != ucBestMove) {
					for (TBoardCell *tpTmp1 = tpMoveStackOld; tpTmp1 - tpMoveStackOld < ucEmpties; tpTmp1++)
						if ((tGlobalSearchParameters.taBoard[*tpTmp1] == EMPTY) && (bfCheckIfAvailableFast(tOpponentColor, *tpTmp1)))
							caFastestFirstOrder[cTmp]++;
				}
				else {
					//--*ucMoveCount;
					caFastestFirstOrder[cTmp] = 0;
				}

				vfUndo(dwFlipCount,tOpponentColor);
				cCurrentValue = 0;
				tGlobalSearchParameters.tHashValue = tHashValue;
			}
		bool bOk;
		do {
			bOk = true;
			for (int i=0; i<ucEmpties-1; i++)
				if (caFastestFirstOrder[i] > caFastestFirstOrder[i+1]) {
					char cTmp = caFastestFirstOrder[i];
					caFastestFirstOrder[i] = caFastestFirstOrder[i+1];
					caFastestFirstOrder[i+1] = cTmp;

					TBoardCell tTmp = tpMoveStackOld[i];
					tpMoveStackOld[i] = tpMoveStackOld[i+1];
					tpMoveStackOld[i+1] = tTmp;

					bOk = false;
				}
		}
		while (!bOk);
		COPY_MEMORY(taGlobalMoveStack, tpMoveStackOld, ucEmpties*sizeof(TBoardCell));
	}

	TIMER_STOP(GetAvailableMovesAndFastestFirst)
	return(tpMoveStackOld);
}
/**/
TBoardCell *tpfGetAvailableMovesAndFastestFirst(u8 ucBestMove, u8 ucEmpties, u8 *ucMoveCount, TBoardCell tColor, TBoardCell tOpponentColor, bool *bpETCFound, u8 *ucpBestMove, int *ipMoveIndex, SFeature tBeta)
{
	TIMER_START(GetAvailableMovesAndFastestFirst)
	TBoardCell *tpTmp = tpMoveStack, *tpMoveStackOld = tpMoveStack;
	COPY_MEMORY(tpTmp,taGlobalMoveStack,ucEmpties*sizeof(TBoardCell));
	tpMoveStack += ucEmpties;
	
	*ucMoveCount = 0;
	*bpETCFound = false;
	if (true) {
		memset(caFastestFirstOrder,0xff,ucEmpties*sizeof(char));
		char cCurrentValue = 0;
		u32 dwFlipCount = 0;
		SHashValue tHashValue = tGlobalSearchParameters.tHashValue;
		for (; tpTmp - tpMoveStackOld < ucEmpties; tpTmp++)
			if ((tGlobalSearchParameters.taBoard[*tpTmp] == EMPTY) && (bfMakeMoveIfAvailableFast(tColor, *tpTmp,&cCurrentValue,&dwFlipCount))) {
				if (ucEmpties >= 9) {
					u32 dwIndex = (u32)(tGlobalSearchParameters.tHashValue & (((u32)1 << MAX_TT_HASH_BITS) - 1));
					u32 dwHashValue = (u32)((tGlobalSearchParameters.tHashValue >> 32) & 0xffffffff);
					if ((taHash[dwIndex].taCells[0].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[0].ucDepth == ucEmpties - 0) && 
						(taHash[dwIndex].taCells[0].ucMove) && 
						(taHash[dwIndex].taCells[0].tBeta <= -tBeta)) {
						
						*bpETCFound = true;
						*ucpBestMove = *tpTmp;
						*ipMoveIndex = *ucMoveCount;

						vfUndo(dwFlipCount,tOpponentColor);
						tGlobalSearchParameters.tHashValue = tHashValue;

						tpMoveStack -= ucEmpties;
						
						qwVariationCount++;
						return(0);
					}
					if ((taHash[dwIndex].taCells[1].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[1].ucDepth == ucEmpties - 0) && 
						(taHash[dwIndex].taCells[1].ucMove) && 
						(taHash[dwIndex].taCells[1].tBeta <= -tBeta)) {
						
						*bpETCFound = true;
						*ucpBestMove = *tpTmp;
						*ipMoveIndex = *ucMoveCount;

						vfUndo(dwFlipCount,tOpponentColor);
						tGlobalSearchParameters.tHashValue = tHashValue;

						tpMoveStack -= ucEmpties;
						qwVariationCount++;
						return(0);
					}
				}
				
				++*ucMoveCount;

				char cTmp = tpTmp - tpMoveStackOld;	
				caFastestFirstOrder[cTmp] = 0;
				if (*tpTmp != ucBestMove) {
					for (TBoardCell *tpTmp1 = tpMoveStackOld; tpTmp1 - tpMoveStackOld < ucEmpties; tpTmp1++)
						if ((tGlobalSearchParameters.taBoard[*tpTmp1] == EMPTY) && (bfCheckIfAvailableFast(tOpponentColor, *tpTmp1)))
							caFastestFirstOrder[cTmp]++;
				}

				vfUndo(dwFlipCount,tOpponentColor);
				cCurrentValue = 0;
				tGlobalSearchParameters.tHashValue = tHashValue;
			}
		bool bOk;
		do {
			bOk = true;
			for (int i=0; i<ucEmpties-1; i++)
				if (caFastestFirstOrder[i] > caFastestFirstOrder[i+1]) {
					char cTmp = caFastestFirstOrder[i];
					caFastestFirstOrder[i] = caFastestFirstOrder[i+1];
					caFastestFirstOrder[i+1] = cTmp;

					TBoardCell tTmp = tpMoveStackOld[i];
					tpMoveStackOld[i] = tpMoveStackOld[i+1];
					tpMoveStackOld[i+1] = tTmp;

					bOk = false;
				}
		}
		while (!bOk);
		COPY_MEMORY(taGlobalMoveStack, tpMoveStackOld, ucEmpties*sizeof(TBoardCell));
	}

	TIMER_STOP(GetAvailableMovesAndFastestFirst)
	return(tpMoveStackOld);
}

void vfGetAvailableMovesAndFastestFirst(u8 ucBestMove, u8 ucEmpties, u8 *ucMoveCount, TBoardCell tColor, TBoardCell tOpponentColor, bool *bpETCFound, u8 *ucpBestMove, int *ipMoveIndex, SFeature tBeta, char cCurrentValue, SSavedMove *taSavedMoves)
{
	TIMER_START(GetAvailableMovesAndFastestFirst)
	TBoardCell *tpTmp = taGlobalMoveStack, *tpMoveStackOld = taGlobalMoveStack;
//	TBoardCell *tpTmp = tpMoveStack, *tpMoveStackOld = tpMoveStack;
//	COPY_MEMORY(tpTmp,taGlobalMoveStack,ucEmpties*sizeof(TBoardCell));
//	tpMoveStack += ucEmpties;
//	memset(caFastestFirstOrder,0xff,ucEmpties*sizeof(char));
	
	*ucMoveCount = 0;
	*bpETCFound = false;
	char cOldCurrentValue = cCurrentValue;
	u32 dwFlipCount = 0;
	SHashValue tHashValue = tGlobalSearchParameters.tHashValue;
	for (; tpTmp - tpMoveStackOld < ucEmpties; tpTmp++)
		if ((tGlobalSearchParameters.taBoard[*tpTmp] == EMPTY) && (bfMakeMoveIfAvailableFast(tColor, *tpTmp,&cCurrentValue,&dwFlipCount))) {
			//qwVariationCount++;
			if (ucEmpties >= 9) {
				u32 dwIndex = (u32)(tGlobalSearchParameters.tHashValue & (((u32)1 << MAX_TT_HASH_BITS) - 1));
				u32 dwHashValue = (u32)((tGlobalSearchParameters.tHashValue >> 32) & 0xffffffff);
				if ((taHash[dwIndex].taCells[0].tHash == dwHashValue) && 
					(taHash[dwIndex].taCells[0].ucDepth == ucEmpties - 0) && 
					(taHash[dwIndex].taCells[0].ucMove) && 
					(taHash[dwIndex].taCells[0].tBeta <= -tBeta)) {
					
					*bpETCFound = true;
					*ucpBestMove = *tpTmp;
					*ipMoveIndex = *ucMoveCount;

					vfUndo(dwFlipCount,tOpponentColor);
					tGlobalSearchParameters.tHashValue = tHashValue;

					//tpMoveStack = tpMoveStackOld;
					
					qwVariationCount++;
					return;//(0);
				}
				if ((taHash[dwIndex].taCells[1].tHash == dwHashValue) && 
					(taHash[dwIndex].taCells[1].ucDepth == ucEmpties - 0) && 
					(taHash[dwIndex].taCells[1].ucMove) && 
					(taHash[dwIndex].taCells[1].tBeta <= -tBeta)) {
					
					*bpETCFound = true;
					*ucpBestMove = *tpTmp;
					*ipMoveIndex = *ucMoveCount;

					vfUndo(dwFlipCount,tOpponentColor);
					tGlobalSearchParameters.tHashValue = tHashValue;

					//tpMoveStack = tpMoveStackOld;

					qwVariationCount++;
					return;//(0);
				}
			}
			
			taSavedMoves[*ucMoveCount].cCurrentValue = cCurrentValue;
			taSavedMoves[*ucMoveCount].tHashValue = tGlobalSearchParameters.tHashValue;
			taSavedMoves[*ucMoveCount].tMove = *tpTmp;
			taSavedMoves[*ucMoveCount].ucFlipCount = (u8)dwFlipCount;
			taSavedMoves[*ucMoveCount].tppFlipStack = tppFlipStack;
			taSavedMoves[*ucMoveCount].ucFastestFirstValue = 0;

			if (*tpTmp != ucBestMove) {
				for (TBoardCell *tpTmp1 = tpMoveStackOld; tpTmp1 - tpMoveStackOld < ucEmpties; tpTmp1++)
					if ((tGlobalSearchParameters.taBoard[*tpTmp1] == EMPTY) && (bfCheckIfAvailableFast(tOpponentColor, *tpTmp1)))
						taSavedMoves[*ucMoveCount].ucFastestFirstValue++;
			}
			//caFastestFirstOrder[tpTmp - tpMoveStackOld] = taSavedMoves[*ucMoveCount].ucFastestFirstValue;

			vfUndo(dwFlipCount,tOpponentColor);
			cCurrentValue = cOldCurrentValue;
			tGlobalSearchParameters.tHashValue = tHashValue;
			tppFlipStack = taSavedMoves[*ucMoveCount].tppFlipStack;
			++*ucMoveCount;
		}

	/**/
	for (int i=1; i<*ucMoveCount; i++)
		if (taSavedMoves[i - 1].ucFastestFirstValue > taSavedMoves[i].ucFastestFirstValue) {
			SWAP_SAVED_MOVES((char *)(&(taSavedMoves[i - 1])),(char *)(&(taSavedMoves[i])));
			for (int j = i - 1; j>0; j--)
				if (taSavedMoves[j - 1].ucFastestFirstValue > taSavedMoves[j].ucFastestFirstValue) {
					SWAP_SAVED_MOVES((char *)(&(taSavedMoves[j - 1])),(char *)(&(taSavedMoves[j])));
				}
				else
					break;
		}
	/**
	bool bOk;
	do {
		bOk = true;
		for (int i=0; i<(*ucMoveCount) - 1; i++)
			if (taSavedMoves[i].ucFastestFirstValue > taSavedMoves[i + 1].ucFastestFirstValue) {
				SWAP_SAVED_MOVES((char *)(&(taSavedMoves[i])),(char *)(&(taSavedMoves[i + 1])));
				bOk = false;
			}
	}
	while (!bOk);
	/**
	for ( i=0; i<*ucMoveCount; i++)
		for (int j=0; j<ucEmpties; j++)
			if (tpMoveStackOld[j] == taSavedMoves[i].tMove) {
				TBoardCell tTemp = tpMoveStackOld[i];
				tpMoveStackOld[i] = tpMoveStackOld[j];
				tpMoveStackOld[j] = tTemp;
				break;
			}
	/**

	do {
		bOk = true;
		for (int i=0; i<ucEmpties-1; i++)
			if (caFastestFirstOrder[i] > caFastestFirstOrder[i+1]) {
				char cTmp = caFastestFirstOrder[i];
				caFastestFirstOrder[i] = caFastestFirstOrder[i+1];
				caFastestFirstOrder[i+1] = cTmp;

				TBoardCell tTmp = tpMoveStackOld[i];
				tpMoveStackOld[i] = tpMoveStackOld[i+1];
				tpMoveStackOld[i+1] = tTmp;

				bOk = false;
			}
	}
	while (!bOk);

	/**
	for (int i=0; i<*ucMoveCount; i++)
		if (taSavedMoves[i].tMove != tpMoveStackOld[i]) {
			__asm int 3;
		}
	/**/
	//COPY_MEMORY(taGlobalMoveStack, tpMoveStackOld, ucEmpties*sizeof(TBoardCell));

	TIMER_STOP(GetAvailableMovesAndFastestFirst);
	return;//(tpMoveStackOld);
}

TBoardCell *tpfGetAvailableMovesAndFastestFirstX(u8 ucBestMove, u8 ucEmpties, u8 *ucMoveCount, TBoardCell tColor, TBoardCell tOpponentColor, bool *bpETCFound, u8 *ucpBestMove, int *ipMoveIndex, SFeature tBeta)
{
	TIMER_START(GetAvailableMovesAndFastestFirst)
	TBoardCell *tpTmp = tpMoveStack, *tpMoveStackOld = tpMoveStack;
	COPY_MEMORY(tpTmp,taGlobalMoveStack,MAX_MOVE_COUNT*sizeof(TBoardCell));
	tpMoveStack += MAX_MOVE_COUNT;
	
	*ucMoveCount = 0;
	*bpETCFound = false;
	if (true) {
		memset(caFastestFirstOrder,0xff,ucEmpties*sizeof(char));
		char cCurrentValue = 0;
		u32 dwFlipCount = 0;
		SHashValue tHashValue = tGlobalSearchParameters.tHashValue;
		for (; tpTmp - tpMoveStackOld < ucEmpties; tpTmp++)
			if ((tGlobalSearchParameters.taBoard[*tpTmp] == EMPTY) && (bfMakeMoveIfAvailableFast(tColor, *tpTmp,&cCurrentValue,&dwFlipCount))) {
				if (ucEmpties >= 5) {
					u32 dwIndex = (u32)(tGlobalSearchParameters.tHashValue & (((u32)1 << MAX_TT_HASH_BITS) - 1));
					u32 dwHashValue = (u32)((tGlobalSearchParameters.tHashValue >> 32) & 0xffffffff);
					if ((taHash[dwIndex].taCells[0].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[0].ucDepth == ucEmpties - 0) && 
						(taHash[dwIndex].taCells[0].ucMove) && 
						(taHash[dwIndex].taCells[0].tBeta <= -tBeta)) {
						
						*bpETCFound = true;
						*ucpBestMove = *tpTmp;
						*ipMoveIndex = *ucMoveCount;

						vfUndo(dwFlipCount,tOpponentColor);
						tGlobalSearchParameters.tHashValue = tHashValue;

						tpMoveStack -= ucEmpties;
						
						qwVariationCount++;
						return(0);
					}
					if ((taHash[dwIndex].taCells[1].tHash == dwHashValue) && 
						(taHash[dwIndex].taCells[1].ucDepth == ucEmpties - 0) && 
						(taHash[dwIndex].taCells[1].ucMove) && 
						(taHash[dwIndex].taCells[1].tBeta <= -tBeta)) {
						
						*bpETCFound = true;
						*ucpBestMove = *tpTmp;
						*ipMoveIndex = *ucMoveCount;

						vfUndo(dwFlipCount,tOpponentColor);
						tGlobalSearchParameters.tHashValue = tHashValue;

						tpMoveStack -= ucEmpties;
						qwVariationCount++;
						return(0);
					}
				}
				
				++*ucMoveCount;

				char cTmp = tpTmp - tpMoveStackOld;	
				caFastestFirstOrder[cTmp] = 0;
				if (*tpTmp != ucBestMove) {
					for (TBoardCell *tpTmp1 = tpMoveStackOld; tpTmp1 - tpMoveStackOld < ucEmpties; tpTmp1++)
						if ((tGlobalSearchParameters.taBoard[*tpTmp1] == EMPTY) && (bfCheckIfAvailableFast(tOpponentColor, *tpTmp1)))
							caFastestFirstOrder[cTmp]++;
				}

				vfUndo(dwFlipCount,tOpponentColor);
				cCurrentValue = 0;
				tGlobalSearchParameters.tHashValue = tHashValue;
			}
		bool bOk;
		do {
			bOk = true;
			for (int i=0; i<ucEmpties-1; i++)
				if (caFastestFirstOrder[i] > caFastestFirstOrder[i+1]) {
					char cTmp = caFastestFirstOrder[i];
					caFastestFirstOrder[i] = caFastestFirstOrder[i+1];
					caFastestFirstOrder[i+1] = cTmp;

					TBoardCell tTmp = tpMoveStackOld[i];
					tpMoveStackOld[i] = tpMoveStackOld[i+1];
					tpMoveStackOld[i+1] = tTmp;

					bOk = false;
				}
		}
		while (!bOk);
		COPY_MEMORY(taGlobalMoveStack, tpMoveStackOld, ucEmpties*sizeof(TBoardCell));
	}

	TIMER_STOP(GetAvailableMovesAndFastestFirst)
	return(tpMoveStackOld);
}
