#pragma once

class CAnimTriple;
class CBaseMonster;
class CMotionDef;

class CJumpingAbility {
	CBaseMonster	*m_object;
	
	CAnimTriple		*m_animation_holder;

	u32				m_time_next_allowed;
	u32				m_time_started;
	
	u32				m_delay_after_jump;
	float			m_jump_factor;

	bool			m_active;

	CMotionDef		*m_def_glide;
	float			m_jump_time;

	float			m_blend_speed;

public:
					CJumpingAbility		();
	virtual			~CJumpingAbility	();

	virtual void	init_external		(CBaseMonster *obj, CMotionDef *m_def1, CMotionDef *m_def2, CMotionDef *m_def3);

	virtual void	jump				(Fvector target);
	virtual void	update_frame		();
	virtual void	update_scheduled	();
	virtual void	pointbreak			();
	
	virtual void	set_animation_speed	();

			bool	active				() {return m_active;}

			void	stop				();
	// service routines
			Fvector get_target			(CObject *obj);

			bool	hit_test			();
			bool	is_landing			();
};

