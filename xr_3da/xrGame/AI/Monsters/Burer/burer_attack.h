#pragma once

class CEntityAlive;
class CBurer;

class CBurerAttack : public IState {
	typedef	IState inherited;
	CBurer	*pMonster;

	enum {
		ACTION_MELEE,
		ACTION_TELE,
		ACTION_GRAVI_START,
		ACTION_GRAVI_CONTINUE,
		ACTION_GRAVI_FIRE,

		ACTION_DEFAULT,
	} m_tAction, m_tPrevAction;

	const CEntityAlive *enemy;

public:	
					CBurerAttack		(CBurer *p);

	virtual	void	Init				();
	virtual void	Run					();

private:
			void	Execute_Telekinetic		();
			
			void	Execute_Gravi_Start		();
			void	Execute_Gravi_Continue	();
			void	Execute_Gravi_Fire		(){}

			void	Execute_Melee			();


			u32		get_number_available_objects(xr_vector<CObject*> &tpObjects);

			// checking			
			bool	CheckGraviStart			();
			bool	CheckGraviContinue		();

			void	update					();
};
