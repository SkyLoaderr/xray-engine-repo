#pragma once

class CEntityAlive;
class CBurer;

class CBurerAttack : public IState {
	typedef	IState inherited;
	CBurer	*pMonster;

	const CEntityAlive *enemy;

	IState				*stateTele;
	IState				*stateMelee;
	IState				*stateGravi;
	IState				*stateRunAround;
	IState				*stateFaceEnemy;

	IState				*cur_state;

	bool				b_need_reselect;
	bool				prev_state_run_around;

public:	
					CBurerAttack		(CBurer *p);
	virtual			~CBurerAttack		();

	virtual	void	Init				();
	virtual void	Run					();
	virtual void	Done				();

private:
			void	select_state		(IState *state);
			
	// выбор состояния
			void	ReselectState		();
};


