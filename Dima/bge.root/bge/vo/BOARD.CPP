//////////////////////////////////////////////////////////////////////////////////
//	Module			:	Board routines											//
//	File			:	board.cpp												//
//	Creation date	:	19.12.2001												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "board.h"
#include "eval.h"
#include "game.h"
#include "mg_pvs.h"
#include "misc.h"
#include "mobil.h"
#include "tt.h"
#include "_time.h"

const u8 ucaFlippingDirections[BOARD_SIZE] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 1, 1, 2, 2, 2, 2, 3, 3,
	0, 1, 1, 2, 2, 2, 2, 3, 3,
	0, 4, 4, 5, 5, 5, 5, 6, 6,
	0, 4, 4, 5, 5, 5, 5, 6, 6,
	0, 4, 4, 5, 5, 5, 5, 6, 6,
	0, 4, 4, 5, 5, 5, 5, 6, 6,
	0, 7, 7, 8, 8, 8, 8, 9, 9,
	0, 7, 7, 8, 8, 8, 8, 9, 9,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

TBoardCell	*tpaGlobalFlipStack[65536];
TBoardCell	**tppFlipStack;
char		caNotationBoard[BOARD_SIZE];

//////////////////////////////////////////////////////////
// Implementation
//////////////////////////////////////////////////////////

TBoardCell tfMoveToIndex(char *S)
{
	return((S[1] - '1')*9 + ((S[0] >= 'a' && S[0] <= 'z' ? S[0] + 'A' -'a' : S[0]) - 'A') + BOARD_START);
}

u8 ucfGetEmptiesCount()
{
	u8 ucResult = 0;
	
	for (int i=0; i<BOARD_SIZE; i++)
		ucResult += (tGlobalSearchParameters.taBoard[i] == EMPTY ? 1 : 0);

	return(ucResult);
}

char cfGetDiskDifference()
{
    for (char i=BOARD_START, cResult=0 ; i<BOARD_SIZE-BOARD_START; i++)
        if (tGlobalSearchParameters.taBoard[i] == BLACK) 
			cResult++;
        else
            if (tGlobalSearchParameters.taBoard[i] == WHITE) 
				cResult--;
    
	return(cResult);
}

#define bfCheckDirectionIfAvailable(tpSquare,iIncrement,tMoveColor,tOpponentColor) \
{\
	if (tpCurrent[1*iIncrement] == tOpponentColor) {\
        if (tpCurrent[2*iIncrement] == tOpponentColor) {\
            if (tpCurrent[3*iIncrement] == tOpponentColor) {\
                if (tpCurrent[4*iIncrement] == tOpponentColor) {\
                    if (tpCurrent[5*iIncrement] == tOpponentColor) {\
						if (tpCurrent[6*iIncrement] == tOpponentColor) {\
							if (tpCurrent[7*iIncrement] == tMoveColor) {\
								return(true);\
							}\
						}\
						else {\
							if (tpCurrent[6*iIncrement] == tMoveColor)\
								return(true);\
						}\
					}\
					else {\
						if (tpCurrent[5*iIncrement] == tMoveColor)\
							return(true);\
					}\
				}\
				else {\
					if (tpCurrent[4*iIncrement] == tMoveColor)\
						return(true);\
				}\
			}\
			else {\
				if (tpCurrent[3*iIncrement] == tMoveColor)\
					return(true);\
			}\
		}\
		else {\
			if (tpCurrent[2*iIncrement] == tMoveColor)\
				return(true);\
		}\
    }\
}

bool bfCheckIfAvailableFast(TBoardCell tMoveColor, int iIndex)
{
	TBoardCell tOpponentColor = (tMoveColor == BLACK ? WHITE : BLACK);
    u8 ucMask = ucaFlippingDirections[iIndex];
    TBoardCell *tpSquare = tGlobalSearchParameters.taBoard + iIndex;
	TBoardCell *tpCurrent = tpSquare;

    if (tMoveColor == BLACK) {
		switch(ucMask) {
			case 1 : {
				bfCheckDirectionIfAvailable(tpSquare,   1, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   9, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  10, BLACK, WHITE);
				return(false);
			}
			case 2 : {
				bfCheckDirectionIfAvailable(tpSquare,  -1, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   1, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   8, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   9, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  10, BLACK, WHITE);
				return(false);
			}
			case 3 : {
				bfCheckDirectionIfAvailable(tpSquare,  -1, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   8, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   9, BLACK, WHITE);
				return(false);
			}
			case 4 : {
				bfCheckDirectionIfAvailable(tpSquare,  -9, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  -8, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   1, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   9, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  10, BLACK, WHITE);
				return(false);
			}
			case 5 : {
				bfCheckDirectionIfAvailable(tpSquare, -10, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  -9, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  -8, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  -1, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   1, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   8, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   9, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  10, BLACK, WHITE);
				return(false);
			}
			case 6 : {
				bfCheckDirectionIfAvailable(tpSquare, -10, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  -9, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  -1, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   8, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   9, BLACK, WHITE);
				return(false);
			}
			case 7 : {
				bfCheckDirectionIfAvailable(tpSquare,  -9, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  -8, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   1, BLACK, WHITE);
				return(false);
			}
			case 8 : {
				bfCheckDirectionIfAvailable(tpSquare, -10, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  -9, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  -8, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  -1, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,   1, BLACK, WHITE);
				return(false);
			}
			case 9 : {
				bfCheckDirectionIfAvailable(tpSquare, -10, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  -9, BLACK, WHITE);
				bfCheckDirectionIfAvailable(tpSquare,  -1, BLACK, WHITE);
				return(false);
			}
		}
	}
	else {
		switch(ucMask) {
			case 1 : {
				bfCheckDirectionIfAvailable(tpSquare,   1, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   9, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  10, WHITE, BLACK);
				return(false);
			}
			case 2 : {
				bfCheckDirectionIfAvailable(tpSquare,  -1, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   1, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   8, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   9, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  10, WHITE, BLACK);
				return(false);
			}
			case 3 : {
				bfCheckDirectionIfAvailable(tpSquare,  -1, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   8, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   9, WHITE, BLACK);
				return(false);
			}
			case 4 : {
				bfCheckDirectionIfAvailable(tpSquare,  -9, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  -8, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   1, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   9, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  10, WHITE, BLACK);
				return(false);
			}
			case 5 : {
				bfCheckDirectionIfAvailable(tpSquare, -10, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  -9, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  -8, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  -1, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   1, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   8, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   9, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  10, WHITE, BLACK);
				return(false);
			}
			case 6 : {
				bfCheckDirectionIfAvailable(tpSquare, -10, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  -9, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  -1, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   8, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   9, WHITE, BLACK);
				return(false);
			}
			case 7 : {
				bfCheckDirectionIfAvailable(tpSquare,  -9, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  -8, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   1, WHITE, BLACK);
				return(false);
			}
			case 8 : {
				bfCheckDirectionIfAvailable(tpSquare, -10, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  -9, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  -8, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  -1, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,   1, WHITE, BLACK);
				return(false);
			}
			case 9 : {
				bfCheckDirectionIfAvailable(tpSquare, -10, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  -9, WHITE, BLACK);
				bfCheckDirectionIfAvailable(tpSquare,  -1, WHITE, BLACK);
				return(false);
			}
		}
	}
    return(false);
}

bool bfCheckIfAvailable(TBoardCell tMoveColor, int iIndex)
{
	TIMER_START(CheckIfAvailable_FALSE)
	if (tGlobalSearchParameters.taBoard[iIndex] != EMPTY) {
		TIMER_STOP(CheckIfAvailable_FALSE)
		return(false);
	}
	if (bfCheckIfAvailableFast(tMoveColor,iIndex)) {
		TIMER_STOP_PREVIOUS(CheckIfAvailable_TRUE,CheckIfAvailable_FALSE)
		return(true);
	}
	else {
		TIMER_STOP(CheckIfAvailable_FALSE)
		return(false);
	}
}

