#pragma once

#include "../effectorPP.h"
#include "../cameramanager.h"

class CZoneEffectPP : public CEffectorPP {
	typedef CEffectorPP inherited;

	SPPInfo		state;
	float		factor;

public:
					CZoneEffectPP	(const SPPInfo &ppi);
	virtual			~CZoneEffectPP	();
	
			void	Update			(float new_factor) {factor = new_factor;}
			void	Destroy			();
private:
	virtual	BOOL	Process			(SPPInfo& pp);
};


class CZoneEffector {
	SPPInfo			state;
	float			r_min_perc;		// min_radius (percents [0..1])
	float			r_max_perc;		// max_radius (percents [0..1])
	float			radius;
	
	CZoneEffectPP	*p_effector;

public:
			CZoneEffector	();
			~CZoneEffector	();

	void	Load			(LPCSTR section);
	void	SetRadius		(float r);
	void	Update			(float dist);
	void	Stop			();

	// Test
	enum PP_Type {
		DUALITY_V = 0, 
		DUALITY_H, 
		NOISE_INTENSITY,
		NOISE_GRAIN,
		NOISE_FPS,
		BLUR,
		GRAY,
		COLOR_BASE_R,
		COLOR_BASE_G,
		COLOR_BASE_B,
		COLOR_GRAY_R,
		COLOR_GRAY_G,
		COLOR_GRAY_B,
		COLOR_ADD_R,
		COLOR_ADD_G,
		COLOR_ADD_B,
		DIST_MIN,
		DIST_MAX
	};
	
	void	SetParam		(u32 type, float val);

private:
	void	Activate		();


};



