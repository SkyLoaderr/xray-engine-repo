// Weapon.h: interface for the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_)
#define AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_
#pragma once

#include "PhysicsShell.h"
#include "weaponammo.h"

// refs
class CEntity;
class CWeaponHUD;
class ENGINE_API CMotionDef;

#include "xrServer_Objects_ALife_Items.h"


class CWeapon : public CInventoryItem//CGameObject
{
	friend class CWeaponList;
private:
	typedef CInventoryItem		inherited;
public:
	enum					{ MAX_ANIM_COUNT = 8 };
	typedef					svector<CMotionDef*,MAX_ANIM_COUNT>		MotionSVec;
	
	enum EHandDependence{
		hdNone	= 0,
		hd1Hand	= 1,
		hd2Hand	= 2
	};

protected:
	EHandDependence			eHandDependence;		// 0-используется без участия рук, 1-одна рука, 2-две руки
	bool					bWorking, bWorking2;	// Weapon fires now
	bool					bPending;				// Weapon needs some time to update itself, even if hidden
	ref_str					m_WpnName;


	bool					bMisfire;				// a misfire happens, you'll need to rearm weapon

	Fmatrix					m_Offset;

	void					signal_HideComplete		();

	Fvector					vFirePoint;
	Fvector					vShellPoint;

	// Media :: flames
	svector<ref_shader,8>	hFlames;
	int						iFlameDiv;
	float					fFlameLength;
	float					fFlameSize;
	float					fFlameTime;

	void					animGet					(MotionSVec& lst, LPCSTR prefix);
protected:
	//CPhysicsShell*			m_pPhysicsShell;
	CWeaponHUD*				m_pHUD;
	BOOL					hud_mode;
	ref_str					hud_sect;
	int						iSlotBinding;

	ref_str					pstrWallmark;
	ref_shader				hWallmark;
	ref_shader				hUIIcon;
	float					fWallmarkSize;
									
	////int						iAmmoLimit;			// maximum ammo we can have
	////int						iAmmoCurrent;		// ammo we have now
	int						iAmmoElapsed;		// ammo in magazine, currently
	int						iMagazineSize;		// size (in bullets) of magazine
	int						iBuckShot;
							
	float					fTimeToFire;
	int						iHitPower;
	float					fHitImpulse;

	Fvector					vLastFP, vLastFD, vLastSP;
		
	//рассеивание во время стрельбы
	float					fireDistance;
	float					fireDispersionBase;
	float					fireDispersion;
	float					fireDispersion_Inc;
	float					fireDispersion_Dec;
	float					fireDispersion_Current;
	//фактор увеличения дисперсии при максимальной изношености (в процентах)
	float					fireDispersionConditionFactor;
	//вероятность осечки при максимальной изношености
	float					misfireProbability;
	//увеличение изношености при выстреле
	float					conditionDecreasePerShot;

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
							
	Fcolor					light_base_color;
	float					light_base_range;
	Fcolor					light_build_color;
	float					light_build_range;
	IRender_Light*			light_render;
	float					light_var_color;
	float					light_var_range;
	float					light_lifetime;
	u32						light_frame;
	float					light_time;
	bool					m_shotLight;

	u32						dwFP_Frame;
	u32						dwXF_Frame;

	float					fZoomFactor;


	//возможность подключения различных аддонов
	CSE_ALifeItemWeapon::EAddonStatus			m_eScopeStatus;
	CSE_ALifeItemWeapon::EAddonStatus			m_eSilencerStatus;
	CSE_ALifeItemWeapon::EAddonStatus			m_eGrenadeLauncherStatus;

	//назваания секций подключаемых аддонов
	ref_str		m_sScopeName;
	ref_str		m_sSilencerName;
	ref_str		m_sGrenadeLauncherName;

	//смещение иконов апгрейдов в инвентаре
	int	m_iScopeX, m_iScopeY;
	int	m_iSilencerX, m_iSilencerY;
	int	m_iGrenadeLauncherX, m_iGrenadeLauncherY;

	//состояние подключенных аддонов
	u8			m_flagsAddOnState;
protected:
	struct	net_update 		
	{
		u32					dwTimeStamp;
		u8					flags;
		u16					ammo_current;
		u16					ammo_elapsed;
		Fvector				pos,angles;
		u8					m_flagsAddOnState;
		void	lerp		(net_update& A,net_update& B, float f);
	};
	xr_deque<net_update>	NET;
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
	void					SoundCreate			(ref_sound&		dest, LPCSTR name, int iType=0, BOOL bCtrlFreq=FALSE);
	void					SoundDestroy		(ref_sound&		dest);
	void					ShaderCreate		(ref_shader&	dest, LPCSTR S, LPCSTR T);
	void					ShaderDestroy		(ref_shader&	dest);

public:
	enum EState
	{
		eIdle		= 0,
		eFire,
		eFire2,
		eReload,
		eShowing,
		eHiding,
		eHidden,
		eMisfire
	};


	// Events/States
	u32						STATE, NEXT_STATE;
	

	float					m_fMinRadius;
	float					m_fMaxRadius;

	virtual void			SwitchState			(u32 S);

