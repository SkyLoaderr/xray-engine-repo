#include "stdafx.h"
#include "../xr_ioconsole.h"
#include "customzone.h"
#include "actor.h"
#include "hudmanager.h"
#include "ParticlesObject.h"
#include "xrserver_objects_alife_monsters.h"
#include "../LightAnimLibrary.h"
#include "level.h"
#include "game_cl_base.h"




//////////////////////////////////////////////////////////////////////////
//количество предварительно проспавненых артефактов
#define PREFETCHED_ARTEFACTS_NUM 4
//расстояние до актера, когда появляется ветер 
#define WIND_RADIUS (4*Radius())

CCustomZone::CCustomZone(void) 
{
	m_fMaxPower = 100.f;
	m_fAttenuation = 1.f;
	m_dwPeriod = 1100;
	
	m_bZoneReady = false;
	m_bZoneActive = false;

	m_bIgnoreNonAlive = false;
	m_bIgnoreSmall = false;
	m_bIgnoreArtefacts = true;

	m_eHitTypeBlowout = ALife::eHitTypeWound;

	m_pLocalActor = NULL;

	m_pIdleParticles = NULL;

	m_sIdleParticles = NULL;
	m_sBlowoutParticles = NULL;

	m_sHitParticlesSmall = NULL;
	m_sHitParticlesBig = NULL;
	m_sIdleObjectParticlesSmall = NULL;
	m_sIdleObjectParticlesBig = NULL;


	m_pLight = NULL;
	m_pIdleLight = NULL;
	m_pIdleLAnim = NULL;
	

	m_StateTime.resize(eZoneStateMax);
	for(int i=0; i<eZoneStateMax; i++)
		m_StateTime[i] = 0;


	m_dwAffectFrameNum = 0;
	m_dwLastTimeMoved = 0;

	m_bSpawnBlowoutArtefacts = false;
	m_fArtefactSpawnProbability = 0.f;
	m_sArtefactSpawnParticles = NULL;

	m_fThrowOutPower = 0.f;
	m_fArtefactSpawnHeight = 0.f;

	m_fBlowoutWindPowerCur = m_fBlowoutWindPowerMax = m_fRealWindPower = 0.f;
	m_bBlowoutWindActive = false;

	m_fDistanceToCurEntity = flt_max;

}

CCustomZone::~CCustomZone(void) 
{	
	m_idle_sound.destroy();
	m_blowout_sound.destroy();
	m_hit_sound.destroy();
	m_entrance_sound.destroy();
	m_ArtefactBornSound.destroy();
}

