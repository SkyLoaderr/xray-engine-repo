////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_stalker.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_manager_state.h"

class CStateManagerStalker : public CStateManagerState {
protected:
	typedef CStateManagerState inherited;

	enum EStalkerStates {
		eStalkerStateDeath = u32(0),
		eStalkerStateNoALife,
		eStalkerStateALife,
		eStalkerStateCombat,
		eStalkerStateDummy = u32(-1),
	};
public:
						CStateManagerStalker	();
	virtual				~CStateManagerStalker	();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Stalker *object);
	virtual	void		reload					(LPCSTR section);
	virtual	void		execute					();
	virtual	void		update					(u32 time_delta);
};

#include "state_manager_stalker_inline.h"