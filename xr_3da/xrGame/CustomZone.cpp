#include "stdafx.h"
#include "../xr_ioconsole.h"
#include "customzone.h"
#include "actor.h"
#include "hudmanager.h"
#include "ParticlesObject.h"
#include "xrserver_objects_alife_monsters.h"

#define SMALL_OBJECT_RADIUS 0.6f


CCustomZone::CCustomZone(void) 
{
	m_fMaxPower = 100.f;
	m_fAttenuation = 1.f;
	m_dwPeriod = 1100;
	
	m_bZoneReady = false;
	m_bZoneActive = false;

	m_bIgnoreNonAlive = false;
	m_bIgnoreSmall = false;


	m_pLocalActor = NULL;

	m_pIdleParticles = NULL;

	m_sIdleParticles = NULL;
	m_sBlowoutParticles = NULL;

	m_sHitParticlesSmall = NULL;
	m_sHitParticlesBig = NULL;
	m_sIdleObjectParticlesSmall = NULL;
	m_sIdleObjectParticlesBig = NULL;


	m_pLight = NULL;
}

CCustomZone::~CCustomZone(void) 
{
	m_idle_sound.destroy();
	m_blowout_sound.destroy();
	m_hit_sound.destroy();
	m_entrance_sound.destroy();

	if(m_pLight)
        ::Render->light_destroy	(m_pLight);
}

