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

					
			void	StartFireball		();
			void	StopFireball		();

			ref_str m_firball_sound_prepare;
			ref_str m_firball_sound_fire;
			ref_str m_firball_sound_finalize;

};
