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

//���������� � �������, ����������� � ����
struct SZoneObjectInfo
{
	SZoneObjectInfo():object(NULL),zone_ignore(false),time_in_zone(0),hit_num(0),total_damage(0),small_object(false),nonalive_object(false) {}
	CGameObject*			object; 
	bool					small_object;
	bool					nonalive_object;
	//������������� ������� � ����
	bool					zone_ignore;
	//�������������� ��������
	PARTICLES_PTR_VECTOR	particles_vector;
	//����� ���������� � ����
	u32						time_in_zone;
	//���������� ���, ������� ���� �������������� �� ������
	u32						hit_num;
	//���������� ����������� ���������� �����
	float					total_damage;

	bool operator == (const CGameObject* O) const {return object==O;}
};


class CCustomZone :
	public CSpaceRestrictor,
	public Feel::Touch
{
private:
    typedef	CSpaceRestrictor inherited;

public:
	CZoneEffector		m_effector;

public:

						CCustomZone						(void);
	virtual				~CCustomZone					();

	virtual		BOOL	net_Spawn						(CSE_Abstract* DC);
	virtual		void	Load							(LPCSTR section);
	virtual		void	net_Destroy						();

	virtual		void	UpdateCL						();
	virtual		void	UpdateWorkload					(u32	dt	);				// related to fast-mode optimizations
	virtual		void	shedule_Update					(u32	dt	);

	virtual		void	feel_touch_new					(CObject* O	);
	virtual		void	feel_touch_delete				(CObject* O	);
	virtual		BOOL	feel_touch_contact				(CObject* O	);
	virtual		float	effective_radius				();
	virtual		float	distance_to_center				(CObject* O	);			
	virtual		void	Postprocess						(float val)					{}

	virtual		void	OnEvent							(NET_Packet& P, u16 type);
	virtual		void	OnOwnershipTake					(u16 id);

				float	GetMaxPower						()							{return m_fMaxPower;}

	//���������� ���� ���� � ����������� �� ���������� �� ������ ����
	//������������� ������ ���� (�� 0 �� 1)
	virtual		float	RelativePower					(float dist);
	//���������� ������
	virtual		float	Power							(float dist);

	virtual CCustomZone	*cast_custom_zone				()							{return this;}

protected:
	//������ ��������, ����������� � ����
	CActor*				m_pLocalActor;

	//������������ ���� ������ ����
	float				m_fMaxPower;

	//�������� ����������� ��������� � ����������� �� ����������
	float				m_fAttenuation;
	//������� ����� ����, ������� ������ �� ���������� �������	
	float				m_fHitImpulseScale;
	//������ ������� � ��������� �� �������������, 
	//��� ��������� ����
	float				m_fEffectiveRadius;

	//��� ���������� ����
	ALife::EHitType		m_eHitTypeBlowout;

	
	//��������� ��������� � ������� ����� ���������� ����
	typedef enum {
		eZoneStateIdle = 0,		//��������� ����, ����� ������ ��� ��� �������� ��������
		eZoneStateAwaking,		//����������� ���� (������ ����� � ����)
		eZoneStateBlowout,		//������
        eZoneStateAccumulate,	//������������ �������, ����� �������
		eZoneStateDisabled,
		eZoneStateMax
	} EZoneState;

	EZoneState			m_eZoneState;


	//������� ����� ���������� ���� � ������������ ��������� 
	int					m_iStateTime;
	int					m_iPreviousStateTime;
	
	//������ � ���������, ������� ������ ��������� ������ 
	//������� (���� 0, �� ��������� -1 - �������������, 
	//-2 - ������ �� ������ ����������)
	typedef	svector<int, eZoneStateMax>					StateTimeSVec;
	StateTimeSVec		m_StateTime;

	virtual		void		SwitchZoneState				(EZoneState new_state);
	virtual		void		OnStateSwitch				(EZoneState new_state);

	//��������� ���� � ��������� ����������
	virtual		bool		IdleState					();
	virtual		bool		AwakingState				();
	virtual		bool		BlowoutState				();
	virtual		bool		AccumulateState				();

	virtual		bool		Enable						();
	virtual		bool		Disable						();
public:
	virtual		bool		IsEnabled					()	{return m_eZoneState != eZoneStateDisabled; };
	virtual		void		ZoneEnable					();	
	virtual		void		ZoneDisable					();
protected:


	//����������� ����� �� ������
	virtual		void		Affect						(SZoneObjectInfo* O)  {}

	//�������������� �� ��� ������� � ����
	virtual		void		AffectObjects				();

	u32						m_dwAffectFrameNum;	

	u32						m_dwDeltaTime;
	u32						m_dwPeriod;
	bool					m_bZoneReady;
	//���� � ���� ���� �� disabled �������
	bool					m_bZoneActive;


	//��������� ��� �������, � ����� ��������� 
	//�������� ������� � ������
	u32						m_dwBlowoutParticlesTime;
	u32						m_dwBlowoutLightTime;
	u32						m_dwBlowoutSoundTime;
	u32						m_dwBlowoutExplosionTime;
	void					UpdateBlowout				();
	
	//�����
	bool					m_bBlowoutWindEnable;
	bool					m_bBlowoutWindActive;
	u32						m_dwBlowoutWindTimeStart;
	u32						m_dwBlowoutWindTimePeak;
	u32						m_dwBlowoutWindTimeEnd;
	//���� ����� (���������� ��������) (0,1) ����� � �������� �������� �����
	float					m_fBlowoutWindPowerMax;
	float					m_fBlowoutWindPowerCur;
	float					m_fRealWindPower;
				
	void					StartWind					();
	void					StopWind					();
	void					UpdateWind					();


	//������������� ����������� ���� �� ���� ��������
	bool					m_bIgnoreNonAlive;
	bool					m_bIgnoreSmall;
	bool					m_bIgnoreArtefacts;

	//�����, ����� �������, ���� ��������� ����������� 
	//�� ������ ������� ������ (-1 ���� �� �������)
	int						m_iDisableHitTime;
	//���� ����� �� ��� ��������� ��������
	int						m_iDisableHitTimeSmall;
	int						m_iDisableIdleTime;

	////////////////////////////////
	// ����� ��������� ����

	//������� ��������� ����
	shared_str				m_sIdleParticles;
	//������ ����
	shared_str				m_sBlowoutParticles;


	//��������� �������� � ���������� ������� � ����
	shared_str				m_sEntranceParticlesSmall;
	shared_str				m_sEntranceParticlesBig;
	//��������� �������� � ���������� ������� � ����
	shared_str				m_sHitParticlesSmall;
	shared_str				m_sHitParticlesBig;
	//���������� �������� � ���������� ������� � ����
	shared_str				m_sIdleObjectParticlesSmall;
	shared_str				m_sIdleObjectParticlesBig;

	ref_sound				m_idle_sound;
	ref_sound				m_blowout_sound;
	ref_sound				m_hit_sound;
	ref_sound				m_entrance_sound;

	//������ ��������� �������� ��������� ����
	CParticlesObject*		m_pIdleParticles;

	//////////////////////////////
	//��������� ��������

	//��������� idle ���������
	bool					m_bIdleLight;
	ref_light				m_pIdleLight;
	Fcolor					m_IdleLightColor;
	float					m_fIdleLightRange;
	float					m_fIdleLightHeight;
	float					m_fIdleLightRangeDelta;
	CLAItem*				m_pIdleLAnim;

	void					StartIdleLight				();
	void					StopIdleLight				();
	void					UpdateIdleLight				();


	//��������� �������
	bool					m_bBlowoutLight;
	ref_light				m_pLight;
	float					m_fLightRange;
	Fcolor					m_LightColor;
	float					m_fLightTime;
	float					m_fLightTimeLeft;
	float					m_fLightHeight;



	void					StartBlowoutLight			();
	void					StopBlowoutLight			();
	void					UpdateBlowoutLight			();

	//������ ��������� ��� ������� ������ ����
//	DEFINE_MAP (CObject*, SZoneObjectInfo, OBJECT_INFO_MAP, OBJECT_INFO_MAP_IT);
	DEFINE_VECTOR(SZoneObjectInfo,OBJECT_INFO_VEC,OBJECT_INFO_VEC_IT);
	OBJECT_INFO_VEC			m_ObjectInfoMap;


	virtual	void Hit (float P, Fvector &dir,	
					  CObject* who, s16 element,
					  Fvector position_in_object_space, 
					  float impulse, 
					  ALife::EHitType hit_type = ALife::eHitTypeWound);


	//��� ������������ ����
	virtual		void		PlayIdleParticles			();
	virtual		void		StopIdleParticles			();

	virtual		void		PlayBlowoutParticles		();
	
	virtual		void		PlayEntranceParticles		(CGameObject* pObject);
	virtual		void		PlayBulletParticles			(Fvector& pos );

	virtual		void		PlayHitParticles			(CGameObject* pObject);

	virtual		void		PlayObjectIdleParticles		(CGameObject* pObject);
	virtual		void		StopObjectIdleParticles		(CGameObject* pObject);

	virtual		bool		EnableEffector				() {return false;}

	virtual		bool		IsVisibleForZones			() { return false;}

	//����������, ���� ���� �������������
	virtual		void		OnMove						();
	Fvector					m_vPrevPos;
	u32						m_dwLastTimeMoved;

	//��������� ���� ����������
public:
	virtual			bool	VisibleByDetector			() {return m_bVisibleByDetector;}
protected:
	bool					m_bVisibleByDetector;

	//////////////////////////////////////////////////////////////////////////
	// ������ ����������
protected:
	virtual			void	SpawnArtefact				();

	//�������� ��������� � ����, �� ����� �� ������������
	//� ������������� ��� � ����
	virtual			void	BornArtefact				();
	//������ ���������� �� ����
	virtual			void	ThrowOutArtefact			(CArtefact* pArtefact);
	
	virtual			void	PrefetchArtefacts			();

protected:
	DEFINE_VECTOR(CArtefact*, ARTEFACT_VECTOR, ARTEFACT_VECTOR_IT);
	ARTEFACT_VECTOR			m_SpawnedArtefacts;

	//���� �� ������ ������� ������������ ���������� �� ����� ������������
	bool					m_bSpawnBlowoutArtefacts;
	//����������� ����, ��� �������� ������������ ��� ��������� 
	//������������ ��������
	float					m_fArtefactSpawnProbability;
	//�������� �������� ����������� ��������� �� ����
	float					 m_fThrowOutPower;
	//������ ��� ������� ����, ��� ����� ���������� ��������
	float					m_fArtefactSpawnHeight;

	//��� ���������, ������� ������������� �� ����� � �� ����� �������� ���������
	shared_str				m_sArtefactSpawnParticles;
	//���� �������� ���������
	ref_sound				m_ArtefactBornSound;

	struct ARTEFACT_SPAWN
	{
		shared_str	section;
		float		probability;
	};

	DEFINE_VECTOR(ARTEFACT_SPAWN, ARTEFACT_SPAWN_VECTOR, ARTEFACT_SPAWN_IT);
	ARTEFACT_SPAWN_VECTOR	m_ArtefactSpawn;

	//���������� �� ���� �� �������� ������
	float					m_fDistanceToCurEntity;

protected:
	u32						m_ef_anomaly_type;
	u32						m_ef_weapon_type;

public:
	virtual u32				ef_anomaly_type				() const;
	virtual u32				ef_weapon_type				() const;

	// optimization FAST/SLOW mode
public:						
	BOOL					o_fastmode;		
	IC void					o_switch_2_fast				()	{
		if (o_fastmode)		return	;
		o_fastmode			= TRUE	;
		processing_activate			();
	}
	IC void					o_switch_2_slow				()	{
		if (!o_fastmode)	return	;
		o_fastmode			= FALSE	;
		processing_deactivate		();
	}
};
