////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_no_alife.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager when no simulation started (for test purposes only!)
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_manager_abstract.h"
#include "state_base.h"

class CStateManagerNoALife : 
	public CStateManagerAbstract<
		CStateBase
	> 
{
public:
public:
						CStateManagerNoALife	();
	virtual				~CStateManagerNoALife	();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					();
	virtual	void		reload					(LPCSTR section);

			void		add						(CStateBase *state, u32 state_id, u32 priority);
			void		remove					(u32 state_id);
			void		update					(u32 time_delta);
};

#include "state_manager_no_alife_inline.h"