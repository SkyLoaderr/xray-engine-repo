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
		ACTION_ENEMY_POSITION_APPROACH,
		ACTION_ROTATION_JUMP
	} m_tAction,m_tPrevAction;

	VisionElem		m_tEnemy;

	float			m_fDistMin;						//!< ���������� ���������� ���������� ��� ������
	float			m_fDistMax;						//!< ����������� ���������� ���������� ��� ������
	float			dist;							// ������� ���������

	TTime			m_dwFaceEnemyLastTime;			// ����� ��������� ��������� target.yaw ��� ��������� ����� � ���������� 

	bool			m_bInvisibility;				// ����������� �����������

	bool			bCanThreaten;					// ����� ������
	TTime			ThreatenTimeStarted;			// ����� ������ Threaten
	TTime			ThreatenMinDelay;
	
	bool			bEnableBackAttack;				// ����� ������������ BackAttack 

	TTime			LastTimeRebuild;				// ����� ���������� ���������� ����

	Flags32			init_flags;
	Flags32			frame_flags;
	Flags32			flags;

	bool			b_silent_run;					// �� �������� ����� �� ����� ����
	bool			b_in_threaten;

	CJumping		*pJumping;

	TTime			next_rot_jump_enabled;

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

			bool	CheckRotationJump	();
};
