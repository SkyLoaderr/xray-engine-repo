//////////////////////////////////////////////////////////////////////////////////
//	Module			:	Mobility routines										//
//	File			:	mobil.cpp												//
//	Creation date	:	19.12.2001												//
//	Author			:	Dmitriy Iassenev										//
//////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "game.h"
#include "mobil.h"
#include "misc.h"

#define _B3(x1,x2,x3)							(tGlobalSearchParameters.taBoard[x1]*9 + tGlobalSearchParameters.taBoard[x2]*3 + tGlobalSearchParameters.taBoard[x3])
#define _B4(x1,x2,x3,x4)						(tGlobalSearchParameters.taBoard[x1]*27 + tGlobalSearchParameters.taBoard[x2]*9 + tGlobalSearchParameters.taBoard[x3]*3 + tGlobalSearchParameters.taBoard[x4])
#define _B5(x1,x2,x3,x4,x5)						(tGlobalSearchParameters.taBoard[x1]*81 + tGlobalSearchParameters.taBoard[x2]*27 + tGlobalSearchParameters.taBoard[x3]*9 + tGlobalSearchParameters.taBoard[x4]*3 + tGlobalSearchParameters.taBoard[x5])
#define _B6(x1,x2,x3,x4,x5,x6)					(tGlobalSearchParameters.taBoard[x1]*243 + tGlobalSearchParameters.taBoard[x2]*81 + tGlobalSearchParameters.taBoard[x3]*27 + tGlobalSearchParameters.taBoard[x4]*9 + tGlobalSearchParameters.taBoard[x5]*3 + tGlobalSearchParameters.taBoard[x6])
#define _B7(x1,x2,x3,x4,x5,x6,x7)				(tGlobalSearchParameters.taBoard[x1]*729 + tGlobalSearchParameters.taBoard[x2]*243 + tGlobalSearchParameters.taBoard[x3]*81 + tGlobalSearchParameters.taBoard[x4]*27 + tGlobalSearchParameters.taBoard[x5]*9 + tGlobalSearchParameters.taBoard[x6]*3 + tGlobalSearchParameters.taBoard[x7])
#define _B8(x1,x2,x3,x4,x5,x6,x7,x8)			(tGlobalSearchParameters.taBoard[x1]*2187 + tGlobalSearchParameters.taBoard[x2]*729 + tGlobalSearchParameters.taBoard[x3]*243 + tGlobalSearchParameters.taBoard[x4]*81 + tGlobalSearchParameters.taBoard[x5]*27 + tGlobalSearchParameters.taBoard[x6]*9 + tGlobalSearchParameters.taBoard[x7]*3 + tGlobalSearchParameters.taBoard[x8])
#define _B9(x1,x2,x3,x4,x5,x6,x7,x8,x9)			(tGlobalSearchParameters.taBoard[x3]*6561 + tGlobalSearchParameters.taBoard[x2]*2187 + tGlobalSearchParameters.taBoard[x6]*729 + tGlobalSearchParameters.taBoard[x1]*243 + tGlobalSearchParameters.taBoard[x5]*81 + tGlobalSearchParameters.taBoard[x9]*27 + tGlobalSearchParameters.taBoard[x8]*9 + tGlobalSearchParameters.taBoard[x4]*3 + tGlobalSearchParameters.taBoard[x7])
#define _B10(x1,x2,x3,x4,x5,x6,x7,x8,x9,x10)	(tGlobalSearchParameters.taBoard[x1]*19683 + tGlobalSearchParameters.taBoard[x2]*6561 + tGlobalSearchParameters.taBoard[x3]*2187 + tGlobalSearchParameters.taBoard[x4]*729 + tGlobalSearchParameters.taBoard[x5]*243 + tGlobalSearchParameters.taBoard[x6]*81 + tGlobalSearchParameters.taBoard[x7]*27 + tGlobalSearchParameters.taBoard[x8]*9 + tGlobalSearchParameters.taBoard[x9]*3 + tGlobalSearchParameters.taBoard[x10])

