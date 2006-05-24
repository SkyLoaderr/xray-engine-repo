#pragma once
////////////////////////////////////////////////////////////////////////
// Effector controlling class
////////////////////////////////////////////////////////////////////////
#include "../../../pp_effector_custom.h"

class CPPEffectorControllerAura : public CPPEffectorCustom {
	typedef CPPEffectorCustom inherited;

	enum {eStateFadeIn, eStateFadeOut, eStatePermanent} m_effector_state;

	u32				m_time_state_started;
	u32				m_time_to_fade;

public:
					CPPEffectorControllerAura	(const SPPInfo &ppi, u32 time_to_fade);
	virtual BOOL	update						();
	void			switch_off					();
};

class CController;

class CControllerAura : public CPPEffectorCustomController<CPPEffectorControllerAura>{
	CController				*m_object;
	u32						m_time_last_update;
public:
			CControllerAura					(CController *monster) : m_object(monster){}
	void	on_death						();
	void	update_schedule					();
};


