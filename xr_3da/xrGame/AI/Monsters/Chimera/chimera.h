#pragma once
#include "../../biting/ai_biting.h"

class CStateManagerChimera;

class CChimera : public CAI_Biting {

	typedef		CAI_Biting	inherited;

	bool		b_upper_state;

	CStateManagerChimera	*StateMan;
public:
					CChimera			();
	virtual			~CChimera			();	

	virtual void	Load				(LPCSTR section);

			void	Init				();
	virtual void	reinit				();
	virtual void	StateSelector		();

	virtual	void	ProcessTurn			();

	virtual bool	UpdateStateManager	();
	virtual void	CheckSpecParams		(u32 spec_params);

			void	SetUpperState		(bool state = true) {b_upper_state = state;}

	virtual u8		get_legs_number		() {return QUADRUPEDAL;}

};


