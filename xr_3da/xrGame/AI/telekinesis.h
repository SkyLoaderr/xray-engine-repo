#pragma once

#include "../entity_alive.h"
#include "../PhysicsShell.h"

enum ETelekineticState {
	TS_None,
	TS_Raise,
	TS_Keep,
};

class CTelekineticObject {
public:
	ETelekineticState	state;
	CEntityAlive		*pE;
	Fvector				start_pos;
	u32					time_state_started;

	CTelekineticObject set(CEntityAlive *new_obj, ETelekineticState	new_state) {
		state				= new_state;
		pE					= new_obj;
		start_pos			= pE->Position();
		time_state_started	= Level().timeServer();
		return	*this;
	}
};


template <typename CMonster>
class CTelekinesis {
	xr_vector<CTelekineticObject>	objects;
	bool							active;

	CMonster						*monster;

public:
	CTelekinesis	();
	virtual	~CTelekinesis	();

	void	Activate	();
	void	Deactivate	();
	bool	IsActive	() {return active;}

	void	Throw		(const Fvector &target);
	
	void	Update		(float dt);

private:
	void	Raise		(CEntityAlive *pE);
	void	Keep		(CEntityAlive *pE);
	void	Release		(CEntityAlive *pE);
	void	Throw		(CEntityAlive *pE, const Fvector &target);
};


#include "telekinesis_inline.h"

