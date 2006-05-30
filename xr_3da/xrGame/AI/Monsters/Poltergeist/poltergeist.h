#pragma once
#include "../BaseMonster/base_monster.h"
#include "../telekinesis.h"
#include "../energy_holder.h"
#include "../../../script_export_space.h"

class CPhysicsShellHolder;
class CStateManagerPoltergeist;
class CPoltergeisMovementManager;

class CPoltergeist :	public CBaseMonster ,
						public CTelekinesis,
						public CEnergyHolder {
	
	typedef		CBaseMonster	inherited;
	typedef		CEnergyHolder	Energy;

	friend class CPoltergeisMovementManager;


	CParticlesObject	*m_particles_object;

	float				m_height;

	LPCSTR				m_particles_hidden;
	LPCSTR				m_particles_hide;

	bool				m_disable_hide;

	SMotionVel			invisible_vel;

	//telekinesis
	float				m_tele_radius;
	u32					m_tele_hold_time;
	float				m_tele_fly_time;

public:
	struct SDelay {
		u32 min, normal, aggressive;
	} m_flame_delay, m_tele_delay, m_scare_delay;

public:
					CPoltergeist		();
	virtual			~CPoltergeist		();	

	virtual void	Load				(LPCSTR section);
	virtual void	reload				(LPCSTR section);
	virtual void	reinit				();

	virtual BOOL	net_Spawn			(CSE_Abstract* DC);
	virtual void	net_Destroy			();
	virtual void	net_Relcase			(CObject *O);

	virtual void	UpdateCL			();
	virtual	void	shedule_Update		(u32 dt);

	virtual void	Die					(CObject* who);

	virtual CMovementManager *create_movement_manager();
	
	virtual void	ForceFinalAnimation	();

	virtual	void	on_activate			();
	virtual	void	on_deactivate		();


			bool	is_hidden			() {return state_invisible;}

	// FireBall

			void	LoadFlame				(LPCSTR section);
			void	FireFlame				(const CObject *target_object);
			bool	GetValidFlamePosition	(const CObject *target_object, Fvector &res_pos);
			void	UpdateFlame				();
			void	RemoveFlames			();
	
			// ltx params
			ref_sound			m_flame_sound;
			LPCSTR				m_flame_particles_prepare;
			LPCSTR				m_flame_particles_fire;
			u32					m_flame_fire_delay;
			float				m_flame_length;
			float				m_flame_hit_value;


			struct SFlameElement {
				const CObject		*target_object;
				Fvector				position;
				Fvector				target_dir;
				u32					time_started;
				ref_sound			sound;
			};
			
			DEFINE_VECTOR(SFlameElement*, FLAME_ELEMS_VEC, FLAME_ELEMS_IT);
			FLAME_ELEMS_VEC	m_flames;
		

	// Poltergeist ability
			void	PhysicalImpulse		(const Fvector &position);

			void	StrangeSounds		(const Fvector &position);
			
			ref_sound m_strange_sound;

	// Telekinesis
	
			void	ProcessTelekinesis	(const CObject *target);
			void	UpdateTelekinesis	();

			u32					time_tele_start;
			const CObject		*tele_enemy;
			CPhysicsShellHolder *tele_object;

	// Movement
			Fvector m_current_position;		// Позиция на ноде

	// Dynamic Height
			u32		time_height_updated;
			float	target_height;

			void	UpdateHeight			();

	// Invisibility 

			void	EnableHide				(){m_disable_hide = false;}
			void	DisableHide				(){m_disable_hide = true;}
	

private:
			void	Hide					();
			void	Show					();


public:
#ifdef DEBUG
			virtual CBaseMonster::SDebugInfo show_debug_info();
#endif


	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CPoltergeist)
#undef script_type_list
#define script_type_list save_type_list(CPoltergeist)























