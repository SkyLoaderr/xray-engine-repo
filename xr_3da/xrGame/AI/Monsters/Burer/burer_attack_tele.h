#pragma once

class CBurer;
class CGameObject;
class CEntityAlive;

class CBurerAttackTele : public IState {
	typedef	IState inherited;
	CBurer	*pMonster;

	const CEntityAlive			*enemy;
	xr_vector<CGameObject *>	tele_objects;
	
	CGameObject					*selected_object;

	enum {
		ACTION_TELE_STARTED,
		ACTION_TELE_CONTINUE,
		ACTION_TELE_FIRE,
		ACTION_WAIT_TRIPLE_END,
		ACTION_COMPLETED,
	} m_tAction;

	TTime						time_started;
	TTime						time_enemy_last_faced;

public:

					CBurerAttackTele	(CBurer *p);

	virtual	void	Init				();
	virtual void	Run					();
	virtual void	Done				();

	virtual bool	CheckStartCondition ();
	virtual bool	IsCompleted			();

	virtual void	CriticalInterrupt	();
	virtual void	UpdateExternal		();

private:
			// ����� �������� ��� ����������	
			void	FindObjects			();

			// ��������� ���������
			void	ExecuteTeleStart	();
			void	ExecuteTeleContinue ();
			void	ExecuteTeleFire		();

			// ��������, ���� �� ���� ���� ������ ��� ���������
			bool	IsActiveObjects		();

			// ���������, ����� �� ���������� ���������
			bool	CheckTeleStart		();
			// ����� ���������� �������� ��� ����������
			void	SelectObjects		();
};

