////////////////////////////////////////////////////////////////////////////
//	Module 		: setup_manager.h
//	Created 	: 05.04.2004
//  Modified 	: 05.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Setup manager
////////////////////////////////////////////////////////////////////////////

#pragma once

template <
	typename _action_type,
	typename _object_type,
	typename _action_id_type
>
class CSetupManager {
protected:
	xr_map<_action_id_type,_action_type*>	m_actions;
	_action_id_type							m_current_action_id;
	_object_type							*m_object;
	bool									m_actuality;

public:
	IC								CSetupManager		();
	virtual 						~CSetupManager		();
	IC		void					init				();
	virtual void					reinit				(_object_type *object);
	virtual void					update				(u32 time_delta);
	IC		void					add_action			(const _action_id_type &action_id, _action_type *action);
	IC		_action_type			&action				(const _action_id_type &action_id) const;
	IC		_action_type			&current_action		() const;
	IC		const _action_id_type	&current_action_id	() const;
	IC		void					select_action		();
	IC		void					clear				();
};

#include "setup_manager_inline.h"
