#pragma once
#include "../../biting/ai_biting.h"
#include "burer_state_manager.h"
#include "../../telekinesis.h"
#include "../../anim_triple.h"

class CCharacterPhysicsSupport;

class CBurer :	public CAI_Biting,
				public CStateManagerBurer,
				public CTelekinesis {

	typedef		CAI_Biting					inherited;
public:
	typedef		CTelekinesis				TTelekinesis;

	xr_vector<CGameObject *>			tele_objects;

public:
					CBurer				();
	virtual			~CBurer				();	

	

			void	Init				();
	virtual void	reinit				();
	virtual void	reload				(LPCSTR section);

	virtual void	Load				(LPCSTR section);

	virtual	void	shedule_Update		(u32 dt);

	virtual void	StateSelector		();
			void	ProcessTurn			();

	virtual bool	UpdateStateManager	();

	virtual u8		get_legs_number		() {return BIPEDAL;}

	virtual void	CheckSpecParams		(u32 spec_params);

private:

	IState			*stateBurerAttack;

public:
	CAnimTriple		anim_triple_gravi;
	CAnimTriple		anim_triple_tele;
};

