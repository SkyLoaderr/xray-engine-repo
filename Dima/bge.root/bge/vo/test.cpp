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
	qwVariationCount++;

	if (!ucDepth || !ucEmpties)
		return				(cCurrentValue);

	TBoardCell				tOpponentColor = (tColor == BLACK) ? WHITE : BLACK;

	int						tBest = -INFINITY;
	int						tValue;

	u32						dwFlipCount;
	char					cSafeValue = cCurrentValue;
	bool					bFound = false;

	TBoardCell				*tpCurMove = tpfGetAvailableMoves(ucEmpties);
	TBoardCell				*tpStartMove = tpCurMove;
	
	for (int i=0; i<ucEmpties; i++,tpCurMove++) {
		if (bfMakeMove(tGlobalSearchParameters.taBoard,tColor,*tpCurMove,&cCurrentValue,&dwFlipCount)) {
			
			vfMoveToBackByValue	(*tpCurMove,ucEmpties);
			
			bFound = true;

			tValue = -mini_max(
				ucDepth - 1,
				ucEmpties - 1,
				tOpponentColor,
				-cCurrentValue
			);

			if (tValue > tBest)
				tBest				= tValue;

			vfUndo					(dwFlipCount, tOpponentColor);
			cCurrentValue			= cSafeValue;
		}
	}
	
	if (!bFound) {
		tpCurMove = tpStartMove;
		for ( i=0; i<MAX_MOVE_COUNT; i++, tpCurMove++) {
			if (bfCheckIfAvailable(tOpponentColor,*tpCurMove)) {
				tBest = -mini_max(
					ucDepth - 1,
					ucEmpties,
					tOpponentColor,
					-cCurrentValue
				);

				tpMoveStack = tpStartMove;
				return(tBest);
			}
		}
		tpMoveStack = tpStartMove;
		tBest		= cCurrentValue > 0 ? cCurrentValue + ucEmpties : cCurrentValue - ucEmpties;
		qwVariationCount++;
		return		(tBest);
	}
	else {
		tpMoveStack = tpStartMove;
		return		(tBest);
	}
}
