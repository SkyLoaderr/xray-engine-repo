////////////////////////////////////////////////////////////////////////////
//	Module 		: board_classic_othello.h
//	Created 	: 07.12.2004
//  Modified 	: 07.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Calssic othello board implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

class CBoardClassicOthello {
public:
	enum EBoardConstants {
		BOARD_SIZE  = 91,
		BOARD_START = 10,
		BOARD_LINE  =  9,
	};

private:
	static const u8 flipping_directions[BOARD_SIZE];
	u8				m_board;

public:
					CBoardClassicOthello	();

};

#include "board_classic_othello_inline.h"