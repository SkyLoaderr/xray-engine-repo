#pragma once
#include "weaponmagazined.h"
#include "..\feel_touch.h"


class CWeaponFakeGrenade;


class CWeaponMagazinedWGrenade : public CWeaponMagazined
{
	typedef CWeaponMagazined inherited;
protected:
	enum EState
	{
		eSwitchingState = 256,	// for pistols, etc
	};
public:
	CWeaponMagazinedWGrenade(LPCSTR name,ESoundTypes eSoundType);
	virtual ~CWeaponMagazinedWGrenade(void);

	virtual void	Load			(LPCSTR section);
	
	virtual BOOL	net_Spawn		(LPVOID DC);
	
	virtual void	OnH_B_Chield	();
	virtual void	OnH_B_Independent();

	virtual bool	Attach(PIItem pIItem);
	virtual bool	Detach(const char* item_section_name);
	virtual bool	CanAttach(PIItem pIItem);
	virtual bool	CanDetach(const char* item_section_name);

	
	virtual void	OnStateSwitch	(u32 S);
	
	virtual void	switch2_Idle	();
	virtual void	switch2_Reload	();
	virtual void	state_Fire		(float dt);
	virtual void	OnShot			();
	virtual void	SwitchState		(u32 S);
	virtual void	OnEvent			(NET_Packet& P, u16 type);
	virtual void	ReloadMagazine	();

	virtual bool	Action			(s32 cmd, u32 flags);

	//переключение в режим подствольника
	void			SwitchMode		();
	void			OnAnimationEnd	();

	//виртуальные функции для проигрывания анимации HUD
	virtual void	PlayAnimShow();
	virtual void	PlayAnimHide();
	virtual void	PlayAnimReload();
	virtual void	PlayAnimIdle();
	virtual void	PlayAnimShoot();


	static void	__stdcall GrenadeCallback(CBoneInstance*);

	ref_sound			sndShotG;
	ref_sound			sndReloadG;
	MotionSVec			mhud_idle_g;
	MotionSVec			mhud_reload_g;
	MotionSVec			mhud_shots_g;
	MotionSVec			mhud_switch_g, mhud_switch;
	MotionSVec			mhud_show_g;
	MotionSVec			mhud_hide_g;

	//анимации дублирующие обычные для WeaponMagazined, но 
	//с подствольником
	MotionSVec			mhud_idle_w_gl;
	MotionSVec			mhud_reload_w_gl;
	MotionSVec			mhud_shots_w_gl;
	MotionSVec			mhud_show_w_gl;
	MotionSVec			mhud_hide_w_gl;


	//дополнительные параметры патронов 
	//для подствольника
	CWeaponAmmo* m_pAmmo2;
	char m_ammoSect2[255];
	xr_vector<ref_str> m_ammoTypes2;
	u32 m_ammoType2;
	ref_str m_ammoName2;
	int iMagazineSize2;
	xr_stack<CCartridge> m_magazine2;
	
	bool m_bHideGrenade;
	bool m_bGrenadeMode;


	//имя пратиклов для огня из подствольника
	LPCSTR m_sGrenadeFlameParticles;


	Fvector* m_pGrenadePoint;
	CWeaponFakeGrenade* m_pGrenade;

	//название косточки для гранаты подствольника в HUD
	LPCSTR grenade_bone_name;

	//скорость вылета гранаты из подствольника
	float m_fGrenadeVel;
	
protected:
	void SpawFakeGrenade(const char*  grenade_section_name);
};