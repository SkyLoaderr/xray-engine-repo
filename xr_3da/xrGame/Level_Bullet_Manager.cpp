// Level_Bullet_Manager.cpp:	��� ����������� ������ ���� �� ����������
//								��� ���� � ������� ���������� ����
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Level_Bullet_Manager.h"


#define HIT_POWER_EPSILON 0.05f
#define WALLMARK_SIZE 0.04f



SBullet::SBullet()
{
}

SBullet::~SBullet()
{
}


void SBullet::Init(const Fvector& position,
				   const Fvector& direction,
				   float starting_speed,
				   float power,
				   float impulse,
				   u16	sender_id,
				   ALife::EHitType e_hit_type,
				   float maximum_distance,
				   const CCartridge& cartridge)
{
	flags.zero();

	pos = position;

	speed = max_speed = starting_speed;
	VERIFY(speed>0);

	dir = direction;
	VERIFY(dir.magnitude()>0);
	dir.normalize();

	hit_power		= power;
	hit_impulse		= impulse;
	fly_dist		= 0;

	parent_id		= sender_id;
	hit_type		= e_hit_type;

	max_dist		= maximum_distance;
	dist_k			= cartridge.m_kDist;
	hit_k			= cartridge.m_kHit;
	impulse_k		= cartridge.m_kImpulse;
	pierce_k		= cartridge.m_kPierce;
	wallmark_size	= cartridge.fWallmarkSize;

	flags.set(TRACER_FLAG, cartridge.m_tracer);
	flags.set(RICOCHET_ENABLED_FLAG);
}


//////////////////////////////////////////////////////////
//

CBulletManager::CBulletManager()
{
	m_dwStepTime = STEP_TIME;
//	Device.seqRender.Add		(this,REG_PRIORITY_LOW-1000);
}

CBulletManager::~CBulletManager()
{
//	Device.seqRender.Remove		(this);
}


void CBulletManager::Clear		()
{
	for(BULLET_LIST_it it = m_BulletList.begin();
		it != m_BulletList.end() ;it++)
	{
		xr_delete(*it);
	}
	m_BulletList.clear();
}

void CBulletManager::AddBullet(SBullet* bullet)
{
	VERIFY(bullet);
	bullet->frame_num = Device.dwFrame;
	m_BulletList.push_back(bullet);
}

void CBulletManager::Update()
{
	u32 delta_time = Device.dwTimeDelta + m_dwTimeRemainder;
	u32 step_num = delta_time/m_dwStepTime;
	m_dwTimeRemainder = delta_time%m_dwStepTime;
	

	for(BULLET_LIST_it it = m_BulletList.begin();
						it != m_BulletList.end();)
	{
		BULLET_LIST_it cur_it = it;
		SBullet* bullet = *cur_it;
		
		//��� ���� �������� �� ���� �� ����� ������� ������ 1 ���
		//(���� �� ������ ������ ������ ������� �� ����)
		//������� ��������� �� ��������� �����, 
		//��� �������� ��� ���� ���� ��� ������� FPS �� ���������
		//� 2� ������
		u32 cur_step_num = step_num;

		u32 frames_pass = Device.dwFrame - bullet->frame_num;
		if(frames_pass == 0)
			cur_step_num = 1;
		else if (frames_pass == 1 && step_num>0)
			cur_step_num -= 1;

		for(u32 i=0; i<cur_step_num; i++)
		{
			if(!CalcBullet(bullet, m_dwStepTime))
			{
				xr_delete(bullet);
				it++;
				m_BulletList.erase(cur_it);
				break;
			}
		}
		if(!bullet) continue;
		it++;
	}
}

#define GRAVITY_CONST 9.81f
//������������� �������, �������, ������� ���������� �� ��������
//������ ����
#define AIR_RESISTANCE_K 0.3f

