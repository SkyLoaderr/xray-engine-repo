#include "stdafx.h"
#include "customdetector.h"
#include "hudmanager.h"
#include "artifact.h"

CCustomDetector::CCustomDetector(void) 
{
}

CCustomDetector::~CCustomDetector(void) 
{
}

BOOL CCustomDetector::net_Spawn(LPVOID DC) 
{
	return		(inherited::net_Spawn(DC));
}

void CCustomDetector::Load(LPCSTR section) 
{
	// verify class
	LPCSTR Class = pSettings->r_string(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	inherited::Load(section);

	m_fRadius = pSettings->r_float(section,"radius");
	m_fBuzzerRadius = pSettings->r_float(section,"buzzer_radius");

	LPCSTR sound_name = pSettings->r_string(section,"noise");
	
	SoundCreate(m_noise, sound_name);
	sound_name = pSettings->r_string(section,"buzzer");
	SoundCreate(m_buzzer, sound_name);
	u32 i = 1;
	string256 temp;

	//��������� ����� ��� ����������� ��������� ����� ���
	do 
	{
		sprintf(temp, "zone_class_%d", i);
		if(pSettings->line_exist(section,temp))
		{
			LPCSTR z_Class = pSettings->r_string(section,temp);
			CLASS_ID zone_cls = TEXT2CLSID(pSettings->r_string(z_Class,"class"));
			

			ZONE_TYPE zone_type;
			sprintf(temp, "zone_min_freq_%d", i);
			zone_type.min_freq = pSettings->r_float(section,temp);
			sprintf(temp, "zone_max_freq_%d", i);
			zone_type.max_freq = pSettings->r_float(section,temp);
			R_ASSERT(zone_type.min_freq<zone_type.max_freq);

			sprintf(temp, "zone_sound_%d", i);
			sound_name = pSettings->r_string(section,temp);
			ref_sound *pSound = xr_new<ref_sound>();
			SoundCreate(*pSound, sound_name);
			zone_type.detect_snd = pSound;
			m_ZoneTypeMap[zone_cls] = zone_type;

			++i;
		}
		else break;
	} while(true);
}

void CCustomDetector::net_Destroy() 
{
	inherited::net_Destroy();
	SoundDestroy(m_noise);
	
	ZONE_TYPE_MAP_IT it;
	for(it = m_ZoneTypeMap.begin(); m_ZoneTypeMap.end() != it; ++it) 
		SoundDestroy(*it->second.detect_snd);
}

void CCustomDetector::shedule_Update(u32 dt) 
{
	inherited::shedule_Update	(dt);

	if(!H_Parent()) return;

	Position().set(H_Parent()->Position());

	Fvector					P; 
	P.set					(H_Parent()->Position());
	feel_touch_update		(P,m_fRadius);

//	float fMaxPow = 0;
//	BOOL l_buzzer = false;
	ZONE_INFO_MAP_IT it;
		
	///////////////////////////////////
	//����� ����������� ���������� ���
	///////////////////////////////////
	for(it = m_ZoneInfoMap.begin(); m_ZoneInfoMap.end() != it; ++it) 
	{
		CCustomZone *pZone = it->first;
		ZONE_INFO& zone_info = it->second;

		//����� ��� ��� �� ��������������
		if(m_ZoneTypeMap.find(pZone->SUB_CLS_ID) == m_ZoneTypeMap.end())
			return;
		
		ZONE_TYPE& zone_type = m_ZoneTypeMap[pZone->SUB_CLS_ID];
		
		
		float dist_to_zone = P.distance_to(pZone->Position()); 
		//float fRelPow = pZone->Power(dist_to_zone) / pZone->GetMaxPower();
		float fRelPow = 1.f - dist_to_zone / m_fRadius;
		clamp(fRelPow, 0.f, 1.f);

//		if(fRelPow > 0 && pZone->feel_touch_contact(this)) 
//			l_buzzer = true;
		//fMaxPow = _max(fMaxPow, fRelPow);
		
		//���������� ������ �� ����� ������ ��������� ���� ������ ���������
		float cur_freq = zone_type.min_freq + 
			(zone_type.max_freq - zone_type.min_freq) * fRelPow* fRelPow* fRelPow;
		
		float current_snd_time = 1000.f*1.f/cur_freq;

		//float current_snd_time = 50.f + 3000.f * fRelPow* fRelPow;
		
		if((float)zone_info.snd_time > current_snd_time)
		{
			zone_info.snd_time = 0;
			Sound->play_at_pos(*zone_type.detect_snd, this, P);
		} 
		else 
			zone_info.snd_time += dt;
	}
	
/*	if(l_buzzer) 
	{
		if(!m_buzzer.feedback) m_buzzer.play_at_pos(this, P, true);
		if(m_buzzer.feedback) m_buzzer.set_position(P);
	} 
	else 
	{
		m_buzzer.stop();
	}

	
	if(fMaxPow > 0) 
	{
		if(!m_noise.feedback) Sound->play_at_pos(m_noise, this, P, true);
		if(m_noise.feedback) 
		{
			//l_maxPow = _max(logf(l_maxPow) / 10.f + 1.f, .0f);
			m_noise.set_volume	(fMaxPow);
			m_noise.set_position(P);
		}
	} 
	else if(m_noise.feedback) 
		m_noise.stop();
*/	
	//////////////////////////////////
	//����� ����������� ����������
	//////////////////////////////////
/*	
	xr_set<CArtifact*>::iterator l_it2;
	for(l_it2 = CArtifact::m_all.begin(); CArtifact::m_all.end() != l_it2; ++l_it2) 
	{
		CArtifact &l_af = **l_it2;
		float l_dst = P.distance_to(l_af.Position());
		if(!l_af.H_Parent() && l_dst < l_af.m_detectorDist) 
		{
			if(!l_af.m_detectorSound.feedback)
				Sound->play_at_pos(l_af.m_detectorSound, this, P, true);
			if(l_af.m_detectorSound.feedback) 
			{
				//l_af.m_detectorSound.set_volume(_max(logf((l_af.m_detectorDist-l_dst)/l_af.m_detectorDist) / 10.f + 1.f, .0f));
				l_af.m_detectorSound.set_volume((l_af.m_detectorDist-l_dst)/l_af.m_detectorDist);
				l_af.m_detectorSound.set_position(P);
			}
		} else if(l_af.m_detectorSound.feedback) l_af.m_detectorSound.stop();
	}
*/
}

void CCustomDetector::UpdateCL() 
{
	inherited::UpdateCL();
}

void CCustomDetector::feel_touch_new(CObject* O) 
{
	CCustomZone *pZone = dynamic_cast<CCustomZone*>(O);
	if(pZone) 
	{
		if(bDebug) HUD().outMessage(0xffffffff,cName(),"started to feel a zone.");
		m_ZoneInfoMap[pZone].snd_time = 0;
	}
}

void CCustomDetector::feel_touch_delete(CObject* O)
{
	CCustomZone *pZone = dynamic_cast<CCustomZone*>(O);
	if(pZone)
	{
		if(bDebug) HUD().outMessage(0xffffffff,cName(),"stoped to feel a zone.");
		m_ZoneInfoMap.erase(pZone);
	}
}

BOOL CCustomDetector::feel_touch_contact(CObject* O) 
{
	return (NULL != dynamic_cast<CCustomZone*>(O));
}

void CCustomDetector::SoundCreate(ref_sound& dest, LPCSTR s_name, int iType, BOOL /**bCtrlFreq/**/) 
{
	string256 temp;
	if (FS.exist(temp,"$game_sounds$",s_name,".ogg")) 
	{
		Sound->create(dest,TRUE,s_name,iType);
		return;
	}
	Debug.fatal	("Can't find ref_sound '%s'",s_name,*cName());
}

void CCustomDetector::SoundDestroy(ref_sound& dest) 
{
	::Sound->destroy			(dest);
}

void CCustomDetector::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
}

void CCustomDetector::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	
	//���������
	m_buzzer.stop				();
	m_noise.stop				();
}

#ifdef DEBUG
void CCustomDetector::OnRender() 
{
	if(!bDebug) return;
	RCache.OnFrameEnd();
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
