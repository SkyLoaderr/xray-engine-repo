#pragma once

#include "telekinetic_object.h"
#include "../PHObject.h"


class CTelekinesis : public CPHUpdateObject /* make this object update physics correctly */ {
	xr_vector<CTelekineticObject>	objects;
	bool							active;

	u32								max_time_keep;
	float							height;
	float							strength;

public:
					CTelekinesis		();
	virtual			~CTelekinesis		();

			void	InitExtern			(float s, float h, u32 keep_time);
	
			void	Activate			(const Fvector &pos);
			void	Deactivate			();
			bool	IsActive			() {return active;}

			void	Throw				(const Fvector &target);
	
	
			void	UpdateSched			();
private:

	virtual void 	PhDataUpdate		(dReal step);
	virtual void 	PhTune				(dReal step);
};

