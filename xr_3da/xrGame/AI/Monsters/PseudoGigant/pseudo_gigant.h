#pragma once
#include "../BaseMonster/base_monster.h"
#include "../controlled_entity.h"
#include "../../../script_export_space.h"


class CPseudoGigant : public CBaseMonster,
					  public CControlledEntity<CPseudoGigant> {
	
	typedef		CBaseMonster						inherited;
	typedef		CControlledEntity<CPseudoGigant>	CControlled;

	// step_effector
	struct {
		float time;
		float amplitude;	
		float period_number;
	} step_effector;

	SAttackEffector m_threaten_effector;
	ref_sound		m_sound_threaten_hit;		// ����, ������� �������� � ������ � ������
	ref_sound		m_sound_start_threaten;		// ����, ������� �������� � ������ � ������
	
	u32				m_time_last_threaten;

	float			m_kick_damage;
		
public:
					CPseudoGigant				();
	virtual			~CPseudoGigant				();	

	virtual void	Load				(LPCSTR section);
	virtual void	reinit				();

	virtual bool	ability_earthquake	() {return true;}
	virtual void	event_on_step		();

	virtual bool	check_start_conditions	(ControlCom::EControlType type);
	virtual void	on_activate_control		(ControlCom::EControlType);

	virtual	void	on_threaten_execute	();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPseudoGigant)
#undef script_type_list
#define script_type_list save_type_list(CPseudoGigant)
