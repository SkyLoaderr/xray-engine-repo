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
//////////////////////////////////////////////////////////////////////////
//  General
//////////////////////////////////////////////////////////////////////////
private:
	typedef CHudItem inherited;
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
	virtual void			OnH_A_Chield		();
	virtual void			OnH_B_Independent	();
	virtual void			OnH_A_Independent	();
	virtual void			OnEvent				(NET_Packet& P, u16 type) {inherited::OnEvent(P,type);}

	virtual	void			Hit					(float P, Fvector &dir,	
												CObject* who, s16 element,
												Fvector position_in_object_space, 
												float impulse, 
												ALife::EHitType hit_type = ALife::eHitTypeWound);

	virtual void			reinit				();
	virtual void			reload				(LPCSTR section);
	virtual void			create_physic_shell	();
	virtual void			activate_physic_shell();
	virtual void			setup_physic_shell	();

//////////////////////////////////////////////////////////////////////////
//  Network
//////////////////////////////////////////////////////////////////////////

public:
	virtual void			make_Interpolation	();
	virtual void			PH_B_CrPr			(); // actions & operations before physic correction-prediction steps
	virtual void			PH_I_CrPr			(); // actions & operations after correction before prediction steps
	virtual void			PH_A_CrPr			(); // actions & operations after phisic correction-prediction steps
	virtual bool			can_kill			() const;
	virtual CInventoryItem	*can_kill			(CInventory *inventory) const;
	virtual const CInventoryItem *can_kill		(const xr_set<const CGameObject*> &items) const;
	virtual bool			ready_to_kill		() const;
	virtual bool			NeedToDestroyObject	() const; 
	virtual ALife::_TIME_ID	TimePassedAfterIndependant() const;
protected:
	//����� �������� ������
	ALife::_TIME_ID			m_dwWeaponRemoveTime;
	ALife::_TIME_ID			m_dwWeaponIndependencyTime;

//////////////////////////////////////////////////////////////////////////
//  Animation 
//////////////////////////////////////////////////////////////////////////
public:
	enum					{ MAX_ANIM_COUNT = 8 };
	typedef					svector<CMotionDef*,MAX_ANIM_COUNT>		MotionSVec;

	void					animGet				(MotionSVec& lst, LPCSTR prefix);
	virtual void			OnAnimationEnd		()			{};
	void					signal_HideComplete	();

//////////////////////////////////////////////////////////////////////////
//  InventoryItem methods
//////////////////////////////////////////////////////////////////////////
public:
	virtual bool			Action(s32 cmd, u32 flags);
	virtual LPCSTR			Name();

//////////////////////////////////////////////////////////////////////////
//  Weapon state
//////////////////////////////////////////////////////////////////////////
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

	virtual bool			IsHidden			()	const		{	return STATE == eHidden;}						// Does weapon is in hidden state
	virtual bool			IsHiding			()	const		{	return STATE == eHiding;}

	IC BOOL					IsWorking			()	const		{	return bWorking;							}
	IC BOOL					IsValid				()	const		{	return iAmmoElapsed;						}
	// Does weapon need's update?
	IC BOOL					IsUpdating			()	const		{	return bWorking || m_bPending || getVisible();}	


	BOOL					IsMisfire			() const;
	BOOL					CheckForMisfire		();

protected:
	// Weapon fires now
	bool					bWorking, bWorking2;
	// a misfire happens, you'll need to rearm weapon
	bool					bMisfire;				

//////////////////////////////////////////////////////////////////////////
//  Weapon Addons
//////////////////////////////////////////////////////////////////////////
public:
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

	virtual const ref_str& GetGrenadeLauncherName	()		{return m_sGrenadeLauncherName;}
	virtual const ref_str& GetScopeName				()		{return  m_sScopeName;}
	virtual const ref_str& GetSilencerName			()		{return m_sSilencerName;}

protected:
	//��������� ������������ �������
	u8 m_flagsAddOnState;

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

///////////////////////////////////////////////////
//	��� ������ ����������� � ������������ �������
///////////////////////////////////////////////////
protected:
	//���������� ������ �����������
	bool			m_bZoomEnabled;
	//������� ������ �����������
	float			m_fZoomFactor;
	//�������� ��� ������������ �������, � ������ �����������
	CUIStaticItem	m_UIScope;
	//����������� ���������� �������
	float			m_fScopeZoomFactor;
	//����� ����� ����������� �������
	bool			m_bZoomMode;

public:

	IC bool					IsZoomEnabled		()	const	{return m_bZoomEnabled;}
	virtual void			OnZoomIn			();
	virtual void			OnZoomOut			();
	virtual bool			IsZoomed			()	const	{return m_bZoomMode;}
	CUIStaticItem*			ZoomTexture			();
	IC float				GetZoomFactor		()	const		{	return m_fZoomFactor;	}

