#pragma once
#include "HudItem.h"

class CWeaponHUD;

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

class CMissile : public CHudItem
{
	typedef CHudItem inherited;
public:
			CMissile	(void);
	virtual ~CMissile	(void);

	virtual void reinit				();

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();

	virtual void UpdateCL();
	virtual void shedule_Update(u32 dt);
	virtual void renderable_Render();

	virtual void OnH_B_Chield();
	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent();

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
	virtual void UpdateFP();
	virtual void UpdateXForm();
	void UpdatePosition(const Fmatrix& trans);
	void spawn_fake_missile	();

	//инициализация если вещь в активном слоте или спрятана на OnH_B_Chield
	virtual void	OnActiveItem		();
	virtual void	OnHiddenItem		();

	//для сети
	virtual void	StartIdleAnim		();
protected:

	u32 m_state;
	bool m_throw;

	//время уничтожения
	u32 m_dwDestroyTime;
	u32 m_dwDestroyTimeMax;

	Fmatrix					m_offset;

	Fvector					m_throw_direction;
	Fmatrix					m_throw_matrix;

	CMissile				*m_fake_missile;

	//параметры броска
	float m_fThrowForce;
	float m_fMinForce, m_fMaxForce, m_fForceGrowSpeed;

	//относительная точка и направление вылета гранаты
	Fvector m_vThrowPoint;
	Fvector m_vThrowDir;
	//для HUD
	Fvector m_vHudThrowPoint;
	Fvector m_vHudThrowDir;

	//имена анимаций
	ref_str	m_sAnimShow;
	ref_str	m_sAnimHide;
	ref_str	m_sAnimIdle;
	ref_str	m_sAnimPlaying;
	ref_str	m_sAnimThrowBegin;
	ref_str	m_sAnimThrowIdle;
	ref_str	m_sAnimThrowAct;
	ref_str	m_sAnimThrowEnd;

	//звук анимации "играния"
	HUD_SOUND sndPlaying;

protected:
	u32					m_animation_slot;
			void	setup_throw_params		();
public:
	IC		u32		animation_slot			()	{	return m_animation_slot;}
	virtual void	activate_physic_shell	();
	virtual void	setup_physic_shell		();
	virtual void	create_physic_shell		();
	IC		void	set_destroy_time		(u32 destroy_time)
	{
		m_dwDestroyTime		= destroy_time;
	}
};