#define ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, iIncrement, iHashIncrement, tMoveColor, tOpponentColor) \
{\
    tpCurrent = tpSquare + iIncrement;\
    tpHashCurrent = tpHSquare + iHashIncrement;\
    \
	if (*tpCurrent == tOpponentColor) {\
        \
		tpCurrent += iIncrement;\
        tpHashCurrent += iHashIncrement;\
        \
		if (*tpCurrent == tOpponentColor) {\
            \
			tpCurrent += iIncrement;\
            tpHashCurrent += iHashIncrement;\
        \
			if (*tpCurrent == tOpponentColor) {\
                \
				tpCurrent += iIncrement;\
                tpHashCurrent += iHashIncrement;\
            \
				if (*tpCurrent == tOpponentColor) {\
                    \
					tpCurrent += iIncrement;\
                    tpHashCurrent += iHashIncrement;\
                \
					if (*tpCurrent == tOpponentColor) {\
                        \
						tpCurrent += iIncrement;\
                        tpHashCurrent += iHashIncrement;\
                    \
						if (*tpCurrent == tOpponentColor) {\
                            tpCurrent += iIncrement;\
                            tpHashCurrent += iHashIncrement;\
                        }\
                    }\
                }\
            }\
        }\
		if (*tpCurrent==tMoveColor) {\
            \
			tpCurrent -= iIncrement;\
            tpHashCurrent -= iHashIncrement;\
			do {\
                tGlobalSearchParameters.tHashValue ^= *(tpHashCurrent + BLACK_WHITE);\
                *tpCurrent = tMoveColor;\
                *(tppFlipStack++) = tpCurrent;\
                tpCurrent -= iIncrement;\
				tpHashCurrent -= iHashIncrement;\
                *cpCurrentValue += 2;\
            } \
			while (tpCurrent != tpSquare);\
			\
        }\
    }\
\
}

bool bfMakeMoveIfAvailableFast(TBoardCell tMoveColor, int iIndex, char *cpCurrentValue, u32 *dwpFlipCount)
{
    TIMER_START(MakeMoveIfAvailableFast_FALSE)
	TBoardCell tOpponentColor = (tMoveColor == BLACK ? WHITE : BLACK);
    u8 ucMask = ucaFlippingDirections[iIndex];
	char cOldValue = *cpCurrentValue;

	TBoardCell *tpSquare  = tGlobalSearchParameters.taBoard + iIndex;
    const SHashBoard *tpHSquare = taHashBoard + HASH_VALUES*iIndex;
	TBoardCell *tpCurrent = tpSquare;
	const SHashBoard *tpHashCurrent = tpHSquare;

    if (tMoveColor == BLACK) {
		switch(ucMask) {
			case 1 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 2 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 3 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 4 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 5 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 6 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 7 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 8 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 9 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				break;
			}
		}
	
		*dwpFlipCount = (*cpCurrentValue - cOldValue) >> 1;
    
		if (*dwpFlipCount) {
        
			*tpSquare = BLACK;
			tGlobalSearchParameters.tHashValue ^= *(tpHSquare + BLACK_EMPTY);
			*(tppFlipStack++) = tpSquare;
			++(*cpCurrentValue);
			++(*dwpFlipCount);
			TIMER_STOP_PREVIOUS(MakeMoveIfAvailableFast_TRUE,MakeMoveIfAvailableFast_FALSE)
			return(true);

		}
		else {
			TIMER_STOP(MakeMoveIfAvailableFast_FALSE)
			return(false);
		}
	
	}
	else {
		switch(ucMask) {
			case 1 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 2 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 3 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 4 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 5 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 6 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 7 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 8 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 9 : {
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfFlipDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				break;
			}
		}
		
		*dwpFlipCount = (*cpCurrentValue - cOldValue) >> 1;
    
		if (*dwpFlipCount) {
        
			*tpSquare = WHITE;
			tGlobalSearchParameters.tHashValue ^= *(tpHSquare + WHITE_EMPTY);
			*(tppFlipStack++) = tpSquare;
			++(*cpCurrentValue);
			++(*dwpFlipCount);
			TIMER_STOP_PREVIOUS(MakeMoveIfAvailableFast_TRUE,MakeMoveIfAvailableFast_FALSE)
			return(true);
		}
		else {
			TIMER_STOP(MakeMoveIfAvailableFast_FALSE)
			return(false);
		}
	
	}
}

bool bfMakeMoveIfAvailable(TBoardCell tMoveColor, int iIndex, char *cpCurrentValue, u32 *dwpFlipCount)
{
    if (tGlobalSearchParameters.taBoard[iIndex]!=EMPTY) 
		return(false);

	if (bfMakeMoveIfAvailableFast(tMoveColor,iIndex,cpCurrentValue,dwpFlipCount))
		return(true);
	else
		return(false);
}

#define UPDATE_ACCESSABILITY_MASKS(a,b,c,d) \
	if (*c == EMPTY) {\
		a[c] &= d ^ 0xff;\
		b[c] |= d;\
	}
			