//////////////////////////////////////////////////////////////////////////
//  Weapon generic logic
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//  Position and oritentation
//////////////////////////////////////////////////////////////////////////

public:
	enum EHandDependence{
		hdNone	= 0,
		hd1Hand	= 1,
		hd2Hand	= 2
	};

	IC EHandDependence		HandDependence		()	const		{	return eHandDependence;}
protected:
	EHandDependence			eHandDependence;		// 0-������������ ��� ������� ���, 1-���� ����, 2-��� ����
	Fmatrix					m_Offset;

	//������� ��������� � ���������� ��� ���������
	Fvector					vLastFP, vLastFP2; //����
	Fvector					vLastFD;
	Fvector					vLastSP, vLastSD;  //�����	

	//����������� ���������
	Fvector					vFirePoint;
	Fvector					vFirePoint2;
	Fvector					vShellPoint;

protected:
	virtual void			UpdatePosition		(const Fmatrix& transform);
	virtual void			UpdateFP			();
	virtual void			UpdateXForm			();


//////////////////////////////////////////////////////////////////////////
// Weapon fire
//////////////////////////////////////////////////////////////////////////
protected:
	virtual void			SetDefaults			();

	//������������� ������ ����
	virtual BOOL			FireTrace			(const Fvector& P, const Fvector& Peff,	Fvector& D);

	virtual void			FireStart			();
	virtual void			FireEnd				();
	virtual void			Fire2Start			();
	virtual void			Fire2End			();
	virtual void			Reload				();


	// ��������� ������������ ��������
	virtual void			OnShot				(){};
	virtual void			AddShotEffector		();
	virtual void			RemoveShotEffector	();

public:
	//������� ��������� (� ��������) ������ � ������ ������������� �������
	float					GetFireDispersion	()					const;
	float					GetFireDispersion	(float cartridge_k) const;
	const Fvector&			GetRecoilDeltaAngle	();

	//�������� ������ � ���������� �� ��� ��������� �����������
	float					GetConditionDispersionFactor	() const;
	float					GetConditionMisfireProbability	() const;


protected:
	float					fTimeToFire;
	int						iHitPower;
	float					fHitImpulse;

	//�������� ������ ���� �� ������
	float					m_fStartBulletSpeed;
	//������������ ���������� ��������
	float					fireDistance;

protected:
	//����������� �� ����� ��������
	float					fireDispersionBase;
	//������ ���������� ��������� ��� ������������ ����������� 
	//(�� ������� ��������� ���������� ���������)
	float					fireDispersionConditionFactor;
	//����������� ������ ��� ������������ �����������
	float					misfireProbability;
	//���������� ����������� ��� ��������
	float					conditionDecreasePerShot;

	//������ ��� �������� 
	float					camMaxAngle;
	float					camRelaxSpeed;
	float					camDispersion;

	//��������
	float					tracerHeadSpeed;
	float					tracerTrailCoeff;
	float					tracerStartLength;
	float					tracerWidth;
	u32						tracerFrame;

protected:
	//��� ������ ������
	Fvector					m_vRecoilDeltaAngle;

	//��� ���������, ���� ��� ����� ����������� ������� ������������� 
	//������
	float					m_fMinRadius;
	float					m_fMaxRadius;

//////////////////////////////////////////////////////////////////////////
// Lights
//////////////////////////////////////////////////////////////////////////
protected:
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
public:
	void					Light_Start			();
	void					Light_Render		(Fvector& P);


//////////////////////////////////////////////////////////////////////////
// Weapon and ammo
//////////////////////////////////////////////////////////////////////////
public:
	IC int					GetAmmoElapsed		()	const		{	return /*int(m_magazine.size())*/iAmmoElapsed;						}
	IC int					GetAmmoMagSize		()	const		{	return iMagazineSize;						}
	int						GetAmmoCurrent		()  const;

	virtual void			OnMagazineEmpty		();
			void			SpawnAmmo			(u32 boxCurr = 0xffffffff, 
													LPCSTR ammoSect = NULL, 
													u32 ParentID = 0xffffffff);
protected:
	int						iAmmoElapsed;		// ammo in magazine, currently
	int						iMagazineSize;		// size (in bullets) of magazine
	int						iBuckShot;

public:
	xr_vector<ref_str>		m_ammoTypes;

	CWeaponAmmo*			m_pAmmo;
	u32						m_ammoType;
	ref_str					m_ammoName;
	string64				m_tmpName;

	// Multitype ammo support
	xr_stack<CCartridge> m_magazine;

//////////////////////////////////////////////////////////////////////////
// ����������� �������
//////////////////////////////////////////////////////////////////////////
protected:
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

protected:
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

	//��� ��������� ��� �����
	LPCSTR				m_sShellParticles;
};

#endif // !defined(AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_)
