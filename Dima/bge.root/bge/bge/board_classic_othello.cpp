////////////////////////////////////////////////////////////////////////////
//	Module 		: board_classic_othello.cpp
//	Created 	: 07.12.2004
//  Modified 	: 07.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Calssic othello board implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "board_classic_othello.h"

const u8 CBoardClassicOthello::flipping_directions[BOARD_SIZE] = {
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

