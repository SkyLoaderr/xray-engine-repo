#pragma once

#include "../effectorPP.h"
#include "../cameramanager.h"

//////////////////////////////////////////////////////////////////////////
// CPPEffectDistance
//////////////////////////////////////////////////////////////////////////
class CPPEffectDistance : public CEffectorPP {
	typedef CEffectorPP inherited;

	SPPInfo		state;
	float		factor;

public:
					CPPEffectDistance	(const SPPInfo &ppi);
	virtual			~CPPEffectDistance	();

			void	Update				(float new_factor) {factor = new_factor;}
			void	Destroy				();

protected:
	virtual	BOOL	Process				(SPPInfo& pp);

};

//////////////////////////////////////////////////////////////////////////
// CPPEffectorDistance
//////////////////////////////////////////////////////////////////////////

class CPPEffectorDistance {
	SPPInfo			state;
	float			r_min_perc;		// min_radius (percents [0..1])
	float			r_max_perc;		// max_radius (percents [0..1])
	float			radius;

	CPPEffectDistance *p_effector;

public:
				CPPEffectorDistance		();
	virtual 	~CPPEffectorDistance	();

			void	Load				(LPCSTR section);
			void	SetRadius			(float r) {radius = r;}
			void	Update				(float dist);

			bool	IsActive			() {return (p_effector != 0);}

private:
			void	Activate			();
};



