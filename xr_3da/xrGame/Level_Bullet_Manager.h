// Level_Bullet_Manager.h:  ��� ����������� ������ ���� �� ����������
//							��� ���� � ������� ���������� ����
//////////////////////////////////////////////////////////////////////

#pragma once


#include "weaponammo.h"
#include "tracer.h"




//���������, ����������� ���� � �� �������� � ������
struct SBullet
{
	SBullet();
	~SBullet();

	void Init(const Fvector& position,
		const Fvector& direction,
		float start_speed,
		float power,
		float impulse,
		u16	sender_id,
		ALife::EHitType e_hit_type,
		float maximum_distance,
		const CCartridge& cartridge);

	//����� ����� �� ������� ���� �������� ����
	u32				frame_num;
	
	enum EBulletFlags {
		RICOCHET_FLAG				= u16(1 << 0),	//���� �����������
		PARTICLES_FLAG				= u16(1 << 1),
		LIFE_TIME_FLAG				= u16(1 << 2),
		TRACER_FLAG					= u16(1 << 3),
		RICOCHET_ENABLED_FLAG		= u16(1 << 4)	//��������� �������
	};
	
	Flags16			flags;
	//������� �������
	Fvector			pos;
	//������� ��������
	Fvector			dir;
	float			speed;
	//ID ��������� ������� ���������� ��������
	u16				parent_id;
	//��� �������
	//���������� �������
	Fvector			prev_pos;

	//��������� ������� ���� ���������
	float			fly_dist;

	//����������� � ��������� �������
	float			hit_power;
	float			hit_impulse;
	
	float			max_speed;

	float			max_dist;
	float			dist_k;
	float			hit_k;
	float			impulse_k;
	float			pierce_k;
	float			wallmark_size;
	//��� ���������� ����
	ALife::EHitType hit_type;

};



class CLevel;



class CBulletManager
{
	friend CLevel;
public:
	CBulletManager();
	virtual ~CBulletManager();

	void Load       ();

	void Clear		();

	void AddBullet	(SBullet* pBullet);
	void Update		();

	void Render		();

protected:

	//������� ��������� ����� ��������
	static BOOL __stdcall firetrace_callback(Collide::rq_result& result, LPVOID params);
	//��������� �� ������������� �������
	void DynamicObjectHit	(SBullet* bullet, const Fvector& end_point, 
							Collide::rq_result& R, 	u16 target_material);
	//��������� �� ������������ �������
	void StaticObjectHit	(SBullet* bullet, const Fvector& end_point, 
							Collide::rq_result& R, u16 target_material);
	//������� �� ���������� �������
	void FireShotmark		(const SBullet* bullet, const Fvector& vDir, 
							 const Fvector &vEnd, Collide::rq_result& R, 
							 u16 target_material,
							 Fvector& vNormal);

	//������� ������ ���� �� ��������� ���������� �������
	//����������� ��� �� ���� ������� ���� �������� ������������
	//� ����������, � ����� �������� ����� ���������� �������
	//�������� � ��������� � ������ ���������� � �����
	//���������� true ���� ���� ���������� �����
	bool CalcBullet (SBullet* bullet, u32 delta_time);


	DEF_LIST(BULLET_LIST, SBullet*);
	//������ ���� ����������� � ������ ������ �� ������
	BULLET_LIST m_BulletList;

	//������� �������, ������� �� ��� ����� �� ���������� �����
	u32	m_dwTimeRemainder;

	//�������� ��� ���� � �������� (���������������� � CLevel::Load)
	u16 bullet_material_idx;

	//��������� ��������� �� ����
	CTracer tracers;

	//������������� ����� ���� �������� ����
	u32	m_dwStepTime;	
	//����������� ��������, �� ������� ���� ��� ���������
	static float m_fMinBulletSpeed;

	//��������� G
	float m_fGravityConst;
	//������������� �������, �������, ������� ���������� �� ��������
	//������ ����
	float m_fAirResistanceK;

	//��������� ��������� ���������
	float m_fTracerWidth;
	float m_fTracerLength;
	float m_fTracerLengthMin;
	float m_fLengthToWidthRatio;
	//������� � ������� ���������� ������ �������� (��� ��� ����� �����)
	float m_fMinViewDist;
	float m_fMaxViewDist;
};