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
#	include "othello_classic_board.h"
	typedef COthelloClassicBoard COthelloBoard;
#else
#	define OTHELLO_BOARD_BIT
#	include "othello_bit_board.h"
	typedef COthelloBitBoard COthelloBoard;
#endif