void CCustomZone::Load(LPCSTR section) 
{
	// verify class
	LPCSTR Class = pSettings->r_string(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	inherited::Load(section);

	m_iDisableHitTime = pSettings->r_s32(section,"disable_time");	
	m_iDisableIdleTime = pSettings->r_s32(section,"disable_idle_time");	
	m_fHitImpulseScale = pSettings->r_float(section,"hit_impulse_scale");	

	m_bIgnoreNonAlive = !!pSettings->r_bool(section, "ignore_nonalive");
	m_bIgnoreSmall	  = !!pSettings->r_bool(section, "ignore_small");
	
//////////////////////////////////////////////////////////////////////////
	ISpatial*		self				=	dynamic_cast<ISpatial*> (this);
	if (self)		self->spatial.type	|=	STYPE_COLLIDEABLE;
//////////////////////////////////////////////////////////////////////////

	LPCSTR sound_str = NULL;
	
	if(pSettings->line_exist(section,"idle_sound")) 
	{
		sound_str = pSettings->r_string(section,"idle_sound");
		m_idle_sound.create(TRUE, sound_str, st_SourceType);
	}
	
	if(pSettings->line_exist(section,"blowout_sound")) 
	{
		sound_str = pSettings->r_string(section,"blowout_sound");
		m_blowout_sound.create(TRUE, sound_str, st_SourceType);
	}
	
	if(pSettings->line_exist(section,"hit_sound")) 
	{
		sound_str = pSettings->r_string(section,"hit_sound");
		m_hit_sound.create(TRUE, sound_str, st_SourceType);
	}

	if(pSettings->line_exist(section,"entrance_sound")) 
	{
		sound_str = pSettings->r_string(section,"entrance_sound");
		m_entrance_sound.create(TRUE, sound_str, st_SourceType);
	}


	if(pSettings->line_exist(section,"idle_particles")) 
		m_sIdleParticles	= pSettings->r_string(section,"idle_particles");
	if(pSettings->line_exist(section,"blowout_particles")) 
		m_sBlowoutParticles = pSettings->r_string(section,"blowout_particles");


	if(pSettings->line_exist(section,"entrance_small_particles")) 
		m_sEntranceParticlesSmall = pSettings->r_string(section,"entrance_small_particles");
	if(pSettings->line_exist(section,"entrance_big_particles")) 
		m_sEntranceParticlesBig = pSettings->r_string(section,"entrance_big_particles");

	if(pSettings->line_exist(section,"hit_small_particles")) 
		m_sHitParticlesSmall = pSettings->r_string(section,"hit_small_particles");
	if(pSettings->line_exist(section,"hit_big_particles")) 
		m_sHitParticlesBig = pSettings->r_string(section,"hit_big_particles");

	if(pSettings->line_exist(section,"idle_small_particles")) 
		m_sIdleObjectParticlesBig = pSettings->r_string(section,"idle_big_particles");
	if(pSettings->line_exist(section,"idle_big_particles")) 
		m_sIdleObjectParticlesSmall = pSettings->r_string(section,"idle_small_particles");

	m_effector.Load(pSettings->r_string(section,"postprocess"));


	//загрузить параметры световой вспышки от взрыва
	m_bBlowoutLight = !!pSettings->r_bool (section, "blowout_light");

	if(m_bBlowoutLight) 
	{
		sscanf(pSettings->r_string(section,"light_color"), "%f,%f,%f", &m_LightColor.r, &m_LightColor.g, &m_LightColor.b);
		m_fLightRange			= pSettings->r_float(section,"light_range");
		m_dwLightTime			= iFloor(pSettings->r_float(section,"light_time")*1000.f);
		m_dwLightTimeLeft		= 0;

		m_pLight = ::Render->light_create();
		m_pLight->set_shadow(true);
	}
	else
	{
		m_pLight = NULL;
	}
}

BOOL CCustomZone::net_Spawn(LPVOID DC) 
{
	CCF_Shape *l_pShape			= xr_new<CCF_Shape>(this);
	collidable.model			= l_pShape;
	CSE_Abstract				*e = (CSE_Abstract*)(DC);
	CSE_ALifeAnomalousZone		*Z = dynamic_cast<CSE_ALifeAnomalousZone*>(e);
	
	for (u32 i=0; i < Z->shapes.size(); ++i) 
	{
		CSE_Shape::shape_def	&S = Z->shapes[i];
		switch (S.type) 
		{
			case 0 : l_pShape->add_sphere(S.data.sphere); break;
			case 1 : l_pShape->add_box(S.data.box); break;
		}
	}

	BOOL bOk = inherited::net_Spawn(DC);
	
	if (bOk)
	{
		l_pShape->ComputeBounds		();
		m_fMaxPower					= Z->m_maxPower;
		m_fAttenuation				= Z->m_attn;
		m_dwPeriod					= Z->m_period;

		Fvector						P;
		XFORM().transform_tiny		(P,CFORM()->getSphere().P);
		
		//	setVisible(true);
		setEnabled(true);

		PlayIdleParticles();	
	}

	m_effector.SetRadius(CFORM()->getSphere().R);

	return bOk;
}

void CCustomZone::net_Destroy() 
{
	inherited::net_Destroy();
	
	StopIdleParticles();
	if(m_pLight)
		m_pLight->set_active(false);
}

void CCustomZone::UpdateCL() 
{
	inherited::UpdateCL();

	//вычислить время срабатывания зоны
	if(m_bZoneActive)
		m_dwDeltaTime += Device.dwTimeDelta;	
	else
		m_dwDeltaTime = 0;

	if(m_dwDeltaTime > m_dwPeriod) 
	{
		m_dwDeltaTime = m_dwPeriod;
		m_bZoneReady = true;
	}


	const Fsphere& s		= CFORM()->getSphere();
	Fvector					P;
	XFORM().transform_tiny(P,s.P);
	feel_touch_update		(P,s.R);
	
	if(m_bZoneReady) 
	{
		xr_set<CObject*>::iterator it;
		for(it = m_inZone.begin(); m_inZone.end() != it; ++it) 
		{
			Affect(*it);
		}
		m_bZoneReady = false;
	}
	
	if (EnableEffector())
		m_effector.Update(Level().CurrentEntity()->Position().distance_to(Position()));

	UpdateBlowoutLight	();
}

void CCustomZone::shedule_Update(u32 dt)
{
	m_bZoneActive = false;

	//пройтись по всем объектам в зоне
	//и проверить их состояние
	for(OBJECT_INFO_MAP_IT it = m_ObjectInfoMap.begin(); 
								m_ObjectInfoMap.end() != it; ++it) 
	{
		CObject* pObject = (*it).first;
		CEntityAlive* pEntityAlive = dynamic_cast<CEntityAlive*>(pObject);
		SZoneObjectInfo& info = (*it).second;

		info.time_in_zone += dt;
		
		if(m_iDisableHitTime != -1 && (int)info.time_in_zone > m_iDisableHitTime)
		{
			if(!pEntityAlive || !pEntityAlive->g_Alive())
				info.zone_ignore = true;
		}
		if(m_iDisableIdleTime != -1 && (int)info.time_in_zone > m_iDisableIdleTime)
		{
			if(!pEntityAlive || !pEntityAlive->g_Alive())
				StopObjectIdleParticles(dynamic_cast<CGameObject*>(pObject));
		}

		//если есть хотя бы один не дисабленый объект, то
		//зона считается активной
		if(info.zone_ignore == false) 
			m_bZoneActive = true;
	}

	inherited::shedule_Update(dt);
}

void CCustomZone::feel_touch_new(CObject* O) 
{
	if (dynamic_cast<CCustomZone*>(O)) return;
	
	if(bDebug) HUD().outMessage(0xffffffff,O->cName(),"entering a zone.");
	m_inZone.insert(O);
	if(dynamic_cast<CActor*>(O) && O == Level().CurrentEntity())
					m_pLocalActor = dynamic_cast<CActor*>(O);

	CGameObject* pGameObject =dynamic_cast<CGameObject*>(O);
	CEntityAlive* pEntityAlive =dynamic_cast<CEntityAlive*>(pGameObject);
	
	SZoneObjectInfo object_info;
	
	if(pEntityAlive && pEntityAlive->g_Alive())
		object_info.nonalive_object = false;
	else
		object_info.nonalive_object = true;

	if(pGameObject->Radius()<SMALL_OBJECT_RADIUS)
		object_info.small_object = true;
	else
		object_info.small_object = false;

	if((object_info.small_object && m_bIgnoreSmall) ||
		(object_info.nonalive_object && m_bIgnoreNonAlive))
		object_info.zone_ignore = true;
	else
		object_info.zone_ignore = false;

	m_ObjectInfoMap[O] = object_info;
	

	PlayEntranceParticles(pGameObject);
	PlayObjectIdleParticles(pGameObject);
}

void CCustomZone::feel_touch_delete(CObject* O) 
{
	if(bDebug) HUD().outMessage(0xffffffff,O->cName(),"leaving a zone.");
	
	m_inZone.erase(O);
	if(dynamic_cast<CActor*>(O)) m_pLocalActor = NULL;
	CGameObject* pGameObject =dynamic_cast<CGameObject*>(O);
	StopObjectIdleParticles(pGameObject);

	m_ObjectInfoMap.erase(O);
}

BOOL CCustomZone::feel_touch_contact(CObject* O) 
{
	if(O == this) return false;
	if(!O->Local() || !((CGameObject*)O)->IsVisibleForZones()) return false;
	
	return ((CCF_Shape*)CFORM())->Contact(O);
}

float CCustomZone::Power(float dist) 
{
	float radius = CFORM()->getRadius()*3/4.f;

//	f32 l_r = Visual()->vis.sphere.R;
//	return l_r < dist ? 0 : m_fMaxPower * (1.f - m_fAttenuation*dist/l_r);
	float power = radius < dist ? 0 : m_fMaxPower * (1.f - m_fAttenuation*(dist/radius)*(dist/radius));
	return power < 0 ? 0 : power;
}
/*
void CCustomZone::spatial_register()
{
	R_ASSERT2				(CFORM(),"Invalid or no CForm!");
	Fvector					P;
	XFORM().transform_tiny	(P,CFORM()->getSphere().P);
	spatial.center.set		(P);
	spatial.radius			= CFORM()->getRadius();
	ISpatial::spatial_register();
}

void CCustomZone::spatial_unregister()
{
	ISpatial::spatial_unregister();
}

void CCustomZone::spatial_move()
{
	R_ASSERT2				(CFORM(),"Invalid or no CForm!");
	Fvector					P;
	XFORM().transform_tiny	(P,CFORM()->getSphere().P);
	spatial.center.set		(P);
	spatial.radius			= CFORM()->getRadius();
	ISpatial::spatial_move	();
}*/


void CCustomZone::Center(Fvector& C) const
{
	XFORM().transform_tiny	(C,CFORM()->getSphere().P);
}
float CCustomZone::Radius()	const
{
	return CFORM()->getRadius();
}


#ifdef DEBUG
void CCustomZone::OnRender() 
{
	if(!bDebug) return;
	RCache.OnFrameEnd();
	Fvector l_half; l_half.set(.5f, .5f, .5f);
	Fmatrix l_ball, l_box;
	xr_vector<CCF_Shape::shape_def> &l_shapes = ((CCF_Shape*)CFORM())->Shapes();
	xr_vector<CCF_Shape::shape_def>::iterator l_pShape;
	
	for(l_pShape = l_shapes.begin(); l_shapes.end() != l_pShape; ++l_pShape) 
	{
		switch(l_pShape->type)
		{
		case 0:
			{
                Fsphere &l_sphere = l_pShape->data.sphere;
				l_ball.scale(l_sphere.R, l_sphere.R, l_sphere.R);
				//l_ball.scale(1.f, 1.f, 1.f);
				Fvector l_p; XFORM().transform(l_p, l_sphere.P);
				l_ball.translate_add(l_p);
				//l_ball.mul(XFORM(), l_ball);
				//l_ball.mul(l_ball, XFORM());
				RCache.dbg_DrawEllipse(l_ball, D3DCOLOR_XRGB(0,255,255));
			}
			break;
		case 1:
			{
				l_box.mul(XFORM(), l_pShape->data.box);
				RCache.dbg_DrawOBB(l_box, l_half, D3DCOLOR_XRGB(0,255,255));
			}
			break;
		}
	}
}
#endif

void CCustomZone::Affect(CObject* O) 
{
	CGameObject *pGameObject = dynamic_cast<CGameObject*>(O);
	if(!pGameObject) return;

	if(m_ObjectInfoMap[O].zone_ignore) return;

	Fvector P; 
	XFORM().transform_tiny(P,CFORM()->getSphere().P);
	
	char l_pow[255]; 
	sprintf(l_pow, "zone hit. %.1f", Power(pGameObject->Position().distance_to(P)));
	if(bDebug) HUD().outMessage(0xffffffff,pGameObject->cName(), l_pow);
	
	Fvector hit_dir; 
	hit_dir.set(::Random.randF(-.5f,.5f), 
				::Random.randF(.0f,1.f), 
				::Random.randF(-.5f,.5f)); 
	hit_dir.normalize();
	

	Fvector position_in_bone_space;
	
	float power = Power(pGameObject->Position().distance_to(P));
	float impulse = m_fHitImpulseScale*power*pGameObject->GetMass();

	//статистика по объекту
	m_ObjectInfoMap[O].total_damage += power;
	m_ObjectInfoMap[O].hit_num++;
	
	if(power > 0.01f) 
	{
		m_dwDeltaTime = 0;
		position_in_bone_space.set(0.f,0.f,0.f);
		
		NET_Packet	l_P;
		u_EventGen	(l_P,GE_HIT, pGameObject->ID());
		l_P.w_u16	(u16(pGameObject->ID()));
		l_P.w_u16	(ID());
		l_P.w_dir	(hit_dir);
		l_P.w_float	(power);
		l_P.w_s16	((s16)0);
		l_P.w_vec3	(position_in_bone_space);
		l_P.w_float	(impulse);
		l_P.w_u16	(ALife::eHitTypeWound);
		u_EventSend	(l_P);


		PlayHitParticles(pGameObject);
		PlayBlowoutParticles();
	}
	
}


void CCustomZone::PlayIdleParticles()
{
	m_idle_sound.play_at_pos(this, Position(), true);

	if(!m_sIdleParticles) return;
	m_pIdleParticles = xr_new<CParticlesObject>(*m_sIdleParticles,Sector(),false);
	m_pIdleParticles->UpdateParent(XFORM(),zero_vel);
	m_pIdleParticles->Play();
}

void CCustomZone::StopIdleParticles()
{
	m_idle_sound.stop();

	if(!m_pIdleParticles) return;
	m_pIdleParticles->Stop();
	m_pIdleParticles->PSI_destroy();
	m_pIdleParticles = NULL;
}

void CCustomZone::PlayBlowoutParticles()
{
	m_blowout_sound.play_at_pos	(this, Position());

	StartBlowoutLight ();

	if(!m_sBlowoutParticles) return;

	CParticlesObject* pParticles;
	pParticles = xr_new<CParticlesObject>(*m_sBlowoutParticles,Sector());
	pParticles->UpdateParent(XFORM(),zero_vel);
	pParticles->Play();
}

void CCustomZone::PlayHitParticles(CGameObject* pObject)
{
	m_hit_sound.play_at_pos(this, pObject->Position());

	ref_str particle_str = NULL;

	//разные партиклы для объектов разного размера
	if(pObject->Radius()<SMALL_OBJECT_RADIUS)
	{
		if(!m_sHitParticlesSmall) return;
		particle_str = m_sHitParticlesSmall;
	}
	else
	{
		if(!m_sHitParticlesBig) return;
		particle_str = m_sHitParticlesBig;
	}

	// play particles
	CParticlesPlayer* PP = dynamic_cast<CParticlesPlayer*>(pObject);
	if (PP){
		u16 play_bone = pObject->GetRandomBone(); 
		if (play_bone!=BI_NONE)
			PP->StartParticles	(*particle_str,play_bone,Fvector().set(0,1,0),(u16)ID());
	}
}

void CCustomZone::PlayEntranceParticles(CGameObject* pObject)
{
	m_entrance_sound.play_at_pos(this, pObject->Position());

	ref_str particle_str = NULL;

	//разные партиклы для объектов разного размера
	if(pObject->Radius()<SMALL_OBJECT_RADIUS)
	{
		if(!m_sEntranceParticlesSmall) return;
		particle_str = m_sEntranceParticlesSmall;
	}
	else
	{
		if(!m_sEntranceParticlesBig) return;
		particle_str = m_sEntranceParticlesBig;
	}

	//выбрать случайную косточку на объекте
	CParticlesPlayer* PP = dynamic_cast<CParticlesPlayer*>(pObject);
	if (PP){
		u16 play_bone = pObject->GetRandomBone(); 
		if (play_bone!=BI_NONE){
			CParticlesObject* pParticles = CParticlesObject::Create(*particle_str);
			Fmatrix xform;
			PP->MakeXFORM			(pObject,play_bone,Fvector().set(0,1,0),Fvector().set(0,0,0),xform);
			pParticles->UpdateParent(xform,zero_vel);
			pParticles->Play		();
		}
	}
}


void CCustomZone::PlayBulletParticles(Fvector& pos)
{
	m_entrance_sound.play_at_pos(this, pos);

	if(!m_sEntranceParticlesSmall) return;
	
	CParticlesObject* pParticles;
	pParticles = xr_new<CParticlesObject>(*m_sEntranceParticlesSmall,Sector());
	
	Fmatrix M;
	M = XFORM();
	M.c.set(pos);

	pParticles->UpdateParent(M,zero_vel);
	pParticles->Play();
}

void CCustomZone::PlayObjectIdleParticles(CGameObject* pObject)
{
	ref_str particle_str = NULL;

	//разные партиклы для объектов разного размера
	if(pObject->Radius()<SMALL_OBJECT_RADIUS)
	{
		if(!m_sIdleObjectParticlesSmall) return;
		particle_str = m_sIdleObjectParticlesSmall;
	}
	else
	{
		if(!m_sIdleObjectParticlesBig) return;
		particle_str = m_sIdleObjectParticlesBig;
	}

	
	//запустить партиклы на объекте
	pObject->StartParticles(*particle_str, Fvector().set(0,1,0), (u16)ID());
}
void CCustomZone::StopObjectIdleParticles(CGameObject* pObject)
{
	OBJECT_INFO_MAP_IT it	= m_ObjectInfoMap.find(pObject);
	if(m_ObjectInfoMap.end() == it) return;
	//остановить партиклы
	pObject->StopParticles	(ID());
}


void  CCustomZone::Hit(float P, Fvector &dir,
					  CObject* who, s16 element,
					  Fvector position_in_object_space,
					  float impulse,
					  ALife::EHitType hit_type)
{
	Fmatrix M;
	M.identity();
	M.translate_over(position_in_object_space);
	M.mulA(XFORM());
	PlayBulletParticles(M.c);


	inherited::Hit(P, dir, who, element, position_in_object_space, impulse, hit_type);
}


void CCustomZone::StartBlowoutLight		()
{
	if(!m_pLight || m_dwLightTime<=0) return;
	
	m_dwLightTimeLeft = m_dwLightTime;

	m_pLight->set_color(m_LightColor.r, 
						m_LightColor.g, 
						m_LightColor.b);
	m_pLight->set_range(m_fLightRange);
	
	Fvector pos = Position();
	pos.y += 0.5f;
	m_pLight->set_position(pos);
	m_pLight->set_active(true);
}
void CCustomZone::UpdateBlowoutLight	()
{
	if(!m_pLight || !m_pLight->get_active()) return;

	if(m_dwLightTimeLeft>0)
	{
		m_dwLightTimeLeft -= Device.dwTimeDelta;

		float scale = float(m_dwLightTimeLeft)/float(m_dwLightTime);
		m_pLight->set_color(m_LightColor.r*scale, 
							m_LightColor.g*scale, 
							m_LightColor.b*scale);
		m_pLight->set_range(m_fLightRange*scale);
	}
	else
	{
		m_dwLightTimeLeft = 0;
		m_pLight->set_active(false);
		m_pLight->set_range(0.1f);
	}
}