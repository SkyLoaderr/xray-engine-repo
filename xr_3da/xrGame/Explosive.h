// Explosive.h: интерфейс для взврывающихся объектов
//
//////////////////////////////////////////////////////////////////////

#pragma once

#define SND_RIC_COUNT 5

#include "../feel_touch.h"
#include "inventory_item.h"
#include "ai_sounds.h"

class CExplosive:
	virtual public CInventoryItem,
    public Feel::Touch
{
private:
	typedef CInventoryItem inherited;
public:
	CExplosive(void);
	virtual ~CExplosive(void);

	virtual void Load(LPCSTR section);
	
	virtual BOOL net_Spawn			(LPVOID DC);
	virtual void net_Destroy		();

	virtual void UpdateCL();

	virtual void feel_touch_new(CObject* O);

	virtual void Explode();
	virtual void ExplodeParams(const Fvector& pos, const Fvector& dir);

	static float ExplosionEffect(CGameObject* pExpObject,  const Fvector &expl_centre, const float expl_radius, xr_list<s16> &elements, xr_list<Fvector> &bs_positions);

	virtual void OnH_B_Chield		() {inherited::OnH_B_Chield();}
	virtual void OnH_B_Independent	() {inherited::OnH_B_Independent();}
	virtual void OnEvent (NET_Packet& P, u16 type) ;//{inherited::OnEvent( P, type);}
	virtual	void Hit	(float P, Fvector &dir,	CObject* who, s16 element,
						 Fvector position_in_object_space, float impulse, 
						 ALife::EHitType hit_type = ALife::eHitTypeWound)	{inherited::Hit(P, dir, who, element, position_in_object_space,impulse,hit_type);}
	
	virtual void renderable_Render	();

	virtual void SetCurrentParentID	(u16 parent_id) {m_iCurrentParentID = parent_id;}
	virtual void GenExplodeEvent (const Fvector& pos, const Fvector& normal);
	virtual void FindNormal(Fvector& normal);

protected:
	//ID персонажа который иницировал действие
	u16 m_iCurrentParentID;
	
	bool	m_bReadyToExplode;
	Fvector m_vExplodePos;
	Fvector m_vExplodeDir;

	//параметры взрыва
	float m_fBlastHit;
	float m_fBlastHitImpulse;
	float m_fBlastRadius;
	
	//параметры и количество осколков
	float m_fFragsRadius; 
	float m_fFragHit;
	float m_fFragHitImpulse;
	int	  m_iFragsNum;

	//типы наносимых хитов
	ALife::EHitType m_eHitTypeBlast;
	ALife::EHitType m_eHitTypeFrag;

	//фактор подпроса предмета вверх взрывной волной 
	float m_fUpThrowFactor;

	//список пораженных объектов
	xr_list<CGameObject*> m_blasted;

	//текущая продолжительность взрыва
	float m_fExplodeDuration;
	//общее время взрыва
	float m_fExplodeDurationMax;
	//флаг состояния взрыва
	bool  m_bExploding;

	//////////////////////////////////////////////
	//для разлета осколков
	float					tracerHeadSpeed;
	float					tracerMaxLength;

	//звуки
	ref_sound	sndExplode;
	ESoundTypes m_eSoundExplode;

	//размер отметки на стенах
	float		fWallmarkSize;
	
	//эффекты и подсветка
	ref_str		m_sExplodeParticles;
	
	//подсветка взрыва
	IRender_Light*		m_pLight;
	Fcolor				m_LightColor;
	float				m_fLightRange;
	float				m_fLightTime;
	
	virtual	void		StartLight	();
	virtual	void		StopLight	();

	// эффектор
	struct {
		float time;
		float amplitude;	
		float period_number;
	} effector;
};