bool CBulletManager::CalcBullet (SBullet* bullet, u32 delta_time)
{
	VERIFY(bullet);

	float delta_time_sec = float(delta_time)/1000.f;
	float range = bullet->speed*delta_time_sec;
	
	float max_range = bullet->max_dist - bullet->fly_dist;
	if(range>max_range) 
		range = max_range;

	//��������� ������� �������� ����, �.�. �
	//RayQuery() ��� ����� ���������� ��-�� ���������
	//� ������������ � ���������
	Fvector cur_dir = bullet->dir;
	
	bullet->flags.set(SBullet::RICOCHET_FLAG,0);
	Collide::ray_defs RD(bullet->pos, bullet->dir, range, 0 ,/*Collide::rqtAll*/Collide::rqtBoth);
	BOOL result = FALSE;
	result = Level().ObjectSpace.RayQuery( RD, firetrace_callback, bullet);

	if(!bullet->flags.test(SBullet::RICOCHET_FLAG))
	{
		//�������� ��������� ����
		bullet->prev_pos = bullet->pos;
		bullet->pos.mad(bullet->pos, cur_dir, range);
		bullet->fly_dist += range;

		if(bullet->fly_dist>=bullet->max_dist)
			return false;

		Fbox level_box = Level().ObjectSpace.GetBoundingVolume();
		if(!level_box.contains(bullet->pos))
			return false;

		//�������� �������� � ����������� �� ������
		//� ������ ����������
		bullet->dir.mul(bullet->speed);

		Fvector air_resistance = bullet->dir;
		air_resistance.mul(-AIR_RESISTANCE_K*delta_time_sec);

		bullet->dir.add(air_resistance);
		bullet->dir.y -= GRAVITY_CONST*delta_time_sec;

		bullet->speed = bullet->dir.magnitude();
		//������ normalize(),	 ���� �� ������� 2 ���� magnitude()
#pragma todo("� ��� ������ bullet->speed==0")
		bullet->dir.x /= bullet->speed;
		bullet->dir.y /= bullet->speed;
		bullet->dir.z /= bullet->speed;
	}

	if(bullet->speed<SPEED_LOWER_BOUND)
		return false;

	return true;
}

#define TRACER_WIDTH 0.07f
#define TRACER_LENGHT 25.f
#define TRACER_LENGHT_MIN 0.1f
#define TRACER_LENGTH_TO_WIDTH_RATIO 10.f

void CBulletManager::Render	()
{
	if(m_BulletList.empty()) return;

	u32	vOffset;
	u32 bullet_num = m_BulletList.size();

	

	FVF::V	*verts		=	(FVF::V	*) RCache.Vertex.Lock((u32)bullet_num*8,
										tracers.sh_Geom->vb_stride,
										vOffset);
	FVF::V	*start		=	verts;



	for(BULLET_LIST_it it = m_BulletList.begin();
		it != m_BulletList.end(); it++)
	{
		SBullet* bullet = *it;

		Fvector dist;
		dist.sub(bullet->prev_pos,bullet->pos);
		float length = dist.magnitude();
	
		if(length<TRACER_LENGHT_MIN)
			continue;

		if(length>TRACER_LENGHT)
			length = TRACER_LENGHT;

		float width;
		if(length>(TRACER_WIDTH*TRACER_LENGTH_TO_WIDTH_RATIO))
			width = TRACER_WIDTH;
		else 
			width = length/TRACER_LENGTH_TO_WIDTH_RATIO;

		dist.normalize();

		Fvector center;
		center.mad(bullet->pos, dist,  -length/2.f);
		tracers.Render(verts, center, dist, length, width);
	}

	u32 vCount					= (u32)(verts-start);
	RCache.Vertex.Unlock		(vCount,tracers.sh_Geom->vb_stride);

	if (vCount)
	{
		RCache.set_CullMode			(CULL_NONE);
		RCache.set_xform_world		(Fidentity);
		RCache.set_Shader			(tracers.sh_Tracer);
		RCache.set_Geometry			(tracers.sh_Geom);
		RCache.Render				(D3DPT_TRIANGLELIST,vOffset,0,vCount,0,vCount/2);
		RCache.set_CullMode			(CULL_CCW);
	}
}