#define ucfChangeAllIfAvailable(tpSquare, tpHSquare, iIncrement, iHashIncrement, tMoveColor, tOpponentColor) \
{\
    tpCurrent = tpSquare + iIncrement;\
	if (*tpCurrent == tOpponentColor) {\
		tpCurrent += iIncrement;\
		if (*tpCurrent == tOpponentColor) {\
			tpCurrent += iIncrement;\
			if (*tpCurrent == tOpponentColor) {\
				tpCurrent += iIncrement;\
				if (*tpCurrent == tOpponentColor) {\
					tpCurrent += iIncrement;\
					if (*tpCurrent == tOpponentColor) {\
						tpCurrent += iIncrement;\
						if (*tpCurrent == tOpponentColor) {\
                            tpCurrent += iIncrement;\
                        }\
                    }\
                }\
            }\
        }\
		if (*tpCurrent==tMoveColor) {\
            \
			tpCurrent -= iIncrement;\
			tpMPCurrent += tpCurrent - tpSquare;\
            tpHashCurrent += (tpCurrent - tpSquare) * HASH_VALUES;\
			do {\
                tGlobalSearchParameters.tHashValue ^= *(tpHashCurrent + BLACK_WHITE);\
                *tpCurrent = tMoveColor;\
                *(tppFlipStack++) = tpCurrent;\
				switch (tpMPCurrent->ucCount) {\
					case 4 : {\
						*(tpMPCurrent->taPatt[0].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[0].wMPIndexChange;\
						*(tpMPCurrent->taPatt[1].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[1].wMPIndexChange;\
						*(tpMPCurrent->taPatt[2].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[2].wMPIndexChange;\
						*(tpMPCurrent->taPatt[3].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[3].wMPIndexChange;\
						break;\
					}\
					case 5 : {\
						*(tpMPCurrent->taPatt[0].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[0].wMPIndexChange;\
						*(tpMPCurrent->taPatt[1].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[1].wMPIndexChange;\
						*(tpMPCurrent->taPatt[2].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[2].wMPIndexChange;\
						*(tpMPCurrent->taPatt[3].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[3].wMPIndexChange;\
						*(tpMPCurrent->taPatt[4].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[4].wMPIndexChange;\
						break;\
					}\
					case 6 : {\
						*(tpMPCurrent->taPatt[0].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[0].wMPIndexChange;\
						*(tpMPCurrent->taPatt[1].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[1].wMPIndexChange;\
						*(tpMPCurrent->taPatt[2].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[2].wMPIndexChange;\
						*(tpMPCurrent->taPatt[3].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[3].wMPIndexChange;\
						*(tpMPCurrent->taPatt[4].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[4].wMPIndexChange;\
						*(tpMPCurrent->taPatt[5].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[5].wMPIndexChange;\
						break;\
					}\
					case 7 : {\
						*(tpMPCurrent->taPatt[0].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[0].wMPIndexChange;\
						*(tpMPCurrent->taPatt[1].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[1].wMPIndexChange;\
						*(tpMPCurrent->taPatt[2].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[2].wMPIndexChange;\
						*(tpMPCurrent->taPatt[3].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[3].wMPIndexChange;\
						*(tpMPCurrent->taPatt[4].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[4].wMPIndexChange;\
						*(tpMPCurrent->taPatt[5].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[5].wMPIndexChange;\
						*(tpMPCurrent->taPatt[6].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[6].wMPIndexChange;\
						break;\
					}\
					case 8 : {\
						*(tpMPCurrent->taPatt[0].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[0].wMPIndexChange;\
						*(tpMPCurrent->taPatt[1].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[1].wMPIndexChange;\
						*(tpMPCurrent->taPatt[2].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[2].wMPIndexChange;\
						*(tpMPCurrent->taPatt[3].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[3].wMPIndexChange;\
						*(tpMPCurrent->taPatt[4].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[4].wMPIndexChange;\
						*(tpMPCurrent->taPatt[5].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[5].wMPIndexChange;\
						*(tpMPCurrent->taPatt[6].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[6].wMPIndexChange;\
						*(tpMPCurrent->taPatt[7].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[7].wMPIndexChange;\
						break;\
					}\
				}\
				tpMPCurrent -= iIncrement;\
				tpCurrent -= iIncrement;\
				tpHashCurrent -= iHashIncrement;\
                *cpCurrentValue += 2;\
            } \
			while (tpCurrent != tpSquare);\
			\
        }\
    }\
\
}

bool bfMakeMoveMobilityIfAvailableFast(TBoardCell tMoveColor, int iIndex, char *cpCurrentValue, u32 *dwpFlipCount)
{
	TIMER_START(MakeMoveMobilityIfAvailableFast_FALSE)
    u8 ucMask = ucaFlippingDirections[iIndex];

	TBoardCell *tpSquare  = tGlobalSearchParameters.taBoard + iIndex;
	TBoardCell *tpCurrent = tpSquare;
	TMPInfo    *tpMPSquare  = taMPStructures + iIndex;
	TMPInfo    *tpMPCurrent = taMPStructures + iIndex;
    const SHashBoard *tpHSquare = taHashBoard + HASH_VALUES*iIndex;
	const SHashBoard *tpHashCurrent = tpHSquare;
	char cOldValue = *cpCurrentValue;

	if (tMoveColor == BLACK) {
		switch(ucMask) {
			case 1 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 2 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 3 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 4 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 5 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 6 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 7 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 8 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 9 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				break;
			}
		}
	
		*dwpFlipCount = (*cpCurrentValue - cOldValue) >> 1;
    
		if (*dwpFlipCount) {
        
			*tpSquare = BLACK;
			tGlobalSearchParameters.tHashValue ^= *(tpHSquare + BLACK_EMPTY);
			*(tppFlipStack++) = tpSquare;
			++(*cpCurrentValue);
			++(*dwpFlipCount);
        
			switch (tpMPSquare->ucCount) {
				case 4 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					break;
				}
				case 5 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					break;
				}
				case 6 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					*(tpMPSquare->taPatt[5].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[5].wMPIndexChange;
					break;
				}
				case 7 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					*(tpMPSquare->taPatt[5].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[5].wMPIndexChange;
					*(tpMPSquare->taPatt[6].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[6].wMPIndexChange;
					break;
				}
				case 8 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					*(tpMPSquare->taPatt[5].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[5].wMPIndexChange;
					*(tpMPSquare->taPatt[6].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[6].wMPIndexChange;
					*(tpMPSquare->taPatt[7].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[7].wMPIndexChange;
					break;
				}
			}

			TIMER_STOP_PREVIOUS(MakeMoveMobilityIfAvailableFast_TRUE,MakeMoveMobilityIfAvailableFast_FALSE)
			return(true);

		}
		else {
			TIMER_STOP(MakeMoveMobilityIfAvailableFast_FALSE)
			return(false);
		}
	
	}
	else {
		switch(ucMask) {
			case 1 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 2 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 3 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 4 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 5 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 6 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 7 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 8 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 9 : {
				ucfChangeAllIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfChangeAllIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				break;
			}
		}
		
		*dwpFlipCount = (*cpCurrentValue - cOldValue) >> 1;
    
		if (*dwpFlipCount) {
        
			*tpSquare = WHITE;
			tGlobalSearchParameters.tHashValue ^= *(tpHSquare + WHITE_EMPTY);
			*(tppFlipStack++) = tpSquare;
			++(*cpCurrentValue);
			++(*dwpFlipCount);

			switch (tpMPSquare->ucCount) {
				case 4 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					break;
				}
				case 5 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					break;
				}
				case 6 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					*(tpMPSquare->taPatt[5].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[5].wMPIndexChange;
					break;
				}
				case 7 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					*(tpMPSquare->taPatt[5].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[5].wMPIndexChange;
					*(tpMPSquare->taPatt[6].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[6].wMPIndexChange;
					break;
				}
				case 8 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					*(tpMPSquare->taPatt[5].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[5].wMPIndexChange;
					*(tpMPSquare->taPatt[6].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[6].wMPIndexChange;
					*(tpMPSquare->taPatt[7].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[7].wMPIndexChange;
					break;
				}
			}

			TIMER_STOP_PREVIOUS(MakeMoveMobilityIfAvailableFast_TRUE,MakeMoveMobilityIfAvailableFast_FALSE)
			return(true);

		}
		else {
			TIMER_STOP(MakeMoveMobilityIfAvailableFast_FALSE)
			return(false);
		}
	
	}
}


#define ucfChangeEFIndexesDirectionIfAvailable(tpSquare, iIncrement, tMoveColor, tOpponentColor) \
{\
    tpCurrent = tpSquare + iIncrement;\
	if (*tpCurrent == tOpponentColor) {\
		tpCurrent += iIncrement;\
		if (*tpCurrent == tOpponentColor) {\
			tpCurrent += iIncrement;\
			if (*tpCurrent == tOpponentColor) {\
				tpCurrent += iIncrement;\
				if (*tpCurrent == tOpponentColor) {\
					tpCurrent += iIncrement;\
					if (*tpCurrent == tOpponentColor) {\
						tpCurrent += iIncrement;\
						if (*tpCurrent == tOpponentColor) {\
                            tpCurrent += iIncrement;\
                        }\
                    }\
                }\
            }\
        }\
		if (*tpCurrent==tMoveColor) {\
            \
			tpMPCurrent += tpCurrent - tpSquare - iIncrement;\
			do {\
				switch (tpMPCurrent->ucCount) {\
					case 4 : {\
						*(tpMPCurrent->taPatt[0].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[0].wMPIndexChange;\
						*(tpMPCurrent->taPatt[1].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[1].wMPIndexChange;\
						*(tpMPCurrent->taPatt[2].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[2].wMPIndexChange;\
						*(tpMPCurrent->taPatt[3].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[3].wMPIndexChange;\
						break;\
					}\
					case 5 : {\
						*(tpMPCurrent->taPatt[0].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[0].wMPIndexChange;\
						*(tpMPCurrent->taPatt[1].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[1].wMPIndexChange;\
						*(tpMPCurrent->taPatt[2].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[2].wMPIndexChange;\
						*(tpMPCurrent->taPatt[3].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[3].wMPIndexChange;\
						*(tpMPCurrent->taPatt[4].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[4].wMPIndexChange;\
						break;\
					}\
					case 6 : {\
						*(tpMPCurrent->taPatt[0].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[0].wMPIndexChange;\
						*(tpMPCurrent->taPatt[1].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[1].wMPIndexChange;\
						*(tpMPCurrent->taPatt[2].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[2].wMPIndexChange;\
						*(tpMPCurrent->taPatt[3].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[3].wMPIndexChange;\
						*(tpMPCurrent->taPatt[4].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[4].wMPIndexChange;\
						*(tpMPCurrent->taPatt[5].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[5].wMPIndexChange;\
						break;\
					}\
					case 7 : {\
						*(tpMPCurrent->taPatt[0].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[0].wMPIndexChange;\
						*(tpMPCurrent->taPatt[1].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[1].wMPIndexChange;\
						*(tpMPCurrent->taPatt[2].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[2].wMPIndexChange;\
						*(tpMPCurrent->taPatt[3].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[3].wMPIndexChange;\
						*(tpMPCurrent->taPatt[4].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[4].wMPIndexChange;\
						*(tpMPCurrent->taPatt[5].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[5].wMPIndexChange;\
						*(tpMPCurrent->taPatt[6].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[6].wMPIndexChange;\
						break;\
					}\
					case 8 : {\
						*(tpMPCurrent->taPatt[0].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[0].wMPIndexChange;\
						*(tpMPCurrent->taPatt[1].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[1].wMPIndexChange;\
						*(tpMPCurrent->taPatt[2].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[2].wMPIndexChange;\
						*(tpMPCurrent->taPatt[3].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[3].wMPIndexChange;\
						*(tpMPCurrent->taPatt[4].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[4].wMPIndexChange;\
						*(tpMPCurrent->taPatt[5].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[5].wMPIndexChange;\
						*(tpMPCurrent->taPatt[6].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[6].wMPIndexChange;\
						*(tpMPCurrent->taPatt[7].wpMPIndex) += (tMoveColor - tOpponentColor) * tpMPCurrent->taPatt[7].wMPIndexChange;\
						break;\
					}\
				}\
				tpMPCurrent -= iIncrement;\
            } \
			while (tpMPCurrent != tpMPSquare);\
			\
			bChanged = true;\
        }\
    }\
\
}

bool bfChangeEFIndexesIfAvailableFast(TBoardCell tMoveColor, int iIndex)
{
	TIMER_START(ChangeEFIndexesIfAvailableFast_FALSE)
    u8 ucMask = ucaFlippingDirections[iIndex];

	TBoardCell *tpSquare  = tGlobalSearchParameters.taBoard + iIndex;
	TBoardCell *tpCurrent = tpSquare;
	TMPInfo    *tpMPSquare  = taMPStructures + iIndex;
	TMPInfo    *tpMPCurrent = taMPStructures + iIndex;
	bool bChanged = false;

	if (tMoveColor == BLACK) {
		switch(ucMask) {
			case 1 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  1, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  9, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, 10, BLACK, WHITE);
				break;
			}
			case 2 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -1, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  1, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  8, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  9, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, 10, BLACK, WHITE);
				break;
			}
			case 3 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -1, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  8, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  9, BLACK, WHITE);
				break;
			}
			case 4 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -9, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -8, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  1, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  9, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, 10, BLACK, WHITE);
				break;
			}
			case 5 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,-10, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -9, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -8, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -1, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  1, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  8, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  9, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, 10, BLACK, WHITE);
				break;
			}
			case 6 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,-10, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -9, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -1, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  8, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  9, BLACK, WHITE);
				break;
			}
			case 7 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -9, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -8, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  1, BLACK, WHITE);
				break;
			}
			case 8 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,-10, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -9, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -8, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -1, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  1, BLACK, WHITE);
				break;
			}
			case 9 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,-10, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -9, BLACK, WHITE);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -1, BLACK, WHITE);
				break;
			}
		}
	
		if (bChanged) {
        
			//tGlobalSearchParameters.tHashValue ^= *(tpHSquare + BLACK_EMPTY);
        
			switch (tpMPSquare->ucCount) {
				case 4 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					break;
				}
				case 5 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					break;
				}
				case 6 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					*(tpMPSquare->taPatt[5].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[5].wMPIndexChange;
					break;
				}
				case 7 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					*(tpMPSquare->taPatt[5].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[5].wMPIndexChange;
					*(tpMPSquare->taPatt[6].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[6].wMPIndexChange;
					break;
				}
				case 8 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					*(tpMPSquare->taPatt[5].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[5].wMPIndexChange;
					*(tpMPSquare->taPatt[6].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[6].wMPIndexChange;
					*(tpMPSquare->taPatt[7].wpMPIndex) += (BLACK - EMPTY) * tpMPSquare->taPatt[7].wMPIndexChange;
					break;
				}
			}

			TIMER_STOP_PREVIOUS(ChangeEFIndexesIfAvailableFast_TRUE,ChangeEFIndexesIfAvailableFast_FALSE)
			return(true);

		}
		else {
			TIMER_STOP(ChangeEFIndexesIfAvailableFast_FALSE)
			return(false);
		}
	
	}
	else {
		switch(ucMask) {
			case 1 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  1, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  9, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, 10, WHITE, BLACK);
				break;
			}
			case 2 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -1, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  1, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  8, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  9, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, 10, WHITE, BLACK);
				break;
			}
			case 3 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -1, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  8, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  9, WHITE, BLACK);
				break;
			}
			case 4 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -9, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -8, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  1, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  9, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, 10, WHITE, BLACK);
				break;
			}
			case 5 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,-10, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -9, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -8, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -1, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  1, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  8, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  9, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, 10, WHITE, BLACK);
				break;
			}
			case 6 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,-10, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -9, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -1, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  8, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  9, WHITE, BLACK);
				break;
			}
			case 7 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -9, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -8, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  1, WHITE, BLACK);
				break;
			}
			case 8 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,-10, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -9, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -8, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -1, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,  1, WHITE, BLACK);
				break;
			}
			case 9 : {
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare,-10, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -9, WHITE, BLACK);
				ucfChangeEFIndexesDirectionIfAvailable(tpSquare, -1, WHITE, BLACK);
				break;
			}
		}
		
		if (bChanged) {
        
			//tGlobalSearchParameters.tHashValue ^= *(tpHSquare + WHITE_EMPTY);
			switch (tpMPSquare->ucCount) {
				case 4 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					break;
				}
				case 5 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					break;
				}
				case 6 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					*(tpMPSquare->taPatt[5].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[5].wMPIndexChange;
					break;
				}
				case 7 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					*(tpMPSquare->taPatt[5].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[5].wMPIndexChange;
					*(tpMPSquare->taPatt[6].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[6].wMPIndexChange;
					break;
				}
				case 8 : {
					*(tpMPSquare->taPatt[0].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[0].wMPIndexChange;
					*(tpMPSquare->taPatt[1].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[1].wMPIndexChange;
					*(tpMPSquare->taPatt[2].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[2].wMPIndexChange;
					*(tpMPSquare->taPatt[3].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[3].wMPIndexChange;
					*(tpMPSquare->taPatt[4].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[4].wMPIndexChange;
					*(tpMPSquare->taPatt[5].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[5].wMPIndexChange;
					*(tpMPSquare->taPatt[6].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[6].wMPIndexChange;
					*(tpMPSquare->taPatt[7].wpMPIndex) += (WHITE - EMPTY) * tpMPSquare->taPatt[7].wMPIndexChange;
					break;
				}
			}

			TIMER_STOP_PREVIOUS(ChangeEFIndexesIfAvailableFast_TRUE,ChangeEFIndexesIfAvailableFast_FALSE)
			return(true);

		}
		else {
			TIMER_STOP(ChangeEFIndexesIfAvailableFast_FALSE)
			return(false);
		}
	
	}
}

#define ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, iIncrement, iHashIncrement, tMoveColor, tOpponentColor) \
{\
    tpCurrent = tpSquare + iIncrement;\
	if (*tpCurrent == tOpponentColor) {\
		tpCurrent += iIncrement;\
		if (*tpCurrent == tOpponentColor) {\
			tpCurrent += iIncrement;\
			if (*tpCurrent == tOpponentColor) {\
				tpCurrent += iIncrement;\
				if (*tpCurrent == tOpponentColor) {\
					tpCurrent += iIncrement;\
					if (*tpCurrent == tOpponentColor) {\
						tpCurrent += iIncrement;\
						if (*tpCurrent == tOpponentColor) {\
                            tpCurrent += iIncrement;\
                        }\
                    }\
                }\
            }\
        }\
		if (*tpCurrent==tMoveColor) {\
            tpHashCurrent += (tpCurrent - iIncrement - tpSquare)*HASH_VALUES;\
			do {\
                tGlobalSearchParameters.tHashValue ^= *(tpHashCurrent + BLACK_WHITE);\
				tpHashCurrent -= iHashIncrement;\
            } \
			while (tpHashCurrent != tpHSquare);\
			\
        }\
    }\
\
}

bool bfChangeHashIfAvailable(TBoardCell tMoveColor,int iIndex)
{
	TIMER_START(ChangeHashIfAvailable_FALSE)
    if (tGlobalSearchParameters.taBoard[iIndex]!=EMPTY) 
		return(false);

    TBoardCell tOpponentColor = (tMoveColor == BLACK ? WHITE : BLACK);
    u8 ucMask = ucaFlippingDirections[iIndex];
	SHashValue tHashValue = tGlobalSearchParameters.tHashValue;

	TBoardCell *tpSquare  = tGlobalSearchParameters.taBoard + iIndex;
    const SHashBoard *tpHSquare = taHashBoard + HASH_VALUES*iIndex;
	TBoardCell *tpCurrent = tpSquare;
	const SHashBoard *tpHashCurrent = tpHSquare;

    if (tMoveColor == BLACK) {
		switch(ucMask) {
			case 1 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 2 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 3 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 4 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 5 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 6 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 7 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 8 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, BLACK, WHITE);
				break;
			}
			case 9 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, BLACK, WHITE);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, BLACK, WHITE);
				break;
			}
		}
	
		if (tGlobalSearchParameters.tHashValue != tHashValue) {
			tGlobalSearchParameters.tHashValue ^= *(tpHSquare + BLACK_EMPTY);
			TIMER_STOP_PREVIOUS(ChangeHashIfAvailable_TRUE,ChangeHashIfAvailable_FALSE)
			return(true);
		}
		else {
			TIMER_STOP(ChangeHashIfAvailable_FALSE)
			return(false);
		}
	
	}
	else {
		switch(ucMask) {
			case 1 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 2 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 3 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 4 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 5 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, 10, 10*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 6 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  8,  8*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  9,  9*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 7 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 8 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -8, -8*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,  1,  1*HASH_VALUES, WHITE, BLACK);
				break;
			}
			case 9 : {
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare,-10,-10*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -9, -9*HASH_VALUES, WHITE, BLACK);
				ucfChangeDirectionIfAvailable(tpSquare, tpHSquare, -1, -1*HASH_VALUES, WHITE, BLACK);
				break;
			}
		}
		
		if (tGlobalSearchParameters.tHashValue != tHashValue) {
			tGlobalSearchParameters.tHashValue ^= *(tpHSquare + WHITE_EMPTY);
			TIMER_STOP_PREVIOUS(ChangeHashIfAvailable_TRUE,ChangeHashIfAvailable_FALSE)
			return(true);
		}
		else {
			TIMER_STOP(ChangeHashIfAvailable_FALSE)
			return(false);
		}
	
	}
}

#define ucfCountDirectionIfAvailable(tpSquare,iIncrement,tMoveColor,tOpponentColor) \
{\
	if (tpCurrent[1*iIncrement] == tOpponentColor) {\
        if (tpCurrent[2*iIncrement] == tOpponentColor) {\
            if (tpCurrent[3*iIncrement] == tOpponentColor) {\
                if (tpCurrent[4*iIncrement] == tOpponentColor) {\
                    if (tpCurrent[5*iIncrement] == tOpponentColor) {\
						if (tpCurrent[6*iIncrement] == tOpponentColor) {\
							if (tpCurrent[7*iIncrement] == tMoveColor) {\
								*cpCurrentValue += 12;\
							}\
						}\
						else {\
							if (tpCurrent[6*iIncrement] == tMoveColor)\
								*cpCurrentValue += 10;\
						}\
					}\
					else {\
						if (tpCurrent[5*iIncrement] == tMoveColor)\
							*cpCurrentValue += 8;\
					}\
				}\
				else {\
					if (tpCurrent[4*iIncrement] == tMoveColor)\
						*cpCurrentValue += 6;\
				}\
			}\
			else {\
				if (tpCurrent[3*iIncrement] == tMoveColor)\
					*cpCurrentValue += 4;\
			}\
		}\
		else {\
			if (tpCurrent[2*iIncrement] == tMoveColor)\
				*cpCurrentValue += 2;\
		}\
    }\
}

bool bfCountIfAvailable(TBoardCell tMoveColor, int iIndex, char *cpCurrentValue)
{
    if (tGlobalSearchParameters.taBoard[iIndex]!=EMPTY) {
		return(false);
	}

    TBoardCell tOpponentColor = (tMoveColor == BLACK ? WHITE : BLACK);
    u8 ucMask = ucaFlippingDirections[iIndex];
	char OldValue = *cpCurrentValue;
    TBoardCell *tpSquare = tGlobalSearchParameters.taBoard+iIndex;
	TBoardCell *tpCurrent = tpSquare;
    
    if (tMoveColor == BLACK) {
		switch(ucMask) {
			case 1 : {
				ucfCountDirectionIfAvailable(tpSquare,   1, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   9, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  10, BLACK, WHITE);
				break;
			}
			case 2 : {
				ucfCountDirectionIfAvailable(tpSquare,  -1, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   1, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   8, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   9, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  10, BLACK, WHITE);
				break;
			}
			case 3 : {
				ucfCountDirectionIfAvailable(tpSquare,  -1, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   8, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   9, BLACK, WHITE);
				break;
			}
			case 4 : {
				ucfCountDirectionIfAvailable(tpSquare,  -9, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  -8, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   1, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   9, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  10, BLACK, WHITE);
				break;
			}
			case 5 : {
				ucfCountDirectionIfAvailable(tpSquare, -10, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  -9, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  -8, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  -1, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   1, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   8, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   9, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  10, BLACK, WHITE);
				break;
			}
			case 6 : {
				ucfCountDirectionIfAvailable(tpSquare, -10, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  -9, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  -1, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   8, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   9, BLACK, WHITE);
				break;
			}
			case 7 : {
				ucfCountDirectionIfAvailable(tpSquare,  -9, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  -8, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   1, BLACK, WHITE);
				break;
			}
			case 8 : {
				ucfCountDirectionIfAvailable(tpSquare, -10, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  -9, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  -8, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  -1, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,   1, BLACK, WHITE);
				break;
			}
			case 9 : {
				ucfCountDirectionIfAvailable(tpSquare, -10, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  -9, BLACK, WHITE);
				ucfCountDirectionIfAvailable(tpSquare,  -1, BLACK, WHITE);
				break;
			}
		}
	}
	else {
		switch(ucMask) {
			case 1 : {
				ucfCountDirectionIfAvailable(tpSquare,   1, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   9, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  10, WHITE, BLACK);
				break;
			}
			case 2 : {
				ucfCountDirectionIfAvailable(tpSquare,  -1, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   1, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   8, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   9, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  10, WHITE, BLACK);
				break;
			}
			case 3 : {
				ucfCountDirectionIfAvailable(tpSquare,  -1, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   8, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   9, WHITE, BLACK);
				break;
			}
			case 4 : {
				ucfCountDirectionIfAvailable(tpSquare,  -9, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  -8, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   1, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   9, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  10, WHITE, BLACK);
				break;
			}
			case 5 : {
				ucfCountDirectionIfAvailable(tpSquare, -10, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  -9, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  -8, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  -1, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   1, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   8, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   9, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  10, WHITE, BLACK);
				break;
			}
			case 6 : {
				ucfCountDirectionIfAvailable(tpSquare, -10, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  -9, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  -1, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   8, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   9, WHITE, BLACK);
				break;
			}
			case 7 : {
				ucfCountDirectionIfAvailable(tpSquare,  -9, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  -8, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   1, WHITE, BLACK);
				break;
			}
			case 8 : {
				ucfCountDirectionIfAvailable(tpSquare, -10, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  -9, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  -8, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  -1, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,   1, WHITE, BLACK);
				break;
			}
			case 9 : {
				ucfCountDirectionIfAvailable(tpSquare, -10, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  -9, WHITE, BLACK);
				ucfCountDirectionIfAvailable(tpSquare,  -1, WHITE, BLACK);
				break;
			}
		}
	}

    if (*cpCurrentValue > OldValue) {
        ++(*cpCurrentValue);
        return(true);
    }
    else 
		return(false);
}

/**/
void vfUndo(u32 dwFlipCount, TBoardCell tOpponentColor)
{
	TIMER_START(Undo)
	dwFlipCount--;
	*(*(--tppFlipStack)) = EMPTY;

	__ASSERT(tppFlipStack - tpaGlobalFlipStack > 0);
	__ASSERT(tppFlipStack - tpaGlobalFlipStack < 65536);

	while (dwFlipCount) {
		dwFlipCount--;
        *(*(--tppFlipStack)) = tOpponentColor;
    }
	TIMER_STOP(Undo)
}

void vfUndoAndRestore(u32 dwFlipCount, TBoardCell tOpponentColor)
{
	TIMER_START(Undo);
	if (tOpponentColor == BLACK) {
		TMPInfo    *tpMPCurrent = &(taMPStructures[*(tppFlipStack - 1) - tGlobalSearchParameters.taBoard]);
		dwFlipCount--;
		*(*(--tppFlipStack)) = EMPTY;

		switch (tpMPCurrent->ucCount) {
			case 4 : {
				*(tpMPCurrent->taPatt[0].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[0].wMPIndexChange;
				*(tpMPCurrent->taPatt[1].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[1].wMPIndexChange;
				*(tpMPCurrent->taPatt[2].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[2].wMPIndexChange;
				*(tpMPCurrent->taPatt[3].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[3].wMPIndexChange;
				break;
			}
			case 5 : {
				*(tpMPCurrent->taPatt[0].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[0].wMPIndexChange;
				*(tpMPCurrent->taPatt[1].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[1].wMPIndexChange;
				*(tpMPCurrent->taPatt[2].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[2].wMPIndexChange;
				*(tpMPCurrent->taPatt[3].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[3].wMPIndexChange;
				*(tpMPCurrent->taPatt[4].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[4].wMPIndexChange;
				break;
			}
			case 6 : {
				*(tpMPCurrent->taPatt[0].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[0].wMPIndexChange;
				*(tpMPCurrent->taPatt[1].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[1].wMPIndexChange;
				*(tpMPCurrent->taPatt[2].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[2].wMPIndexChange;
				*(tpMPCurrent->taPatt[3].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[3].wMPIndexChange;
				*(tpMPCurrent->taPatt[4].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[4].wMPIndexChange;
				*(tpMPCurrent->taPatt[5].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[5].wMPIndexChange;
				break;
			}
			case 7 : {
				*(tpMPCurrent->taPatt[0].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[0].wMPIndexChange;
				*(tpMPCurrent->taPatt[1].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[1].wMPIndexChange;
				*(tpMPCurrent->taPatt[2].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[2].wMPIndexChange;
				*(tpMPCurrent->taPatt[3].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[3].wMPIndexChange;
				*(tpMPCurrent->taPatt[4].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[4].wMPIndexChange;
				*(tpMPCurrent->taPatt[5].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[5].wMPIndexChange;
				*(tpMPCurrent->taPatt[6].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[6].wMPIndexChange;
				break;
			}
			case 8 : {
				*(tpMPCurrent->taPatt[0].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[0].wMPIndexChange;
				*(tpMPCurrent->taPatt[1].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[1].wMPIndexChange;
				*(tpMPCurrent->taPatt[2].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[2].wMPIndexChange;
				*(tpMPCurrent->taPatt[3].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[3].wMPIndexChange;
				*(tpMPCurrent->taPatt[4].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[4].wMPIndexChange;
				*(tpMPCurrent->taPatt[5].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[5].wMPIndexChange;
				*(tpMPCurrent->taPatt[6].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[6].wMPIndexChange;
				*(tpMPCurrent->taPatt[7].wpMPIndex) += (EMPTY - WHITE) * tpMPCurrent->taPatt[7].wMPIndexChange;
				break;
			}
		}

		while (dwFlipCount) {
			tpMPCurrent = &(taMPStructures[*(tppFlipStack - 1) - tGlobalSearchParameters.taBoard]);
			dwFlipCount--;
			*(*(--tppFlipStack)) = BLACK;
			switch (tpMPCurrent->ucCount) {
				case 4 : {
					*(tpMPCurrent->taPatt[0].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[0].wMPIndexChange;
					*(tpMPCurrent->taPatt[1].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[1].wMPIndexChange;
					*(tpMPCurrent->taPatt[2].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[2].wMPIndexChange;
					*(tpMPCurrent->taPatt[3].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[3].wMPIndexChange;
					break;
				}
				case 5 : {
					*(tpMPCurrent->taPatt[0].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[0].wMPIndexChange;
					*(tpMPCurrent->taPatt[1].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[1].wMPIndexChange;
					*(tpMPCurrent->taPatt[2].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[2].wMPIndexChange;
					*(tpMPCurrent->taPatt[3].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[3].wMPIndexChange;
					*(tpMPCurrent->taPatt[4].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[4].wMPIndexChange;
					break;
				}
				case 6 : {
					*(tpMPCurrent->taPatt[0].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[0].wMPIndexChange;
					*(tpMPCurrent->taPatt[1].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[1].wMPIndexChange;
					*(tpMPCurrent->taPatt[2].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[2].wMPIndexChange;
					*(tpMPCurrent->taPatt[3].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[3].wMPIndexChange;
					*(tpMPCurrent->taPatt[4].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[4].wMPIndexChange;
					*(tpMPCurrent->taPatt[5].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[5].wMPIndexChange;
					break;
				}
				case 7 : {
					*(tpMPCurrent->taPatt[0].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[0].wMPIndexChange;
					*(tpMPCurrent->taPatt[1].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[1].wMPIndexChange;
					*(tpMPCurrent->taPatt[2].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[2].wMPIndexChange;
					*(tpMPCurrent->taPatt[3].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[3].wMPIndexChange;
					*(tpMPCurrent->taPatt[4].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[4].wMPIndexChange;
					*(tpMPCurrent->taPatt[5].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[5].wMPIndexChange;
					*(tpMPCurrent->taPatt[6].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[6].wMPIndexChange;
					break;
				}
				case 8 : {
					*(tpMPCurrent->taPatt[0].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[0].wMPIndexChange;
					*(tpMPCurrent->taPatt[1].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[1].wMPIndexChange;
					*(tpMPCurrent->taPatt[2].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[2].wMPIndexChange;
					*(tpMPCurrent->taPatt[3].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[3].wMPIndexChange;
					*(tpMPCurrent->taPatt[4].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[4].wMPIndexChange;
					*(tpMPCurrent->taPatt[5].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[5].wMPIndexChange;
					*(tpMPCurrent->taPatt[6].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[6].wMPIndexChange;
					*(tpMPCurrent->taPatt[7].wpMPIndex) += (BLACK - WHITE) * tpMPCurrent->taPatt[7].wMPIndexChange;
					break;
				}
			}
		}
	}
	else {
		TMPInfo    *tpMPCurrent = &(taMPStructures[*(tppFlipStack - 1) - tGlobalSearchParameters.taBoard]);
		dwFlipCount--;
		*(*(--tppFlipStack)) = EMPTY;
		switch (tpMPCurrent->ucCount) {
			case 4 : {
				*(tpMPCurrent->taPatt[0].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[0].wMPIndexChange;
				*(tpMPCurrent->taPatt[1].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[1].wMPIndexChange;
				*(tpMPCurrent->taPatt[2].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[2].wMPIndexChange;
				*(tpMPCurrent->taPatt[3].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[3].wMPIndexChange;
				break;
			}
			case 5 : {
				*(tpMPCurrent->taPatt[0].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[0].wMPIndexChange;
				*(tpMPCurrent->taPatt[1].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[1].wMPIndexChange;
				*(tpMPCurrent->taPatt[2].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[2].wMPIndexChange;
				*(tpMPCurrent->taPatt[3].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[3].wMPIndexChange;
				*(tpMPCurrent->taPatt[4].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[4].wMPIndexChange;
				break;
			}
			case 6 : {
				*(tpMPCurrent->taPatt[0].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[0].wMPIndexChange;
				*(tpMPCurrent->taPatt[1].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[1].wMPIndexChange;
				*(tpMPCurrent->taPatt[2].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[2].wMPIndexChange;
				*(tpMPCurrent->taPatt[3].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[3].wMPIndexChange;
				*(tpMPCurrent->taPatt[4].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[4].wMPIndexChange;
				*(tpMPCurrent->taPatt[5].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[5].wMPIndexChange;
				break;
			}
			case 7 : {
				*(tpMPCurrent->taPatt[0].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[0].wMPIndexChange;
				*(tpMPCurrent->taPatt[1].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[1].wMPIndexChange;
				*(tpMPCurrent->taPatt[2].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[2].wMPIndexChange;
				*(tpMPCurrent->taPatt[3].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[3].wMPIndexChange;
				*(tpMPCurrent->taPatt[4].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[4].wMPIndexChange;
				*(tpMPCurrent->taPatt[5].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[5].wMPIndexChange;
				*(tpMPCurrent->taPatt[6].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[6].wMPIndexChange;
				break;
			}
			case 8 : {
				*(tpMPCurrent->taPatt[0].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[0].wMPIndexChange;
				*(tpMPCurrent->taPatt[1].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[1].wMPIndexChange;
				*(tpMPCurrent->taPatt[2].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[2].wMPIndexChange;
				*(tpMPCurrent->taPatt[3].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[3].wMPIndexChange;
				*(tpMPCurrent->taPatt[4].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[4].wMPIndexChange;
				*(tpMPCurrent->taPatt[5].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[5].wMPIndexChange;
				*(tpMPCurrent->taPatt[6].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[6].wMPIndexChange;
				*(tpMPCurrent->taPatt[7].wpMPIndex) += (EMPTY - BLACK) * tpMPCurrent->taPatt[7].wMPIndexChange;
				break;
			}
		}

		while (dwFlipCount) {
			tpMPCurrent = &(taMPStructures[*(tppFlipStack - 1) - tGlobalSearchParameters.taBoard]);
			dwFlipCount--;
			*(*(--tppFlipStack)) = WHITE;
			switch (tpMPCurrent->ucCount) {
				case 4 : {
					*(tpMPCurrent->taPatt[0].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[0].wMPIndexChange;
					*(tpMPCurrent->taPatt[1].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[1].wMPIndexChange;
					*(tpMPCurrent->taPatt[2].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[2].wMPIndexChange;
					*(tpMPCurrent->taPatt[3].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[3].wMPIndexChange;
					break;
				}
				case 5 : {
					*(tpMPCurrent->taPatt[0].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[0].wMPIndexChange;
					*(tpMPCurrent->taPatt[1].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[1].wMPIndexChange;
					*(tpMPCurrent->taPatt[2].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[2].wMPIndexChange;
					*(tpMPCurrent->taPatt[3].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[3].wMPIndexChange;
					*(tpMPCurrent->taPatt[4].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[4].wMPIndexChange;
					break;
				}
				case 6 : {
					*(tpMPCurrent->taPatt[0].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[0].wMPIndexChange;
					*(tpMPCurrent->taPatt[1].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[1].wMPIndexChange;
					*(tpMPCurrent->taPatt[2].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[2].wMPIndexChange;
					*(tpMPCurrent->taPatt[3].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[3].wMPIndexChange;
					*(tpMPCurrent->taPatt[4].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[4].wMPIndexChange;
					*(tpMPCurrent->taPatt[5].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[5].wMPIndexChange;
					break;
				}
				case 7 : {
					*(tpMPCurrent->taPatt[0].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[0].wMPIndexChange;
					*(tpMPCurrent->taPatt[1].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[1].wMPIndexChange;
					*(tpMPCurrent->taPatt[2].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[2].wMPIndexChange;
					*(tpMPCurrent->taPatt[3].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[3].wMPIndexChange;
					*(tpMPCurrent->taPatt[4].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[4].wMPIndexChange;
					*(tpMPCurrent->taPatt[5].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[5].wMPIndexChange;
					*(tpMPCurrent->taPatt[6].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[6].wMPIndexChange;
					break;
				}
				case 8 : {
					*(tpMPCurrent->taPatt[0].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[0].wMPIndexChange;
					*(tpMPCurrent->taPatt[1].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[1].wMPIndexChange;
					*(tpMPCurrent->taPatt[2].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[2].wMPIndexChange;
					*(tpMPCurrent->taPatt[3].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[3].wMPIndexChange;
					*(tpMPCurrent->taPatt[4].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[4].wMPIndexChange;
					*(tpMPCurrent->taPatt[5].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[5].wMPIndexChange;
					*(tpMPCurrent->taPatt[6].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[6].wMPIndexChange;
					*(tpMPCurrent->taPatt[7].wpMPIndex) += (WHITE - BLACK) * tpMPCurrent->taPatt[7].wMPIndexChange;
					break;
				}
			}
		}
	}

	TIMER_STOP(Undo)
}
/**/

bool bfCheckIfAnyAvailable(TBoardCell tMoveColor)
{
	for (int iIndex=0; iIndex<BOARD_SIZE; iIndex++)
		if (bfCheckIfAvailable(tMoveColor,iIndex))
			return(true);
	return(false);
}

#define ucfFlipDirectionIfAvailableX(tpSquare, iIncrement, tMoveColor, tOpponentColor) \
{\
    tpCurrent = tpSquare + iIncrement;\
    \
	if (*tpCurrent == tOpponentColor) {\
        \
		tpCurrent += iIncrement;\
        \
		if (*tpCurrent == tOpponentColor) {\
            \
			tpCurrent += iIncrement;\
        \
			if (*tpCurrent == tOpponentColor) {\
                \
				tpCurrent += iIncrement;\
            \
				if (*tpCurrent == tOpponentColor) {\
                    \
					tpCurrent += iIncrement;\
                \
					if (*tpCurrent == tOpponentColor) {\
                        \
						tpCurrent += iIncrement;\
                    \
						if (*tpCurrent == tOpponentColor) {\
                            tpCurrent += iIncrement;\
                        }\
                    }\
                }\
            }\
        }\
		if (*tpCurrent==tMoveColor) {\
            \
			tpCurrent -= iIncrement;\
			do {\
                *tpCurrent = tMoveColor;\
                tpCurrent -= iIncrement;\
                *cpCurrentValue += 2;\
            } \
			while (tpCurrent != tpSquare);\
			\
        }\
    }\
\
}

bool bfMakeMove(TBoardCell *taBoard, TBoardCell tMoveColor, TBoardCell tIndex, char *cpCurrentValue)
{
    TBoardCell tOpponentColor = (tMoveColor == BLACK ? WHITE : BLACK);
    u8 ucMask = ucaFlippingDirections[tIndex];
	char cOldValue = *cpCurrentValue;

	TBoardCell *tpSquare  = taBoard + tIndex;
	TBoardCell *tpCurrent = tpSquare;

    if (tMoveColor == BLACK) {
		switch(ucMask) {
			case 1 : {
				ucfFlipDirectionIfAvailableX(tpSquare,  1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, 10, BLACK, WHITE);
				break;
			}
			case 2 : {
				ucfFlipDirectionIfAvailableX(tpSquare, -1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, 10, BLACK, WHITE);
				break;
			}
			case 3 : {
				ucfFlipDirectionIfAvailableX(tpSquare, -1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  9, BLACK, WHITE);
				break;
			}
			case 4 : {
				ucfFlipDirectionIfAvailableX(tpSquare, -9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, -8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, 10, BLACK, WHITE);
				break;
			}
			case 5 : {
				ucfFlipDirectionIfAvailableX(tpSquare,-10, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, -9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, -8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, -1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, 10, BLACK, WHITE);
				break;
			}
			case 6 : {
				ucfFlipDirectionIfAvailableX(tpSquare,-10, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, -9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, -1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  9, BLACK, WHITE);
				break;
			}
			case 7 : {
				ucfFlipDirectionIfAvailableX(tpSquare, -9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, -8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  1, BLACK, WHITE);
				break;
			}
			case 8 : {
				ucfFlipDirectionIfAvailableX(tpSquare,-10, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, -9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, -8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, -1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare,  1, BLACK, WHITE);
				break;
			}
			case 9 : {
				ucfFlipDirectionIfAvailableX(tpSquare,-10, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, -9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX(tpSquare, -1, BLACK, WHITE);
				break;
			}
		}
	
		if (*cpCurrentValue - cOldValue) {
        
			*tpSquare = BLACK;
			++(*cpCurrentValue);
			return(true);

		}
		else 
			return(false);
	
	}
	else {
		switch(ucMask) {
			case 1 : {
				ucfFlipDirectionIfAvailableX(tpSquare,  1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, 10, WHITE, BLACK);
				break;
			}
			case 2 : {
				ucfFlipDirectionIfAvailableX(tpSquare, -1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, 10, WHITE, BLACK);
				break;
			}
			case 3 : {
				ucfFlipDirectionIfAvailableX(tpSquare, -1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  9, WHITE, BLACK);
				break;
			}
			case 4 : {
				ucfFlipDirectionIfAvailableX(tpSquare, -9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, -8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, 10, WHITE, BLACK);
				break;
			}
			case 5 : {
				ucfFlipDirectionIfAvailableX(tpSquare,-10, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, -9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, -8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, -1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, 10, WHITE, BLACK);
				break;
			}
			case 6 : {
				ucfFlipDirectionIfAvailableX(tpSquare,-10, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, -9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, -1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  9, WHITE, BLACK);
				break;
			}
			case 7 : {
				ucfFlipDirectionIfAvailableX(tpSquare, -9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, -8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  1, WHITE, BLACK);
				break;
			}
			case 8 : {
				ucfFlipDirectionIfAvailableX(tpSquare,-10, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, -9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, -8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, -1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare,  1, WHITE, BLACK);
				break;
			}
			case 9 : {
				ucfFlipDirectionIfAvailableX(tpSquare,-10, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, -9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX(tpSquare, -1, WHITE, BLACK);
				break;
			}
		}
		
		if (*cpCurrentValue - cOldValue) {
        
			*tpSquare = WHITE;
			++(*cpCurrentValue);
			return(true);

		}
		else 
			return(false);
	}
}

void vfInitBoard(TBoardCell *taBoard)
{
	memset(taBoard,DUMMY,BOARD_SIZE*sizeof(TBoardCell));
	
	for (int i=0; i<8; i++)
		for (int j=0; j<8; j++)
			taBoard[Index(i,j)] = EMPTY;

	taBoard[Index(3,3)] = WHITE;
	taBoard[Index(4,3)] = BLACK;
	taBoard[Index(3,4)] = BLACK;
	taBoard[Index(4,4)] = WHITE;
}

char cfShowDisk(TBoardCell tColor, char cIndex)
{
	if (tGlobalSearchParameters.taBoard[cIndex] == BLACK)
		return(BLACK_DISK_CHARACTER);
	else
		if (tGlobalSearchParameters.taBoard[cIndex] == WHITE)
			return(WHITE_DISK_CHARACTER);
		else
			if (bfCheckIfAvailable(tColor,cIndex))
				return(EMPTY_MOVE_AVAILABLE);
			else
				return(EMPTY_MOVE_UNAVAILABLE);
}

char *cafTimeToString(f64 dTime)
{
	static char caString[] = "00:00:00.000";
	caString[0] = '0' + ((int)floor(dTime)/3600) / 10;
	caString[1] = '0' + ((int)floor(dTime)/3600) % 10;
	caString[3] = '0' + (((int)floor(dTime) % 3600) / 60) / 10;
	caString[4] = '0' + (((int)floor(dTime) % 3600) / 60) % 10;
	caString[6] = '0' + ((int)floor(dTime) % 60) / 10;
	caString[7] = '0' + ((int)floor(dTime) % 60) % 10;
	caString[9] = '0' + ((int)ceil(dTime*1000) % 1000) / 100;
	caString[10] = '0' + ((int)ceil(dTime*1000) % 100) / 10;
	caString[11] = '0' + ((int)ceil(dTime*1000)) % 10;
	return(caString);
}

void vfShowBoard(TBoardCell tColor, f64 dBlackTime, f64 dWhiteTime)
{
	char cBlackDisks = 0, cWhiteDisks = 0, cBlackMoves = 0, cWhiteMoves = 0;
	
	for (int i=0; i<8; i++)
		for (int j=0; j<8; j++)
			if (tGlobalSearchParameters.taBoard[Index(i,j)] == BLACK)
				cBlackDisks++;
			else
				if (tGlobalSearchParameters.taBoard[Index(i,j)] == WHITE)
					cWhiteDisks++;
				else {
					if (bfCheckIfAvailable(BLACK,Index(i,j)))
						cBlackMoves++;
					if (bfCheckIfAvailable(WHITE,Index(i,j)))
						cWhiteMoves++;
				}

	vfDualPrintF("\n   A B C D E F G H           BLACK (%c)            A  B  C  D  E  F  G  H\n",BLACK_DISK_CHARACTER);
	for ( i=0; i<8; i++) {
		
		vfDualPrintF("%2d ",i+1);

		for (int j=0; j<8; j++)
			vfDualPrintF("%c ",cfShowDisk(tColor,Index(i,j)));

		vfDualPrintF("%d ",i+1);

		switch (i) {
			case 0  : 
				vfDualPrintF("%18s%8d ",cafTimeToString(dBlackTime), i + 1);
				break;
			case 1  : 
				vfDualPrintF(" %2d disk(s) %2d move(s)  %2d ",cBlackDisks, cBlackMoves, i + 1);
				break;
			case 3  : 
				vfDualPrintF("%9d empties %8d ",64 - (cBlackDisks + cWhiteDisks), i + 1);
				break;
			case 4  :
				vfDualPrintF("%9s's turn (%c) %5d ", (tColor == BLACK ? "Black" : "White"), (tColor == BLACK ? BLACK_DISK_CHARACTER : WHITE_DISK_CHARACTER), i + 1);
				break;
			case 6  : 
				vfDualPrintF(" %2d disk(s) %2d move(s)  %2d ",cWhiteDisks, cWhiteMoves, i + 1);
				break;
			case 7  : 
				vfDualPrintF("%18s%8d ",cafTimeToString(dWhiteTime), i + 1);
				break;
			default :
				vfDualPrintF("%26d ",i+1);
		}
		
		for ( j=0; j<8; j++)
			if (caNotationBoard[Index(i,j)])
				vfDualPrintF("|%2d", caNotationBoard[Index(i,j)]);
			else
				if ((i > 2) && (i < 5) && (j > 2) && (j < 5)) {
					if ((i + j) % 2)
						vfDualPrintF("|%s",BLACK_START_DISK_STRING);
					else
						vfDualPrintF("|%s",WHITE_START_DISK_STRING);
				}
				else
					vfDualPrintF("|  ", caNotationBoard[Index(i,j)]);

		vfDualPrintF("|%2d\n",i+1);
	}
	vfDualPrintF("   A B C D E F G H           WHITE (%c)            A  B  C  D  E  F  G  H\n\n",WHITE_DISK_CHARACTER);
}

#define ucfFlipDirectionIfAvailableX2(tpSquare, iIncrement, tMoveColor, tOpponentColor) \
{\
    tpCurrent = tpSquare + iIncrement;\
    \
	if (*tpCurrent == tOpponentColor) {\
        \
		tpCurrent += iIncrement;\
        \
		if (*tpCurrent == tOpponentColor) {\
            \
			tpCurrent += iIncrement;\
        \
			if (*tpCurrent == tOpponentColor) {\
                \
				tpCurrent += iIncrement;\
            \
				if (*tpCurrent == tOpponentColor) {\
                    \
					tpCurrent += iIncrement;\
                \
					if (*tpCurrent == tOpponentColor) {\
                        \
						tpCurrent += iIncrement;\
                    \
						if (*tpCurrent == tOpponentColor) {\
                            tpCurrent += iIncrement;\
                        }\
                    }\
                }\
            }\
        }\
		if (*tpCurrent==tMoveColor) {\
            \
			tpCurrent -= iIncrement;\
			do {\
                *tpCurrent = tMoveColor;\
                *(tppFlipStack++) = tpCurrent;\
                tpCurrent -= iIncrement;\
                *cpCurrentValue += 2;\
            } \
			while (tpCurrent != tpSquare);\
			\
        }\
    }\
\
}

bool bfMakeMove(TBoardCell *taBoard, TBoardCell tMoveColor, TBoardCell tIndex, char *cpCurrentValue, u32 *dwpFlipCount)
{
    TBoardCell tOpponentColor = (tMoveColor == BLACK ? WHITE : BLACK);
    u8 ucMask = ucaFlippingDirections[tIndex];
	char cOldValue = *cpCurrentValue;

	TBoardCell *tpSquare  = taBoard + tIndex;
	TBoardCell *tpCurrent = tpSquare;

    if (tMoveColor == BLACK) {
		switch(ucMask) {
			case 1 : {
				ucfFlipDirectionIfAvailableX2(tpSquare,  1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, 10, BLACK, WHITE);
				break;
			}
			case 2 : {
				ucfFlipDirectionIfAvailableX2(tpSquare, -1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, 10, BLACK, WHITE);
				break;
			}
			case 3 : {
				ucfFlipDirectionIfAvailableX2(tpSquare, -1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  9, BLACK, WHITE);
				break;
			}
			case 4 : {
				ucfFlipDirectionIfAvailableX2(tpSquare, -9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, -8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, 10, BLACK, WHITE);
				break;
			}
			case 5 : {
				ucfFlipDirectionIfAvailableX2(tpSquare,-10, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, -9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, -8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, -1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, 10, BLACK, WHITE);
				break;
			}
			case 6 : {
				ucfFlipDirectionIfAvailableX2(tpSquare,-10, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, -9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, -1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  9, BLACK, WHITE);
				break;
			}
			case 7 : {
				ucfFlipDirectionIfAvailableX2(tpSquare, -9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, -8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  1, BLACK, WHITE);
				break;
			}
			case 8 : {
				ucfFlipDirectionIfAvailableX2(tpSquare,-10, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, -9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, -8, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, -1, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare,  1, BLACK, WHITE);
				break;
			}
			case 9 : {
				ucfFlipDirectionIfAvailableX2(tpSquare,-10, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, -9, BLACK, WHITE);
				ucfFlipDirectionIfAvailableX2(tpSquare, -1, BLACK, WHITE);
				break;
			}
		}
	
		*dwpFlipCount = (*cpCurrentValue - cOldValue) >> 1;
		if (*dwpFlipCount) {
			*tpSquare			= BLACK;
			*(tppFlipStack++)	= tpSquare;
			++(*cpCurrentValue);
			++(*dwpFlipCount);
			return				(true);
		}
		else 
			return				(false);
	
	}
	else {
		switch(ucMask) {
			case 1 : {
				ucfFlipDirectionIfAvailableX2(tpSquare,  1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, 10, WHITE, BLACK);
				break;
			}
			case 2 : {
				ucfFlipDirectionIfAvailableX2(tpSquare, -1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, 10, WHITE, BLACK);
				break;
			}
			case 3 : {
				ucfFlipDirectionIfAvailableX2(tpSquare, -1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  9, WHITE, BLACK);
				break;
			}
			case 4 : {
				ucfFlipDirectionIfAvailableX2(tpSquare, -9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, -8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, 10, WHITE, BLACK);
				break;
			}
			case 5 : {
				ucfFlipDirectionIfAvailableX2(tpSquare,-10, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, -9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, -8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, -1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, 10, WHITE, BLACK);
				break;
			}
			case 6 : {
				ucfFlipDirectionIfAvailableX2(tpSquare,-10, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, -9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, -1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  9, WHITE, BLACK);
				break;
			}
			case 7 : {
				ucfFlipDirectionIfAvailableX2(tpSquare, -9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, -8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  1, WHITE, BLACK);
				break;
			}
			case 8 : {
				ucfFlipDirectionIfAvailableX2(tpSquare,-10, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, -9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, -8, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, -1, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare,  1, WHITE, BLACK);
				break;
			}
			case 9 : {
				ucfFlipDirectionIfAvailableX2(tpSquare,-10, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, -9, WHITE, BLACK);
				ucfFlipDirectionIfAvailableX2(tpSquare, -1, WHITE, BLACK);
				break;
			}
		}
		
		*dwpFlipCount = (*cpCurrentValue - cOldValue) >> 1;
		if (*dwpFlipCount) {
			*tpSquare			= WHITE;
			*(tppFlipStack++)	= tpSquare;
			++(*cpCurrentValue);
			++(*dwpFlipCount);
			return				(true);
		}
		else
			return(false);
	}
}

