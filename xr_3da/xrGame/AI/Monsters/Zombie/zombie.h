#pragma once
#include "../../biting/ai_biting.h"

class CStateManagerZombie;

class CZombie :	public CAI_Biting {
	
	typedef		CAI_Biting		inherited;

	CStateManagerZombie *StateMan;

public:
					CZombie		();
	virtual			~CZombie		();	

	virtual void	Load				(LPCSTR section);
	virtual void	reload				(LPCSTR section);
	virtual void	reinit				();

	virtual void	net_Destroy			();
	virtual void	UpdateCL			();
	virtual	void	shedule_Update		(u32 dt);

	virtual void	Die					();

	virtual void	ForceFinalAnimation	();

	virtual bool	UpdateStateManager	();
};
