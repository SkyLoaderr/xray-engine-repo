////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_alife.h
//	Created 	: 14.01.2004
//  Modified 	: 14.01.2004
//	Author		: Dmitriy Iassenev
//	Description : Stalker state manager alife
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_manager_state.h"

class CStateManagerALife : public CStateManagerState {
protected:
	typedef CStateManagerState inherited;

	enum EALifeStates {
		eALifeStateXXXX		= u32(0),
		eALifeStateDummy	= u32(-1),
	};
public:
						CStateManagerALife		();
	virtual				~CStateManagerALife		();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Stalker *object);
	virtual	void		reload					(LPCSTR section);
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "state_manager_alife_inline.h"