// Level_Bullet_Manager.h:  ��� ����������� ������ ���� �� ����������
//							��� ���� � ������� ���������� ����
//////////////////////////////////////////////////////////////////////

#pragma once


#include "weaponammo.h"


//���������, ����������� ���� � �� �������� � ������
struct SBullet
{
	SBullet(const Fvector& position,
			const Fvector& direction,
			float start_speed,
			float power,
			float impulse,
			u16	sender_id,
			ALife::EHitType e_hit_type,
			const CCartridge& cartridge);
	~SBullet();


	//����� ����� �� ������� ���� �������� ����
	u32				frame_num;

	//������� �������
	Fvector			pos;
	//������� ��������
	Fvector			dir;
	float			speed;
	
	float			hit_power;
	float			hit_impulse;

	//ID ��������� ������� ���������� ��������
	u16				parent_id;		

	//����������� � ��������� �������
	float			dist_k;
	float			hit_k;
	float			impulse_k;
	float			pierce_k;
	bool			tracer;
	float			wallmark_size;
	//��� ���������� ����
	ALife::EHitType hit_type;

	//���������� �������
	Fvector			prev_pos;
};



class CLevel;



class CBulletManager: public pureRender
{
	friend CLevel;
public:
	CBulletManager();
	virtual ~CBulletManager();

	void Clear		();

	void AddBullet	(SBullet* pBullet);
	void Update		();

	virtual void OnRender ();

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
							 u16 target_material);


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
	//������������� ����� ���� �������� ����
	u32	m_dwStepTime;


	//�������� ��� ���� � �������� (���������������� � CLevel::Load)
	u16 bullet_material_idx;
};