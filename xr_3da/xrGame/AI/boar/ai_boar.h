#pragma once

#include "../\biting/ai_biting.h"

class CAI_Boar : public CAI_Biting {

	typedef		CAI_Biting	inherited;
public:
					CAI_Boar			();
	virtual			~CAI_Boar			();	

	virtual void	Load				(LPCSTR section);

			void	Init				();
	virtual void	StateSelector		();

	virtual	void	LookPosition		(Fvector to_point, float angular_speed = PI_DIV_3);

	virtual bool	CanExecRotationJump	() {return true;}
	virtual void	CheckSpecParams		(u32 spec_params);
};
