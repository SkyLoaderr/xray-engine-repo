////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_combat.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager combat
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_manager_state.h"

class CStateManagerCombat : public CStateManagerState {
protected:
	typedef CStateManagerState inherited;

	enum ECombatStates {
		eCombatStateXXXX	= u32(0),
		eCombatStateDummy	= u32(-1),
	};
public:
						CStateManagerCombat		();
	virtual				~CStateManagerCombat	();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Stalker *object);
	virtual	void		reload					(LPCSTR section);
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
	virtual	void		update					(u32 time_delta);
};

#include "state_manager_combat_inline.h"