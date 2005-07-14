// Level_Bullet_Manager.cpp:	для обеспечения полета пули по траектории
//								все пули и осколки передаются сюда
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Level.h"
#include "Level_Bullet_Manager.h"
#include "game_cl_base.h"
#include "Actor.h"
#include "AI/Stalker/ai_stalker.h"
#include "gamepersistent.h"

#define HIT_POWER_EPSILON 0.05f
#define WALLMARK_SIZE 0.04f

float CBulletManager::m_fMinBulletSpeed = 2.f;


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
				   u16 sendersweapon_id,
				   ALife::EHitType e_hit_type,
				   float maximum_distance,
				   const CCartridge& cartridge,
				   bool SendHit,
				   float tracer_length)
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
	m_bSendHit		= SendHit;
	weapon_id		= sendersweapon_id;
	hit_type		= e_hit_type;

	max_dist		= maximum_distance;
	dist_k			= cartridge.m_kDist;
	hit_k			= cartridge.m_kHit;
	impulse_k		= cartridge.m_kImpulse;
	pierce_k		= cartridge.m_kPierce;
	wallmark_size	= cartridge.fWallmarkSize;

	tracer_max_length = tracer_length;

	bullet_material_idx = cartridge.bullet_material_idx;
	VERIFY			(u16(-1)!=bullet_material_idx);

	flags.set(TRACER_FLAG,				cartridge.m_flags.test(CCartridge::cfTracer));
	flags.set(RICOCHET_ENABLED_FLAG,	cartridge.m_flags.test(CCartridge::cfRicochet));
	flags.set(EXPLOSIVE_FLAG,			cartridge.m_flags.test(CCartridge::cfExplosive) );
	render_offset = ::Random.randF(0.f,1.0f);
}


//////////////////////////////////////////////////////////
//

CBulletManager::CBulletManager()
{
	m_Bullets.clear			();
	m_Bullets.reserve		(100);
}

CBulletManager::~CBulletManager()
{
	m_Bullets.clear			();
	m_WhineSounds.clear		();
}

#define BULLET_MANAGER_SECTION "bullet_manager"

void CBulletManager::Load		()
{
	m_fTracerWidth			= pSettings->r_float(BULLET_MANAGER_SECTION, "tracer_width");
	m_fTracerLength			= pSettings->r_float(BULLET_MANAGER_SECTION, "tracer_length_max");
	m_fTracerLengthMin		= pSettings->r_float(BULLET_MANAGER_SECTION, "tracer_length_min");
	m_fLengthToWidthRatio	= pSettings->r_float(BULLET_MANAGER_SECTION, "tracer_length_to_width_ratio");

	m_fMinViewDist			= pSettings->r_float(BULLET_MANAGER_SECTION, "min_view_dist");
	m_fMaxViewDist			= pSettings->r_float(BULLET_MANAGER_SECTION, "max_view_dist");

	m_fGravityConst			= pSettings->r_float(BULLET_MANAGER_SECTION, "gravity_const");
	m_fAirResistanceK		= pSettings->r_float(BULLET_MANAGER_SECTION, "air_resistance_k");

	m_dwStepTime			= pSettings->r_u32	(BULLET_MANAGER_SECTION, "time_step");
	m_fMinBulletSpeed		= pSettings->r_float(BULLET_MANAGER_SECTION, "min_bullet_speed");
	m_fCollisionEnergyMin	= pSettings->r_float(BULLET_MANAGER_SECTION, "collision_energy_min");
	m_fCollisionEnergyMax	= pSettings->r_float(BULLET_MANAGER_SECTION, "collision_energy_max");
	LPCSTR whine_sounds		= pSettings->r_string(BULLET_MANAGER_SECTION, "whine_sounds");
	int cnt					= _GetItemCount(whine_sounds);
	xr_string tmp;
	for (int k=0; k<cnt; ++k){
		m_WhineSounds.push_back	(ref_sound());
		m_WhineSounds.back().create(TRUE,_GetItem(whine_sounds,k,tmp));
	}

	LPCSTR explode_particles= pSettings->r_string(BULLET_MANAGER_SECTION, "explode_particles");
	cnt						= _GetItemCount(explode_particles);
	for (int k=0; k<cnt; ++k)
		m_ExplodeParticles.push_back	(_GetItem(explode_particles,k,tmp));
}

void CBulletManager::PlayExplodePS		(const Fmatrix& xf)
{
	if (!m_ExplodeParticles.empty()){
		const shared_str& ps_name		= m_ExplodeParticles[Random.randI(0, m_ExplodeParticles.size())];

		CParticlesObject* ps = xr_new<CParticlesObject>(*ps_name,TRUE);
		ps->UpdateParent(xf,zero_vel);
		GamePersistent().ps_needtoplay.push_back(ps);
	}
}