void CCustomZone::Load(LPCSTR section) 
{
	// verify class
	LPCSTR Class = pSettings->r_string(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	inherited::Load(section);

	m_iDisableHitTime = pSettings->r_s32(section,"disable_time");	
	m_iDisableHitTimeSmall = pSettings->r_s32(section,"disable_time_small");	
	m_iDisableIdleTime = pSettings->r_s32(section,"disable_idle_time");	
	m_fHitImpulseScale = pSettings->r_float(section,"hit_impulse_scale");
	m_eHitTypeBlowout = ALife::g_tfString2HitType(pSettings->r_string(section, "hit_type"));

	m_bIgnoreNonAlive = !!pSettings->r_bool(section, "ignore_nonalive");
	m_bIgnoreSmall	  = !!pSettings->r_bool(section, "ignore_small");
	m_bIgnoreArtefacts = !!pSettings->r_bool(section, "ignore_artefacts");


	m_bVisibleByDetector = !!pSettings->r_bool(section, "visible_by_detector");

	//загрузить времена для зоны
	m_StateTime[eZoneStateIdle]			= -1;
	m_StateTime[eZoneStateAwaking]		= pSettings->r_s32(section, "awaking_time");
	m_StateTime[eZoneStateBlowout]		= pSettings->r_s32(section, "blowout_time");
	m_StateTime[eZoneStateAccumulate]	= pSettings->r_s32(section, "accamulate_time");
	
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



	if(pSettings->line_exist(section,"blowout_particles_time")) 
	{
		m_dwBlowoutParticlesTime = pSettings->r_u32(section,"blowout_particles_time");
    	R_ASSERT((s32)m_dwBlowoutParticlesTime<=m_StateTime[eZoneStateBlowout]);
	}
	else
		m_dwBlowoutParticlesTime = 0;

	if(pSettings->line_exist(section,"blowout_light_time")) 
	{
		m_dwBlowoutLightTime = pSettings->r_u32(section,"blowout_light_time");
		R_ASSERT((s32)m_dwBlowoutLightTime<=m_StateTime[eZoneStateBlowout]);
	}
	else
		m_dwBlowoutLightTime = 0;

	if(pSettings->line_exist(section,"blowout_sound_time")) 
	{
		m_dwBlowoutSoundTime = pSettings->r_u32(section,"blowout_sound_time");
    	R_ASSERT((s32)m_dwBlowoutSoundTime<=m_StateTime[eZoneStateBlowout]);
	}
	else
		m_dwBlowoutSoundTime = 0;

	if(pSettings->line_exist(section,"blowout_explosion_time")) 
	{
		m_dwBlowoutExplosionTime = pSettings->r_u32(section,"blowout_explosion_time"); 
		R_ASSERT((s32)m_dwBlowoutExplosionTime<=m_StateTime[eZoneStateBlowout]);
	}
	else
		m_dwBlowoutExplosionTime = 0;


	m_bBlowoutWindEnable = !!pSettings->r_bool(section,"blowout_wind"); 
	if(m_bBlowoutWindEnable) 
	{
		m_dwBlowoutWindTimeStart = pSettings->r_u32(section,"blowout_wind_time_start"); 
		m_dwBlowoutWindTimePeak = pSettings->r_u32(section,"blowout_wind_time_peak"); 
		m_dwBlowoutWindTimeEnd = pSettings->r_u32(section,"blowout_wind_time_end"); 
		R_ASSERT(m_dwBlowoutWindTimeStart < m_dwBlowoutWindTimePeak);
		R_ASSERT(m_dwBlowoutWindTimePeak < m_dwBlowoutWindTimeEnd);
		R_ASSERT((s32)m_dwBlowoutWindTimeEnd < m_StateTime[eZoneStateBlowout]);
		
		m_fBlowoutWindPowerMax = pSettings->r_float(section,"blowout_wind_power");
	}

	//загрузить параметры световой вспышки от взрыва
	m_bBlowoutLight = !!pSettings->r_bool (section, "blowout_light");

	if(m_bBlowoutLight)
	{
		sscanf(pSettings->r_string(section,"light_color"), "%f,%f,%f", &m_LightColor.r, &m_LightColor.g, &m_LightColor.b);
		m_fLightRange			= pSettings->r_float(section,"light_range");
		m_fLightTime			= pSettings->r_float(section,"light_time");
		m_fLightTimeLeft		= 0;

		m_fLightHeight		= pSettings->r_float(section,"light_height");
	}

	//загрузить параметры idle подсветки
	m_bIdleLight = !!pSettings->r_bool (section, "idle_light");

	if(m_bIdleLight)
	{
		m_fIdleLightRange = pSettings->r_float(section,"idle_light_range");
		m_fIdleLightRangeDelta = pSettings->r_float(section,"idle_light_range_delta");
		LPCSTR light_anim = pSettings->r_string(section,"idle_light_anim");
		m_pIdleLAnim	 = LALib.FindItem(light_anim);
		m_fIdleLightHeight = pSettings->r_float(section,"idle_light_height");
	}


	//загрузить параметры для разбрасывания артефактов
	
//	if (Game().Type() == GAME_SINGLE)
		m_bSpawnBlowoutArtefacts = !!pSettings->r_bool(section,"spawn_blowout_artefacts");
//	else
//		m_bSpawnBlowoutArtefacts = false;
	
	
	if(m_bSpawnBlowoutArtefacts)
	{
		m_fArtefactSpawnProbability	= pSettings->r_float (section,"artefact_spawn_probability");
		if(pSettings->line_exist(section,"artefact_spawn_particles")) 
			m_sArtefactSpawnParticles = pSettings->r_string(section,"artefact_spawn_particles");
		else
			m_sArtefactSpawnParticles = NULL;

		if(pSettings->line_exist(section,"artefact_born_sound"))
		{
			sound_str = pSettings->r_string(section,"artefact_born_sound");
			m_ArtefactBornSound.create(TRUE, sound_str, st_SourceType);
		}

		m_fThrowOutPower = pSettings->r_float (section,			"throw_out_power");
		m_fArtefactSpawnHeight = pSettings->r_float (section,	"artefact_spawn_height");

		LPCSTR						l_caParameters = pSettings->r_string(section, "artefacts");
		u16 m_wItemCount			= (u16)_GetItemCount(l_caParameters);
		R_ASSERT2					(!(m_wItemCount & 1),"Invalid number of parameters in string 'artefacts' in the 'system.ltx'!");
		m_wItemCount			>>= 1;

		m_ArtefactSpawn.clear();
		string512 l_caBuffer;

		float total_probability = 0.f;

		m_ArtefactSpawn.resize(m_wItemCount);
		for (u16 i=0; i<m_wItemCount; ++i) 
		{
			ARTEFACT_SPAWN& artefact_spawn = m_ArtefactSpawn[i];
			artefact_spawn.section = _GetItem(l_caParameters,i << 1,l_caBuffer);
			artefact_spawn.probability = (float)atof(_GetItem(l_caParameters,(i << 1) | 1,l_caBuffer));
			total_probability += artefact_spawn.probability;
		}

		R_ASSERT2(!fis_zero(total_probability), "The probability of artefact spawn is zero!");
		//нормализировать вероятности
		for(i=0; i<m_ArtefactSpawn.size(); ++i)
		{
			m_ArtefactSpawn[i].probability = m_ArtefactSpawn[i].probability/total_probability;
		}
	}
}

BOOL CCustomZone::net_Spawn(LPVOID DC) 
{
	if (!inherited::net_Spawn(DC))
		return					(FALSE);

	CSE_Abstract				*e = (CSE_Abstract*)(DC);
	CSE_ALifeCustomZone			*Z = dynamic_cast<CSE_ALifeCustomZone*>(e);
	
	m_fMaxPower					= Z->m_maxPower;
	m_fAttenuation				= Z->m_attn;
	m_dwPeriod					= Z->m_period;

	//добавить источники света
	if (m_bIdleLight) {
		m_pIdleLight = ::Render->light_create();
		m_pIdleLight->set_shadow(true);
	}
	else
		m_pIdleLight = NULL;

	if (m_bBlowoutLight) {
		m_pLight = ::Render->light_create();
		m_pLight->set_shadow(true);
	}
	else
		m_pLight = NULL;

//	setVisible(true);
	shedule_register			();
	setEnabled					(true);

	PlayIdleParticles			();

	m_eZoneState				= eZoneStateIdle;
	m_iPreviousStateTime		= m_iStateTime = 0;

	m_effector.SetRadius		(CFORM()->getSphere().R);

	m_dwLastTimeMoved			= Device.dwTimeGlobal;
	m_vPrevPos.set				(Position());

	if (Game().Type() != GAME_SINGLE)
		OnStateSwitch			(eZoneStateDisabled);

	PrefetchArtefacts ();

	m_fRealWindPower = g_pGamePersistent->Environment.wind_strength;
	m_fDistanceToCurEntity = flt_max;
	m_bBlowoutWindActive = false;

	return						(TRUE);
}

void CCustomZone::net_Destroy() 
{
	inherited::net_Destroy();
	
	StopIdleParticles();
	StopWind();

	if(m_pLight)
	{
		m_pLight->set_active(false);
		::Render->light_destroy	(m_pLight);
		m_pLight = NULL;
	}

	if(m_pIdleLight)
	{
		m_pIdleLight->set_active(false);
		::Render->light_destroy	(m_pIdleLight);
		m_pIdleLight = NULL;
	}

	if (m_pIdleParticles)
	{
		m_pIdleParticles->PSI_destroy();
		m_pIdleParticles = NULL;
	}
}

bool CCustomZone::IdleState()
{
	return false;
}

bool CCustomZone::AwakingState()
{
	if(m_iStateTime>=m_StateTime[eZoneStateAwaking])
	{
		SwitchZoneState(eZoneStateBlowout);
		return true;
	}
	return false;
}

bool CCustomZone::BlowoutState()
{
	if(m_iStateTime>=m_StateTime[eZoneStateBlowout])
	{
		SwitchZoneState(eZoneStateAccumulate);
		return true;
	}
	return false;
}
bool CCustomZone::AccumulateState()
{
	if(m_iStateTime>=m_StateTime[eZoneStateAccumulate])
	{
		if(m_bZoneActive)
			SwitchZoneState(eZoneStateBlowout);
		else
			SwitchZoneState(eZoneStateIdle);

		return true;
	}
	return false;
}


void CCustomZone::UpdateCL() 
{
	inherited::UpdateCL();

	if (!IsEnabled()) 
	{
		if (EnableEffector())
			m_effector.Stop();
		return;
	};

	UpdateIdleLight		();

	m_iPreviousStateTime = m_iStateTime;
	m_iStateTime += (int)Device.dwTimeDelta;
	switch(m_eZoneState)
	{
	case eZoneStateIdle:
		IdleState();
		break;
	case eZoneStateAwaking:
		AwakingState();
		break;
	case eZoneStateBlowout:
		BlowoutState();
		break;
	case eZoneStateAccumulate:
		AccumulateState();
		break;
	case eZoneStateDisabled:
		break;
	default: NODEFAULT;
	}


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

	m_fDistanceToCurEntity = Level().CurrentEntity()->Position().distance_to(Position());

	if (EnableEffector())// && Level().CurrentEntity() && Level().CurrentEntity()->SUB_CLS_ID != CLSID_SPECTATOR)
		m_effector.Update(m_fDistanceToCurEntity);

	UpdateBlowoutLight	();
}

void CCustomZone::shedule_Update(u32 dt)
{
	m_bZoneActive			= false;

	const Fsphere& s		= CFORM()->getSphere();
	Fvector					P;
	XFORM().transform_tiny	(P,s.P);
	feel_touch_update		(P,s.R);

	if (IsEnabled())
	{
		//пройтись по всем объектам в зоне
		//и проверить их состояние
		for(OBJECT_INFO_MAP_IT it = m_ObjectInfoMap.begin(); 
			m_ObjectInfoMap.end() != it; ++it) 
		{
			CObject* pObject = (*it).first;
			CEntityAlive* pEntityAlive = dynamic_cast<CEntityAlive*>(pObject);
			SZoneObjectInfo& info = (*it).second;

			info.time_in_zone += dt;

			if((!info.small_object && m_iDisableHitTime != -1 && (int)info.time_in_zone > m_iDisableHitTime) ||
				(info.small_object && m_iDisableHitTimeSmall != -1 && (int)info.time_in_zone > m_iDisableHitTimeSmall))
			{
				if(!pEntityAlive || !pEntityAlive->g_Alive())
					info.zone_ignore = true;
			}
			if(m_iDisableIdleTime != -1 && (int)info.time_in_zone > m_iDisableIdleTime)
			{
				if(!pEntityAlive || !pEntityAlive->g_Alive())
					StopObjectIdleParticles(dynamic_cast<CPhysicsShellHolder*>(pObject));
			}

			//если есть хотя бы один не дисабленый объект, то
			//зона считается активной
			if(info.zone_ignore == false) 
				m_bZoneActive = true;
		}
	};

	//в зону попал объект, разбудить ее
	if(m_bZoneActive && eZoneStateIdle ==  m_eZoneState)
		SwitchZoneState(eZoneStateAwaking);
	
	inherited::shedule_Update(dt);
}

void CCustomZone::feel_touch_new(CObject* O) 
{
	if (dynamic_cast<CCustomZone*>(O)) return;
	
	if(bDebug) HUD().outMessage(0xffffffff,O->cName(),"entering a zone.");
	m_inZone.insert(O);
	if(dynamic_cast<CActor*>(O) && O == Level().CurrentEntity())
					m_pLocalActor = dynamic_cast<CActor*>(O);

	CGameObject*	pGameObject = dynamic_cast<CGameObject*>(O);
	CEntityAlive*	pEntityAlive = dynamic_cast<CEntityAlive*>(pGameObject);
	CArtefact*		pArtefact = dynamic_cast<CArtefact*>(pGameObject);
	
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
		(object_info.nonalive_object && m_bIgnoreNonAlive) || 
		(pArtefact && m_bIgnoreArtefacts))
		object_info.zone_ignore = true;
	else
		object_info.zone_ignore = false;

	m_ObjectInfoMap[O] = object_info;
	

	if (IsEnabled())
	{
		PlayEntranceParticles(pGameObject);
		PlayObjectIdleParticles(pGameObject);
	}
};

