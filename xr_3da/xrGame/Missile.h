#pragma once
#include "hud_item_object.h"
#include "HudSound.h"

#define MS_HIDDEN	0
#define MS_SHOWING	1
#define MS_IDLE		2
#define MS_THREATEN	3
#define MS_READY	4
#define MS_THROW	5
#define MS_END		6
#define MS_EMPTY	7
#define MS_HIDING	8
#define MS_PLAYING	9

class CMissile : public CHudItemObject
{
	typedef CHudItemObject inherited;
public:
			CMissile	(void);
	virtual ~CMissile	(void);

	virtual BOOL						AlwaysTheCrow				()				{ return TRUE; }
	virtual void						OnDrawUI					();

	virtual void						reinit						();
	virtual CMissile		*			cast_missile				()				{return this;}

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(CSE_Abstract* DC);
	virtual void net_Destroy();

	virtual void UpdateCL();
	virtual void shedule_Update(u32 dt);
	virtual void renderable_Render();

	virtual void OnH_B_Chield();
	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent();
	virtual void OnH_A_Independent();

	virtual void OnEvent(NET_Packet& P, u16 type);

	virtual void OnAnimationEnd();

	virtual void Show();
	virtual void Hide();
	virtual bool IsHidden() const {return m_state == MS_HIDDEN;}
	virtual bool IsHiding() const {return m_state == MS_HIDING;}

	virtual void Throw();
	virtual void Destroy();

	virtual bool Action(s32 cmd, u32 flags);

	IC u32		 State			()				{return m_state;}
	virtual u32	 State			(u32 state);
	virtual void OnStateSwitch	(u32 S);
protected:
	virtual void	UpdateFireDependencies_internal	();
	virtual void	UpdateXForm						();
	void			UpdatePosition					(const Fmatrix& trans);
	void			spawn_fake_missile				();

	//������������� ���� ���� � �������� ����� ��� �������� �� OnH_B_Chield
	virtual void	OnActiveItem		();
	virtual void	OnHiddenItem		();

	//��� ����
	virtual void	StartIdleAnim		();
protected:

	u32 m_state;
	bool m_throw;
	

	//����� �����������
	u32 m_dwDestroyTime;
	u32 m_dwDestroyTimeMax;

	Fvector					m_throw_direction;
	Fmatrix					m_throw_matrix;

	CMissile				*m_fake_missile;

	//��������� ������
	
	float m_fMinForce, m_fConstForce, m_fMaxForce, m_fForceGrowSpeed;
//private:
	bool m_constpower;
	float m_fThrowForce;
protected:
	//������������� ����� � ����������� ������ �������
	Fvector m_vThrowPoint;
	Fvector m_vThrowDir;
	//��� HUD
	Fvector m_vHudThrowPoint;
	Fvector m_vHudThrowDir;

	//����� ��������
	shared_str	m_sAnimShow;
	shared_str	m_sAnimHide;
	shared_str	m_sAnimIdle;
	shared_str	m_sAnimPlaying;
	shared_str	m_sAnimThrowBegin;
	shared_str	m_sAnimThrowIdle;
	shared_str	m_sAnimThrowAct;
	shared_str	m_sAnimThrowEnd;

	//���� �������� "�������"
	HUD_SOUND sndPlaying;

protected:
			void	setup_throw_params		();
public:
	virtual void	activate_physic_shell	();
	virtual void	setup_physic_shell		();
	virtual void	create_physic_shell		();
	IC		void	set_destroy_time		(u32 delta_destroy_time)
	{
		m_dwDestroyTime		= delta_destroy_time + Device.dwTimeGlobal;
	}

protected:
	u32						m_ef_weapon_type;

public:
	virtual u32				ef_weapon_type		() const;
	IC		u32				destroy_time		() const {return m_dwDestroyTime;};
};