#pragma once

class CEntityAlive;
class CBurer;

class CBurerAttack : public IState {
	typedef	IState inherited;
	CBurer	*pMonster;

	enum {
		
		ACTION_RUN,
		ACTION_MELEE,
		
		ACTION_TELE_STARTED,
		ACTION_TELE_CONTINUE,
		ACTION_TELE_FIRE,

		ACTION_GRAVI_STARTED,
		ACTION_GRAVI_CONTINUE,
		ACTION_GRAVI_FIRE,
		
		ACTION_WAIT_TRIPLE_END,

		ACTION_RUN_AROUND,
		ACTION_FACE_ENEMY,

		ACTION_DEFAULT,
	} m_tAction, m_tPrevAction;

	const CEntityAlive *enemy;

	TTime time_next_gravi_available;
	TTime time_gravi_started;

	TTime LastTimeRebuild;
	TTime m_dwFaceEnemyLastTime;

	Fvector selected_random_point;

	TTime run_around_time_started;

public:	
					CBurerAttack		(CBurer *p);

	virtual	void	Init				();
	virtual void	Run					();

private:
		// Checking			
			bool	CheckTele			();
			bool	CheckGravi			();
	
		// Executing
			void	Execute_Tele		();
			void	Execute_Gravi		();

		
		// additional stuff
			void	find_tele_objects		();
};