void CBulletManager::PlayWhineSound(CObject* object, const Fvector& pos)
{
	if (!m_WhineSounds.empty()){
		ref_sound& snd			= m_WhineSounds[Random.randI(0, m_WhineSounds.size())];
		snd.play_at_pos_unlimited(object,pos);
	}
}

void CBulletManager::Clear		()
{
	m_Bullets.clear();
}

void CBulletManager::AddBullet(const Fvector& position,
							   const Fvector& direction,
							   float starting_speed,
							   float power,
							   float impulse,
							   u16	sender_id,
							   u16 sendersweapon_id,
							   ALife::EHitType e_hit_type,
							   float maximum_distance,
							   const CCartridge& cartridge,
							   bool SendHit,
							   float tracer_length)
{
	VERIFY		(u16(-1)!=cartridge.bullet_material_idx);
//	u32 CurID = Level().CurrentControlEntity()->ID();
//	u32 OwnerID = sender_id;
	m_Bullets.push_back(SBullet());
	SBullet& bullet	= m_Bullets.back();
	bullet.Init		(position, direction, starting_speed, power, impulse, sender_id, sendersweapon_id, e_hit_type, maximum_distance, cartridge, SendHit, tracer_length);
	bullet.frame_num = Device.dwFrame;
	if (SendHit && GameID() != GAME_SINGLE)
		Game().m_WeaponUsageStatistic.OnBullet_Fire(&bullet, cartridge);
}


void CBulletManager::Update()
{
	m_Lock.Enter		()	;
	u32 delta_time		=	Device.dwTimeDelta + m_dwTimeRemainder;
	u32 step_num		=	delta_time/m_dwStepTime;
	m_dwTimeRemainder	=	delta_time%m_dwStepTime;
	
	collide::rq_results		rq_storage	;
	xr_vector<ISpatial*>	rq_spatial	;

	for(int k=m_Bullets.size()-1; k>=0; k--){
		SBullet& bullet = m_Bullets[k];
		//для пули пущенной на этом же кадре считаем только 1 шаг
		//(хотя по теории вообще ничего считать на надо)
		//который пропустим на следующем кадре, 
		//это делается для того чтоб при скачках FPS не промазать
		//с 2х метров
		u32 cur_step_num = step_num;

		u32 frames_pass = Device.dwFrame - bullet.frame_num;
		if(frames_pass == 0)						cur_step_num = 1;
		else if (frames_pass == 1 && step_num>0)	cur_step_num -= 1;

		// calculate bullet
		for(u32 i=0; i<cur_step_num; i++){
			if(!CalcBullet(rq_storage,rq_spatial,&bullet, m_dwStepTime)){
				if (bullet.m_bSendHit && GameID() != GAME_SINGLE)
					Game().m_WeaponUsageStatistic.OnBullet_Remove(&bullet);
				m_Bullets[k] = m_Bullets.back();
				m_Bullets.pop_back();
				break;
			}
		}
	}
	m_Lock.Leave		();
}

