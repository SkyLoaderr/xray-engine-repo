#pragma once

#include "../\biting/ai_biting.h"
#include "../\ai_monster_jump.h"

class CAI_PseudoDog : public CAI_Biting, public CJumping {
	typedef		CAI_Biting	inherited;

	bool			strike_in_jump;

	SAttackEffector m_psi_effector;
public:
					CAI_PseudoDog		();
	virtual			~CAI_PseudoDog		();	

	virtual void	Load				(LPCSTR section);

			void	Init				();
	virtual void	StateSelector		();
	virtual void	UpdateCL			();

	virtual void	OnJumpStop			();
	virtual bool	CanJump				() {return true;}
	
	virtual	void	ProcessTurn			();

	virtual bool	ability_can_drag	() {return true;}
	virtual bool	ability_psi_attack	() {return true;}

	virtual void	CheckSpecParams		(u32 spec_params);

};
