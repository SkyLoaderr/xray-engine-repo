// Weapon.h: interface for the CWeapon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_)
#define AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_
#pragma once

#include "PhysicsShell.h"
#include "weaponammo.h"
#include "UIStaticItem.h"
#include "ParticlesObject.h"
#include "PHShellCreator.h"

#include "ShootingObject.h"
#include "HudItem.h"


// refs
class CEntity;
class ENGINE_API CMotionDef;
class CSE_ALifeItemWeapon;
class CSE_ALifeItemWeaponAmmo;

class CWeapon : public CHudItem,
				public CShootingObject
{
private:
	typedef CHudItem inherited;
	friend BOOL __stdcall firetrace_callback(Collide::rq_result& result, LPVOID params);
public:
	enum					{ MAX_ANIM_COUNT = 8 };
	typedef					svector<CMotionDef*,MAX_ANIM_COUNT>		MotionSVec;
	
	enum EHandDependence{
		hdNone	= 0,
		hd1Hand	= 1,
		hd2Hand	= 2
	};

protected:
	EHandDependence			eHandDependence;		// 0-������������ ��� ������� ���, 1-���� ����, 2-��� ����
	bool					bWorking, bWorking2;	// Weapon fires now
	ref_str					m_WpnName;


	bool					bMisfire;				// a misfire happens, you'll need to rearm weapon

	Fmatrix					m_Offset;

	void					signal_HideComplete		();

	Fvector					vFirePoint;
	Fvector					vFirePoint2;

protected:
	ref_shader				hUIIcon;
									
	int						iAmmoElapsed;		// ammo in magazine, currently
	int						iMagazineSize;		// size (in bullets) of magazine
	int						iBuckShot;
							
	float					fTimeToFire;
	int						iHitPower;
	float					fHitImpulse;
	//�������� ������ ���� �� ������
	float					m_fStartBulletSpeed;


	Fvector					vLastFP, vLastFP2;
	Fvector					vLastFD;
	//����������� � ����� ������ �����
	Fvector					vLastSP, vLastSD;

	//����������� �� ����� ��������
	float					fireDistance;
	float					fireDispersionBase;
	float					fireDispersion;
	float					fireDispersion_Inc;
	float					fireDispersion_Dec;
	float					fireDispersion_Current;
	//������ ���������� ��������� ��� ������������ ����������� (� ���������)
	float					fireDispersionConditionFactor;
	//����������� ������ ��� ������������ �����������
	float					misfireProbability;
	//���������� ����������� ��� ��������
	float					conditionDecreasePerShot;

	float					camMaxAngle;
	float					camRelaxSpeed;
	float					camDispersion;
							
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
	//��������� ��������� �� ����� ��������
	bool					m_bShotLight;

	float					fZoomFactor;

	//����������� ����������� ��������� �������
	ALife::EWeaponAddonStatus	m_eScopeStatus;
	ALife::EWeaponAddonStatus	m_eSilencerStatus;
	ALife::EWeaponAddonStatus	m_eGrenadeLauncherStatus;

	//�������� ������ ������������ �������
	ref_str		m_sScopeName;
	ref_str		m_sSilencerName;
	ref_str		m_sGrenadeLauncherName;

	//�������� ������ ��������� � ���������
	int	m_iScopeX, m_iScopeY;
	int	m_iSilencerX, m_iSilencerY;
	int	m_iGrenadeLauncherX, m_iGrenadeLauncherY;
	
	//�������� ��� ������������ �������, � ������ �����������
	CUIStaticItem	m_UIScope;
	//����������� ���������� �������
	float			m_fScopeZoomFactor;
	//����� ����������� �����������
	bool			m_bZoomMode;

	//��������� ������������ �������
	u8			m_flagsAddOnState;

	//����� �������� ������
	ALife::_TIME_ID			m_dwWeaponRemoveTime;
	ALife::_TIME_ID			m_dwWeaponIndependencyTime;

protected:
	////////////////// network ///////////////////	
protected:
	void					Light_Start			();
	void					Light_Render		(Fvector& P);

	virtual void			UpdatePosition		(const Fmatrix& transform);
	virtual void			UpdateFP			();
	virtual void			UpdateXForm			();


	//������������� ������ ����
	virtual BOOL			FireTrace			(const Fvector& P, const Fvector& Peff,	Fvector& D);

	// Utilities
	void					ShaderCreate		(ref_shader&	dest, LPCSTR S, LPCSTR T);
	void					ShaderDestroy		(ref_shader&	dest);

public:

	enum EWeaponStates {
		eIdle		= 0,
		eFire,
		eFire2,
		eReload,
		eShowing,
		eHiding,
		eHidden,
		eMisfire,
		eMagEmpty,
		eSwitch,
	};

	
	//��� ������ ����������� � ������������ �������
	virtual void			OnZoomIn			();
	virtual void			OnZoomOut			();
	virtual bool			IsZoomed			()			{return m_bZoomMode;}
	CUIStaticItem*			ZoomTexture			();


	//������� ����� ��� ����� ������
	virtual void			OnMagazineEmpty		();
	virtual void			OnAnimationEnd		()			{};
	

	////////////////////////////////////////////////
	//����� ������� ��� ������ � ���������� ������
	virtual void			StartParticles		(CParticlesObject*& pParticles, LPCSTR particles_name, const Fvector& pos, const Fvector& vel = zero_vel, bool auto_remove_flag = false);
	virtual void			StopParticles		(CParticlesObject*& pParticles);
	virtual void			UpdateParticles		(CParticlesObject*& pParticles, const Fvector& pos, const  Fvector& vel = zero_vel);

	//������������� ������� ��� ���������
	//�������� ����
	virtual void			StartFlameParticles	();
	virtual void			StopFlameParticles	();
	virtual void			UpdateFlameParticles();
	//��� ������� ������
	virtual void			StartFlameParticles2();
	virtual void			StopFlameParticles2	();
	virtual void			UpdateFlameParticles2();

	//�������� ����
	virtual void			StartSmokeParticles	();
	//�������� �����
	virtual void			OnShellDrop		();

	virtual ALife::_TIME_ID	TimePassedAfterIndependant();
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

	virtual bool			NeedToDestroyObject();
	
	virtual void			renderable_Render	();
	
	virtual void			OnH_B_Chield		();
	virtual void			OnH_B_Independent	();
	virtual void			OnH_A_Independent	();
	virtual void			OnEvent				(NET_Packet& P, u16 type) {inherited::OnEvent(P,type);}

	virtual	void			Hit					(float P, Fvector &dir,	
												 CObject* who, s16 element,
												 Fvector position_in_object_space, 
												 float impulse, 
												 ALife::EHitType hit_type = ALife::eHitTypeWound);

	// logic & effects
	virtual void			SetDefaults			();
	
	virtual void			FireStart			();
	virtual void			FireEnd				();
	virtual void			Fire2Start			();
	virtual void			Fire2End			();
	virtual void			Reload				();
	// ��������� ������������ ��������
	virtual void			OnShot				(){};
	virtual void			AddShotEffector		();
	virtual void			RemoveShotEffector	();
	
	   BOOL					IsMisfire			() const;
	   BOOL					CheckForMisfire		();

	IC BOOL					IsWorking			()	const		{	return bWorking;							}
	IC BOOL					IsValid				()	const		{	return iAmmoElapsed;						}
	IC BOOL					IsVisible			()	const		{	return getVisible();						}	// Weapon change occur only after visibility change
	IC BOOL					IsUpdating			()	const		{	return bWorking || m_bPending || getVisible();}	// Does weapon need's update?
	virtual bool			IsHidden			()				{	return STATE == eHidden;}						// Does weapon is in hidden state
	virtual bool			IsHiding			()				{	return STATE == eHiding;}
		
	IC EHandDependence		HandDependence		()				{	return eHandDependence;}
	virtual float			GetZoomFactor		()				{	return fZoomFactor;							}

	//������ ��������� ��� �������� �� ������
	float					GetPrecision		();

	IC LPCSTR				GetName				()	const		{	return *m_WpnName;							}
	IC int					GetAmmoElapsed		()	const		{	return /*int(m_magazine.size())*/iAmmoElapsed;						}
	IC int					GetAmmoMagSize		()	const		{	return iMagazineSize;						}

	//�������� ������ � ���������� �� ��� ��������� �����������
	float					GetConditionDispersionFactor	();
	float					GetConditionMisfireProbability	();

	int						GetAmmoCurrent		()  const;
	IC ref_shader			GetUIIcon			()				{	return hUIIcon;								}
	
	void					animGet	(MotionSVec& lst, LPCSTR prefix);

	// Inventory
	virtual bool Action(s32 cmd, u32 flags);
	
	///////////////////////////////////////////
	// ������ � �������� � ������
	//////////////////////////////////////////

	virtual bool Attach(PIItem pIItem);
	virtual bool Detach(const char* item_section_name);

	virtual bool IsGrenadeLauncherAttached();
	virtual bool IsScopeAttached();
	virtual bool IsSilencerAttached();

	virtual bool GrenadeLauncherAttachable();
	virtual bool ScopeAttachable();
	virtual bool SilencerAttachable();

	//���������� ��������� ��� �������� �������
	virtual void UpdateAddonsVisibility();
	//������������� ������� �������������� �������
	virtual void InitAddons();


	//��� ������������ ������ ��������� � ����������
	int	GetScopeX() {return m_iScopeX;}
	int	GetScopeY() {return m_iScopeY;}
	int	GetSilencerX() {return m_iSilencerX;}
	int	GetSilencerY() {return m_iSilencerY;}
	int	GetGrenadeLauncherX() {return m_iGrenadeLauncherX;}
	int	GetGrenadeLauncherY() {return m_iGrenadeLauncherY;}

	virtual const ref_str& GetGrenadeLauncherName() {return m_sGrenadeLauncherName;}
	virtual const ref_str& GetScopeName()			{return  m_sScopeName;}
	virtual const ref_str& GetSilencerName()		{return m_sSilencerName;}

	virtual LPCSTR	Name();

	void SpawnAmmo		(u32 boxCurr = 0xffffffff, LPCSTR ammoSect = NULL, u32 ParentID = 0xffffffff);

	CWeaponAmmo*		m_pAmmo;
	xr_vector<ref_str>	m_ammoTypes;
	u32					m_ammoType;
	ref_str				m_ammoName;
	string64			m_tmpName;

	// Multitype ammo support
	xr_stack<CCartridge> m_magazine;

	//��� ������ ������
	Fvector				m_vRecoilDeltaAngle;
	const Fvector&		GetRecoilDeltaAngle();

	//////////////////////////////////
	// ����������� �������
	/////////////////////////////////

	//��� ��������� ��� ����
	LPCSTR				m_sFlameParticlesCurrent;
	//��� �������� 1� � 2� ����� ��������
	LPCSTR				m_sFlameParticles;
	LPCSTR				m_sFlameParticles2;
	
//	#define PARTICLES_CACHE_SIZE 4
	//������ ��������� ����
	CParticlesObject*	m_pFlameParticles;
//	CParticlesObject*	m_pFlameParticlesCache[PARTICLES_CACHE_SIZE];
//	int					m_iNextParticle;


	//������ ��������� ��� �������� �� 2-�� ������
	CParticlesObject*	m_pFlameParticles2;

	//��� ��������� ��� ����
	LPCSTR				m_sSmokeParticlesCurrent;
	LPCSTR				m_sSmokeParticles;

	//����� ������ ��������� �����
	Fvector				vShellPoint;
	//��� ��������� ��� �����
	LPCSTR				m_sShellParticles;


	//��� ���������, ���� ��� ����� ����������� ������� ������������� 
	//������
	float					m_fMinRadius;
	float					m_fMaxRadius;

public:
	virtual void			make_Interpolation	();
	virtual void			PH_B_CrPr			(); // actions & operations before physic correction-prediction steps
	virtual void			PH_I_CrPr			(); // actions & operations after correction before prediction steps
	virtual void			PH_A_CrPr			(); // actions & operations after phisic correction-prediction steps
	virtual void			OnH_A_Chield		();
	virtual void			reinit				();
	virtual void			reload				(LPCSTR section);
	virtual void			create_physic_shell	();
	virtual void			activate_physic_shell();
	virtual void			setup_physic_shell	();
	virtual bool			can_kill			() const;
	virtual CInventoryItem	*can_kill			(CInventory *inventory) const;
	virtual const CInventoryItem *can_kill		(const xr_set<const CGameObject*> &items) const;
	virtual bool			ready_to_kill		() const;
};

#endif // !defined(AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_)
