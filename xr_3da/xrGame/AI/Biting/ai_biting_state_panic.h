#pragma once 

class CBitingPanic : public IState {
	CAI_Biting		*pMonster;

	typedef IState inherited;

	enum {
		ACTION_RUN, 
		ACTION_FACE_BACK_SCARED,
	} m_tAction;

public:
					CBitingPanic	(CAI_Biting *p);
	
	virtual bool	CheckCompletion	();

private:
	virtual void	Init			();
	virtual void	Run				();
};
