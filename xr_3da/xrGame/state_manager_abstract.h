////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_abstract.h
//	Created 	: 12.01.2004
//  Modified 	: 12.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager abstract
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "graph_manager.h"

namespace StateManagerAbstract {
	template <typename T>
	struct CStateAbstract {
		T				*m_state;
		u32				m_priority;
				CStateAbstract(T *state, u32 priority) : 
					m_state(state), 
					m_priority(priority)
		{
		}

		void	destroy()
		{
			xr_delete	(m_state);
		}
	};
};

template <typename T>
class CStateManagerAbstract : 
	public CGraphManagerAbstract<
		StateManagerAbstract::CStateAbstract<T>,
		float,
		u32,
		u32
	>
{
public:
	typedef StateManagerAbstract::CStateAbstract<T> CState;
	typedef CGraphManagerAbstract<CState,float,u32,u32> inherited;
private:

protected:
	virtual	void		add						(T *state, u32 state_id, u32 priority);
	virtual	void		remove					(u32 state_id);

public:
						CStateManagerAbstract	();
	virtual				~CStateManagerAbstract	();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(const u32 start_vertex_id);
	virtual	void		reload					(LPCSTR section);
	virtual	void		update					(u32 time_delta);
	IC		T			&state					(const u32 state_id);
};

#include "state_manager_abstract_inline.h"