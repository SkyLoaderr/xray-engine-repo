#pragma once
#include "weaponmagazined.h"
#include "rocketlauncher.h"


class CWeaponFakeGrenade;


class CWeaponMagazinedWGrenade : public CWeaponMagazined,
								 public CRocketLauncher
{
	typedef CWeaponMagazined inherited;
public:
	CWeaponMagazinedWGrenade(LPCSTR name,ESoundTypes eSoundType);
	virtual ~CWeaponMagazinedWGrenade(void);

	virtual void	Load			(LPCSTR section);
	
	virtual BOOL	net_Spawn		(LPVOID DC);
	virtual void	net_Destroy		();
	
	virtual void	OnH_B_Chield	();
	virtual void	OnH_B_Independent();


	virtual bool	Attach(PIItem pIItem);
	virtual bool	Detach(const char* item_section_name);
	virtual bool	CanAttach(PIItem pIItem);
	virtual bool	CanDetach(const char* item_section_name);
	virtual void	InitAddons();

	
	virtual void	OnStateSwitch	(u32 S);
	
	virtual void	switch2_Idle	();
	virtual void	switch2_Reload	();
	virtual void	state_Fire		(float dt);
	virtual void	OnShot			();
	virtual void	SwitchState		(u32 S);
	virtual void	OnEvent			(NET_Packet& P, u16 type);
	virtual void	ReloadMagazine	();

	virtual bool	Action			(s32 cmd, u32 flags);

	virtual void	UpdateSounds	();
	virtual void	StopHUDSounds	();

	//������������ � ����� �������������
	virtual void	SwitchMode		();
	void			PerformSwitch	();
	void			OnAnimationEnd	();

	//����������� ������� ��� ������������ �������� HUD
	virtual void	PlayAnimShow();
	virtual void	PlayAnimHide();
	virtual void	PlayAnimReload();
	virtual void	PlayAnimIdle();
	virtual void	PlayAnimShoot();
	virtual void	PlayAnimModeSwitch();
	
	HUD_SOUND			sndShotG;
	HUD_SOUND			sndReloadG;
	HUD_SOUND			sndSwitch;


	//�������� � ������������ ��������������
	//(����� ������� ��������)
	MotionSVec			mhud_idle_g;
	MotionSVec			mhud_idle_g_aim;
	MotionSVec			mhud_reload_g;
	MotionSVec			mhud_shots_g;
	MotionSVec			mhud_switch_g, mhud_switch;
	MotionSVec			mhud_show_g;
	MotionSVec			mhud_hide_g;
	//(����� �������� �� �������������)
	MotionSVec			mhud_idle_w_gl;
	MotionSVec			mhud_idle_w_gl_aim;
	MotionSVec			mhud_reload_w_gl;
	MotionSVec			mhud_shots_w_gl;
	MotionSVec			mhud_show_w_gl;
	MotionSVec			mhud_hide_w_gl;


	//�������������� ��������� �������� 
	//��� �������������
	CWeaponAmmo* m_pAmmo2;
	char m_ammoSect2[255];
	xr_vector<ref_str> m_ammoTypes2;
	u32 m_ammoType2;
	ref_str m_ammoName2;
	int iMagazineSize2;
	xr_stack<CCartridge> m_magazine2;
	
	bool m_bGrenadeMode;

	Fvector* m_pGrenadePoint;

	virtual void UpdateGrenadeVisibility(bool visibility);

	//�������� �������� ��� ������� ������������� � HUD
	ref_str grenade_bone_name;
};