// WeaponFakeGrenade.h: объект для эмулирования подствольной гранаты
//
//////////////////////////////////////////////////////////////////////


#pragma once

#include "gameobject.h"
#include "explosive.h"




class CWeaponFakeGrenade :	
	public CGameObject, public Feel::Touch
{
	typedef CGameObject inherited;
public:
	CWeaponFakeGrenade(void);
	virtual ~CWeaponFakeGrenade(void);

	void SoundCreate(ref_sound& dest, LPCSTR name, int iType, BOOL bCtrlFreq=FALSE);
	void SoundDestroy(ref_sound& dest);

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();

	virtual void OnH_B_Independent();
	virtual void UpdateCL();

	virtual void feel_touch_new(CObject* O);

	void Explode(const Fvector &pos, const Fvector &normal);
	virtual void FragWallmark(const Fvector& vDir, const Fvector &vEnd, Collide::rq_result& R);

	Fvector				m_pos, m_vel;
	CGameObject*		m_pOwner;

	f32					m_blast, m_blastR, m_fragsR, m_fragHit;
	s32					m_frags;
	xr_list<CGameObject*> m_blasted;

	enum EState{
		stInactive,
		stEngine,
		stFlying,
		stExplode,
		stDestroying
	};
	EState				m_state;

	ref_str				pstrWallmark;
	ref_shader			hWallmark;
	float				fWallmarkSize;
	ref_sound			sndRicochet[SND_RIC_COUNT], sndExplode;
	ESoundTypes			m_eSoundRicochet, m_eSoundExplode;
	s32					m_engineTime, m_explodeTime, m_flashTime;
	xr_vector<ref_str>	m_effects;
	IRender_Light*		m_pLight;
	Fcolor				m_lightColor;
	Fcolor				m_curColor;
	f32					m_lightRange;
	u32					m_lightTime;
	f32					m_mass, m_engine_f, m_engine_u;
	
	//высота подлета гранаты при взрыве
	float				m_jump;

	static void __stdcall ObjectContactCallback(bool& do_colide,dContact& c);
};

