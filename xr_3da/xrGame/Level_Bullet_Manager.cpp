// Level_Bullet_Manager.cpp:	для обеспечения полета пули по траектории
//								все пули и осколки передаются сюда
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Level_Bullet_Manager.h"


#define HIT_POWER_EPSILON 0.05f
#define WALLMARK_SIZE 0.04f

float CBulletManager::m_fMinBulletSpeed = 3.f;


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
}

CBulletManager::~CBulletManager()
{
}

#define BULLET_MANAGER_SECTION "bullet_manager"

void CBulletManager::Load		()
{
	m_fTracerWidth		= pSettings->r_float(BULLET_MANAGER_SECTION, "tracer_width");
	m_fTracerLength		= pSettings->r_float(BULLET_MANAGER_SECTION, "tracer_length_max");
	m_fTracerLengthMin	= pSettings->r_float(BULLET_MANAGER_SECTION, "tracer_length_min");
	m_fLengthToWidthRatio = pSettings->r_float(BULLET_MANAGER_SECTION, "tracer_length_to_width_ratio");

	m_fMinViewDist = pSettings->r_float(BULLET_MANAGER_SECTION, "min_view_dist");
	m_fMaxViewDist = pSettings->r_float(BULLET_MANAGER_SECTION, "max_view_dist");

	m_fGravityConst = pSettings->r_float(BULLET_MANAGER_SECTION, "gravity_const");
	m_fAirResistanceK = pSettings->r_float(BULLET_MANAGER_SECTION, "air_resistance_k");

	m_dwStepTime = pSettings->r_u32(BULLET_MANAGER_SECTION,	   "time_step");
	m_fMinBulletSpeed = pSettings->r_float(BULLET_MANAGER_SECTION, "min_bullet_speed");
	m_fCollisionEnergyMin  = pSettings->r_float(BULLET_MANAGER_SECTION, "collision_energy_min");
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
	Collide::ray_defs RD(bullet->pos, bullet->dir, range, 0 ,/*Collide::rqtAll*/Collide::rqtBoth);
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
		air_resistance.mul(-m_fAirResistanceK*delta_time_sec);

		bullet->dir.add(air_resistance);
		bullet->dir.y -= m_fGravityConst*delta_time_sec;

		bullet->speed = bullet->dir.magnitude();
		//вместо normalize(),	 чтоб не считать 2 раза magnitude()
#pragma todo("а как насчет bullet->speed==0")
		bullet->dir.x /= bullet->speed;
		bullet->dir.y /= bullet->speed;
		bullet->dir.z /= bullet->speed;
	}

	if(bullet->speed<m_fMinBulletSpeed)
		return false;

	return true;
}


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

		if(length<m_fTracerLengthMin)
			continue;

		if(length>m_fTracerLength)
			length = m_fTracerLength;

		//изменить размер трассера в зависимости от расстояния до камеры
		Fvector to_camera;
		to_camera.sub(bullet->pos,Device.vCameraPosition);
		float dist_to_camera = to_camera.magnitude();

		if(dist_to_camera<m_fMinViewDist)
			length = m_fTracerLengthMin;
		else if(dist_to_camera<m_fMaxViewDist)
		{
			float length_max = m_fTracerLengthMin + 
							  (m_fTracerLength - m_fTracerLengthMin)*
							  (dist_to_camera-m_fMinViewDist)/
							  (m_fMaxViewDist-m_fMinViewDist);

			if(length>length_max) length  = length_max;
		}

		float width;
		if(length>(m_fTracerWidth*m_fLengthToWidthRatio))
			width = m_fTracerWidth;
		else 
			width = length/m_fLengthToWidthRatio;

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