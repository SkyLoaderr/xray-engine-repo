#pragma once
#include "weaponcustompistol.h"

class CWeaponShotgun :	public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;
public:
	CWeaponShotgun(void);
	virtual ~CWeaponShotgun(void);

	virtual void	Load			(LPCSTR section);
	
	virtual void	net_Destroy			();

	virtual void	Reload				();
	virtual void	Fire2Start			();
	virtual void	Fire2End			();
	virtual void	OnShot				();
	virtual void	OnShotBoth			();
	virtual void	switch2_Fire		();
	virtual void	switch2_Fire2		();
	void			switch2_StartReload ();
	void			switch2_AddCartgidge();
	void			switch2_EndReload	();

	virtual void	UpdateSounds		();
	virtual void	PlayAnimOpenWeapon	();
	virtual void	PlayAnimAddOneCartridgeWeapon();
	void			PlayAnimCloseWeapon	();

	virtual bool	Action(s32 cmd, u32 flags);

protected:
	virtual void	OnAnimationEnd		();
	void			TriStateReload		();
	virtual void	OnStateSwitch		(u32 S);

	bool			HaveCartridgeInInventory();
	u8				AddCartridge		(u8 cnt);

	HUD_SOUND		sndShotBoth;
	ESoundTypes		m_eSoundShotBoth;

	MotionSVec		mhud_shot_boths;

	ESoundTypes		m_eSoundOpen;
	ESoundTypes		m_eSoundAddCartridge;
	ESoundTypes		m_eSoundClose;
	HUD_SOUND		m_sndOpen;
	HUD_SOUND		m_sndAddCartridge;
	HUD_SOUND		m_sndClose;
	MotionSVec		mhud_open;
	MotionSVec		mhud_add_cartridge;
	MotionSVec		mhud_close;


	bool			m_bTriStateReload;


};