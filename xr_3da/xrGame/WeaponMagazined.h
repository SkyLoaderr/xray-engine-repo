#ifndef __XR_WEAPON_MAG_H__
#define __XR_WEAPON_MAG_H__
#pragma once

#include "weapon.h"

class CWeaponMagazined: public CWeapon
{
private:
	typedef CWeapon inherited;
public:
	enum EState
	{
		eIdle,
		eFire,
		eMagEmpty,
		eReload
	};
private:
	// General
	float			fTime;
	BOOL			bFlame;

	Fvector			vFirePoint;

	DWORD			dwFP_Frame;
	DWORD			dwXF_Frame;

	EState			st_current, st_target;
protected:
	virtual void	FireShotmark	(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R);
	virtual void	OnMagazineEmpty	();

	virtual void	MediaLOAD		()				{};
	virtual void	MediaUNLOAD		()				{};
	virtual void	switch2_Idle	(BOOL bHUDView)	{};
	virtual void	switch2_Fire	(BOOL bHUDView)	{};
	virtual void	switch2_Empty	(BOOL bHUDView)	{};
	virtual void	switch2_Reload	(BOOL bHUDView)	{};
	virtual void	OnShot			(BOOL bHUDView)	{};
	virtual void	OnEmptyClick	(BOOL bHUDView)	{};
	virtual void	OnDrawFlame		(BOOL bHUDView)	{};
	virtual void	OnShotmark		(const Fvector &vDir, const Fvector &vEnd, Collide::ray_query& R) {};
protected:
	virtual void	UpdateFP		(BOOL bHUD);
	virtual void	UpdateXForm		(BOOL bHUD);
public:
					CWeaponMagazined	(LPCSTR name);
	virtual			~CWeaponMagazined	();

	virtual void	Load			(CInifile* ini, const char* section);

	virtual void	SetDefaults		();
	virtual void	FireStart		();
	virtual void	FireEnd			();

	virtual void	Hide			();
	virtual void	Show			();

	virtual	void	Update			(float dt, BOOL bHUDView);
	virtual	void	Render			(BOOL bHUDView);

	virtual void	OnDeviceCreate	();
	virtual void	OnDeviceDestroy	();
};

#endif //__XR_WEAPON_MAG_H__