void CCustomZone::feel_touch_delete(CObject* O) 
{
	if(bDebug) HUD().outMessage(0xffffffff,O->cName(),"leaving a zone.");
	
	m_inZone.erase(O);
	if(dynamic_cast<CActor*>(O)) m_pLocalActor = NULL;
	CPhysicsShellHolder* pGameObject =dynamic_cast<CPhysicsShellHolder*>(O);
	if(!pGameObject->getDestroy())
	{
		StopObjectIdleParticles(pGameObject);
	}

	m_ObjectInfoMap.erase(O);
}

BOOL CCustomZone::feel_touch_contact(CObject* O) 
{
	if(O == this) return false;
	if(/*!O->Local() ||*/ !((CGameObject*)O)->IsVisibleForZones()) return false;
	
	return ((CCF_Shape*)CFORM())->Contact(O);
}

#define REL_POWER_COEFF 0.75f


float CCustomZone::RelativePower(float dist)
{
	float radius = Radius()*REL_POWER_COEFF;
	float power = radius < dist ? 0 : (1.f - m_fAttenuation*(dist/radius)*(dist/radius));
	return power < 0 ? 0 : power;
}

float CCustomZone::Power(float dist) 
{
	return  m_fMaxPower * RelativePower(dist);
}

void CCustomZone::PlayIdleParticles()
{
	m_idle_sound.play_at_pos(this, Position(), true);

	if(*m_sIdleParticles)
	{
		if (!m_pIdleParticles)
		{
			m_pIdleParticles = xr_new<CParticlesObject>(*m_sIdleParticles,Sector(),false);
			m_pIdleParticles->UpdateParent(XFORM(),zero_vel);
		}
		m_pIdleParticles->Play();
	}

	StartIdleLight	();
}

