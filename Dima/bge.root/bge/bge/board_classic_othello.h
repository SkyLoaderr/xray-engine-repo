////////////////////////////////////////////////////////////////////////////
//	Module 		: board_classic_othello.h
//	Created 	: 07.12.2004
//  Modified 	: 07.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Calssic othello board implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"

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
	typedef u8 cell_index;

private:
	static const u8 flipping_directions[BOARD_SIZE];

private:
	cell_type		m_board[BOARD_SIZE];
	cell_type		m_color_to_move;
	u8				m_empties;
	int				m_difference;
	cell_index		*m_flip_stack[4096];
	cell_index		**m_current_flip;


protected:
	IC		void			show_cell				(const cell_type &value) const;

protected:
	template <cell_type color_to_move>
	IC		bool			do_move					(const cell_index &index);

public:
	IC						CBoardClassicOthello	();
			void			start_position			();
			void			show					() const;
			bool			do_move					(const cell_index &index);
	IC		bool			do_move					(const cell_index &index0, const cell_index &index1);

public:
	IC		cell_index		index					(const cell_index &index0, const cell_index &index1) const;
	IC		void			cell					(const cell_index &index,  const cell_type &value);
	IC		void			cell					(const cell_index &index0, const cell_index &index1, const cell_type &value);
	IC		const cell_type	&cell					(const cell_index &index) const;
	IC		const cell_type	&cell					(const cell_index &index0, const cell_index &index1) const;
	IC		const cell_type	&color_to_move			() const;
	IC		u8				empties					() const;
	IC		int				difference				() const;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CBoardClassicOthello)
#undef script_type_list
#define script_type_list save_type_list(CBoardClassicOthello)

#include "board_classic_othello_inline.h"