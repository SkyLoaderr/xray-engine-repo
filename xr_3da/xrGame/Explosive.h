// Explosive.h: интерфейс для взврывающихся объектов
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define SND_RIC_COUNT 5
#define EXPLODE_TIME_MAX 5000


#include "../feel_touch.h"
#include "inventory.h"
#include "ShootingObject.h"

class CExplosive:
	virtual public CInventoryItem,
	public CShootingObject,
    public Feel::Touch
{
private:
	typedef CInventoryItem inherited;
public:
	CExplosive(void);
	virtual ~CExplosive(void);

	virtual void Load(LPCSTR section);
	
	virtual BOOL net_Spawn(LPVOID DC) = 0;
	virtual void net_Destroy() = 0;

	virtual void UpdateCL();

	virtual void feel_touch_new(CObject* O);

	virtual void Explode();
	virtual void Destroy()	=	0;

	virtual void OnH_B_Chield		() {inherited::OnH_B_Chield();}
	virtual void OnH_B_Independent	() {inherited::OnH_B_Independent();}
	virtual void OnEvent (NET_Packet& P, u16 type) {inherited::OnEvent( P, type);}
	virtual	void Hit	(float P, Fvector &dir,	CObject* who, s16 element,
						 Fvector position_in_object_space, float impulse, 
						 ALife::EHitType hit_type = eHitTypeWound)	{inherited::Hit(P, dir, who, element, position_in_object_space,impulse,hit_type);}
	
	virtual void renderable_Render() = 0;



protected:
	//параметры взрыва
	float m_fBlastHit;
	float m_fBlastRadius;
	
	//параметры и количество осколков
	float m_fFragsRadius; 
	float m_fFragHit;
	int	  m_iFragsNum;

	//фактор подпроса предмета вверх взрывной волной 
	float m_fUpThrowFactor;

	//список пораженных объектов
	xr_list<CGameObject*> m_blasted;

	//продолжительность взрыва
	u32	m_expoldeTime;

	//////////////////////////////////////////////
	//для разлета осколков
	float					tracerHeadSpeed;
	float					tracerTrailCoeff;
	float					tracerStartLength;
	float					tracerWidth;

	//звуки
	ref_sound	sndExplode;
	ESoundTypes m_eSoundExplode;

	//отметки на стенах
	float		fWallmarkSize;
	
	//эффекты и подсветка
	ref_str m_sExplodeParticles;
	
	//подсветка
	IRender_Light*		m_pLight;
	Fcolor m_lightColor;
	f32 m_lightRange;
	u32 m_lightTime;
};
