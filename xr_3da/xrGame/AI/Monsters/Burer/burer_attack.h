#pragma once

class CEntityAlive;
class CBurer;

class CBurerAttack : public IState {
	typedef	IState inherited;
	CBurer	*pMonster;

	enum {
		ACTION_MELEE,
		ACTION_TELE,
		ACTION_GRAVI_STARTED,
		ACTION_GRAVI_CONTINUE,
		ACTION_GRAVI_FIRE,

		ACTION_DEFAULT,
	} m_tAction, m_tPrevAction;

	const CEntityAlive *enemy;

	TTime time_next_gravi_available;
	TTime time_gravi_started;

public:	
					CBurerAttack		(CBurer *p);

	virtual	void	Init				();
	virtual void	Run					();

private:
		// Checking			
			bool	CheckGravi			();
			bool	CheckGraviFire		();
	
		// Executing
			void	Execute_Telekinetic		();
			void	Execute_Gravi			();
			void	Execute_Gravi_Fire		();
			void	Execute_Melee			();

		// Updating state
			
			void	update					();
		
		// additional stuff
			u32		get_number_available_objects(xr_vector<CObject*> &tpObjects);
			void	find_tele_objects		();
};
