// Level_Bullet_Manager.h:  ��� ����������� ������ ���� �� ����������
//							��� ���� � ������� ���������� ����
//////////////////////////////////////////////////////////////////////

#pragma once


#include "weaponammo.h"
#include "tracer.h"

//���������, ����������� ���� � �� �������� � ������
struct SBullet
{
	enum EBulletFlags {
		RICOCHET_FLAG				= u16(1 << 0),	//���� �����������
		PARTICLES_FLAG				= u16(1 << 1),
		LIFE_TIME_FLAG				= u16(1 << 2),
		TRACER_FLAG					= u16(1 << 3),
		RICOCHET_ENABLED_FLAG		= u16(1 << 4),	//��������� �������
		EXPLOSIVE_FLAG				= u16(1 << 5),
	};

	//����� ����� �� ������� ���� �������� ����
	u32				frame_num;
	Flags16			flags;
	//������� �������
	Fvector			pos;
	//�������� ������� ������ ������������ ������� �������
	//(� ��������� �� ������� �����)
	//�������� �������� �� �������������
	//���� ������ ������ ���� �� ���������� � ����� �����
	float			render_offset;
	//������� ��������
	Fvector			dir;
	float			speed;
	//ID ��������� ������� ���������� ��������
	u16				parent_id;
	bool			m_bSendHit;
	//ID ������ �� �������� ���� �������� ����
	u16				weapon_id;
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
	//������������ ����� �������� ��� ������ ����
	float			tracer_max_length;
	u16				bullet_material_idx;
	//��� ���������� ����
	ALife::EHitType hit_type;
	//---------------------------------
	u32				m_dwID;
	bool			operator	==		(u32 ID){return	ID == m_dwID;}
public:
					SBullet				();
					~SBullet			();

	void			Init				(const Fvector& position,
										const Fvector& direction,
										float start_speed,
										float power,
										float impulse,
										u16	sender_id,
										u16 sendersweapon_id,
										ALife::EHitType e_hit_type,
										float maximum_distance,
										const CCartridge& cartridge,
										bool SendHit, 
										float tracer_length);
};



class CLevel;



class CBulletManager
{
	friend CLevel;
protected:
	DEFINE_VECTOR(ref_sound,SoundVec,SoundVecIt);
	SoundVec				m_WhineSounds;
	RStringVec				m_ExplodeParticles;
	DEFINE_VECTOR(SBullet,BulletVec,BulletVecIt);

	//������ ���� ����������� � ������ ������ �� ������
	xrCriticalSection		m_Lock		;
	BulletVec				m_Bullets	;

	//������� �������, ������� �� ��� ����� �� ���������� �����
	u32						m_dwTimeRemainder;

	//��������� ��������� �� ����
	CTracer					tracers;

	//������������� ����� ���� �������� ����
	u32						m_dwStepTime;	
	//����������� ��������, �� ������� ���� ��� ���������
	static float			m_fMinBulletSpeed;

	//��������� G
	float					m_fGravityConst;
	//������������� �������, �������, ������� ���������� �� ��������
	//������ ����
	float					m_fAirResistanceK;
	//c������ ��������� ������� �������� ���� ��� ������������ � ���������� (��� ������� ��� ������ �����)
	float					m_fCollisionEnergyMin;
	//�������� ��������� ������� ��������� � ���� ��� ����� ������������
	float					m_fCollisionEnergyMax;

	//��������� ��������� ���������
	float					m_fTracerWidth;
	float 					m_fTracerLength;
	float 					m_fTracerLengthMin;
	float 					m_fLengthToWidthRatio;
	//������� � ������� ���������� ������ �������� (��� ��� ����� �����)
	float 					m_fMinViewDist;
	float 					m_fMaxViewDist;
protected:
	void					PlayWhineSound		(CObject* object, const Fvector& pos);
	void					PlayExplodePS		(const Fmatrix& xf);
	//������� ��������� ����� ��������
	static BOOL __stdcall	test_callback		(CObject* object, LPVOID params);
	static BOOL __stdcall	firetrace_callback	(collide::rq_result& result, LPVOID params);
	//��������� �� ������������� �������
	void					DynamicObjectHit	(SBullet* bullet, const Fvector& end_point, 
												collide::rq_result& R, 	u16 target_material);
	//��������� �� ������������ �������
	void					StaticObjectHit		(SBullet* bullet, const Fvector& end_point, 
												collide::rq_result& R, u16 target_material);
	//��������� �� ������ �������, �� ������ - ������� � ���� ���������� ����� �������
	std::pair<float, float> ObjectHit			(SBullet* bullet, const Fvector& end_point, 
												collide::rq_result& R, u16 target_material, const Fvector& hit_normal);
	//������� �� ���������� �������
	void					FireShotmark		(const SBullet* bullet, const Fvector& vDir, 
												const Fvector &vEnd, collide::rq_result& R,  u16 target_material,
												Fvector& vNormal, bool ShowMark = true);
	//������� ������ ���� �� ��������� ���������� �������
	//����������� ��� �� ���� ������� ���� �������� ������������
	//� ����������, � ����� �������� ����� ���������� �������
	//�������� � ��������� � ������ ���������� � �����
	//���������� true ���� ���� ���������� �����
	bool					CalcBullet			(SBullet* bullet, u32 delta_time);
public:
							CBulletManager		();
	virtual					~CBulletManager		();

	void 					Load				();
	void 					Clear				();
	void 					AddBullet			(const Fvector& position, const Fvector& direction, 
												float starting_speed, float power, float impulse, 
												u16	sender_id, u16 sendersweapon_id,
												ALife::EHitType e_hit_type, float maximum_distance, 
												const CCartridge& cartridge, bool SendHit, 
												float tracer_length = flt_max);
	void 					Update				();
	void 					Render				();
};