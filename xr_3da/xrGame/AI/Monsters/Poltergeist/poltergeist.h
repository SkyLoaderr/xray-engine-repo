#pragma once
#include "../../biting/ai_biting.h"

class CPoltergeist : public CAI_Biting {
	typedef		CAI_Biting	inherited;
public:
					CPoltergeist		();
	virtual			~CPoltergeist		();	

	virtual void	Load				(LPCSTR section) {}
	virtual u8		get_legs_number		() {return BIPEDAL;}
	
	//virtual	void	CheckSpecParams		(u32 spec_params);
	//virtual bool	UpdateStateManager	();
};