bool CBulletManager::CalcBullet (collide::rq_results & rq_storage, xr_vector<ISpatial*>& rq_spatial, SBullet* bullet, u32 delta_time)
{
	VERIFY					(bullet);

	float delta_time_sec	= float(delta_time)/1000.f;
	float range				= bullet->speed*delta_time_sec;
	
	float max_range					= bullet->max_dist - bullet->fly_dist;
	if(range>max_range) 
		range = max_range;

	//запомнить текущую скорость пули, т.к. в
	//RayQuery() она может поменяться из-за рикошетов
	//и столкновений с объектами
	Fvector cur_dir					= bullet->dir;
	
	bullet->flags.set				(SBullet::RICOCHET_FLAG,0);
	collide::ray_defs RD			(bullet->pos, bullet->dir, range, 0, collide::rqtBoth);
	BOOL result						= FALSE;
	result							= Level().ObjectSpace.RayQuery(rq_storage, RD, firetrace_callback, bullet, test_callback);
	if (result) range				= (rq_storage.r_begin()+rq_storage.r_count()-1)->range;
	range		= _max				(EPS_L,range);
	// whine test
	g_SpatialSpace->q_ray			(rq_spatial,0,STYPE_COLLIDEABLE,bullet->pos,bullet->dir,range);
	// Determine visibility for dynamic part of scene
	for (u32 o_it=0; o_it<rq_spatial.size(); o_it++)	{
		CEntity*	entity			= smart_cast<CEntity*>(rq_spatial[o_it]->dcast_CObject());
		if (entity&&entity->g_Alive()&&(entity->ID()!=bullet->parent_id)){
			ICollisionForm*	cform	= entity->collidable.model;
			ECollisionFormType tp	= cform->Type();
			if ((tp==cftObject)&&(smart_cast<CAI_Stalker*>(entity)||smart_cast<CActor*>(entity))){
				Fsphere S			= cform->getSphere();
				entity->XFORM().transform_tiny	(S.P)	;
				float dist			= range				;
				if (Fsphere::rpNone!=S.intersect(bullet->pos, bullet->dir, dist)){
					Fvector			pt;
					pt.mad			(bullet->pos, bullet->dir, dist);
					CObject* initiator	= Level().Objects.net_Find	(bullet->parent_id);
					Level().BulletManager().PlayWhineSound			(initiator,pt);
				}
			}
		}
	}

	if(!bullet->flags.test(SBullet::RICOCHET_FLAG))
	{
		//изменить положение пули
		bullet->prev_pos = bullet->pos;
		bullet->pos.mad(bullet->pos, cur_dir, range);
		bullet->fly_dist += range;

		if(bullet->fly_dist>=bullet->max_dist)
			return false;

		Fbox level_box = Level().ObjectSpace.GetBoundingVolume();
		
/*		if(!level_box.contains(bullet->pos))
			return false;
*/
		if(!((bullet->pos.x>=level_box.x1) && 
			 (bullet->pos.x<=level_box.x2) && 
			 (bullet->pos.y>=level_box.y1) && 
//			 (bullet->pos.y<=level_box.y2) && 
			 (bullet->pos.z>=level_box.z1) && 
			 (bullet->pos.z<=level_box.z2))	)
			 return false;

		//изменить скорость и направление ее полета
		//с учетом гравитации
		bullet->dir.mul(bullet->speed);

		Fvector air_resistance = bullet->dir;
		air_resistance.mul(-m_fAirResistanceK*delta_time_sec);

		bullet->dir.add(air_resistance);
		bullet->dir.y -= m_fGravityConst*delta_time_sec;

		bullet->speed = bullet->dir.magnitude();
		VERIFY(_valid(bullet->speed));
		VERIFY(!fis_zero(bullet->speed));
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

#ifdef DEBUG
	BOOL g_bDrawBulletHit = FALSE;
#endif

void CBulletManager::Render	()
{
#ifdef DEBUG
	//0-рикошет
	//1-застрявание пули в материале
	//2-пробивание материала
	if(g_bDrawBulletHit){
		extern FvectorVec g_hit[];
		FvectorIt it;
		u32 C[3] = {0xffff0000,0xff00ff00,0xff0000ff};
		RCache.set_xform_world(Fidentity);
		for(int i=0; i<3; ++i)
			for(it=g_hit[i].begin();it!=g_hit[i].end();++it){
				RCache.dbg_DrawAABB(*it,0.01f,0.01f,0.01f,C[i]);
			}
	}
#endif

	if(m_Bullets.empty()) return;

	m_Lock.Enter		()		;

	u32	vOffset			=	0	;
	u32 bullet_num		=	m_Bullets.size();

	

	FVF::V	*verts		=	(FVF::V	*) RCache.Vertex.Lock((u32)bullet_num*8,
										tracers.sh_Geom->vb_stride,
										vOffset);
	FVF::V	*start		=	verts;



	for(BulletVecIt it = m_Bullets.begin(); it!=m_Bullets.end(); it++){
		SBullet* bullet = &(*it);

		if(!bullet->flags.is(SBullet::TRACER_FLAG)) 
			continue;

		Fvector dist;
		dist.sub(bullet->prev_pos,bullet->pos);
		float length = dist.magnitude();

		if(length<m_fTracerLengthMin)
			continue;

		//вычислить максимально допустимую длину трассера
		//выбираем между общими настройками и настройками пули
		//(по умолчанию в пуле такая равна flt_max),
		float max_length = _min(m_fTracerLength, bullet->tracer_max_length);
		if(length>max_length)
			length = max_length;

		//изменить размер трассера в зависимости от расстояния до камеры
		Fvector to_camera;
		to_camera.sub(bullet->pos,Device.vCameraPosition);
		float dist_to_camera = to_camera.magnitude();

		if(dist_to_camera<m_fMinViewDist)
			length = m_fTracerLengthMin;
		else if(dist_to_camera<m_fMaxViewDist)
		{
			float length_max = m_fTracerLengthMin + 
							  (max_length - m_fTracerLengthMin)*
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
		center.mad(bullet->pos, dist,  -length*bullet->render_offset);
		tracers.Render			(verts, center, dist, length, width);
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
	m_Lock.Leave		();
}