#pragma once
#include "weaponpistol.h"
#include "..\feel_touch.h"

class CWeaponRPG7Grenade :
	public CGameObject,
	public Feel::Touch
{
	typedef CGameObject inherited;
public:
	CWeaponRPG7Grenade(void);
	virtual ~CWeaponRPG7Grenade(void);

	void SoundCreate(sound& dest, LPCSTR name, int iType=0, BOOL bCtrlFreq=FALSE);
	void SoundDestroy(sound& dest);

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();
	virtual void OnH_B_Independent();
	virtual void UpdateCL();

	virtual void feel_touch_new(CObject* O);

	void Explode(const Fvector &pos, const Fvector &normal);
	virtual void FragWallmark(const Fvector& vDir, const Fvector &vEnd, Collide::ray_query& R);

	Fvector m_pos, m_vel;
	CGameObject *m_pOwner;

	f32 m_blast, m_blastR, m_fragsR, m_fragHit;
	s32 m_frags;
	list<CGameObject*> m_blasted;

	LPSTR pstrWallmark;
	Shader* hWallmark;
	float fWallmarkSize;
	sound sndRicochet[SND_RIC_COUNT], sndExplode;
	ESoundTypes m_eSoundRicochet, m_eSoundExplode;
	u32 m_engineTime, m_explodeTime;
	char m_effectsSTR[255];
	vector<LPCSTR> m_effects;
	char m_trailEffectsSTR[255];
	vector<LPCSTR> m_trailEffects;
	list<CPGObject*> m_trailEffectsPSs;
	IRender_Light* m_pLight;
	Fcolor m_lightColor;
	Fcolor m_curColor;
	f32 m_lightRange;
	u32 m_lightTime;
	f32 m_mass, m_engine_f, m_engine_u;

	static void __stdcall ObjectContactCallback(bool& do_colide,dContact& c);
};

class CWeaponRPG7 :
	public CWeaponCustomPistol
{
	typedef CWeaponCustomPistol inherited;
	float			fMaxZoomFactor;
public:
	CWeaponRPG7(void);
	virtual ~CWeaponRPG7(void);

	virtual BOOL net_Spawn(LPVOID DC);
	virtual void OnStateSwitch(u32 S);
	virtual void OnEvent(NET_Packet& P, u16 type);
	virtual void ReloadMagazine();
	virtual void	Load			(LPCSTR section);
	virtual void	switch2_Fire	();
	virtual void			Fire2Start			();
	virtual void			Fire2End			();
	virtual bool Action(s32 cmd, u32 flags);
	virtual void	FireStart	();
	virtual void			SwitchState			(u32 S);

	static void	__stdcall GrenadeCallback(CBoneInstance*);

	bool m_hideGrenade;
	CWeaponRPG7Grenade *m_pGrenade;
	Fvector *m_pGrenadePoint;
};
