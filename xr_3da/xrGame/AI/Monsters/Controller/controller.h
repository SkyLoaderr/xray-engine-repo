#pragma once
#include "../../biting/ai_biting.h"

class CStateManagerController;

class CController : public CAI_Biting {
	typedef		CAI_Biting	inherited;

	CStateManagerController	*StateMan;

	u8					m_max_controlled_number;

public:
	xr_vector<CEntity*> m_controlled_objects;

public:
					CController			();
	virtual			~CController		();	

	virtual void	Load				(LPCSTR section);
	virtual u8		get_legs_number		() {return BIPEDAL;}
	
	virtual	void	CheckSpecParams		(u32 spec_params);
	virtual bool	UpdateStateManager	();

	// Controller ability
			bool	HasUnderControl		() {return (!m_controlled_objects.empty());}
			void	TakeUnderControl	(CEntity *);
			void	UpdateControlled	();

			void	set_controlled_task (u32 task);
	
};

