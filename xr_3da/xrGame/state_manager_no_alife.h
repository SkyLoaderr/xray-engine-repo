////////////////////////////////////////////////////////////////////////////
//	Module 		: state_manager_no_alife.h
//	Created 	: 13.01.2004
//  Modified 	: 13.01.2004
//	Author		: Dmitriy Iassenev
//	Description : State manager when no simulation started (for test purposes only!)
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "state_manager_state.h"

class CStateManagerNoALife : public CStateManagerState {
protected:
	typedef CStateManagerState inherited;
	enum ENoALifeStates {
		eNoALifeStateFree = u32(0),
		eNoALifeStateDummy = u32(-1),
	};

public:
						CStateManagerNoALife	();
	virtual				~CStateManagerNoALife	();
			void		Init					();
	virtual	void		Load					(LPCSTR section);
	virtual	void		reinit					(CAI_Stalker *object);
	virtual	void		reload					(LPCSTR section);
	virtual	void		initialize				();
	virtual	void		execute					();
	virtual	void		finalize				();
};

#include "state_manager_no_alife_inline.h"