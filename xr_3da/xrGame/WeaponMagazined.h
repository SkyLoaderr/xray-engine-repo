#ifndef __XR_WEAPON_MAG_H__
#define __XR_WEAPON_MAG_H__
#pragma once

#include "weapon.h"

class ENGINE_API CMotionDef;

class CWeaponMagazined: public CWeapon
{
private:
	typedef CWeapon inherited;
protected:
	// Media :: sounds
	ref_sound		sndShow;
	ref_sound		sndHide;
	ref_sound		sndShot;
	ref_sound		sndEmptyClick;
	ref_sound		sndReload;
	//����� ����� ������������� ������
	float			sndShow_delay;
	float			sndHide_delay;
	float			sndShot_delay;
	float			sndEmptyClick_delay;
	float			sndReload_delay;


	ESoundTypes		m_eSoundShow;
	ESoundTypes		m_eSoundHide;
	ESoundTypes		m_eSoundShot;
	ESoundTypes		m_eSoundEmptyClick;
	ESoundTypes		m_eSoundReload;
					
	// HUD :: Animations
	MotionSVec		mhud_idle;
	MotionSVec		mhud_reload;
	MotionSVec		mhud_hide;
	MotionSVec		mhud_show;
	MotionSVec		mhud_shots;

	// General
	float			fTime;
	BOOL			bFlame;

	//���� ������� ��������� UpdateSounds
	u32						dwUpdateSounds_Frame;
protected:
	virtual void	FireShotmark	(const Fvector &vDir, const Fvector &vEnd, Collide::rq_result& R, u16 target_material);
	virtual void	OnMagazineEmpty	();

	virtual void	switch2_Idle	();
	virtual void	switch2_Fire	();
	virtual void	switch2_Fire2	(){}
	virtual void	switch2_Empty	();
	virtual void	switch2_Reload	();
	virtual void	switch2_Hiding	();
	virtual void	switch2_Hidden	();
	virtual void	switch2_Showing	();
	
	virtual void	OnShot			();
	
	virtual void	OnEmptyClick	();

	virtual void	OnAnimationEnd	();
	virtual void	OnStateSwitch	(u32 S);

	virtual void	UpdateSounds	();

	void			TryReload		();
protected:
	virtual void	ReloadMagazine	();
	virtual void	UnloadMagazine	();

	virtual void	state_Fire		(float dt);
	virtual void	state_MagEmpty	(float dt);
	virtual void	state_Misfire	(float dt);
public:
					CWeaponMagazined	(LPCSTR name,ESoundTypes eSoundType);
	virtual			~CWeaponMagazined	();

	virtual void	Load			(LPCSTR section);

	virtual void	SetDefaults		();
	virtual void	FireStart		();
	virtual void	FireEnd			();
	virtual void	Reload			();
	
	virtual void	Hide			();
	virtual void	Show			();

	virtual	void	UpdateCL		();
	virtual	void	renderable_Render		();

	virtual void	net_Destroy		();

	virtual bool	Attach			(PIItem pIItem);
	virtual bool	Detach(const char* item_section_name);
	virtual bool	CanAttach(PIItem pIItem);
	virtual bool	CanDetach(const char* item_section_name);

	virtual bool	Action			(s32 cmd, u32 flags);
	bool			IsAmmoAvailable	();

	u32				m_queueSize, m_shotNum;
	void			SetQueueSize	(u32 size) { m_queueSize = size; }

	//����������� ������� ��� ������������ �������� HUD
	virtual void	PlayAnimShow();
	virtual void	PlayAnimHide();
	virtual void	PlayAnimReload();
	virtual void	PlayAnimIdle();
	virtual void	PlayAnimShoot();

};

#endif //__XR_WEAPON_MAG_H__
