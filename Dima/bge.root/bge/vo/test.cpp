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

int mini_max(u8 ucDepth, u8 ucEmpties, TBoardCell tColor, char cCurrentValue)
{
	if (!ucDepth || !ucEmpties)
		return				(cCurrentValue);

	qwVariationCount++;

	TBoardCell tOpponentColor = (tColor == BLACK) ? WHITE : BLACK;

	int tBest = -INFINITY;
	int tValue;

	SHashValue tOldHashValue = tGlobalSearchParameters.tHashValue;
	
	u32 dwFlipCount;
	char cSafeValue = cCurrentValue;
	bool bFound = false;

	int iIndex = -1;
	u8 ucLevel;
	u8 ucBestMove;

	u8 ucMoveCount = ucEmpties;
	TBoardCell *tpCurMove = tpfGetAvailableMoves(ucEmpties);
	TBoardCell *tpStartMove = tpCurMove;
	
	for (int i=0; i<60; i++,tpCurMove++) {

		if (bfMakeMoveIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {
			vfMoveToBackByValue	(*tpCurMove,ucEmpties);

		if (tGlobalSearchParameters.taBoard[*tpCurMove] != EMPTY)
			continue;
		
		if (bfMakeMoveIfAvailableFast(tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {
			bFound = true;

			tValue = -mini_max(
				ucDepth - 1,
				ucEmpties - 1,
				tOpponentColor,
				-cCurrentValue
			);

			if (tValue > tBest) {
				ucBestMove			= *tpCurMove;
				vfMoveToFrontByIndex(i,tpStartMove,ucBestMove);
			}

			vfUndo					(dwFlipCount, tOpponentColor);
			cCurrentValue			= cSafeValue;
			tGlobalSearchParameters.tHashValue = tOldHashValue;
		}
	}
	
	if (!bFound) {
		tpCurMove = tpStartMove;
		for ( i=0; i<MAX_MOVE_COUNT; i++, tpCurMove++) {
			if (bfCheckIfAvailable(tOpponentColor,*tpCurMove)) {
				tGlobalSearchParameters.tHashValue ^= tChangeColor;
				tBest = -mini_max(
					ucDepth,
					ucEmpties,
					tOpponentColor,
					-cCurrentValue
				);

				tGlobalSearchParameters.tHashValue = tOldHashValue;
				
				tpMoveStack = tpStartMove;

				return(tBest);
			}
		}
		tpMoveStack = tpStartMove;

		tBest = cCurrentValue > 0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties;
		
		qwVariationCount++;

		return(tBest);
	}
	else {
		tpMoveStack = tpStartMove;
		return(tBest);
	}
}
