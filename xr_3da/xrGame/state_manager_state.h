////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_state.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager state
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_manager_abstract.h"
#include "state_base.h"

class CStateManagerState : 
	public CStateManagerAbstract<
		CStateBase
	>,
	public CStateBase
{
protected:
	typedef CStateManagerAbstract<CStateBase> CSStateManagerAbstract;

public:
						CStateManagerState		();
	virtual				~CStateManagerState		();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Stalker *object);
	virtual	void		reload					(LPCSTR section);
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();

protected:
	virtual	void		update					(u32 time_delta);
};

#include "state_manager_state_inline.h"