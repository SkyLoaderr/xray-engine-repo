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
	virtual void renderable_Render();

	virtual void OnH_B_Chield();
	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent();

	virtual void OnEvent(NET_Packet& P, u16 type);

	virtual void OnAnimationEnd();

	virtual void Show();
	virtual void Hide();
	virtual bool IsHidden() {return m_state == MS_HIDDEN;}
	virtual bool IsHiding() {return m_state == MS_HIDING;}

	virtual void Throw();
	virtual void Destroy();

	virtual bool Action(s32 cmd, u32 flags);

	u32 State();
	virtual u32 State(u32 state);
	virtual void OnStateSwitch	(u32 S);
protected:
	virtual void UpdateFP();
	virtual void UpdateXForm();
	void UpdatePosition(const Fmatrix& trans);
	void spawn_fake_missile	();

protected:

	u32 m_state;
	bool m_throw;

	//время уничтожения
	u32 m_dwDestroyTime;
	u32 m_dwDestroyTimeMax;

	Fmatrix					m_offset;

	Fvector					m_throw_direction;
	Fvector					m_throw_point;
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
public:
	IC u32				HandDependence		()				{	return m_animation_slot;}
};