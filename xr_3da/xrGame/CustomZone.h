#pragma once
#include "gameobject.h"
#include "../feel_touch.h"
#include "actor.h"

struct SZonePPInfo 
{
	f32 duality_h, duality_v, blur, gray, noise, noise_scale;
	u32 r, g, b, a;
};


class CCustomZone :
	public CGameObject,
	public Feel::Touch
{
private:
    typedef	CGameObject	inherited;
public:
	CCustomZone(void);
	virtual ~CCustomZone(void);

	virtual BOOL net_Spawn(LPVOID DC);
	virtual void Load(LPCSTR section);
	virtual void net_Destroy();

	virtual void UpdateCL();

	virtual void feel_touch_new(CObject* O);
	virtual void feel_touch_delete(CObject* O);
	virtual BOOL feel_touch_contact(CObject* O);

	virtual void Postprocess(f32 /**val/**/) {}

	virtual void spatial_register();
	virtual void spatial_unregister();
	virtual void spatial_move();

	virtual void OnRender();

	float GetMaxPower() {return m_fMaxPower;}

	//вычисление силы хита в зависимости от расстояния до центра зоны
	virtual float Power(float dist);

	//воздействие зоной на объект
	virtual void Affect(CObject* O);

protected:
	xr_set<CObject*> m_inZone;
	CActor *m_pLocalActor;

	//максимальная сила заряда зоны
	float m_fMaxPower;
	//линейный коэффициент затухания в зависимости от расстояния
	float m_fAttenuation;
	
	float m_fHitImpulseScale;

	u32 m_dwDeltaTime;
	u32 m_dwPeriod;
	BOOL m_bZoneReady;

	////////////////////////////////
	// имена партиклов зоны

	//обычное состояние зоны
	ref_str m_sIdleParticles;
	//выброс зоны
	ref_str m_sBlowoutParticles;


	//появление большого и мальнекого объекта в зоне
	ref_str m_sEntranceParticlesSmall;
	ref_str m_sEntranceParticlesBig;
	//поражение большого и мальнекого объекта в зоне
	ref_str m_sHitParticlesSmall;
	ref_str m_sHitParticlesBig;
	//нахождение большого и мальнекого объекта в зоне
	ref_str m_sIdleObjectParticlesSmall;
	ref_str m_sIdleObjectParticlesBig;

	ref_sound m_idle_sound;
	ref_sound m_blowout_sound;
	ref_sound m_hit_sound;
	ref_sound m_entrance_sound;

	//объект партиклов обычного состояния зоны
	CParticlesObject* m_pIdleParticles;

	//список партиклов для объетов внутри зоны (каждому объету соответствует вектор указателей партиклов)
	DEFINE_MAP (CObject*, PARTICLES_PTR_VECTOR, ATTACHED_PARTICLES_MAP, ATTACHED_PARTICLES_MAP_IT);
	ATTACHED_PARTICLES_MAP m_IdleParticlesMap;

	virtual	void Hit (float P, Fvector &dir,	
					  CObject* who, s16 element,
					  Fvector position_in_object_space, 
					  float impulse, 
					  ALife::EHitType hit_type = eHitTypeWound);


	//для визуализации зоны
	virtual void PlayIdleParticles();
	virtual void StopIdleParticles();

	virtual void PlayBlowoutParticles();
	
	virtual void PlayEntranceParticles(CGameObject* pObject);
	virtual void PlayBulletParticles(Fvector& pos );

	virtual void PlayHitParticles(CGameObject* pObject);

	virtual void PlayObjectIdleParticles(CGameObject* pObject);
	virtual void StopObjectIdleParticles(CGameObject* pObject);
};