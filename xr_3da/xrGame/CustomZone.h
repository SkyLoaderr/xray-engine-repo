#pragma once

#include "space_restrictor.h"
#include "../feel_touch.h"
#include "zone_effector.h"
#include "PhysicsShellHolder.h"

class CActor;
class CLAItem;
class CArtefact;

#define SMALL_OBJECT_RADIUS 0.6f

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
	public CSpaceRestrictor,
	public Feel::Touch
{
private:
    typedef	CSpaceRestrictor inherited;

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

	virtual void feel_touch_new		(CObject* O);
	virtual void feel_touch_delete	(CObject* O);
	virtual BOOL feel_touch_contact	(CObject* O);

	virtual void Postprocess		(float val) {}

	virtual void OnEvent			(NET_Packet& P, u16 type);

	float GetMaxPower() {return m_fMaxPower;}

	//вычисление силы хита в зависимости от расстояния до центра зоны
	//относительный размер силы (от 0 до 1)
	virtual float RelativePower(float dist);
	//абсолютный размер
	virtual float Power(float dist);

protected:
	//список объетков, находящихся в зоне
	xr_set<CObject*> m_inZone;
	CActor *m_pLocalActor;

	//максимальная сила заряда зоны
	float m_fMaxPower;

	//линейный коэффициент затухания в зависимости от расстояния
	float m_fAttenuation;
	
	float m_fHitImpulseScale;

	//тип наносимого хита
	ALife::EHitType m_eHitTypeBlowout;

	
	//различные состояния в которых может находиться зона
	typedef enum {
		eZoneStateIdle = 0,		//состояние зоны, когда внутри нее нет активных объектов
		eZoneStateAwaking,		//пробуждение зоны (объект попал в зону)
		eZoneStateBlowout,		//выброс
        eZoneStateAccumulate,	//накапливание энергии, после выброса
		eZoneStateDisabled,
		eZoneStateMax
	} EZoneState;

	EZoneState m_eZoneState;


	//текущее время пребывания зоны в определенном состоянии 
	int m_iStateTime;
	int m_iPreviousStateTime;
	
	//массив с временами, сколько каждое состояние должно 
	//длиться (если 0, то мгновенно -1 - бесконечность, 
	//-2 - вообще не должно вызываться)
	typedef	svector<int, eZoneStateMax> StateTimeSVec;
	StateTimeSVec m_StateTime;

	virtual void SwitchZoneState(EZoneState new_state);
	virtual void OnStateSwitch	(EZoneState new_state);

	//обработка зоны в различных состояниях
	virtual bool IdleState();
	virtual bool AwakingState();
	virtual bool BlowoutState();
	virtual bool AccumulateState();

	virtual bool Enable();
	virtual bool Disable();
public:
	virtual bool IsEnabled ()	{return m_eZoneState != eZoneStateDisabled; };
	virtual void ZoneEnable();	
	virtual void ZoneDisable();
protected:


	//воздействие зоной на объект
	virtual void Affect(CObject* O)  {}

	//воздействовать на все объекты в зоне
	virtual void AffectObjects();

	u32	m_dwAffectFrameNum;	

		
	u32 m_dwDeltaTime;
	u32 m_dwPeriod;
	bool m_bZoneReady;
	//если в зоне есть не disabled объекты
	bool m_bZoneActive;


	//параметры для выброса, с какой задержкой 
	//включать эффекты и логику
	u32 m_dwBlowoutParticlesTime;
	u32 m_dwBlowoutLightTime;
	u32 m_dwBlowoutSoundTime;
	u32 m_dwBlowoutExplosionTime;
	void UpdateBlowout();
	
	//ветер
	bool m_bBlowoutWindEnable;
	bool m_bBlowoutWindActive;
	u32 m_dwBlowoutWindTimeStart;
	u32 m_dwBlowoutWindTimePeak;
	u32 m_dwBlowoutWindTimeEnd;
	//сила ветра (увеличение текущего) (0,1) когда в аномалию попадает актер
	float m_fBlowoutWindPowerMax;
	float m_fBlowoutWindPowerCur;
	float m_fRealWindPower;
	void StartWind();
	void StopWind();
	void UpdateWind();


	//игнорирование воздействия зоны на виды объектов
	bool m_bIgnoreNonAlive;
	bool m_bIgnoreSmall;
	bool m_bIgnoreArtefacts;

	//время, через которое, зона перестает реагировать 
	//на объект мертвый объект (-1 если не указано)
	int m_iDisableHitTime;
	//тоже самое но для маленьких объектов
	int m_iDisableHitTimeSmall;
	int m_iDisableIdleTime;

	////////////////////////////////
	// имена партиклов зоны

	//обычное состояние зоны
	shared_str m_sIdleParticles;
	//выброс зоны
	shared_str m_sBlowoutParticles;


	//появление большого и мальнекого объекта в зоне
	shared_str m_sEntranceParticlesSmall;
	shared_str m_sEntranceParticlesBig;
	//поражение большого и мальнекого объекта в зоне
	shared_str m_sHitParticlesSmall;
	shared_str m_sHitParticlesBig;
	//нахождение большого и мальнекого объекта в зоне
	shared_str m_sIdleObjectParticlesSmall;
	shared_str m_sIdleObjectParticlesBig;

	ref_sound m_idle_sound;
	ref_sound m_blowout_sound;
	ref_sound m_hit_sound;
	ref_sound m_entrance_sound;

	//объект партиклов обычного состояния зоны
	CParticlesObject* m_pIdleParticles;

	//////////////////////////////
	//подсветка аномалии

	//подсветка idle состояния
	bool				m_bIdleLight;
	IRender_Light*		m_pIdleLight;
	Fcolor				m_IdleLightColor;
	float				m_fIdleLightRange;
	float				m_fIdleLightHeight;
	float				m_fIdleLightRangeDelta;
	CLAItem*			m_pIdleLAnim;

	virtual	void		StartIdleLight	();
	virtual	void		StopIdleLight	();
	virtual	void		UpdateIdleLight	();


	//подсветка выброса
	bool				m_bBlowoutLight;
	IRender_Light*		m_pLight;
	float				m_fLightRange;
	Fcolor				m_LightColor;
	float				m_fLightTime;
	float				m_fLightTimeLeft;
	float				m_fLightHeight;



	virtual	void		StartBlowoutLight	();
	virtual	void		StopBlowoutLight	();
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

	virtual bool IsVisibleForZones() { return false;}

	//обновление, если зона передвигается
	virtual void OnMove	();
	Fvector m_vPrevPos;
	u32		m_dwLastTimeMoved;

	//видимость зоны детектором
public:
	virtual bool VisibleByDetector	() {return m_bVisibleByDetector;}
protected:
	bool	m_bVisibleByDetector;

	//////////////////////////////////////////////////////////////////////////
	// список артефактов
protected:
	virtual void SpawnArtefact			();
	virtual void AddArtefact			(CArtefact* pArtefact);

	//рождение артефакта в зоне, во время ее срабатывания
	//и присоединение его к зоне
	virtual void BornArtefact			();
	//выброс артефактов из зоны
	virtual void ThrowOutArtefact		(CArtefact* pArtefact);
	
	virtual void PrefetchArtefacts		();

protected:
	DEFINE_VECTOR(CArtefact*, ARTEFACT_VECTOR, ARTEFACT_VECTOR_IT);
	ARTEFACT_VECTOR m_SpawnedArtefacts;

	//есть ли вообще функция выбрасывания артефактов во время срабатывания
	bool m_bSpawnBlowoutArtefacts;
	//вероятность того, что артефакт засповниться при единичном 
	//срабатывании аномалии
	float m_fArtefactSpawnProbability;
	//величина импульса выкидывания артефакта из зоны
	float m_fThrowOutPower;
	//высота над центром зоны, где будет появляться артефакт
	float m_fArtefactSpawnHeight;

	//имя партиклов, которые проигрываются во время и на месте рождения артефакта
	shared_str m_sArtefactSpawnParticles;
	//звук рождения артефакта
	ref_sound m_ArtefactBornSound;

	struct ARTEFACT_SPAWN
	{
		shared_str section;
		float probability;
	};

	DEFINE_VECTOR(ARTEFACT_SPAWN, ARTEFACT_SPAWN_VECTOR, ARTEFACT_SPAWN_IT);
	ARTEFACT_SPAWN_VECTOR m_ArtefactSpawn;

	//расстояние от зоны до текущего актера
	float m_fDistanceToCurEntity;
};