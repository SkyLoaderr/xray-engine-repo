#pragma once
#include "missile.h"
#include "../feel_touch.h"

#define SND_RIC_COUNT 5

class CGrenade :
	public CMissile,
	public Feel::Touch
{
	typedef CMissile inherited;
public:
	CGrenade(void);
	virtual ~CGrenade(void);

	virtual void Load(LPCSTR section);
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();
	virtual void OnH_A_Chield();
	virtual void OnH_B_Independent();
	virtual void OnEvent(NET_Packet& P, u16 type);
	virtual void OnAnimationEnd();
	virtual void UpdateCL();

	virtual void feel_touch_new(CObject* O);

	virtual bool Activate();
	virtual void Deactivate();
	virtual void Throw();
	virtual void Destroy();
	virtual void Explode();
	virtual void FragWallmark(const Fvector& vDir, const Fvector &vEnd, Collide::ray_query& R);

	virtual bool Action(s32 cmd, u32 flags);
	virtual bool Useful();
	virtual u32 State(u32 state);

protected:

	CGrenade *m_pFake;

	//��������� ������
	float m_blast, m_blastR, m_fragsR, m_fragHit;
	int m_frags;
	u32	m_expoldeTime;
	xr_list<CGameObject*> m_blasted;

	ref_str		pstrWallmark;
	ref_shader	hWallmark;
	float		fWallmarkSize;

	//�������
	xr_vector<ref_str>	m_effects;
	IRender_Light*		m_pLight;
	Fcolor m_lightColor;
	f32 m_lightRange;
	u32 m_lightTime;

	//�����
	ref_sound	sndRicochet[SND_RIC_COUNT], sndExplode, sndCheckout;
	ESoundTypes m_eSoundRicochet, m_eSoundExplode, m_eSoundCheckout;
};
