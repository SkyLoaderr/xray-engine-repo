////////////////////////////////////////////////////////////////////////////
//	Module 		: othello_bit_board.h
//	Created 	: 24.12.2004
//  Modified 	: 24.12.2004
//	Author		: Dmitriy Iassenev
//	Description : Othello bit board implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_space.h"
#include "ui.h"

class COthelloBitBoard {
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
	typedef s8 cell_index;

public:
	union CStackCell {
		cell_type	*m_cell;
		struct {
			u16		m_flip_count;
			bool	m_passed;
		};

		IC	CStackCell(cell_type *cell)
		{
			m_cell	= cell;
		}

		IC	CStackCell(u16 flip_count, bool passed)
		{
			m_flip_count	= flip_count;
			m_passed		= passed;
		}
	};
	
public:
	typedef std::stack<CStackCell,std::vector<CStackCell> > flip_stack;

private:
	static const u8 flipping_directions[BOARD_SIZE];

private:
	cell_type				m_board[BOARD_SIZE];
	cell_type				m_color_to_move;
	u8						m_empties;
	int						m_difference;
	bool					m_passed;
	flip_stack				m_flip_stack;
	mutable string256		m_temp;

protected:
	IC		int				move_cell				(const cell_index &index) const;
	IC		void			show_cell				(const cell_index &index) const;
	IC		void			show_move_cell			(const cell_index &index) const;
	IC		void			show_letters			() const;
	IC		void			show_move_digit			(const cell_index &index) const;
	IC		void			show_digit				(const cell_index &index) const;

protected:
	IC		void			change_color			();

protected:
	template <int increment, cell_type _color_to_move, cell_type opponent_color>
	IC		void			try_flip_direction		(cell_type *start_cell, int &difference);
	
	template <cell_type color_to_move>
	IC		void			do_move					(const cell_index &index);

protected:
	template <cell_type opponent_color>
	IC		void			undo_move				();

protected:
	template <int increment, cell_type _color_to_move, cell_type opponent_color>
	IC		bool			can_move_in_direction	(cell_type const *start_cell) const;
	
	template <cell_type color_to_move>
	IC		bool			can_move				(const cell_index &index) const;

	template <cell_type color_to_move>
	IC		bool			can_move				() const;

protected:
	template <int increment, cell_type _color_to_move, cell_type opponent_color>
	IC		void			compute_direction		(cell_type const *start_cell, int &difference) const;
	
	template <cell_type color_to_move>
	IC		int				compute_difference		(const cell_index &index) const;

	template <cell_type color_to_move>
	IC		int				compute_difference		(const cell_index &index, bool) const;

public:
	IC						COthelloBitBoard		();
			void			start_position			();
			void			show					() const;
	IC		LPCSTR			move_to_string			(const cell_index &index) const;
	IC		LPCSTR			move_to_string			(const cell_index &index0, const cell_index &index1) const;
	IC		cell_index		string_to_move			(LPCSTR move) const;

public:
	IC		const cell_type	&color_to_move			() const;
	IC		u8				empties					() const;
	IC		int				difference				() const;
	IC		bool			passed					() const;
	IC		bool			terminal_position		() const;

public:
	IC		cell_index		index					(const cell_index &index0, const cell_index &index1) const;
	IC		void			index					(const cell_index &index, cell_index &index0, cell_index &index1) const;
	IC		void			cell					(const cell_index &index,  const cell_type &value);
	IC		void			cell					(const cell_index &index0, const cell_index &index1, const cell_type &value);
	IC		const cell_type	&cell					(const cell_index &index) const;
	IC		const cell_type	&cell					(const cell_index &index0, const cell_index &index1) const;

public:
			void			do_move					(const cell_index &index);
	IC		void			do_move					(const cell_index &index0, const cell_index &index1);
	IC		void			do_move					(LPCSTR move);

public:
	IC		void			undo_move				();

public:
			bool			can_move				() const;
			bool			can_move				(const cell_index &index) const;
	IC		bool			can_move				(const cell_index &index0, const cell_index &index1) const;
	IC		bool			can_move				(LPCSTR move) const;

public:
			int				compute_difference		(const cell_index &index) const;
	IC		int				compute_difference		(const cell_index &index0, const cell_index &index1) const;
	IC		int				compute_difference		(LPCSTR move) const;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(COthelloBitBoard)
#undef script_type_list
#define script_type_list save_type_list(COthelloBitBoard)

#include "othello_bit_board_inline.h"