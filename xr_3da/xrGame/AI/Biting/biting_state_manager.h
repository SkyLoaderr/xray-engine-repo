////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_stalker.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../state_manager_state.h"

class CAI_Biting;

class CStateManagerBiting : public CStateManagerState<CAI_Biting> {
protected:
	typedef CStateManagerState<CAI_Biting> inherited;

	enum EBitingStates {
		eStateRest = u32(0),
		eStateEat,
		eStateAttack,
		eStatePanic,
		eStateDummy = u32(-1),
	};
public:
						CStateManagerBiting		(LPCSTR state_name);
	virtual				~CStateManagerBiting	();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Biting *object);
	virtual	void		reload					(LPCSTR section);
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "biting_state_manager_inline.h"