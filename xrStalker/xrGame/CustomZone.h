#pragma once
#include "gameobject.h"
#include "../feel_touch.h"
#include "zone_effector.h"

class CActor;

struct SZonePPInfo 
{
	float duality_h, duality_v, blur, gray, noise, noise_scale;
	u32 r, g, b, a;
};

//информация о объекте, находящемся в зоне
struct SZoneObjectInfo
{
	SZoneObjectInfo() 
	{
		zone_ignore = false;
		time_in_zone = 0;
		hit_num = 0;
		total_damage = 0;
		small_object = false;
		nonalive_object = false;
	};
	

	bool	small_object;
	bool	nonalive_object;

	//игнорирование объекта в зоне
	bool	zone_ignore;
	//присоединенные партиклы
	PARTICLES_PTR_VECTOR particles_vector;
	//время прибывания в зоне
	u32		time_in_zone;
	//количество раз, сколько зона воздействовала на объект
	u32		hit_num;
	//количество повреждений нанесенных зоной
	float	total_damage;
	
};


class CCustomZone :
	public CGameObject,
	public Feel::Touch
{
private:
    typedef	CGameObject	inherited;

public:

	CZoneEffector	m_effector;

public:

	CCustomZone(void);
	virtual ~CCustomZone(void);

	virtual BOOL net_Spawn(LPVOID DC);
	virtual void Load(LPCSTR section);
	virtual void net_Destroy();

	virtual void UpdateCL();
	virtual void shedule_Update		(u32 dt);

	virtual void feel_touch_new(CObject* O);
	virtual void feel_touch_delete(CObject* O);
	virtual BOOL feel_touch_contact(CObject* O);

	virtual void Postprocess(float val) {}

	virtual void  Center (Fvector& C)	const;
	virtual float Radius () const;

#ifdef DEBUG
	virtual void OnRender();
#endif

	float GetMaxPower() {return m_fMaxPower;}

	//вычисление силы хита в зависимости от расстояния до центра зоны
	virtual float Power(float dist);

	//воздействие зоной на объект
	virtual void Affect(CObject* O);

protected:
	//список объетков, находящихся в зоне
	xr_set<CObject*> m_inZone;
	CActor *m_pLocalActor;

	//максимальная сила заряда зоны
	float m_fMaxPower;

	//линейный коэффициент затухания в зависимости от расстояния
	float m_fAttenuation;
	
	float m_fHitImpulseScale;

	u32 m_dwDeltaTime;
	u32 m_dwPeriod;
	bool m_bZoneReady;
	//если в зоне есть не disabled объекты
	bool m_bZoneActive;
	
	//игнорирование воздействия зоны на виды объектов
	bool m_bIgnoreNonAlive;
	bool m_bIgnoreSmall;

	//время, через которое, зона перестает реагировать 
	//на объект мертвый объект (-1 если не указано)
	int m_iDisableHitTime;
	int m_iDisableIdleTime;

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

	//////////////////////////////
	//подсветка аномалии

	//подсветка выброса
	bool				m_bBlowoutLight;
	IRender_Light*		m_pLight;
	Fcolor				m_LightColor;
	float				m_fLightRange;
	u32					m_dwLightTime;
	u32					m_dwLightTimeLeft;

	virtual	void		StartBlowoutLight	();
	virtual	void		UpdateBlowoutLight	();

	//список партиклов для объетов внутри зоны
	DEFINE_MAP (CObject*, SZoneObjectInfo, OBJECT_INFO_MAP, OBJECT_INFO_MAP_IT);
	OBJECT_INFO_MAP m_ObjectInfoMap;


	virtual	void Hit (float P, Fvector &dir,	
					  CObject* who, s16 element,
					  Fvector position_in_object_space, 
					  float impulse, 
					  ALife::EHitType hit_type = ALife::eHitTypeWound);


	//для визуализации зоны
	virtual void PlayIdleParticles();
	virtual void StopIdleParticles();

	virtual void PlayBlowoutParticles();
	
	virtual void PlayEntranceParticles(CGameObject* pObject);
	virtual void PlayBulletParticles(Fvector& pos );

	virtual void PlayHitParticles(CGameObject* pObject);

	virtual void PlayObjectIdleParticles(CGameObject* pObject);
	virtual void StopObjectIdleParticles(CGameObject* pObject);

	virtual bool EnableEffector() {return false;}

};