//////////////////////////////////////////////////////////////////////////////////
//	Module			:	transposition table										//
//	File			:	tt.cpp													//
//	Creation date	:	15.02.2001												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "eval.h"
#include "board.h"

//////////////////////////////////////////////////////////
// Macros declarations
//////////////////////////////////////////////////////////

#define HASH_VALUES        6

//////////////////////////////////////////////////////////
// Type declarations
//////////////////////////////////////////////////////////

typedef u32	SStoredHash;
typedef u64 SHashValue;
typedef u64 SHashBoard;

typedef struct tagSTT_Structure {
	SStoredHash	tHash;		// hash code
	u8			ucFlag;		// true value(0), alpha(1) or beta(2) bound
	u8			ucDepth;	// looked depth
	u8			ucMove ;	// the best move
	SFeature	tAlpha;
	SFeature	tBeta;
} STT_Structure;

typedef struct tagSHashCell {
	STT_Structure taCells[2];
} SHashCell;

//////////////////////////////////////////////////////////
// Exproted variables
//////////////////////////////////////////////////////////

#ifdef STATIC_TT
	#define MAX_TT_HASH_BITS 24
	extern SHashCell taHash[1 << MAX_TT_HASH_BITS];
#else
	extern SHashCell *taHash;
	extern u8 MAX_TT_HASH_BITS;
#endif

extern char		cGlobalTimeStamp;
extern char		cBugTimeStamp;
extern u64		qwVariationCount;
extern u32		dwRandSeed;
extern const	SHashValue taHashBoard[BOARD_SIZE * HASH_VALUES];
extern const	SHashValue tBlackColor;
extern const	SHashValue tWhiteColor;
extern const	SHashValue tChangeColor;
extern bool		bInHash;

#ifdef _DEBUG
	extern u32  dwTTUseCount;
	extern u32  dwTTAddCount;
	extern u32  dwTTCollision;
	extern u32  dwTTBADCollision;
#endif

//////////////////////////////////////////////////////////
// Exproted functions
//////////////////////////////////////////////////////////

extern int  ifGetTTNode(SHashValue tpHashValue, u8 *cpLevel);
extern bool bfGetEndgameTTNode(SHashValue tpHashValue, STT_Structure &tHashCell);
extern void vfPutTTNode(SHashValue tpHashValue, u8 ucDepth, SFeature tAlpha, SFeature tBeta, SFeature tValue, u8 ucMove);
extern void vfPutEndgameTTNode(SHashValue tpHashValue, u8 ucDepth, SFeature tAlpha, SFeature tBeta, SFeature tValue, u8 ucMove);
extern void vfCreateHashValue(TBoardCell *taBoard, SHashValue *tpHashValue, TBoardCell tColor);
extern void vfInitalizeHash(u32 dwStartSeed);

IC u32 dwfRandom(u32 dwRange)
{
	u32 dwResult;
	__asm {
			mov     eax,dwRange
			imul    edx,dwRandSeed,08088405H
			inc     edx
			mov     dwRandSeed,edx
			mul     edx
			mov     dwResult,edx
	}
	return(dwResult);
}
