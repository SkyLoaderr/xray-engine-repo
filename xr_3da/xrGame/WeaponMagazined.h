#ifndef __XR_WEAPON_MAG_H__
#define __XR_WEAPON_MAG_H__
#pragma once

#include "weapon.h"
#include "ai_sounds.h"

class ENGINE_API CMotionDef;

//������ ������� ��������� �������������
//����������� ��������, ������, ���� ��������� �������
#define WEAPON_ININITE_QUEUE -1

class CWeaponMagazined: public CWeapon
{
private:
	typedef CWeapon inherited;
protected:
	// Media :: sounds
	HUD_SOUND		sndShow;
	HUD_SOUND		sndHide;
	HUD_SOUND		sndShot;
	HUD_SOUND		sndEmptyClick;
	HUD_SOUND		sndReload;
	//���� �������� ��������
	HUD_SOUND*		m_pSndShotCurrent;

	virtual void	StopHUDSounds		();

	//�������������� ���������� � ���������
	LPCSTR			m_sSilencerFlameParticles;
	LPCSTR			m_sSilencerSmokeParticles;
	HUD_SOUND		sndSilencerShot;

	ESoundTypes		m_eSoundShow;
	ESoundTypes		m_eSoundHide;
	ESoundTypes		m_eSoundShot;
	ESoundTypes		m_eSoundEmptyClick;
	ESoundTypes		m_eSoundReload;
					
	// HUD :: Animations
	MotionSVec		mhud_idle;
	MotionSVec		mhud_idle_aim;
	MotionSVec		mhud_reload;
	MotionSVec		mhud_hide;
	MotionSVec		mhud_show;
	MotionSVec		mhud_shots;

	// General
	//���� ������� ��������� UpdateSounds
	u32				dwUpdateSounds_Frame;
protected:
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

	bool			TryReload		();
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
	virtual	void	renderable_Render();

	virtual void	net_Destroy		();

	virtual bool	Attach(PIItem pIItem);
	virtual bool	Detach(const char* item_section_name);
	virtual bool	CanAttach(PIItem pIItem);
	virtual bool	CanDetach(const char* item_section_name);

	virtual void	InitAddons();

	virtual bool	Action			(s32 cmd, u32 flags);
	bool			IsAmmoAvailable	();


	//////////////////////////////////////////////
	// ��� �������� ��������� ��� ����������
	//////////////////////////////////////////////
public:
	virtual void	SwitchMode				();
	virtual bool	SingleShotMode			()			{return 1 == m_iQueueSize;}
	virtual void	SetQueueSize			(int size)  {m_iQueueSize = size; }
	virtual bool	StopedAfterQueueFired	()			{return m_bStopedAfterQueueFired; }

protected:
	//������������ ������ �������, ������� ����� ����������
	int				m_iQueueSize;
	//���������� ������� ����������� ��������
	int				m_iShotNum;
	//���� ����, ��� �� ������������ ����� ���� ��� ����������
	//����� ������� ��������, ������� ���� ������ � m_iQueueSize
	bool			m_bStopedAfterQueueFired;
	//���� ����, ��� ���� �� ���� ������� �� ������ �������
	//(���� ���� ����� ������ ������ �� ����� � ��������� FireEnd)
	bool			m_bFireSingleShot;


	//////////////////////////////////////////////
	// ����� �����������
	//////////////////////////////////////////////
public:
	virtual void	OnZoomIn			();
	virtual void	OnZoomOut			();

protected:
	virtual bool	AllowFireWhileWorking() {return false;}

	//����������� ������� ��� ������������ �������� HUD
	virtual void	PlayAnimShow();
	virtual void	PlayAnimHide();
	virtual void	PlayAnimReload();
	virtual void	PlayAnimIdle();
	virtual void	PlayAnimShoot();
};

#endif //__XR_WEAPON_MAG_H__
