#pragma once

#define MIN_LEGS_COUNT	1
#define MAX_LEGS_COUNT	4 

struct SStepParam {
	struct{
		float	time;
		float	power;
	} step[MAX_LEGS_COUNT];

	u8			cycles;
};

DEFINE_MAP(ref_str,	SStepParam, STEPS_MAP, STEPS_MAP_IT);

struct SStepInfo {
	struct {
		ref_sound		sound;

		bool			handled;		// обработан
		u8				cycle;			// цикл в котором отработан
	} activity[MAX_LEGS_COUNT];

	SStepParam		params;
	bool			disable;

	u8				cur_cycle;

	SStepInfo()		{disable = true;}
};

enum ELegType {
	eFrontLeft,
	eFrontRight,
	eBackRight,
	eBackLeft
};


class CCustomMonster;

class CStepManager {
	u8				m_legs_count;

	STEPS_MAP		m_steps_map;
	SStepInfo		m_step_info;

	CCustomMonster	*m_object;

	u16				m_foot_bones[MAX_LEGS_COUNT];
	ref_str			m_animation;

	u32				m_time_anim_started;


public: 
					CStepManager				();
	virtual			~CStepManager				();

			void	init_external				(CCustomMonster	*obj) {m_object = obj;}
	virtual	void	load						(LPCSTR section);
	virtual void	reinit						();

	virtual	void	on_animation_start			(ref_str anim);
			void	update						();

	virtual	void	event_on_step				() {}	
	virtual float	get_current_animation_time	() {return 1.f;}

protected:
			Fvector	get_foot_position			(ELegType leg_type);
			void	reload_foot_bones			();
};
