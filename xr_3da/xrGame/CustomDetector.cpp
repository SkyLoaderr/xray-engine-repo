#include "stdafx.h"
#include "customdetector.h"
#include "hudmanager.h"
#include "artifact.h"
#include "inventory.h"
#include "level.h"
#include "map_manager.h"
#include "night_vision_effector.h"


ZONE_INFO::ZONE_INFO	()
{
	pParticle=NULL;
}

ZONE_INFO::~ZONE_INFO	()
{
	if(pParticle)
		pParticle->PSI_destroy();
}

CCustomDetector::CCustomDetector(void) 
{
	m_bWorking = false;
	m_belt = true;
}

CCustomDetector::~CCustomDetector(void) 
{
	m_noise.destroy	();
	m_buzzer.destroy();

	ZONE_TYPE_MAP_IT it;
	for(it = m_ZoneTypeMap.begin(); m_ZoneTypeMap.end() != it; ++it)
		it->second.detect_snd.destroy();

	m_ZoneInfoMap.clear();
}

BOOL CCustomDetector::net_Spawn(CSE_Abstract* DC) 
{
	m_pCurrentActor		 = NULL;
	m_pCurrentInvOwner	 = NULL;

	return		(inherited::net_Spawn(DC));
}

void CCustomDetector::Load(LPCSTR section) 
{
	// verify class
	LPCSTR Class = pSettings->r_string(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==CLS_ID);

	inherited::Load(section);

	m_fRadius				= pSettings->r_float(section,"radius");
	m_fBuzzerRadius			= pSettings->r_float(section,"buzzer_radius");

	m_noise.create			(TRUE,pSettings->r_string(section,"noise"));
	m_buzzer.create			(TRUE,pSettings->r_string(section,"buzzer"));
	
	if( pSettings->line_exist(section,"night_vision_particle") )
		m_nightvision_particle	= pSettings->r_string(section,"night_vision_particle");

	u32 i = 1;
	string256 temp;

	//загрузить звуки для обозначения различных типов зон
	do 
	{
		sprintf			(temp, "zone_class_%d", i);
		if(pSettings->line_exist(section,temp))
		{
			LPCSTR z_Class			= pSettings->r_string(section,temp);
			CLASS_ID zone_cls		= TEXT2CLSID(pSettings->r_string(z_Class,"class"));

			m_ZoneTypeMap.insert	(std::make_pair(zone_cls,ZONE_TYPE()));
			ZONE_TYPE& zone_type	= m_ZoneTypeMap[zone_cls];
			sprintf					(temp, "zone_min_freq_%d", i);
			zone_type.min_freq		= pSettings->r_float(section,temp);
			sprintf					(temp, "zone_max_freq_%d", i);
			zone_type.max_freq		= pSettings->r_float(section,temp);
			R_ASSERT				(zone_type.min_freq<zone_type.max_freq);
			sprintf					(temp, "zone_sound_%d", i);
			zone_type.detect_snd.create(TRUE,pSettings->r_string(section,temp));

			sprintf					(temp, "zone_map_location_%d", i);
			
			if( pSettings->line_exist(section,temp) )
				zone_type.zone_map_location = pSettings->r_string(section,temp);

			++i;
		}
		else break;
	} while(true);

	m_ef_detector_type	= pSettings->r_u32(section,"ef_detector_type");
}

void CCustomDetector::net_Destroy() 
{
	inherited::net_Destroy();
}

void CCustomDetector::shedule_Update(u32 dt) 
{
	inherited::shedule_Update	(dt);
	
	if( !IsWorking() ) return;
	if( !H_Parent()  ) return;

	Position().set(H_Parent()->Position());

	Fvector					P; 
	P.set					(H_Parent()->Position());
	feel_touch_update		(P,m_fRadius);

	UpdateNightVisionMode();
}

void CCustomDetector::StopAllSounds()
{
	ZONE_TYPE_MAP_IT it;
	for(it = m_ZoneTypeMap.begin(); m_ZoneTypeMap.end() != it; ++it) 
	{
		ZONE_TYPE& zone_type = (*it).second;
		zone_type.detect_snd.stop();
	}

	//выключить
	m_buzzer.stop				();
	m_noise.stop				();
}

void CCustomDetector::UpdateCL() 
{
	inherited::UpdateCL();

	if( !IsWorking() ) return;
	if( !H_Parent()  ) return;

	//если у актера, то только на поясе
	if(!m_pCurrentActor) return;
	if( !m_pCurrentActor->m_inventory->Get(ID(),false))	return;

	bool sound_2d = m_pCurrentActor && m_pCurrentActor->HUDview();


	///////////////////////////////////
	//звуки обнаружения аномальных зон
	///////////////////////////////////

	ZONE_INFO_MAP_IT it;
	for(it = m_ZoneInfoMap.begin(); m_ZoneInfoMap.end() != it; ++it) 
	{
		CCustomZone *pZone = it->first;
		ZONE_INFO& zone_info = it->second;

		
		//такой тип зон не обнаруживается
		if(m_ZoneTypeMap.find(pZone->CLS_ID) == m_ZoneTypeMap.end() ||
			!pZone->VisibleByDetector())
			continue;

		ZONE_TYPE& zone_type = m_ZoneTypeMap[pZone->CLS_ID];

		float dist_to_zone = H_Parent()->Position().distance_to(pZone->Position()) - 0.8f*pZone->Radius();
		if(dist_to_zone<0) dist_to_zone = 0;
		
		float fRelPow = 1.f - dist_to_zone / m_fRadius;
		clamp(fRelPow, 0.f, 1.f);

		//определить текущую частоту срабатывания сигнала
		zone_info.cur_freq = zone_type.min_freq + 
			(zone_type.max_freq - zone_type.min_freq) * fRelPow* fRelPow* fRelPow* fRelPow;

		float current_snd_time = 1000.f*1.f/zone_info.cur_freq;
			
		if((float)zone_info.snd_time > current_snd_time)
		{
			Fvector				C;
			H_Parent()->Center	(C);
			zone_info.snd_time	= 0;
			zone_type.detect_snd.play_at_pos(this,C,sound_2d?sm_2D:0);
		} 
		else 
			zone_info.snd_time += Device.dwTimeDelta;
	}
}

