#pragma once
#include "../psy_aura.h"
#include "../../../pp_effector_distance.h"

class CActor;

//////////////////////////////////////////////////////////////////////////
// CPsyAuraController
//////////////////////////////////////////////////////////////////////////

class CPsyAuraController : public CPsyAura {
	typedef CPsyAura inherited;

	// звук, исходящий от владельца при входе в поле
	ref_sound			m_sound;

	// эффектор у актера при входе в поле
	CPPEffectorDistance	m_effector;

	CActor				*m_actor;

	float				m_current_radius;

	float				power_down_vel;

public:
					CPsyAuraController		();
	virtual			~CPsyAuraController		();

	virtual void	reload					(LPCSTR section);
	virtual void	reinit					();
	
	virtual void	on_activate				();
	virtual void	on_deactivate			();

	virtual void	schedule_update			();	
	virtual void	frame_update			();
	virtual BOOL	feel_touch_contact		(CObject* O);
	virtual void	feel_touch_new			(CObject* O);
	virtual void	feel_touch_delete		(CObject* O);

			bool	effector_active			() {return m_effector.IsActive();}
};

