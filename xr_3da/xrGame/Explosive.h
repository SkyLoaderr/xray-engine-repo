// Explosive.h: интерфейс для взврывающихся объектов
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define SND_RIC_COUNT 5

#include "../feel_touch.h"
#include "inventory.h"

class CExplosive:
	virtual public CInventoryItem,
	public Feel::Touch
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

	virtual void Destroy()	=	0;

	void SoundCreate(ref_sound& dest, LPCSTR name, int iType=0, BOOL bCtrlFreq=FALSE);
	void SoundDestroy(ref_sound& dest);


protected:
	//параметры взрыва
	float m_blast;
	float m_blastR;
	
	//параметры и количество осколков
	float m_fragsR; 
	float m_fragHit;
	int m_frags;

	//фактор подпроса предмета вверх взрывной волной 
	float m_fUpThrowFactor;

	//список пораженных объектов
	xr_list<CGameObject*> m_blasted;

	//продолжительность взрыва
	u32	m_expoldeTime;

	//отметки на стенах
	ref_str		pstrWallmark;
	ref_shader	hWallmark;
	float		fWallmarkSize;
	
	//эффекты и подсветка
	xr_vector<ref_str>	m_effects;
	IRender_Light*		m_pLight;
	Fcolor m_lightColor;
	f32 m_lightRange;
	u32 m_lightTime;

	//звуки
	ref_sound	sndRicochet[SND_RIC_COUNT], sndExplode, sndCheckout;
	ESoundTypes m_eSoundRicochet, m_eSoundExplode, m_eSoundCheckout;
};