void CCustomDetector::feel_touch_new(CObject* O) 
{
	CCustomZone *pZone = smart_cast<CCustomZone*>(O);
	if(pZone && pZone->IsEnabled()) 
	{
		if(bDebug) HUD().outMessage(0xffffffff,cName(),"started to feel a zone.");
		m_ZoneInfoMap[pZone].snd_time = 0;
		m_pCurrentInvOwner->FoundZone(pZone);
		
		AddRemoveMapSpot(pZone,true);
	}
}

void CCustomDetector::feel_touch_delete(CObject* O)
{
	CCustomZone *pZone = smart_cast<CCustomZone*>(O);
	if(pZone)
	{
		if(bDebug) HUD().outMessage(0xffffffff,cName(),"stoped to feel a zone.");
		m_ZoneInfoMap.erase(pZone);
		m_pCurrentInvOwner->LostZone(pZone);
		AddRemoveMapSpot(pZone,false);
	}
}

BOOL CCustomDetector::feel_touch_contact(CObject* O) 
{
	return (NULL != smart_cast<CCustomZone*>(O));
}

void CCustomDetector::OnH_A_Chield() 
{
	m_pCurrentActor = smart_cast<CActor*>(H_Parent());
	m_pCurrentInvOwner = smart_cast<CInventoryOwner*>(H_Parent());
	inherited::OnH_A_Chield		();
}

void CCustomDetector::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	
	m_pCurrentActor = NULL;
	m_pCurrentInvOwner = NULL;

	StopAllSounds();

	m_ZoneInfoMap.clear();
	Feel::Touch::feel_touch.clear();
}

#ifdef DEBUG
extern	Flags32	dbg_net_Draw_Flags;
void CCustomDetector::OnRender() 
{
	if(!bDebug) return;
	RCache.OnFrameEnd();
	if (!(dbg_net_Draw_Flags.is_any((1<<6)))) return;
	Fmatrix l_ball;
	l_ball.scale(m_fBuzzerRadius, m_fBuzzerRadius, m_fBuzzerRadius);
	Fvector l_p;
	l_p.set(0, 1.f, 0);
	XFORM().transform(l_p, l_p);
	l_ball.translate_add(l_p);
	RCache.dbg_DrawEllipse(l_ball, D3DCOLOR_XRGB(255,0,255));
}
#endif

void CCustomDetector::renderable_Render()
{
	inherited::renderable_Render();
}

u32	CCustomDetector::ef_detector_type	() const
{
	return	(m_ef_detector_type);
}

void CCustomDetector::OnMoveToBelt()
{
	inherited::OnMoveToBelt();
	TurnOn();
}

void CCustomDetector::OnMoveToRuck()
{
	inherited::OnMoveToRuck();
	TurnOff();
}
void CCustomDetector::TurnOn()
{
	m_bWorking = true;
	UpdateMapLocations();
	UpdateNightVisionMode();
}

void CCustomDetector::TurnOff() 
{
	m_bWorking = false;
	UpdateMapLocations();
	UpdateNightVisionMode();
}

void CCustomDetector::AddRemoveMapSpot(CCustomZone* pZone, bool bAdd)
{
	if(m_ZoneTypeMap.find(pZone->CLS_ID) == m_ZoneTypeMap.end() )return;
	
	if ( bAdd && !pZone->VisibleByDetector() ) return;
		

	ZONE_TYPE& zone_type = m_ZoneTypeMap[pZone->CLS_ID];
	if( xr_strlen(zone_type.zone_map_location) ){
		if( bAdd )
			Level().MapManager().AddMapLocation(*zone_type.zone_map_location,pZone->ID());
		else
			Level().MapManager().RemoveMapLocation(*zone_type.zone_map_location,pZone->ID());
	}
}

void CCustomDetector::UpdateMapLocations() // called on turn on/off only
{
	ZONE_INFO_MAP_IT it;
	for(it = m_ZoneInfoMap.begin(); it != m_ZoneInfoMap.end(); ++it)
		AddRemoveMapSpot(it->first,IsWorking());
}

void CCustomDetector::UpdateNightVisionMode()
{
	bool bNightVision = ( Level().Cameras.GetEffector(EEffectorPPType(NIGHT_VISION_EFFECTOR_TYPE_ID))!=NULL );
	bool bOn = bNightVision && m_pCurrentActor && IsWorking() && xr_strlen(*m_nightvision_particle)!=0;

	ZONE_INFO_MAP_IT it;
	for(it = m_ZoneInfoMap.begin(); m_ZoneInfoMap.end() != it; ++it) 
	{
		CCustomZone *pZone = it->first;
		ZONE_INFO& zone_info = it->second;

		if(bOn){
			Fvector zero_vector;
			zero_vector.set(0.f,0.f,0.f);

			if(!zone_info.pParticle)
				zone_info.pParticle = xr_new<CParticlesObject>(*m_nightvision_particle,false);
			
			zone_info.pParticle->UpdateParent(pZone->XFORM(),zero_vector);
			if(!zone_info.pParticle->IsPlaying())
				zone_info.pParticle->Play();
		}else{
			if(zone_info.pParticle){
				zone_info.pParticle->Stop			();
				zone_info.pParticle->PSI_destroy	();
				zone_info.pParticle					= NULL;
			}
		}
	}
}
