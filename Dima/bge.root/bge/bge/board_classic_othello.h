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
		BOARD_SIZE		= 91,
		BOARD_START		= 10,
		BOARD_LINE		=  9,
	
		WHITE			=  0,
		EMPTY			=  1,
		BLACK			=  2,
		BLACK_EMPTY		=  3,
		WHITE_EMPTY		=  4,
		BLACK_WHITE		=  5,
		DUMMY			=  6,
	};

public:
	typedef u8 cell_type;

private:
	static const u8 flipping_directions[BOARD_SIZE];

private:
	cell_type		m_board[BOARD_SIZE];
	cell_type		m_color_to_move;
	u8				m_empties;
	int				m_difference;

protected:
	IC		void			show_cell				(const cell_type &value) const;

protected:
	template <
		int increment, 
		cell_type color_to_move, 
		cell_type opponent_color
	>
	IC		void			try_flip_direction		(cell_type *start_cell, int &difference);

	template <
		cell_type color_to_move
	>
	IC		bool			do_move					(const u8 &index);

public:
							CBoardClassicOthello	();
			void			start_position			();
			void			show					() const;
			bool			do_move					(const u8 &index);
	IC		bool			do_move					(const u8 &index0, const u8 &index1);

public:
	IC		u8				index					(const u8 &index0, const u8 &index1) const;
	IC		void			cell					(const u8 &index,  const cell_type &value);
	IC		void			cell					(const u8 &index0, const u8 &index1, const cell_type &value);
	IC		const cell_type	&cell					(const u8 &index) const;
	IC		const cell_type	&cell					(const u8 &index0, const u8 &index1) const;
	IC		const cell_type	&color_to_move			() const;
	IC		u8				empties					() const;
	IC		int				difference				() const;
};

#include "board_classic_othello_inline.h"