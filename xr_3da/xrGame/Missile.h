#pragma once
#include "inventory.h"

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

class CMissile :
	virtual public CInventoryItem
{
	typedef CInventoryItem inherited;
public:
	CMissile(void);
	virtual ~CMissile(void);

	void SoundCreate(ref_sound& dest, LPCSTR name, int iType, BOOL bCtrlFreq=FALSE);
	void SoundDestroy(ref_sound& dest);

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();

	virtual void UpdateCL();
	virtual void renderable_Render();

	virtual void OnH_B_Chield();
	virtual void OnH_B_Independent();
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

	CWeaponHUD* m_pHUD;
	u32 m_state;
	bool m_throw;
	f32 m_force, m_minForce, m_maxForce, m_forceGrowSpeed;
	u32 m_destroyTime, m_stateTime;

	bool m_bPending;
	virtual void UpdateXForm();
			void UpdatePosition(const Fmatrix& trans);

private:
	u32						dwXF_Frame;

};
