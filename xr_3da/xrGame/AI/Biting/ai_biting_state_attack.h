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

	u32				init_flags;
	u32				frame_flags;
	u32				flags;

	CJumping		*pJumping;

public:	
					CBitingAttack		(CAI_Biting *p, bool bVisibility);

	virtual	void	Init				();
	virtual void	Run					();
	virtual void	Done				();

			bool	CheckThreaten		();
			Fvector RandomPos			(Fvector pos, float R);

			bool	CanAttackFromBack	();

			void	UpdateInitFlags		(); 
			void	UpdateFrameFlags	(); 

};
