////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_death.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager death
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_manager_state.h"

class CStateManagerDeath : public CStateManagerState {
protected:
	typedef CStateManagerState inherited;

	enum EStalkerStates {
		eDeathStateRecentlyDead = u32(0),
		eDeathStateAlreadyDead,
		eDeathStateDummy		= u32(-1),
	};
public:
						CStateManagerDeath		();
	virtual				~CStateManagerDeath		();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Stalker *object);
	virtual	void		reload					(LPCSTR section);
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "state_manager_death_inline.h"