#pragma once
#include "inventory.h"

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
	typedef CInventoryItem inherited;
public:
	CMissile(void);
	virtual ~CMissile(void);

	virtual void reinit				();

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();

	virtual void UpdateCL();
	virtual void renderable_Render();

	virtual void OnH_B_Chield();
	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent();

	virtual void OnEvent			(NET_Packet& P, u16 type);

	virtual void OnAnimationEnd();

	virtual void Show();
	virtual void Hide();
	virtual bool IsHidden() {return MS_HIDDEN == m_state;}
	virtual bool IsPending() {return m_bPending;}

	virtual void Throw();
	virtual void Destroy();

	virtual bool Action(s32 cmd, u32 flags);

	u32 State();
	virtual u32 State(u32 state);

protected:
	virtual void UpdateFP();
	virtual void UpdateXForm();
	void UpdatePosition(const Fmatrix& trans);
	void spawn_fake_missile	();

protected:

	u32 m_state;
	bool m_throw;
	u32 m_destroyTime, m_stateTime;

	Fmatrix					m_offset;

	Fvector					m_throw_direction;
	Fvector					m_throw_point;
	CMissile				*m_fake_missile;

	//параметры броска
	float m_force;
	float m_minForce, m_maxForce, m_forceGrowSpeed;

	//относительная точка и направление вылета гранаты
	Fvector m_vThrowPoint;
	Fvector m_vThrowDir;
	//для HUD
	Fvector m_vHudThrowPoint;
	Fvector m_vHudThrowDir;
};
