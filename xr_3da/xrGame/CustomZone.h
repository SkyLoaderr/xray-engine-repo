#pragma once
#include "gameobject.h"
#include "../feel_touch.h"

class CActor;

struct SZonePPInfo 
{
	float duality_h, duality_v, blur, gray, noise, noise_scale;
	u32 r, g, b, a;
};

//���������� � �������, ����������� � ����
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

	//������������� ������� � ����
	bool	zone_ignore;
	//�������������� ��������
	PARTICLES_PTR_VECTOR particles_vector;
	//����� ���������� � ����
	u32		time_in_zone;
	//���������� ���, ������� ���� �������������� �� ������
	u32		hit_num;
	//���������� ����������� ���������� �����
	float	total_damage;
	
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
	virtual void shedule_Update		(u32 dt);

	virtual void feel_touch_new(CObject* O);
	virtual void feel_touch_delete(CObject* O);
	virtual BOOL feel_touch_contact(CObject* O);

	virtual void Postprocess(float /**val/**/) {}

	/*virtual void spatial_move();
	virtual void spatial_register();
	virtual void spatial_unregister();*/

	virtual void  Center (Fvector& C);
	virtual float Radius ();

#ifdef DEBUG
	virtual void OnRender();
#endif

	float GetMaxPower() {return m_fMaxPower;}

	//���������� ���� ���� � ����������� �� ���������� �� ������ ����
	virtual float Power(float dist);

	//����������� ����� �� ������
	virtual void Affect(CObject* O);

protected:
	//������ ��������, ����������� � ����
	xr_set<CObject*> m_inZone;
	CActor *m_pLocalActor;

	//������������ ���� ������ ����
	float m_fMaxPower;

	//�������� ����������� ��������� � ����������� �� ����������
	float m_fAttenuation;
	
	float m_fHitImpulseScale;

	u32 m_dwDeltaTime;
	u32 m_dwPeriod;
	bool m_bZoneReady;
	//���� � ���� ���� �� disabled �������
	bool m_bZoneActive;
	
	//������������� ����������� ���� �� ���� ��������
	bool m_bIgnoreNonAlive;
	bool m_bIgnoreSmall;

	//�����, ����� �������, ���� ��������� ����������� 
	//�� ������ ������� ������ (-1 ���� �� �������)
	int m_iDisableHitTime;
	int m_iDisableIdleTime;

	////////////////////////////////
	// ����� ��������� ����

	//������� ��������� ����
	ref_str m_sIdleParticles;
	//������ ����
	ref_str m_sBlowoutParticles;


	//��������� �������� � ���������� ������� � ����
	ref_str m_sEntranceParticlesSmall;
	ref_str m_sEntranceParticlesBig;
	//��������� �������� � ���������� ������� � ����
	ref_str m_sHitParticlesSmall;
	ref_str m_sHitParticlesBig;
	//���������� �������� � ���������� ������� � ����
	ref_str m_sIdleObjectParticlesSmall;
	ref_str m_sIdleObjectParticlesBig;

	ref_sound m_idle_sound;
	ref_sound m_blowout_sound;
	ref_sound m_hit_sound;
	ref_sound m_entrance_sound;

	//������ ��������� �������� ��������� ����
	CParticlesObject* m_pIdleParticles;

	//������ ��������� ��� ������� ������ ����
	DEFINE_MAP (CObject*, SZoneObjectInfo, OBJECT_INFO_MAP, OBJECT_INFO_MAP_IT);
	OBJECT_INFO_MAP m_ObjectInfoMap;


	virtual	void Hit (float P, Fvector &dir,	
					  CObject* who, s16 element,
					  Fvector position_in_object_space, 
					  float impulse, 
					  ALife::EHitType hit_type = eHitTypeWound);


	//��� ������������ ����
	virtual void PlayIdleParticles();
	virtual void StopIdleParticles();

	virtual void PlayBlowoutParticles();
	
	virtual void PlayEntranceParticles(CGameObject* pObject);
	virtual void PlayBulletParticles(Fvector& pos );

	virtual void PlayHitParticles(CGameObject* pObject);

	virtual void PlayObjectIdleParticles(CGameObject* pObject);
	virtual void StopObjectIdleParticles(CGameObject* pObject);
};