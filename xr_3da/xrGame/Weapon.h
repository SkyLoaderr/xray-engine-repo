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
#include "hud_item_object.h"
#include "Actor_Flags.h"
#include "../SkeletonAnimated.h"


// refs
class CEntity;
class ENGINE_API CMotionDef;
class CSE_ALifeItemWeapon;
class CSE_ALifeItemWeaponAmmo;
class CWeaponMagazined;

class CWeapon : public CHudItemObject,
				public CShootingObject
{
//////////////////////////////////////////////////////////////////////////
//  General
//////////////////////////////////////////////////////////////////////////
private:
	typedef CHudItemObject inherited;

public:
							CWeapon				(LPCSTR name);
	virtual					~CWeapon			();

	// Generic
	virtual void			Load				(LPCSTR section);

	virtual BOOL			net_Spawn			(CSE_Abstract* DC);
	virtual void			net_Destroy			();
	virtual void			net_Export			(NET_Packet& P);	// export to server
	virtual void			net_Import			(NET_Packet& P);	// import from server
	virtual CWeapon			*cast_weapon			()		 {return this;}
	virtual CWeaponMagazined*cast_weapon_magazined	()		 {return 0;}


	//serialization
	virtual void	save				(NET_Packet &output_packet);
	virtual void	load				(IReader &input_packet);
	virtual BOOL	net_SaveRelevant	()								{return inherited::net_SaveRelevant();}

	virtual void			UpdateCL			();
	virtual void			shedule_Update		(u32 dt);


	virtual const Fmatrix&	ParticlesXFORM() const;
	virtual IRender_Sector*	Sector();

	virtual void			renderable_Render	();

	virtual void			OnH_B_Chield		();
	virtual void			OnH_A_Chield		();
	virtual void			OnH_B_Independent	();
	virtual void			OnH_A_Independent	();
	virtual void			OnEvent				(NET_Packet& P, u16 type);// {inherited::OnEvent(P,type);}

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

	virtual void			SwitchState			(u32 S);
	virtual bool			Activate			();

	//������������� ���� ���� � �������� ����� ��� �������� �� OnH_B_Chield
	virtual void	OnActiveItem		();
	virtual void	OnHiddenItem		();

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
	virtual const CInventoryItem *can_kill		(const xr_vector<const CGameObject*> &items) const;
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
	typedef					svector<MotionID,MAX_ANIM_COUNT>		MotionSVec;

	void					animGet				(MotionSVec& lst, LPCSTR prefix);
	virtual void			OnAnimationEnd		()			{};
	void					signal_HideComplete	();

//////////////////////////////////////////////////////////////////////////
//  InventoryItem methods
//////////////////////////////////////////////////////////////////////////
public:
	virtual bool			Action(s32 cmd, u32 flags);
//	virtual LPCSTR			Name();

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
	enum EWeaponSubStates{
		eSubstateReloadBegin		=0,
		eSubstateReloadInProcess,
		eSubstateReloadEnd,
	};

	virtual bool			IsHidden			()	const		{	return STATE == eHidden;}						// Does weapon is in hidden state
	virtual bool			IsHiding			()	const		{	return STATE == eHiding;}

	IC BOOL					IsValid				()	const		{	return iAmmoElapsed;						}
	// Does weapon need's update?
	IC BOOL					IsUpdating			()	const		{	return bWorking || m_bPending || getVisible();}	


	BOOL					IsMisfire			() const;
	BOOL					CheckForMisfire		();


	BOOL					AutoSpawnAmmo		() const		{ return m_bAutoSpawnAmmo; };

protected:
	u8						m_sub_state;
	// Weapon fires now
	bool					bWorking2;
	// a misfire happens, you'll need to rearm weapon
	bool					bMisfire;				

	BOOL					m_bAutoSpawnAmmo;
//////////////////////////////////////////////////////////////////////////
//  Weapon Addons
//////////////////////////////////////////////////////////////////////////
public:
	///////////////////////////////////////////
	// ������ � �������� � ������
	//////////////////////////////////////////

	virtual bool Attach(PIItem pIItem);
	virtual bool Detach(const char* item_section_name);

			bool IsGrenadeLauncherAttached	() const;
			bool IsScopeAttached			() const;
			bool IsSilencerAttached			() const;

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

	virtual const shared_str& GetGrenadeLauncherName	()		{return m_sGrenadeLauncherName;}
	virtual const shared_str& GetScopeName				()		{return m_sScopeName;}
	virtual const shared_str& GetSilencerName			()		{return m_sSilencerName;}

	virtual	u8		GetAddonsState						()		const		{return m_flagsAddOnState;};
protected:
	//��������� ������������ �������
	u8 m_flagsAddOnState;

	//����������� ����������� ��������� �������
	ALife::EWeaponAddonStatus	m_eScopeStatus;
	ALife::EWeaponAddonStatus	m_eSilencerStatus;
	ALife::EWeaponAddonStatus	m_eGrenadeLauncherStatus;

	//�������� ������ ������������ �������
	shared_str		m_sScopeName;
	shared_str		m_sSilencerName;
	shared_str		m_sGrenadeLauncherName;

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
	//����� �����������
	float			m_fZoomRotateTime;
	//�������� ��� ������������ �������, � ������ �����������
	CUIStaticItem	m_UIScope;
	//����������� ���������� �������
	float			m_fScopeZoomFactor;
	//����� ����� ����������� �������
	bool			m_bZoomMode;
	//�� 0 �� 1, ���������� ��������� ���������
	//�� ���������� HUD  
	float			m_fZoomRotationFactor;
	bool			m_bHideCrosshairInZoom;
public:

	IC bool					IsZoomEnabled		()	const	{return m_bZoomEnabled;}
	virtual void			OnZoomIn			();
	virtual void			OnZoomOut			();
	virtual bool			IsZoomed			()	const	{return m_bZoomMode;}
	CUIStaticItem*			ZoomTexture			();
	virtual bool			ZoomHideCrosshair	()			{return m_bHideCrosshairInZoom || ZoomTexture();}

	IC float				GetZoomFactor		() const		{	return m_fZoomFactor;	}
	//����������, ��� ������ ��������� � ���������� �������� ��� ������������� ������������
	virtual bool			IsRotatingToZoom	() const		{	return (m_fZoomRotationFactor<1.f);}

			void			LoadZoomOffset		(LPCSTR section, LPCSTR prefix);

//////////////////////////////////////////////////////////////////////////
//  Weapon generic logic
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//  Position and oritentation
//////////////////////////////////////////////////////////////////////////

public:
    virtual EHandDependence		HandDependence		()	const		{	return eHandDependence;}
	virtual bool				IsSingleHanded		()	const		{	return m_bIsSingleHanded; }

public:
	IC		LPCSTR			strap_bone0			() const {return m_strap_bone0;}
	IC		LPCSTR			strap_bone1			() const {return m_strap_bone1;}
	IC		void			strapped_mode		(bool value) {m_strapped_mode = value;}
	IC		bool			strapped_mode		() const {return m_strapped_mode;}

protected:
	LPCSTR					m_strap_bone0;
	LPCSTR					m_strap_bone1;
	Fmatrix					m_StrapOffset;
	bool					m_strapped_mode;
	bool					m_can_be_strapped;

	Fmatrix					m_Offset;
	// 0-������������ ��� ������� ���, 1-���� ����, 2-��� ����
	EHandDependence			eHandDependence;
	bool					m_bIsSingleHanded;
	//����������� ��� ��������� ���� � ����
	Fmatrix					m_FireParticlesXForm;

	//������� ��������� � ���������� ��� ���������
	Fvector					vLastFP, vLastFP2; //����
	Fvector					vLastFD;
	Fvector					vLastSP, vLastSD;  //�����	

	//����������� ���������
	Fvector					vFirePoint;
	Fvector					vFirePoint2;

protected:
	virtual void			UpdatePosition		(const Fmatrix& transform);
	virtual void			UpdateFP			();
	virtual void			UpdateXForm			();
	virtual void			UpdateHudAdditonal	(Fmatrix&);

	virtual const Fvector&	CurrentFirePoint	() {return vLastFP;}
//////////////////////////////////////////////////////////////////////////
// Weapon fire
//////////////////////////////////////////////////////////////////////////
protected:
	virtual void			SetDefaults			();

	//������������� ������ ����
	virtual void			FireTrace			(const Fvector& P, const Fvector& D);

	virtual void			FireStart			() {CShootingObject::FireStart();}
	virtual void			FireEnd				() {CShootingObject::FireEnd();}

	virtual void			Fire2Start			();
	virtual void			Fire2End			();
	virtual void			Reload				();
	virtual void			StopShooting		();
    

	// ��������� ������������ ��������
	virtual void			OnShot				(){};
	virtual void			AddShotEffector		();
	virtual void			RemoveShotEffector	();

public:
	//������� ��������� (� ��������) ������ � ������ ������������� �������
	float					GetFireDispersion	(bool with_cartridge)			const;
	float					GetFireDispersion	(float cartridge_k)				const;
	const Fvector&			GetRecoilDeltaAngle	();

	//�������� ������ � ���������� �� ��� ��������� �����������
	float					GetConditionDispersionFactor	() const;
	float					GetConditionMisfireProbability	() const;


protected:
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
	float					camMaxAngleHorz;
	float					camStepAngleHorz;

protected:
	//��� ������ ������
	Fvector					m_vRecoilDeltaAngle;

	//��� ���������, ���� ��� ����� ����������� ������� ������������� 
	//������
	float					m_fMinRadius;
	float					m_fMaxRadius;

//////////////////////////////////////////////////////////////////////////
// ��������
//////////////////////////////////////////////////////////////////////////

protected:	
	//��� ������� ������
	virtual void			StartFlameParticles2();
	virtual void			StopFlameParticles2	();
	virtual void			UpdateFlameParticles2();
protected:
	shared_str					m_sFlameParticles2;
	//������ ��������� ��� �������� �� 2-�� ������
	CParticlesObject*		m_pFlameParticles2;

//////////////////////////////////////////////////////////////////////////
// Weapon and ammo
//////////////////////////////////////////////////////////////////////////
public:
	IC int					GetAmmoElapsed		()	const		{	return /*int(m_magazine.size())*/iAmmoElapsed;						}
	IC int					GetAmmoMagSize		()	const		{	return iMagazineSize;						}
	int						GetAmmoCurrent		()  const;

	void					SetAmmoElapsed		(int ammo_count);

	virtual void			OnMagazineEmpty		();
			void			SpawnAmmo			(u32 boxCurr = 0xffffffff, 
													LPCSTR ammoSect = NULL, 
													u32 ParentID = 0xffffffff);

	//���������� �� ���-�� �������� �� ������
	virtual bool			ShowAmmo			() {return m_bShowAmmo;	}
protected:
	int						iAmmoElapsed;		// ammo in magazine, currently
	int						iMagazineSize;		// size (in bullets) of magazine
	int						iBuckShot;

	bool					m_bShowAmmo;

	//��� �������� � GetAmmoCurrent
	mutable int				iAmmoCurrent;
	mutable u32				m_dwAmmoCurrentCalcFrame;	//���� �� ������� ���������� ���-�� ��������

	virtual bool 			IsNecessaryItem	    (CInventoryItem* item);

public:
	xr_vector<shared_str>	m_ammoTypes;

	CWeaponAmmo*			m_pAmmo;
	u32						m_ammoType;
	shared_str				m_ammoName;

	// Multitype ammo support
	xr_stack<CCartridge>	m_magazine;
	CCartridge				m_DefaultCartridge;
	float					m_fCurrentCartirdgeDisp;

	IC	bool				unlimited_ammo() { return psActorFlags.test(AF_UNLIMITEDAMMO) && m_DefaultCartridge.m_bCanBeUnlimited; };
	IC	bool				can_be_strapped	() const {return m_can_be_strapped;};

protected:
	u32						m_ef_main_weapon_type;
	u32						m_ef_weapon_type;

public:
	virtual u32				ef_main_weapon_type	() const;
	virtual u32				ef_weapon_type		() const;

protected:
	// This is because when scope is attached we can't ask scope for these params
	// therefore we should hold them by ourself :-((
	float					m_addon_holder_range_modifier;
	float					m_addon_holder_fov_modifier;

public:
	virtual	void			modify_holder_params		(float &range, float &fov) const;
};

#endif // !defined(AFX_WEAPON_H__7C42AD7C_0EBD_4AD1_90DE_2F972BF538B9__INCLUDED_)