void CCustomZone::StopIdleParticles()
{
	m_idle_sound.stop();

	if(m_pIdleParticles)
	{
		m_pIdleParticles->Stop();
	}

	StopIdleLight();
}


void  CCustomZone::StartIdleLight	()
{
	if(m_pIdleLight)
	{
		m_pIdleLight->set_range(m_fIdleLightRange);
		Fvector pos = Position();
		pos.y += m_fIdleLightHeight;
		m_pIdleLight->set_position(pos);
		m_pIdleLight->set_active(true);
	}
}
void  CCustomZone::StopIdleLight	()
{
	if(m_pIdleLight)
		m_pIdleLight->set_active(false);
}
void CCustomZone::UpdateIdleLight	()
{
	if(!m_pIdleLight || !m_pIdleLight->get_active())
		return;


	VERIFY(m_pIdleLAnim);

	int frame = 0;
	u32 clr					= m_pIdleLAnim->CalculateBGR(Device.fTimeGlobal,frame); // возвращает в формате BGR
	Fcolor					fclr;
	fclr.set				((float)color_get_B(clr)/255.f,(float)color_get_G(clr)/255.f,(float)color_get_R(clr)/255.f,1.f);
	
	float range = m_fIdleLightRange + m_fIdleLightRangeDelta*::Random.randF(-1.f,1.f);
	m_pIdleLight->set_range	(range);
	m_pIdleLight->set_color	(fclr);
}


