#pragma once
#include "../../biting/ai_biting.h"

class CPseudoGigant : public CAI_Biting {
	typedef		CAI_Biting	inherited;

public:
					CPseudoGigant				();
	virtual			~CPseudoGigant				();	

	virtual void	Load				(LPCSTR section);

			void	Init				();
	virtual void	StateSelector		();
	virtual	void	ProcessTurn			();
	virtual u8		get_legs_number		() {return BIPEDAL;}
};