#define _B3x(x1,x2,x3)							_B3(x3,x2,x1)                       
#define _B4x(x1,x2,x3,x4)						_B4(x4,x3,x2,x1)                    
#define _B5x(x1,x2,x3,x4,x5)					_B5(x5,x4,x3,x2,x1)                 	
#define _B6x(x1,x2,x3,x4,x5,x6)					_B6(x6,x5,x4,x3,x2,x1)              
#define _B7x(x1,x2,x3,x4,x5,x6,x7)				_B7(x7,x6,x5,x4,x3,x2,x1)           
#define _B8x(x1,x2,x3,x4,x5,x6,x7,x8)			_B8(x8,x7,x6,x5,x4,x3,x2,x1)        
#define _B9x(x1,x2,x3,x4,x5,x6,x7,x8,x9)		_B9(x3,x2,x6,x1,x5,x9,x8,x4,x7)     	
#define _B10x(x1,x2,x3,x4,x5,x6,x7,x8,x9,x10)	_B10(x10,x9,x8,x7,x6,x5,x4,x3,x2,x1)

#define ASSIGN_ACCESSABILITY_MASKS(a,b,c,d) \
	if (tGlobalSearchParameters.taBoard[c] == EMPTY) {\
		a[c] &= d ^ 0xff;\
		b[c] |= d;\
	}
			
ALIGN(16)	u16		waMPIndexes		[MP_COUNT];
			TMPInfo	taMPStructures	[BOARD_SIZE];
			u16		wDummy;

u8 ucaBlackAccessabilityMasks		[BOARD_SIZE];
u8 ucaWhiteAccessabilityMasks		[BOARD_SIZE];

void vfInitAccessabilityMasks()
{
	
	for (int i=0; i<BOARD_SIZE; i++) {
		if (tGlobalSearchParameters.taBoard[i] != EMPTY) {
			ucaBlackAccessabilityMasks[i] = 0;
			ucaWhiteAccessabilityMasks[i] = 0;
		}
		
		if (tGlobalSearchParameters.taBoard[i] == BLACK) {
			ASSIGN_ACCESSABILITY_MASKS(ucaBlackAccessabilityMasks,ucaWhiteAccessabilityMasks,i -  1,0x80);
			ASSIGN_ACCESSABILITY_MASKS(ucaBlackAccessabilityMasks,ucaWhiteAccessabilityMasks,i - 10,0x40);
			ASSIGN_ACCESSABILITY_MASKS(ucaBlackAccessabilityMasks,ucaWhiteAccessabilityMasks,i -  9,0x20);
			ASSIGN_ACCESSABILITY_MASKS(ucaBlackAccessabilityMasks,ucaWhiteAccessabilityMasks,i -  8,0x10);
			ASSIGN_ACCESSABILITY_MASKS(ucaBlackAccessabilityMasks,ucaWhiteAccessabilityMasks,i +  1,0x08);
			ASSIGN_ACCESSABILITY_MASKS(ucaBlackAccessabilityMasks,ucaWhiteAccessabilityMasks,i +  8,0x04);
			ASSIGN_ACCESSABILITY_MASKS(ucaBlackAccessabilityMasks,ucaWhiteAccessabilityMasks,i +  9,0x02);
			ASSIGN_ACCESSABILITY_MASKS(ucaBlackAccessabilityMasks,ucaWhiteAccessabilityMasks,i + 10,0x01);
		}
		else 
			if (tGlobalSearchParameters.taBoard[i] == WHITE) {
				ASSIGN_ACCESSABILITY_MASKS(ucaWhiteAccessabilityMasks,ucaBlackAccessabilityMasks,i -  1,0x80);
				ASSIGN_ACCESSABILITY_MASKS(ucaWhiteAccessabilityMasks,ucaBlackAccessabilityMasks,i - 10,0x40);
				ASSIGN_ACCESSABILITY_MASKS(ucaWhiteAccessabilityMasks,ucaBlackAccessabilityMasks,i -  9,0x20);
				ASSIGN_ACCESSABILITY_MASKS(ucaWhiteAccessabilityMasks,ucaBlackAccessabilityMasks,i -  8,0x10);
				ASSIGN_ACCESSABILITY_MASKS(ucaWhiteAccessabilityMasks,ucaBlackAccessabilityMasks,i +  1,0x08);
				ASSIGN_ACCESSABILITY_MASKS(ucaWhiteAccessabilityMasks,ucaBlackAccessabilityMasks,i +  8,0x04);
				ASSIGN_ACCESSABILITY_MASKS(ucaWhiteAccessabilityMasks,ucaBlackAccessabilityMasks,i +  9,0x02);
				ASSIGN_ACCESSABILITY_MASKS(ucaWhiteAccessabilityMasks,ucaBlackAccessabilityMasks,i + 10,0x01);
			}
	}
}
			
