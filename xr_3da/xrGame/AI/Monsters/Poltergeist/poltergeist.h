#pragma once
#include "../../biting/ai_biting.h"
#include "../../telekinesis.h"
#include "../../energy_holder.h"

class CPhysicsShellHolder;
class CStateManagerPoltergeist;

class CPoltergeist :	public CAI_Biting ,
						public CTelekinesis,
						public CEnergyHolder {
	
	typedef		CAI_Biting		inherited;
	typedef		CEnergyHolder	Energy;

	CParticlesObject	*m_particles_object;

	float				m_height;
	bool				m_hidden;

	LPCSTR				m_particles_hidden;

	CStateManagerPoltergeist *StateMan;

public:
					CPoltergeist		();
	virtual			~CPoltergeist		();	

	virtual void	Load				(LPCSTR section);
	virtual void	reload				(LPCSTR section);
	virtual void	reinit				();

	virtual void	net_Destroy			();
	virtual void	UpdateCL			();
	virtual	void	shedule_Update		(u32 dt);

	virtual void	Die					();

	virtual void	ForceFinalAnimation	();

	virtual bool	UpdateStateManager	();

	virtual	void	on_activate			();
	virtual	void	on_deactivate		();


			bool	is_hidden			() {return m_hidden;}

	// FireBall

			void	LoadFlame				(LPCSTR section);
			void	FireFlame				(CObject *target_object);
			bool	GetValidFlamePosition	(CObject *target_object, Fvector &res_pos);
			void	UpdateFlame				();
			void	RemoveFlames			();
	
			// ltx params
			ref_sound			m_flame_sound;
			LPCSTR				m_flame_particles;
			u32					m_flame_fire_delay;
			float				m_flame_length;
			float				m_flame_hit_value;


			struct SFlameElement {
				CObject		*target_object;
				Fvector		position;
				Fvector		target_dir;
				u32			time_started;
				ref_sound	sound;
			};
			
			DEFINE_VECTOR(SFlameElement*, FLAME_ELEMS_VEC, FLAME_ELEMS_IT);
			FLAME_ELEMS_VEC	m_flames;
		

	// Poltergeist ability
			void	PhysicalImpulse		(const Fvector &position);

			void	StrangeSounds		(const Fvector &position);
			
			ref_sound m_strange_sound;

	// Telekinesis
	
			void	ProcessTelekinesis	(CObject *target);
			void	UpdateTelekinesis	();

			u32					time_tele_start;
			CObject				*tele_enemy;
			CPhysicsShellHolder *tele_object;



	// Movement
			Fvector m_current_position;		// Позиция на ноде

	virtual	void	move_along_path			(CPHMovementControl *movement_control, Fvector &dest_position, float time_delta);
			Fvector	CalculateRealPosition	();


	// Dynamic Height
			u32		time_height_updated;
			float	target_height;

			void	UpdateHeight			();

private:
			void	Hide					();
			void	Show					();

};

























