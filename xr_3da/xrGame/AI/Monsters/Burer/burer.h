#pragma once
#include "../../biting/ai_biting.h"
#include "burer_state_manager.h"
#include "../../telekinesis.h"

class CCharacterPhysicsSupport;

class CBurer :	public CAI_Biting,
				public CStateManagerBurer,
				public CTelekinesis<CBurer>,
				public CPHUpdateObject /* make this object update physics correctly */ {

	typedef		CAI_Biting					inherited;
public:
	typedef		CTelekinesis<CBurer>		TTelekinesis;

public:
					CBurer				();
	virtual			~CBurer				();	

	

			void	Init				();
	virtual void	reinit				();

	virtual void	Load				(LPCSTR section);

	virtual	void	shedule_Update		(u32 dt);

	virtual void	StateSelector		();
			void	ProcessTurn			();

	virtual bool	UpdateStateManager	();

	virtual void 	PhDataUpdate		(dReal step);
	virtual void 	PhTune				(dReal step) {}
	virtual u8		get_legs_number		() {return BIPEDAL;}
};