void vfInitMPIndexes()
{
	//horisontals...
	waMPIndexes[ 0] = _B10x(10,11,12,13,20,25,14,15,16,17);
	waMPIndexes[ 1] = _B8x(19,20,21,22,23,24,25,26);
	waMPIndexes[ 2] = _B8x(28,29,30,31,32,33,34,35);
	waMPIndexes[ 3] = _B8x(37,38,39,40,41,42,43,44);
	waMPIndexes[ 4] = _B8(53,52,51,50,49,48,47,46);
	waMPIndexes[ 5] = _B8(62,61,60,59,58,57,56,55);
	waMPIndexes[ 6] = _B8(71,70,69,68,67,66,65,64);
	waMPIndexes[ 7] = _B10(80,79,78,77,70,65,76,75,74,73);
	//verticals...
	waMPIndexes[ 8] = _B10x(10,19,28,37,20,65,46,55,64,73);
	waMPIndexes[ 9] = _B8x(11,20,29,38,47,56,65,74);
	waMPIndexes[10] = _B8x(12,21,30,39,48,57,66,75);
	waMPIndexes[11] = _B8x(13,22,31,40,49,58,67,76);
	waMPIndexes[12] = _B8(77,68,59,50,41,32,23,14);
	waMPIndexes[13] = _B8(78,69,60,51,42,33,24,15);
	waMPIndexes[14] = _B8(79,70,61,52,43,34,25,16);
	waMPIndexes[15] = _B10(80,71,62,53,70,25,44,35,26,17);
	//diagonals left-up => right-down
	waMPIndexes[16] = _B8(80,70,60,50,40,30,20,10);
	waMPIndexes[17] = _B7x(11,21,31,41,51,61,71);
	waMPIndexes[18] = _B7(79,69,59,49,39,29,19);
	waMPIndexes[19] = _B6x(12,22,32,42,52,62);
	waMPIndexes[20] = _B6(78,68,58,48,38,28);
	waMPIndexes[21] = _B5x(13,23,33,43,53);
	waMPIndexes[22] = _B5(77,67,57,47,37);
	waMPIndexes[23] = _B4x(14,24,34,44);
	waMPIndexes[24] = _B4(76,66,56,46);
	//diagonals left-down => right-up
	waMPIndexes[25] = _B8(73,65,57,49,41,33,25,17);
	waMPIndexes[26] = _B7(64,56,48,40,32,24,16);
	waMPIndexes[27] = _B7x(26,34,42,50,58,66,74);
	waMPIndexes[28] = _B6(55,47,39,31,23,15);
	waMPIndexes[29] = _B6x(35,43,51,59,67,75);
	waMPIndexes[30] = _B5(46,38,30,22,14);
	waMPIndexes[31] = _B5x(44,52,60,68,76);
	waMPIndexes[32] = _B4(37,29,21,13);
	waMPIndexes[33] = _B4x(53,61,69,77);
	//3x3 square
	waMPIndexes[34] = _B9(10,11,12,19,20,21,28,29,30);
	waMPIndexes[35] = _B9(17,16,15,26,25,24,35,34,33);
	waMPIndexes[36] = _B9(80,79,78,71,70,69,62,61,60);
	waMPIndexes[37] = _B9(73,74,75,64,65,66,55,56,57);
	//2x5 edge
	waMPIndexes[38] = _B10(10,11,12,13,14,19,20,21,22,23);
	waMPIndexes[39] = _B10(10,19,28,37,46,11,20,29,38,47);
	waMPIndexes[40] = _B10(17,16,15,14,13,26,25,24,23,22);
	waMPIndexes[41] = _B10(17,26,35,44,53,16,25,34,43,52);
	waMPIndexes[42] = _B10(80,79,78,77,76,71,70,69,68,67);
	waMPIndexes[43] = _B10(80,71,62,53,44,79,70,61,52,43);
	waMPIndexes[44] = _B10(73,74,75,76,77,64,65,66,67,68);
	waMPIndexes[45] = _B10(73,64,55,46,37,74,65,56,47,38);
	/**/
	u16 wShortWay = 0;
	waMPIndexes[32] += wShortWay;
	waMPIndexes[23] += wShortWay;
	waMPIndexes[33] += wShortWay;
	waMPIndexes[24] += wShortWay;
		
	wShortWay += 81;
	waMPIndexes[30] += wShortWay;
	waMPIndexes[21] += wShortWay;
	waMPIndexes[31] += wShortWay;
	waMPIndexes[22] += wShortWay;
		
	wShortWay += 243;
	waMPIndexes[28] += wShortWay;
	waMPIndexes[19] += wShortWay;
	waMPIndexes[29] += wShortWay;
	waMPIndexes[20] += wShortWay;
		
	wShortWay += 729;
	waMPIndexes[26] += wShortWay;
	waMPIndexes[17] += wShortWay;
	waMPIndexes[27] += wShortWay;
	waMPIndexes[18] += wShortWay;
		
	wShortWay += 2187;
	waMPIndexes[25] += wShortWay;
	waMPIndexes[16] += wShortWay;
			
	wShortWay += 6561;
	waMPIndexes[ 1] += wShortWay;
	waMPIndexes[14] += wShortWay;
	waMPIndexes[ 6] += wShortWay;
	waMPIndexes[ 9] += wShortWay;
				
	wShortWay += 6561;
	waMPIndexes[ 2] += wShortWay;
	waMPIndexes[13] += wShortWay;
	waMPIndexes[ 5] += wShortWay;
	waMPIndexes[10] += wShortWay;
				
	wShortWay += 6561;
	waMPIndexes[ 3] += wShortWay;
	waMPIndexes[12] += wShortWay;
	waMPIndexes[ 4] += wShortWay;
	waMPIndexes[11] += wShortWay;
			
	wShortWay += 6561;
	waMPIndexes[34] += wShortWay;
	waMPIndexes[35] += wShortWay;
	waMPIndexes[36] += wShortWay;
	waMPIndexes[37] += wShortWay;
		
	/**
	wShortWay += 19683;
	waMPIndexes[38] += wShortWay;
	waMPIndexes[39] += wShortWay;
	waMPIndexes[40] += wShortWay;
	waMPIndexes[41] += wShortWay;
	waMPIndexes[42] += wShortWay;
	waMPIndexes[43] += wShortWay;
	waMPIndexes[44] += wShortWay;
	waMPIndexes[45] += wShortWay;

	wShortWay += 59049;
	waMPIndexes[ 0] += wShortWay;
	waMPIndexes[15] += wShortWay;
	waMPIndexes[ 7] += wShortWay;
	waMPIndexes[ 8] += wShortWay;
	/**/
}

