#pragma once 

class CBitingControlled : public IState {
	typedef IState inherited;
	CAI_Biting		*pMonster;

public:
					CBitingControlled	(CAI_Biting *p);
	virtual void	Init				();
	virtual void	Run					();
};
