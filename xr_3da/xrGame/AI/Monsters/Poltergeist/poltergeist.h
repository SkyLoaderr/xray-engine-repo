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

			void	LoadFlame			(LPCSTR section);
			void	StartFlame			();
			void	FireFlame			();
			
			struct {
				ref_sound	sound;
				LPCSTR		particle;
			} m_flame_prepare, m_flame_fire, m_flame_finalize;

			Fvector m_flame_position;

};

























