#pragma once
#include "../../biting/ai_biting.h"
#include "../../telekinesis.h"
#include "../../anim_triple.h"

class CCharacterPhysicsSupport;

class CBurer :	public CAI_Biting,
				public CTelekinesis {

	typedef		CAI_Biting					inherited;
public:
	typedef		CTelekinesis				TTelekinesis;

	struct	GraviObject {
		bool		active;
		Fvector		cur_pos;
		Fvector		target_pos;
		Fvector		from_pos;

		u32			time_last_update;

		const CEntityAlive *enemy;
		
		GraviObject() {
			active = false;
			enemy = 0;
		}
		
		
		void		activate(const CEntityAlive *e, const Fvector &cp, const Fvector &tp) {
			active				= true;
			from_pos			= cp;
			cur_pos				= cp;
			target_pos			= tp;
			time_last_update	= Level().timeServer();
			enemy				= e;
		}

		void		deactivate() {
			active = false;
		}

	} m_gravi_object;

	LPCSTR	 particle_gravi_wave;
	LPCSTR   particle_gravi_prepare;
	LPCSTR	 particle_tele_object;

	ref_sound	sound_gravi_wave;

	u32		m_gravi_speed;
	u32		m_gravi_step;
	u32		m_gravi_time_to_hold;
	float	m_gravi_radius;
	float	m_gravi_impulse_to_objects;
	float	m_gravi_impulse_to_enemy;
	float	m_gravi_hit_power;
	

	u32		m_tele_max_handled_objects;
	u32		m_tele_time_to_hold;
	float	m_tele_object_min_mass;
	float	m_tele_object_max_mass;
	float	m_tele_find_radius;


	

public:
					CBurer				();
	virtual			~CBurer				();	


	virtual void	reinit				();
	virtual void	reload				(LPCSTR section);

	virtual void	Load				(LPCSTR section);

	virtual	void	shedule_Update		(u32 dt);
	virtual void	UpdateCL			();

	virtual void	StateSelector		();
			void	ProcessTurn			();

	virtual bool	UpdateStateManager	();

	virtual u8		get_legs_number		() {return BIPEDAL;}

	virtual void	CheckSpecParams		(u32 spec_params);

			void	UpdateGraviObject	();

			void	StartGraviPrepare	();
			void	StopGraviPrepare	();

			void	StartTeleObjectParticle(CGameObject *pO);
			void	StopTeleObjectParticle(CGameObject *pO);

private:

	IState			*stateBurerAttack;

public:
	CAnimTriple		anim_triple_gravi;
	CAnimTriple		anim_triple_tele;
};

