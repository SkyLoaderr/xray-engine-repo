#pragma once
#include "../../biting/ai_biting.h"

class CPoltergeist : public CAI_Biting {
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

	virtual void	ForceFinalAnimation	();

			void	Hide				();
			void	Show				();

	// Particles 

			void	StartParticles		();
			void	StopParticles		();
			void	UpdateParticles		();
		
	// FireBall
			
			enum {
				flamePrepare,
				flameFire,
				flameFinalize,
				flameNone
			} m_flame_state;

			u32 flame_state_started;

			void	LoadFlame			(LPCSTR section);
			void	StartFlame			(const Fvector &);
			void	FireFlame			();
			void	FinalizeFlame		();
			void	UpdateFlame			();
			void	PlayFlameParticles	(CParticlesObject *&obj, LPCSTR name, const Fvector &position);

			struct {
				ref_sound			sound;
				LPCSTR				particles;
				CParticlesObject	*ps_object;
				u32					time;
			} m_flame_prepare, m_flame_fire, m_flame_finalize;

			Fvector m_flame_position;


	// Poltergeist ability
			void	PhysicalImpulse		(const Fvector &position);

			void	StrangeSounds		(const Fvector &position);
			
			ref_sound m_strange_sound;

	

};

























