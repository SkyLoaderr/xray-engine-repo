////////////////////////////////////////////////////////////////////////////
//	Module 		: board_othello.h
//	Created 	: 24.12.2004
//  Modified 	: 24.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Othello board
////////////////////////////////////////////////////////////////////////////

#pragma once

#define OTHELLO_BOARD_CLASSIC

#ifdef OTHELLO_BOARD_CLASSIC
#	include "board_classic_othello.h"
	typedef CBoardClassicOthello COthelloBoard;
#else
#	define OTHELLO_BOARD_BIT
#	include "bitboard_othello.h"
	typedef CBitBoardOthello CBoardOthello;
#endif
