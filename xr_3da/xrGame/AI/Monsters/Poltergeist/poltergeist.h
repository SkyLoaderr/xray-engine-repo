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

	virtual void	net_Destroy			();
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
};

