void vfLoadMobilityPatterns()
{
	vfDualPrintF("Initializing mobility patterns for FPIT..."); //Fast Pattern Index Transformation

	TBoardCell taBoard[BOARD_SIZE];
	vfInitBoard(taBoard);
					   
	memset(taMPStructures,0,sizeof(taMPStructures));

	int iMax = 0;
	for (int i=0; i<BOARD_SIZE; i++)
		if (taBoard[i] != DUMMY) {
			int iRow = (i - BOARD_START) / 9;
			int iCol = (i - BOARD_START) % 9;
			
			int iPattIndex = 0;
			
			//horisontals and edge+2x without X squares
			if ((iRow > 0) && (iRow < 7)) {
				taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + iRow;
				taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[iCol];
			}
			else {
				taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + iRow;
				taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = iCol <4 ? (u16)dwaPower[iCol] : (u16)dwaPower[iCol + 2];
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex].wpMPIndex - waMPIndexes < MP_COUNT)
			iPattIndex++;

			//verticals and edge+2x without X squares
			if ((iCol > 0) && (iCol < 7)) {
				taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 8 + iCol;
				taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[iRow];
			}
			else {
				taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 8 + iCol;
				taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = iRow <4 ? (u16)dwaPower[iRow] : (u16)dwaPower[iRow + 2];
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex].wpMPIndex - waMPIndexes < MP_COUNT)
			iPattIndex++;
			
			//diagonals left-up => right-down
			if (iRow == iCol) {
				taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 16;
				taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[iRow];
				iPattIndex++;
			}
			else
				if (iRow < iCol) {
					if (iCol - iRow < 5) {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 15 + (iCol - iRow)*2;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[iRow];
						iPattIndex++;
					}
				}
				else
					if (iRow - iCol < 5) {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 16 + (iRow - iCol)*2;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[iCol];
						iPattIndex++;
					}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)

			//diagonals left-down => right-up
			if (iRow == 7 - iCol) {
				taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 25;
				taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[iRow];
				iPattIndex++;
			}
			else
				if (iRow < 7 - iCol) {
					if (7 - iCol - iRow < 5) {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 24 + (7 - iCol - iRow)*2;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[iRow];
						iPattIndex++;
					}
				}
				else
					if (iRow - 7 + iCol < 5) {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 25 + (iRow - 7 + iCol)*2;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[7 - iCol];
						iPattIndex++;
					}

			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)
			//3x3 square
			//					         5  7  8  1  4  6  0  2  3
			//					         4  2  1  8  5  3  9  7  6
			//tResult += wShortWay[_B9(10,11,12,19,20,21,28,29,30)]; 
			if ((iRow < 3) && (iCol < 3)) {
				switch (i) {
					case 10 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 34;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[5];
						break;
					}
					case 11 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 34;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[7];
						break;
					}
					case 12 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 34;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[8];
						break;
					}
					case 19 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 34;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[1];
						break;
					}
					case 20 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 34;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[4];
						break;
					}
					case 21 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 34;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[6];
						break;
					}
					case 28 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 34;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[0];
						break;
					}
					case 29 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 34;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[2];
						break;
					}
					case 30 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 34;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[3];
						break;
					}
				}
				iPattIndex++;
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)

			if ((iRow < 3) && (iCol > 4)) {
				switch (i) {
					case 17 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 35;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[5];
						break;
					}
					case 16 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 35;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[7];
						break;
					}
					case 15 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 35;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[8];
						break;
					}
					case 26 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 35;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[1];
						break;
					}
					case 25 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 35;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[4];
						break;
					}
					case 24 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 35;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[6];
						break;
					}
					case 35 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 35;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[0];
						break;
					}
					case 34 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 35;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[2];
						break;
					}
					case 33 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 35;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[3];
						break;
					}
				}
				iPattIndex++;
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)

			if ((iRow > 4) && (iCol > 4)) {
				switch (i) {
					case 80 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 36;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[5];
						break;
					}
					case 79 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 36;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[7];
						break;
					}
					case 78 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 36;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[8];
						break;
					}
					case 71 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 36;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[1];
						break;
					}
					case 70 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 36;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[4];
						break;
					}
					case 69 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 36;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[6];
						break;
					}
					case 62 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 36;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[0];
						break;
					}
					case 61 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 36;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[2];
						break;
					}
					case 60 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 36;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[3];
						break;
					}
				}
				iPattIndex++;
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)

			if ((iRow > 4) && (iCol < 3)) {
				switch (i) {
					case 73 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 37;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[5];
						break;
					}
					case 74 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 37;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[7];
						break;
					}
					case 75 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 37;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[8];
						break;
					}
					case 64 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 37;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[1];
						break;
					}
					case 65 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 37;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[4];
						break;
					}
					case 66 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 37;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[6];
						break;
					}
					case 55 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 37;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[0];
						break;
					}
					case 56 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 37;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[2];
						break;
					}
					case 57 : {
						taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 37;
						taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[3];
						break;
					}
				}
				iPattIndex++;
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)

			//edge+2x X squares
			switch (i) {
				case 20 : {
					taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 0;
					taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[4];
					iPattIndex++;
					taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 8;
					taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[4];
					iPattIndex++;
					break;
				}
				case 25 : {
					taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 0;
					taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[5];
					iPattIndex++;
					taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 15;
					taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[4];
					iPattIndex++;
					break;
				}
				case 65 : {
					taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 7;
					taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[4];
					iPattIndex++;
					taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 8;
					taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[5];
					iPattIndex++;
					break;
				}
				case 70 : {
					taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 7;
					taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[5];
					iPattIndex++;
					taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 15;
					taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[5];
					iPattIndex++;
					break;
				}
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)

			//2x5 edges
			if ((iRow < 2) && (iCol < 5)) {
				taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 38;
				taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[9 - (iCol + iRow * 5)];
				iPattIndex++;
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)
			if ((iRow < 5) && (iCol < 2)) {
				taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 39;
				taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[9 - (iRow + iCol * 5)];
				iPattIndex++;
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)
			
			if ((iRow < 2) && (iCol > 2)) {
				taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 40;
				taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[9 - (7 - iCol + iRow * 5)];
				iPattIndex++;
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)
			if ((iRow < 5) && (iCol > 5)) {
				taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 41;
				taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[9 - (iRow + (7 - iCol) * 5)];
				iPattIndex++;
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)
			
			if ((iRow > 5) && (iCol > 2)) {
				taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 42;
				taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[9 - (7 - iCol + (7 - iRow) * 5)];
				iPattIndex++;
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)
			if ((iRow > 2) && (iCol > 5)) {
				taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 43;
				taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[9 - (7 - iRow + (7 - iCol) * 5)];
				iPattIndex++;
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)
			
			if ((iRow > 5) && (iCol < 5)) {
				taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 44;
				taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[9 - (iCol + (7 - iRow) * 5)];
				iPattIndex++;
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)
			if ((iRow > 2) && (iCol < 2)) {
				taMPStructures[i].taPatt[iPattIndex].wpMPIndex = waMPIndexes + 45;
				taMPStructures[i].taPatt[iPattIndex].wMPIndexChange = (u16)dwaPower[9 - (7 - iRow + iCol * 5)];
				iPattIndex++;
			}
			__ASSERT(taMPStructures[i].taPatt[iPattIndex-1].wpMPIndex - waMPIndexes < MP_COUNT)
			
			taMPStructures[i].ucCount = iPattIndex;
			//printf("%d\n",iPattIndex);
			//if (iMax < iPattIndex)
			//	iMax = iPattIndex;
		}
		//printf("\n%d\n",iMax);
		//getchar();
	
	vfDualPrintF("completed\n  64 cells x 8 patterns = %.1f (%d bytes)\n",sizeof(taMPStructures)/1048576.0,sizeof(taMPStructures));

}