	virtual void			OnMagazineEmpty		()			{};
	virtual void			OnAnimationEnd		()			{};
	virtual void			OnZoomIn			()			{};
	virtual void			OnZoomOut			()			{};
	virtual void			OnDrawFlame			();
	virtual void			OnStateSwitch		(u32 /**S/**/)		{};

public:
							CWeapon				(LPCSTR name);
	virtual					~CWeapon			();

	// Generic
	virtual void			Load				(LPCSTR section);
	virtual BOOL			net_Spawn			(LPVOID DC);
	virtual void			net_Destroy			();
	virtual void			net_Export			(NET_Packet& P);	// export to server
	virtual void			net_Import			(NET_Packet& P);	// import from server
	virtual void			UpdateCL			();
	virtual void			shedule_Update		(u32 dt);
	virtual void			renderable_Render	();
	virtual void			OnH_B_Chield		();
	virtual void			OnH_B_Independent	();

	virtual	void			Hit					(float P, Fvector &dir,	
												 CObject* who, s16 element,
												 Fvector position_in_object_space, 
												 float impulse, 
												 ALife::EHitType hit_type = eHitTypeWound);

	// logic & effects
	virtual void			SetDefaults			();
	virtual void			Ammo_add			(int iValue);
	virtual int				Ammo_eject			();
	virtual void			FireStart			()				{ bWorking=true;	}
	virtual void			FireEnd				()				{ bWorking=false;	}
	virtual void			Fire2Start			()				{ bWorking2=true;	}
	virtual void			Fire2End			()				{ bWorking2=false;	}
	virtual void			Reload				();
	
	virtual void			Hide				()				= 0;
	virtual void			Show				()				= 0;

	   BOOL					IsMisfire			();
	   BOOL					CheckForMisfire		();

	IC BOOL					IsWorking			()				{	return bWorking;							}
	IC BOOL					IsValid				()				{	return iAmmoElapsed;						}
	IC BOOL					IsVisible			()				{	return getVisible();						}	// Weapon change occur only after visibility change
	IC BOOL					IsUpdating			()				{	return bWorking || bPending || getVisible();}	// Does weapon need's update?
	virtual bool			IsHidden			()				{	return STATE == eHidden;}						// Does weapon is in hidden state
	virtual bool			IsPending			()				{   return bPending;}
	
	IC EHandDependence		HandDependence		()				{	return eHandDependence;}
	virtual BOOL			HasOpticalAim		()				{	return FALSE;								}
	virtual float			GetZoomFactor		()				{	return fZoomFactor;							}

	float					GetPrecision		();
	IC LPCSTR				GetName				()				{	return *m_WpnName;							}
	IC int					GetAmmoElapsed		()				{	return int(m_magazine.size())/*iAmmoElapsed*/;						}
	IC int					GetAmmoMagSize		()				{	return iMagazineSize;						}

	//параметы оружия в зависимоти от его состояния исправности
	float					GetConditionDispersionFactor	();
	float					GetConditionMisfireProbability	();


	// @@@ WT: Subject to remove
	IC int					GetAmmoLimit		()				{	return 0/*iAmmoLimit*/;						}
	int						GetAmmoCurrent		();//				{	return /*m_pAmmo?m_pAmmo->m_boxSize:*/0;	}
	//

	IC ref_shader			GetUIIcon			()				{	return hUIIcon;								}
	IC void					SetHUDmode			(BOOL H)		{	hud_mode = H;								}
	IC BOOL					GetHUDmode			()				{	return hud_mode;							}
	IC int					GetSlot				()				{	return iSlotBinding;						}

	virtual void			OnEvent				(NET_Packet& P, u16 type);

	// Inventory
	virtual bool Activate();
	virtual void Deactivate();
	virtual bool Action(s32 cmd, u32 flags);
	virtual bool Attach(PIItem pIItem);
	virtual bool Detach(const char* item_section_name);

	virtual bool IsGrenadeLauncherAttached();
	virtual bool IsScopeAttached();
	virtual bool IsSilencerAttached();

	virtual bool GrenadeLauncherAttachable();
	virtual bool ScopeAttachable();
	virtual bool SilencerAttachable();

	//обновление видимости для косточек аддонов
	virtual void UpdateAddonsVisibility();


	//для отоброажения иконок апгрейдов в интерфейсе
	int	GetScopeX() {return m_iScopeX;}
	int	GetScopeY() {return m_iScopeY;}
	int	GetSilencerX() {return m_iSilencerX;}
	int	GetSilencerY() {return m_iSilencerY;}
	int	GetGrenadeLauncherX() {return m_iGrenadeLauncherX;}
	int	GetGrenadeLauncherY() {return m_iGrenadeLauncherY;}

	virtual ref_str GetGrenadeLauncherName() {return m_sGrenadeLauncherName;}
	virtual ref_str GetScopeName() {return  m_sScopeName;}
	virtual ref_str GetSilencerName() {return m_sSilencerName;}

	virtual const char* Name();

	void SpawnAmmo		(u32 boxCurr = 0xffffffff, LPCSTR ammoSect = NULL);

	CWeaponAmmo*		m_pAmmo;
	xr_vector<ref_str>	m_ammoTypes;
	u32					m_ammoType;
	ref_str				m_ammoName;
	f32					m_resource, m_abrasion;
	string64			m_tmpName;

	//

	// Multitype ammo support
	xr_stack<CCartridge> m_magazine;
	//
};

#endif // !defined(AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_)
