// Explosive.h: интерфейс для взврывающихся объектов
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define SND_RIC_COUNT 5

#include "missile.h"
#include "../feel_touch.h"

class CExplosive
{
public:
	CExplosive(void);
	virtual ~CExplosive(void);

	virtual void Load(LPCSTR section);
	
	virtual BOOL net_Spawn(LPVOID DC);
	virtual void net_Destroy();

	virtual void UpdateCL();

	virtual void feel_touch_new(CObject* O);

	virtual void Explode();
	virtual void FragWallmark(const Fvector& vDir, 
							  const Fvector &vEnd, 
							  Collide::rq_result& R);

protected:
	//параметры взрыва
	float m_blast, m_blastR, m_fragsR, m_fragHit;
	//количество осколков
	int m_frags;
	//список пораженных объектов
	xr_list<CGameObject*> m_blasted;

	//продолжительность взрыва
	u32	m_expoldeTime;

	//визуальные эффекты
	ref_str		pstrWallmark;
	ref_shader	hWallmark;
	float		fWallmarkSize;
	
	xr_vector<ref_str>	m_effects;
	IRender_Light*		m_pLight;
	Fcolor m_lightColor;
	f32 m_lightRange;
	u32 m_lightTime;

	//звуки
	ref_sound	sndRicochet[SND_RIC_COUNT], sndExplode, sndCheckout;
	ESoundTypes m_eSoundRicochet, m_eSoundExplode, m_eSoundCheckout;
};
