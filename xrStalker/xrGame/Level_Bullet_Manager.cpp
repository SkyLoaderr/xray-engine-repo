// Level_Bullet_Manager.cpp:	для обеспечения полета пули по траектории
//								все пули и осколки передаются сюда
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
	Device.seqRender.Add		(this,REG_PRIORITY_LOW-1000);
}

CBulletManager::~CBulletManager()
{
	Device.seqRender.Remove		(this);
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
		
		//для пули пущенной на этом же кадре считаем только 1 шаг
		//(хотя по теории вообще ничего считать на надо)
		//который пропустим на следующем кадре, 
		//это делается для того чтоб при скачках FPS не промазать
		//с 2х метров
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
//сопротивление воздуха, процент, который отнимается от скорости
//полета пули
#define AIR_RESISTANCE_K 0.3f

bool CBulletManager::CalcBullet (SBullet* bullet, u32 delta_time)
{
	VERIFY(bullet);

	float delta_time_sec = float(delta_time)/1000.f;
	float range = bullet->speed*delta_time_sec;
	
	float max_range = bullet->max_dist - bullet->fly_dist;
	if(range>max_range) 
		range = max_range;

	//запомнить текущую скорость пули, т.к. в
	//RayQuery() она может поменяться из-за рикошетов
	//и столкновений с объектами
	Fvector cur_dir = bullet->dir;
	
	bullet->flags.set(SBullet::RICOCHET_FLAG,0);
	Collide::ray_defs RD(bullet->pos, bullet->dir, range, 0 ,Collide::rqtAll);
	BOOL result = FALSE;
	result = Level().ObjectSpace.RayQuery( RD, firetrace_callback, bullet);

	if(!bullet->flags.test(SBullet::RICOCHET_FLAG))
	{
		//изменить положение пули
		bullet->prev_pos = bullet->pos;
		bullet->pos.mad(bullet->pos, cur_dir, range);
		bullet->fly_dist += range;

		if(bullet->fly_dist>=bullet->max_dist)
			return false;

		Fbox level_box = Level().ObjectSpace.GetBoundingVolume();
		if(!level_box.contains(bullet->pos))
			return false;

		//изменить скорость и направление ее полета
		//с учетом гравитации
		bullet->dir.mul(bullet->speed);

		Fvector air_resistance = bullet->dir;
		air_resistance.mul(-AIR_RESISTANCE_K*delta_time_sec);

		bullet->dir.add(air_resistance);
		bullet->dir.y -= GRAVITY_CONST*delta_time_sec;

		bullet->speed = bullet->dir.magnitude();
		//вместо normalize(), чтоб не считать 2 раза magnitude()
#pragma todo("а как насчет bullet->speed==0")
		bullet->dir.x /= bullet->speed;
		bullet->dir.y /= bullet->speed;
		bullet->dir.z /= bullet->speed;
	}

	if(bullet->speed<SPEED_LOWER_BOUND)
		return false;

	return true;
}

void CBulletManager::OnRender	()
{
	for(BULLET_LIST_it it = m_BulletList.begin();
		it != m_BulletList.end(); it++)
	{
		SBullet* bullet = *it;
		RCache.dbg_DrawLINE(Fidentity,bullet->prev_pos,bullet->pos,D3DCOLOR_XRGB(0,255,0));
	}
}