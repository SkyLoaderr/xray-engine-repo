#pragma once
#include "control_combase.h"

class CControlRotationJump : public CControl_ComCustom<> {
	u32				m_time_next_rotation_jump;
public:
	virtual void	reinit					();

	virtual void	on_event				(ControlCom::EEventType, ControlCom::IEventData*);
	virtual void	activate				();
	virtual void	on_release				();
	virtual bool	check_start_conditions	();
};

