#pragma once
#include "control_combase.h"

class CControlRunAttack : public CControl_ComCustom<> {
public:
	virtual void	reinit					();

	virtual void	on_event				(ControlCom::EEventType, ControlCom::IEventData*);
	virtual void	activate				();
	virtual void	on_release				();
	virtual bool	check_start_conditions	();
};

