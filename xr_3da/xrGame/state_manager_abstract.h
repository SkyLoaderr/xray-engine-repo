////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_abstract.h
//	Created 	: 12.01.2004
//  Modified 	: 12.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager abstract
////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename T>
class CStateManagerAbstract {
	struct CState {
		T				*m_state;
		u32				m_priority;
						CState(T *state, u32 priority) : 
							m_state(state), 
							m_priority(priority)
		{
		}

						~CState()
		{
			xr_delete	(m_state);
		}
	};
	xr_map<u32,CState>	m_states;
	u32					m_cur_state;
	u32					m_dest_state;
	bool				m_actuality;

protected:
	virtual	void		add						(T *state, u32 state_id, u32 priority);
	virtual	void		remove					(u32 state_id);
	virtual	void		update					(u32 time_delta);
	IC		u32			current_state			() const;
	IC		u32			dest_state				() const;
	IC		void		set_dest_state			(u32 state_id);

public:
						CStateManagerAbstract	();
	virtual				~CStateManagerAbstract	();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					();
	virtual	void		reload					(LPCSTR section);
};

#include "state_manager_abstract_inline.h"