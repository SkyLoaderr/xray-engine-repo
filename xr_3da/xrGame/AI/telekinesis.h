#pragma once

#include "../entity_alive.h"
#include "../PhysicsShell.h"
#include "../PHObject.h"

enum ETelekineticState {
	TS_None,
	TS_Raise,
	TS_Keep,
};

class CTelekineticObject {
public:
	ETelekineticState	state;
	CGameObject			*obj;
	float				start_y;
	u32					time_state_started;
	u32					last_time_updated;

	CTelekineticObject set(CGameObject *new_obj, ETelekineticState	new_state) {
		state				= new_state;
		obj					= new_obj;
		start_y				= obj->Position().y;
		time_state_started	= Level().timeServer();
		last_time_updated	= 0;
		return	*this;
	}
};


template <typename CMonster>
class CTelekinesis {
	xr_vector<CTelekineticObject>	objects;
	bool							active;

	CMonster						*monster;
	
	u32								max_time_keep;
	float							height;
	float							strength;


public:
			CTelekinesis		();
	virtual	~CTelekinesis		();

	void	InitExtern			(CMonster *pM, float s, float h, u32 keep_time);
	
	void	Activate			();
	void	Deactivate			();
	bool	IsActive			() {return active;}

	void	Throw				(const Fvector &target);
	
	void	UpdateCL			(float dt);
	void	UpdateSched			();

private:
	void	Raise				(CTelekineticObject &obj, float power);
	void	Keep				(CTelekineticObject &obj);
	void	Release				(CTelekineticObject &obj);
	void	Throw				(CTelekineticObject &obj, const Fvector &target);

	void	VelocityCorrection	(CPhysicsShell *pShell, float max_val);
};


#include "telekinesis_inline.h"

