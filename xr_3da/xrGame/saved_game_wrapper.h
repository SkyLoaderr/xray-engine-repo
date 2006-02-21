////////////////////////////////////////////////////////////////////////////
//	Module 		: saved_game_wrapper.h
//	Created 	: 21.02.2006
//  Modified 	: 21.02.2006
//	Author		: Dmitriy Iassenev
//	Description : saved game wrapper class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "game_graph_space.h"
#include "script_export_space.h"

class CSavedGameWrapper {
public:
	typedef ALife::_TIME_ID						_TIME_ID;
	typedef GameGraph::_LEVEL_ID				_LEVEL_ID;

private:
	_TIME_ID	m_game_time;
	_LEVEL_ID	m_level_id;

public:
							CSavedGameWrapper	(LPCSTR saved_game_name);
	inline	const _TIME_ID	&game_time			() const;
	inline	const _LEVEL_ID	&level_id			() const;
	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CSavedGameWrapper)
#undef script_type_list
#define script_type_list save_type_list(CSavedGameWrapper)

#include "saved_game_wrapper_inline.h"