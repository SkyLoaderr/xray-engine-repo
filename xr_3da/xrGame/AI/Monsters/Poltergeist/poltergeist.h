#pragma once
#include "../../biting/ai_biting.h"
#include "../../telekinesis.h"

class CPoltergeist :	public CAI_Biting ,
						public CTelekinesis {
	
	typedef		CAI_Biting	inherited;

	CParticlesObject	*m_particles_object;

	float				m_height;
	bool				m_hidden;

	LPCSTR				m_particles_hidden;

public:
					CPoltergeist		();
	virtual			~CPoltergeist		();	

	virtual void	Load				(LPCSTR section);
	virtual void	reload				(LPCSTR section);
	virtual void	StateSelector		();

	virtual void	UpdateCL			();
	virtual	void	shedule_Update		(u32 dt);

	virtual void	ForceFinalAnimation	();

			void	Hide				();
			void	Show				();

	// Particles 

			void	StartParticles		();
			void	StopParticles		();
			void	UpdateParticles		();
		
	// FireBall

			

			void	LoadFlame			(LPCSTR section);
			void	FireFlame			(const Fvector &position, CObject *target_object);
			void	UpdateFlame			();
	
			ref_sound			m_flame_sound;
			LPCSTR				m_flame_particles;
			u32					m_flame_fire_delay;
			float				m_flame_length;
			float				m_flame_hit_value;

			Fvector				m_flame_position;
			Fvector				m_flame_target_dir;
			CObject				*m_flame_target_object;
			u32					m_time_flame_started;
			

	// Poltergeist ability
			void	PhysicalImpulse		(const Fvector &position);

			void	StrangeSounds		(const Fvector &position);
			
			ref_sound m_strange_sound;
};

























