#pragma once


class CBitingAttack : public IState {
	typedef	IState inherited;
	CAI_Biting	*pMonster;

	enum {
		ACTION_RUN,
		ACTION_ATTACK_MELEE,
		ACTION_FIND_ENEMY,
		ACTION_STEAL,
		ACTION_THREATEN,
		ACTION_ENEMY_POSITION_APPROACH
	} m_tAction,m_tPrevAction;

	VisionElem		m_tEnemy;

	float			m_fDistMin;						//!< минимально допустимое расстояния для аттаки
	float			m_fDistMax;						//!< максимально допустимое расстояние для аттаки
	float			dist;							// текущая дистанция

	TTime			m_dwFaceEnemyLastTime;			// время последней установки target.yaw при развороте лицом к противнику 

	bool			m_bInvisibility;				// возможность нивидимости

	bool			bCanThreaten;					// может пугать
	TTime			ThreatenTimeStarted;			// время начала Threaten
	TTime			ThreatenMinDelay;
	
	bool			bEnableBackAttack;				// можно использовать BackAttack 

	TTime			LastTimeRebuild;				// время последнего обновления пути

	u32				init_flags;
	u32				frame_flags;
	u32				flags;

	bool			b_silent_run;					// не издавать звука во время бега

	bool			b_in_threaten;

	CJumping		*pJumping;

public:	
					CBitingAttack		(CAI_Biting *p, bool bVisibility);

	virtual	void	Init				();
	virtual void	Run					();
	virtual void	Done				();

			bool	CheckStartThreaten	();
			bool	CheckEndThreaten	();
			bool	CheckThreaten		();

			bool	CheckSteal			();
			Fvector RandomPos			(Fvector pos, float R);

			bool	CanAttackFromBack	();

			void	UpdateInitFlags		(); 
			void	UpdateFrameFlags	(); 

};
