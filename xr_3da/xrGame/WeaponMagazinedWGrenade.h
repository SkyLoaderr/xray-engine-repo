#pragma once
#include "weaponmagazined.h"
#include "..\feel_touch.h"


class CWeaponFakeGrenade :
	public CGameObject,
	public Feel::Touch
{
	typedef CGameObject inherited;
public:
	CWeaponFakeGrenade(void);
	virtual ~CWeaponFakeGrenade(void);

	void SoundCreate(ref_sound& dest, LPCSTR name, int iType=0, BOOL bCtrlFreq=FALSE);
	void SoundDestroy(ref_sound& dest);

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();
	virtual void OnH_B_Independent();
	virtual void UpdateCL();

	virtual void feel_touch_new(CObject* O);

	void Explode(const Fvector &pos, const Fvector &normal);
	virtual void FragWallmark(const Fvector& vDir, const Fvector &vEnd, Collide::ray_query& R);

	Fvector			m_pos, m_vel;
	CGameObject*	m_pOwner;

	f32				m_blast, m_blastR, m_fragsR, m_fragHit;
	s32				m_frags;
	xr_list<CGameObject*> m_blasted;

	enum EState{
		stInactive,
		stEngine,
		stFlying,
		stExplode,
		stDestroying
	};
	EState			m_state;

	LPSTR			pstrWallmark;
	ref_shader		hWallmark;
	float			fWallmarkSize;
	ref_sound		sndRicochet[SND_RIC_COUNT], sndExplode;
	ESoundTypes		m_eSoundRicochet, m_eSoundExplode;
	s32				m_engineTime, m_explodeTime, m_flashTime;
	char			m_effectsSTR[255];
	xr_vector<LPCSTR>	m_effects;
	//char			m_trailEffectsSTR[255];
	//vector<LPCSTR>	m_trailEffects;
	//list<CPGObject*>m_trailEffectsPSs;
	IRender_Light*	m_pLight;
	Fcolor			m_lightColor;
	Fcolor			m_curColor;
	f32				m_lightRange;
	u32				m_lightTime;
	f32				m_mass, m_engine_f, m_engine_u, m_jump;

	static void __stdcall ObjectContactCallback(bool& do_colide,dContact& c);
};


class CWeaponMagazinedWGrenade :
	public CWeaponMagazined
{
	typedef CWeaponMagazined inherited;
public:
	CWeaponMagazinedWGrenade(LPCSTR name,ESoundTypes eSoundType);
	virtual ~CWeaponMagazinedWGrenade(void);

	virtual void	Load			(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void OnStateSwitch(u32 S);
	virtual void	switch2_Idle	();
	virtual void	switch2_Reload	();
	virtual void	state_Fire		(float dt);
	virtual void	OnShot			();
	virtual void			SwitchState			(u32 S);
	virtual void OnEvent(NET_Packet& P, u16 type);
	virtual void ReloadMagazine();

	virtual bool Action(s32 cmd, u32 flags);

	void SwitchMode();

	static void	__stdcall GrenadeCallback(CBoneInstance*);

	bool m_grenadeMode;
	ref_sound			sndShotG;
	ref_sound			sndReloadG;
	MotionSVec		mhud_idle_g;
	MotionSVec		mhud_reload_g;
	MotionSVec		mhud_shots_g;
	MotionSVec		mhud_switch_g, mhud_switch;

	CWeaponAmmo *m_pAmmo2;
	char m_ammoSect2[255];
	xr_vector<LPCSTR> m_ammoTypes2;
	u32 m_ammoType2;
	LPCSTR m_ammoName2;
	int iMagazineSize2;
	xr_stack<CCartridge> m_magazine2;
	CWeaponFakeGrenade *m_pGrenade;
	bool m_hideGrenade;
	Fvector *m_pGrenadePoint;
};
