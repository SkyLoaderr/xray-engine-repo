#pragma once

class CEntityAlive;

class CBaseMonsterAttack : public IState {
	typedef	IState inherited;
	CBaseMonster	*pMonster;

	enum {
		ACTION_RUN,
		ACTION_ATTACK_MELEE,
		ACTION_FIND_ENEMY,
		ACTION_STEAL,
		ACTION_THREATEN,
		ACTION_ENEMY_POSITION_APPROACH,
		ACTION_ENEMY_WALK_AWAY,
		ACTION_ROTATION_JUMP,
		ACTION_ATTACK_RUN,
		ACTION_PSI_ATTACK,
	} m_tAction,m_tPrevAction;

	const CEntityAlive *enemy;

	float			m_fDistMin;						//!< ���������� ���������� ���������� ��� ������
	float			m_fDistMax;						//!< ����������� ���������� ���������� ��� ������
	float			dist;							// ������� ���������

	TTime			m_dwFaceEnemyLastTime;			// ����� ��������� ��������� target.yaw ��� ��������� ����� � ���������� 

	bool			bCanThreaten;					// ����� ������
	TTime			ThreatenTimeStarted;			// ����� ������ Threaten
	TTime			ThreatenMinDelay;
	
	bool			bEnableBackAttack;				// ����� ������������ BackAttack 

	TTime			LastTimeRebuild;				// ����� ���������� ���������� ����

	Flags32			init_flags;
	Flags32			frame_flags;
	Flags32			flags;

	bool			b_in_threaten;

	CJumping		*pJumping;

	TTime			next_rot_jump_enabled;
	TTime			time_start_walk_away;			// ����� ������ ��������� ACTION_ENEMY_WALK_AWAY (������ �� ����� ��������� �� ������)
	
	TTime			time_next_attack_run;
	TTime			time_next_psi_attack;

public:	
					CBaseMonsterAttack		(CBaseMonster *p);

	virtual	void	Init				();
	virtual void	Run					();
	virtual void	Done				();
	
	virtual bool	CheckCompletion		();

			bool	CheckStartThreaten	();
			bool	CheckEndThreaten	();
			bool	CheckThreaten		();

			bool	CheckSteal			();
			Fvector RandomPos			(Fvector pos, float R);

			bool	CanAttackFromBack	();

			void	UpdateInitFlags		(); 
			void	UpdateFrameFlags	(); 

			bool	CheckRotationJump	();

			bool	CanAttackRun		();

			bool	CheckPsiAttack		();


	virtual bool	GetStateAggressiveness	(){return true;}
};
