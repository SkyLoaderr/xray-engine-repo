// Weapon.h: interface for the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_)
#define AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_
#pragma once

#include "..\lightPPA.h"
#include "GameObject.h"
#include "PhysicsShell.h"

// refs
class CEntity;
class CWeaponHUD;
class ENGINE_API CMotionDef;

class CWeapon : public CGameObject
{
	friend class CWeaponList;
private:
	typedef CGameObject		inherited;
public:
	enum					{ MAX_ANIM_COUNT = 8 };
	typedef					svector<CMotionDef*,MAX_ANIM_COUNT>		MotionSVec;
	enum EHandDependence{
		hdNone	= 0,
		hd1Hand	= 1,
		hd2Hand	= 2
	};
protected:
	EHandDependence			eHandDependence;// 0-используется без участия рук, 1-одна рука, 2-две руки
	BOOL					bWorking;		// Weapon fires now
	BOOL					bPending;		// Weapon needs some time to update itself, even if hidden
	LPSTR					m_WpnName;

	Fmatrix					m_Offset;

	void					signal_HideComplete		();

	Fvector					vFirePoint;
	Fvector					vShellPoint;

	// Media :: flames
	svector<Shader*,8>		hFlames;
	int						iFlameDiv;
	float					fFlameLength;
	float					fFlameSize;
	float					fFlameTime;

	void					animGet					(MotionSVec& lst, LPCSTR prefix);
protected:
	CPhysicsShell*			m_pPhysicsShell;
	CWeaponHUD*				m_pHUD;
	BOOL					hud_mode;
	int						iSlotBinding;

	LPSTR					pstrWallmark;
	Shader*					hWallmark;
	Shader*					hUIIcon;
	float					fWallmarkSize;
									
	int						iAmmoLimit;			// maximum ammo we can have
	int						iAmmoCurrent;		// ammo we have now
	int						iAmmoElapsed;		// ammo in magazine, currently
	int						iMagazineSize;		// size (in bullets) of magazine
							
	float					fTimeToFire;
	int						iHitPower;

	Fvector					vLastFP, vLastFD, vLastSP;
							
	float					fireDistance;
	float					fireDispersionBase;
	float					fireDispersion;
	float					fireDispersion_Inc;
	float					fireDispersion_Dec;
	float					fireDispersion_Current;

	float					camMaxAngle;
	float					camRelaxSpeed;
	float					camDispersion;
							
	float					dispVelFactor;
	float					dispJumpFactor;
	float					dispCrouchFactor;
							
	float					tracerHeadSpeed;
	float					tracerTrailCoeff;
	float					tracerStartLength;
	float					tracerWidth;
	u32						tracerFrame;
							
	CLightPPA				light_base;
	CLightPPA				light_build;
	CLightPPA				light_render;
	float					light_var_color;
	float					light_var_range;
	float					light_lifetime;
	u32						light_frame;
	float					light_time;

	u32						dwFP_Frame;
	u32						dwXF_Frame;
protected:
	struct	net_update 		
	{
		u32					dwTimeStamp;
		u8					flags;
		u8					state;
		u16					ammo_current;
		u16					ammo_elapsed;
		Fvector				pos,angles;
		void	lerp		(net_update& A,net_update& B, float f);
	};
	deque<net_update>		NET;
	net_update				NET_Last;
protected:
	void					Light_Start			();
	void					Light_Render		(Fvector& P);

	virtual BOOL			FireTrace			(const Fvector& P, const Fvector& Peff,	Fvector& D);
	virtual void			FireShotmark		(const Fvector& vDir,	const Fvector &vEnd, Collide::ray_query& R);
	virtual void			UpdatePosition		(const Fmatrix& transform);

	virtual void			UpdateFP			();
	virtual void			UpdateXForm			();
protected:
	// Utilities
	void					SoundCreate			(sound&		dest, LPCSTR name, int iType=0, BOOL bCtrlFreq=FALSE);
	void					SoundDestroy		(sound&		dest);
	void					ShaderCreate		(Shader*	&dest, LPCSTR S, LPCSTR T);
	void					ShaderDestroy		(Shader*	&dest);

public:
	enum EState
	{
		eIdle		= 0,
		eFire,
		eReload,
		eShowing,
		eHiding,
		eHidden
	};
	// Events/States
	u32						STATE;

	virtual void			SwitchState			(u32 S);

	virtual void			OnMagazineEmpty		()			{};
	virtual void			OnAnimationEnd		()			{};
	virtual void			OnZoomIn			()			{};
	virtual void			OnZoomOut			()			{};
	virtual void			OnDrawFlame			();
	virtual void			OnStateSwitch		(u32 S)	{};
public:
							CWeapon				(LPCSTR name);
	virtual					~CWeapon			();

	// Generic
	virtual void			Load				(LPCSTR section);
	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual void			net_Destroy			();
	virtual void			net_Export			(NET_Packet& P);	// export to server
	virtual void			net_Import			(NET_Packet& P);	// import from server
	virtual void			Update				(u32 dt);
	virtual void			UpdateCL			();
	virtual void			OnVisible			();
	virtual void			OnH_B_Chield		();
	virtual void			OnH_B_Independent	();

	// logic & effects
	virtual void			SetDefaults			();
	virtual void			Ammo_add			(int iValue)	{ iAmmoCurrent+=iValue;	}
	virtual int				Ammo_eject			();
	virtual void			FireStart			()				{ bWorking=true;	}
	virtual void			FireEnd				()				{ bWorking=false;	}
	virtual void			Reload				()				{};
	
	virtual void			Hide				()				= 0;
	virtual void			Show				()				= 0;

	IC BOOL					IsWorking			()				{	return bWorking;							}
	IC BOOL					IsValid				()				{	return iAmmoCurrent || iAmmoElapsed;		}
	IC BOOL					IsVisible			()				{	return getVisible();						}	// Weapon change occur only after visibility change
	IC BOOL					IsUpdating			()				{	return bWorking || bPending || getVisible();}	// Does weapon need's update?
	IC EHandDependence		HandDependence		()				{	return eHandDependence;}
	virtual BOOL			HasOpticalAim		()				{	return FALSE;								}
	virtual float			GetZoomFactor		()				{	return DEFAULT_FOV;							}

	float					GetPrecision		();
	IC LPCSTR				GetName				()				{	return m_WpnName;							}
	IC int					GetAmmoElapsed		()				{	return iAmmoElapsed;						}
	IC int					GetAmmoLimit		()				{	return iAmmoLimit;							}
	IC int					GetAmmoCurrent		()				{	return iAmmoCurrent;						}
	IC int					GetAmmoMagSize		()				{	return iMagazineSize;						}
	IC Shader*				GetUIIcon			()				{	return hUIIcon;								}
	IC void					SetHUDmode			(BOOL H)		{	hud_mode = H;								}
	IC int					GetSlot				()				{	return iSlotBinding;						}

	virtual void			OnDeviceCreate		();
	virtual void			OnDeviceDestroy		();
};

#endif // !defined(AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_)
