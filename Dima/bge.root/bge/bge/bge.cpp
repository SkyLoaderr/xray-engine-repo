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

#include "othello_board.h"
#include "mini_max.h"
#include "cpu.h"

template <typename board_type>
struct CPrimitiveEvaluator {
	typedef s8	result_type;
	enum {
		infinity = 127,
	};

	IC	s8		evaluate			(const board_type &board)
	{
		if (board.color_to_move() == board_type::BLACK)
			return	(board.difference());
		return		(-board.difference());
	}
};

typedef CPrimitiveEvaluator<COthelloBoard>				CSPrimitiveEvaluator;
typedef mini_max<COthelloBoard,CSPrimitiveEvaluator>	_mini_max;

void __cdecl main(char argc, char *argv[])
{
	Hardware::detect	();
	script().init		();

	COthelloBoard			b;
	CSPrimitiveEvaluator	e;
	_mini_max				search(&b,&e);
	
	u64						start, finish;

	SetPriorityClass	(GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
	Sleep(1);
	
	start					= CPU::cycles();
	search.search			(9);
	finish					= CPU::cycles();

	SetThreadPriority	(GetCurrentThread(),THREAD_PRIORITY_NORMAL);
	SetPriorityClass	(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);

	ui().log				("Nodes searched : %d\n",search.m_counter);
	ui().log				("Search speed   : %f (nodes per second)\n",float(search.m_counter)/((finish - start)*CPU::cycles2seconds));
	
	ui().execute		(argc,argv);
}