void CCustomZone::PlayBlowoutParticles()
{
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

	if(!particle_str) return;

	// play particles
	CParticlesPlayer* PP = dynamic_cast<CParticlesPlayer*>(pObject);
	if (PP){
		u16 play_bone = pObject->GetRandomBone(); 
		if (play_bone!=BI_NONE)
			PP->StartParticles	(*particle_str,play_bone,Fvector().set(0,1,0), ID());
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

	Fvector vel;
	CPhysicsShellHolder* shell_holder=dynamic_cast<CPhysicsShellHolder*>(pObject);
	if(shell_holder)
		shell_holder->PHGetLinearVell(vel);
	else 
		vel.set(0,0,0);
	
	//выбрать случайную косточку на объекте
	CParticlesPlayer* PP = dynamic_cast<CParticlesPlayer*>(pObject);
	if (PP){
		u16 play_bone = pObject->GetRandomBone(); 
		if (play_bone!=BI_NONE){
			CParticlesObject* pParticles = CParticlesObject::Create(*particle_str);
			Fmatrix xform;

			Fvector dir;
			if(fis_zero(vel.magnitude()))
				dir.set(0,1,0);
			else
			{
				dir.set(vel);
				dir.normalize();
			}

			PP->MakeXFORM			(pObject,play_bone,dir,Fvector().set(0,0,0),xform);
			pParticles->UpdateParent(xform, vel);
			if (IsEnabled())
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
	pObject->StartParticles(*particle_str, Fvector().set(0,1,0), ID());
	if (!IsEnabled())
		pObject->StopParticles	(particle_str);
}
void CCustomZone::StopObjectIdleParticles(CGameObject* pObject)
{
	OBJECT_INFO_MAP_IT it	= m_ObjectInfoMap.find(pObject);
	if(m_ObjectInfoMap.end() == it) return;
	
	
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

	//остановить партиклы
	pObject->StopParticles	(particle_str);
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
}

void CCustomZone::StartBlowoutLight		()
{
	if(!m_pLight || m_fLightTime<=0.f) return;
	
	m_fLightTimeLeft = m_fLightTime;

	m_pLight->set_color(m_LightColor.r, 
						m_LightColor.g, 
						m_LightColor.b);
	m_pLight->set_range(m_fLightRange);
	
	Fvector pos = Position();
	pos.y += m_fLightHeight;
	m_pLight->set_position(pos);
	m_pLight->set_active(true);

	//!!!
	//Msg("blowout light of zone %s started, range %3.2f", cNameSect(), m_fLightRange);
}

void  CCustomZone::StopBlowoutLight		()
{
	m_fLightTimeLeft = 0.f;
	m_pLight->set_active(false);
	//!!!
	//Msg("blowout light of zone %s stoped", cNameSect());
}

void CCustomZone::UpdateBlowoutLight	()
{
	if(!m_pLight || !m_pLight->get_active()) return;

	if(m_fLightTimeLeft>0)
	{
		m_fLightTimeLeft -= Device.fTimeDelta;

		float scale = m_fLightTimeLeft/m_fLightTime;
		m_pLight->set_color(m_LightColor.r*scale, 
							m_LightColor.g*scale, 
							m_LightColor.b*scale);
		m_pLight->set_range(m_fLightRange*scale);

		//!!!
		//Msg("blowout light of zone %s, set range %3.2f", cName(), m_fLightRange*scale);
	}
	else
		StopBlowoutLight ();
}

void CCustomZone::AffectObjects()
{
	if(m_dwAffectFrameNum == Device.dwFrame) return;

	m_dwAffectFrameNum = Device.dwFrame;

	xr_set<CObject*>::iterator it;
	for(it = m_inZone.begin(); m_inZone.end() != it; ++it) 
	{
		Affect(*it);
	}
}

void CCustomZone::UpdateBlowout()
{
	if(m_dwBlowoutParticlesTime>=(u32)m_iPreviousStateTime && 
		m_dwBlowoutParticlesTime<(u32)m_iStateTime)
		PlayBlowoutParticles();

	if(m_dwBlowoutLightTime>=(u32)m_iPreviousStateTime && 
		m_dwBlowoutLightTime<(u32)m_iStateTime)
		StartBlowoutLight ();

	if(m_dwBlowoutSoundTime>=(u32)m_iPreviousStateTime && 
		m_dwBlowoutSoundTime<(u32)m_iStateTime)
		m_blowout_sound.play_at_pos	(this, Position());

	if(m_bBlowoutWindEnable && m_dwBlowoutWindTimeStart>=(u32)m_iPreviousStateTime && 
		m_dwBlowoutWindTimeStart<(u32)m_iStateTime)
		StartWind();

	UpdateWind();


	if(m_dwBlowoutExplosionTime>=(u32)m_iPreviousStateTime && 
		m_dwBlowoutExplosionTime<(u32)m_iStateTime)
	{
		AffectObjects();

		//заспавнить артефакт
		BornArtefact();
	}
}

void  CCustomZone::OnMove	()
{
	if(m_dwLastTimeMoved == 0)
	{
		m_dwLastTimeMoved = Device.dwTimeGlobal;
		m_vPrevPos.set(Position());
	}
	else
	{
		float time_delta = float(Device.dwTimeGlobal - m_dwLastTimeMoved)/1000.f;
		m_dwLastTimeMoved = Device.dwTimeGlobal;
		//скорость движения
		
		Fvector vel;
			
		if(fis_zero(time_delta))
			vel = zero_vel;
		else
		{
			vel.sub(Position(), m_vPrevPos);
			vel.div(time_delta);
		}

		if (m_pIdleParticles)
			m_pIdleParticles->UpdateParent(XFORM(), vel);

		if(m_pLight && m_pLight->get_active())
			m_pLight->set_position(Position());

		if(m_pIdleLight && m_pIdleLight->get_active())
			m_pIdleLight->set_position(Position());
     }
}

void	CCustomZone::OnEvent (NET_Packet& P, u16 type)
{	
	switch (type)
	{
		case GE_ZONE_STATE_CHANGE:
			{
				u8				S;
				P.r_u8			(S);

				switch (S)
				{
				case eZoneStateIdle:
					if(bDebug) HUD().outMessage(0xffffffff,cName(),"zone switch to State Idle");
					break;																
				case eZoneStateAwaking:													
					if(bDebug) HUD().outMessage(0xffffffff,cName(),"zone switch to State Awaking");
					break;																
				case eZoneStateBlowout:													
					if(bDebug) HUD().outMessage(0xffffffff,cName(),"zone switch to State Blowout");
					break;																
				case eZoneStateAccumulate:												
					if(bDebug) HUD().outMessage(0xffffffff,cName(),"zone switch to State Accumulate");
					break;
				case eZoneStateDisabled:												
					if(bDebug) HUD().outMessage(0xffffffff,cName(),"zone switch to State Disabled");
					break;
				};
				OnStateSwitch(EZoneState(S));
				break;
			}
		case GE_OWNERSHIP_TAKE : 
			{
				u16 id;
                P.r_u16(id);
				CArtefact *artefact = dynamic_cast<CArtefact*>(Level().Objects.net_Find(id));  VERIFY(artefact);
				artefact->H_SetParent(this);
				AddArtefact(artefact);
				break;
			} 
         case GE_OWNERSHIP_REJECT : 
			 {
				 u16 id;
                 P.r_u16			(id);
                 CArtefact *artefact = dynamic_cast<CArtefact*>(Level().Objects.net_Find(id)); VERIFY(artefact);
                 artefact->H_SetParent(NULL);
				 ThrowOutArtefact(artefact);
                 break;
			}
	}
	inherited::OnEvent(P, type);
};

void CCustomZone::OnStateSwitch	(EZoneState new_state)
{
	if (eZoneStateDisabled == new_state)
		Disable();
	else
		Enable();

	m_eZoneState = new_state;
	m_iPreviousStateTime = m_iStateTime = 0;
};

void CCustomZone::SwitchZoneState(EZoneState new_state)
{
	if (OnServer())
	{
		// !!! Just single entry for given state !!!
		NET_Packet		P;
		u_EventGen		(P,GE_ZONE_STATE_CHANGE,ID());
		P.w_u8			(u8(new_state));
		u_EventSend		(P);
	};
}

bool CCustomZone::Enable()
{
	if (IsEnabled()) return false;

	PlayIdleParticles();

	for(OBJECT_INFO_MAP_IT it = m_ObjectInfoMap.begin(); 
		m_ObjectInfoMap.end() != it; ++it) 
	{
		CObject* pObject = (*it).first;
		CPhysicsShellHolder* pGameObject =dynamic_cast<CPhysicsShellHolder*>(pObject);

		PlayEntranceParticles(pGameObject);
		PlayObjectIdleParticles(pGameObject);
	}
	return true;
};

bool CCustomZone::Disable()
{
	if (!IsEnabled()) return false;

	StopIdleParticles();

	for(OBJECT_INFO_MAP_IT it = m_ObjectInfoMap.begin(); 
		m_ObjectInfoMap.end() != it; ++it) 
	{
		CObject* pObject = (*it).first;
		CPhysicsShellHolder* pGameObject =dynamic_cast<CPhysicsShellHolder*>(pObject);
		StopObjectIdleParticles(pGameObject);
	}
	return false;
};

void CCustomZone::ZoneEnable()
{
//	if (IsEnabled()) return;
	SwitchZoneState(eZoneStateIdle);
};

void CCustomZone::ZoneDisable()
{
//	if (!IsEnabled()) return;
	SwitchZoneState(eZoneStateDisabled);
};


void CCustomZone::SpawnArtefact()
{
	//вычислить согласно распределению вероятностей
	//какой артефакт из списка ставить
	float rnd = ::Random.randF(.0f,1.f);
	float prob_threshold = 0.f;
	
	std::size_t i=0;
	for(; i<m_ArtefactSpawn.size(); i++)
	{
		prob_threshold += m_ArtefactSpawn[i].probability;
		if(rnd<prob_threshold) break;
	}
	R_ASSERT(i<m_ArtefactSpawn.size());

	Fvector pos;
	Center(pos);
	Level().spawn_item(*m_ArtefactSpawn[i].section, pos, level_vertex_id(), ID());
}

void CCustomZone::AddArtefact(CArtefact* pArtefact)
{
	m_SpawnedArtefacts.push_back(pArtefact);
}

void CCustomZone::BornArtefact()
{
	if(!m_bSpawnBlowoutArtefacts || m_SpawnedArtefacts.empty()) return;

	if(::Random.randF(0.f, 1.f)> m_fArtefactSpawnProbability) return;

	CArtefact* pArtefact = m_SpawnedArtefacts.back(); VERIFY(pArtefact);
	m_SpawnedArtefacts.pop_back();

	if (Local()) {
		NET_Packet						P;
		u_EventGen						(P,GE_OWNERSHIP_REJECT,ID());
		P.w_u16							(pArtefact->ID());
		u_EventSend						(P);
	}

	PrefetchArtefacts ();
}

void CCustomZone::ThrowOutArtefact(CArtefact* pArtefact)
{
	pArtefact->XFORM().c.set(Position());
	pArtefact->XFORM().c.y += m_fArtefactSpawnHeight;

	if(*m_sArtefactSpawnParticles)
	{
		CParticlesObject* pParticles;
		pParticles = xr_new<CParticlesObject>(*m_sArtefactSpawnParticles,Sector());
		pParticles->UpdateParent(pArtefact->XFORM(),zero_vel);
		pParticles->Play();
	}

	m_ArtefactBornSound.play_at_pos(pArtefact, pArtefact->Position());

	Fvector dir;
	dir.random_dir();
	pArtefact->m_pPhysicsShell->applyImpulse (dir, m_fThrowOutPower);
}

void CCustomZone::PrefetchArtefacts()
{
	if (!m_bSpawnBlowoutArtefacts || m_ArtefactSpawn.empty()) return;

	for(std::size_t i = m_SpawnedArtefacts.size(); i < PREFETCHED_ARTEFACTS_NUM; i++)
	{
		SpawnArtefact();
	}
}

void CCustomZone::StartWind()
{
	if(m_fDistanceToCurEntity>WIND_RADIUS) return;

	m_bBlowoutWindActive = true;
	m_fRealWindPower = g_pGamePersistent->Environment.wind_strength;
	clamp(g_pGamePersistent->Environment.wind_strength, 0.f, 1.f);
}

void CCustomZone::StopWind()
{
	if(!m_bBlowoutWindActive) return;
	m_bBlowoutWindActive = false;
	g_pGamePersistent->Environment.wind_strength = m_fRealWindPower;
}

void CCustomZone::UpdateWind()
{
	if(!m_bBlowoutWindActive) return;

	if(m_fDistanceToCurEntity>WIND_RADIUS || m_dwBlowoutWindTimeEnd<(u32)m_iStateTime)
	{
		StopWind();
		return;
	}

	if(m_dwBlowoutWindTimePeak > (u32)m_iStateTime)
	{
		g_pGamePersistent->Environment.wind_strength = m_fBlowoutWindPowerMax + ( m_fRealWindPower - m_fBlowoutWindPowerMax)*
								float(m_dwBlowoutWindTimePeak - (u32)m_iStateTime)/
								float(m_dwBlowoutWindTimePeak - m_dwBlowoutWindTimeStart);
		clamp(g_pGamePersistent->Environment.wind_strength, 0.f, 1.f);
	}
	else
	{
		g_pGamePersistent->Environment.wind_strength = m_fBlowoutWindPowerMax + (m_fRealWindPower - m_fBlowoutWindPowerMax)*
			float((u32)m_iStateTime - m_dwBlowoutWindTimePeak)/
			float(m_dwBlowoutWindTimeEnd - m_dwBlowoutWindTimePeak);
		clamp(g_pGamePersistent->Environment.wind_strength, 0.f, 1.f);
	}
}