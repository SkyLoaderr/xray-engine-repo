////////////////////////////////////////////////////////////////////////////
//	Module 		: bge.cpp
//	Created 	: 01.10.2004
//  Modified 	: 01.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Board game engine
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hardware.h"
#include "script.h"
#include "ui.h"
#include "board_classic_othello.h"

void __cdecl main(char argc, char *argv[])
{
	Hardware::detect	();
	script().init		();
	
	CBoardClassicOthello board;
	board.show			();
	
	board.do_move		(5,4);
	board.show			();
	
	board.do_move		(3,5);
	board.show			();
	
	ui().execute		(argc,argv);
}
