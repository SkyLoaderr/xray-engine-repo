#include "stdafx.h"
#include "customdetector.h"
#include "hudmanager.h"

CCustomDetector::CCustomDetector(void) {
//	m_belt = true;
}

CCustomDetector::~CCustomDetector(void) {
}

BOOL CCustomDetector::net_Spawn(LPVOID DC) {
	BOOL res = inherited::net_Spawn(DC);

	setEnabled(true);

	return res;
}

void CCustomDetector::Load(LPCSTR section) {
	// verify class
	LPCSTR Class = pSettings->r_string(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	inherited::Load(section);

	m_radius = pSettings->r_float(section,"radius");
	m_buzzer_radius = pSettings->r_float(section,"buzzer_radius");
	CCF_Shape*	shape			= xr_new<CCF_Shape>	(this);
	cfModel						= shape;
	Fsphere S;	S.P.set			(0,1.f,0); S.R = m_buzzer_radius;
	shape->add_sphere			(S);
	shape->ComputeBounds						();
	pCreator->ObjectSpace.Object_Register		(this);
	cfModel->OnMove								();

	LPCSTR l_soundName = pSettings->r_string(section,"noise");
	SoundCreate(m_noise, l_soundName);
	l_soundName = pSettings->r_string(section,"buzzer");
	SoundCreate(m_buzzer, l_soundName);
	u32 i = 1;
	string256 temp;
	do {
		sprintf(temp, "zone_class_%d", i);
		if(pSettings->line_exist(section,temp)) {
			LPCSTR z_Class = pSettings->r_string(section,temp);
			CLASS_ID zone_cls = TEXT2CLSID(pSettings->r_string(z_Class,"class"));
			sprintf(temp, "zone_sound_%d", i);
			l_soundName = pSettings->r_string(section,temp);
			sound *l_pSound = xr_new<sound>();
			SoundCreate(*l_pSound, l_soundName);
			m_sounds[zone_cls] = l_pSound;
			i++;
		} else break;
	} while(true);

}

void CCustomDetector::net_Destroy() {
	inherited::net_Destroy();
	SoundDestroy(m_noise);
	map<CLASS_ID, sound*>::iterator l_it;
	for(l_it = m_sounds.begin(); l_it != m_sounds.end(); l_it++) SoundDestroy(*l_it->second);
}

void CCustomDetector::Update(u32 dt) {
	inherited::Update	(dt);

	if(!H_Parent()) return;

	vPosition.set(H_Parent()->Position());
	UpdateTransform();

	//const Fsphere& s		= cfModel->getSphere();
	Fvector					P; P.set(H_Parent()->Position());
	//H_Parent()->clXFORM().transform_tiny(P,P);
	feel_touch_update		(P,m_radius);

	if(H_Parent()) {
		f32 l_maxPow = 0;
		BOOL l_buzzer = false;
		list<CCustomZone*>::iterator l_it;
		for(l_it = m_zones.begin(); l_it != m_zones.end(); l_it++) {
			CCustomZone *l_pZ = *l_it;
			u32 &l_time = m_times[l_pZ];
			//Fvector ZP; ZP.set(0,0,0); l_pZ->clXFORM().transform_tiny(ZP,ZP);
			f32 l_dst = P.distance_to(l_pZ->Position()); if(l_dst > m_radius) l_dst -= m_radius; else l_dst = 0;
			f32 l_relPow = l_pZ->Power(l_dst) / l_pZ->m_maxPower;
			if(l_relPow > 0 && l_pZ->feel_touch_contact(this)) l_buzzer = true;
			l_maxPow = max(l_maxPow, l_relPow); l_relPow = 1.f - l_relPow;
			if((f32)l_time > 5000.f * (l_relPow/**l_relPow*l_relPow*l_relPow*/)) {
				l_time = 0;
				if(m_sounds.find(l_pZ->SUB_CLS_ID) != m_sounds.end()) {
					sound *l_pSound = m_sounds[l_pZ->SUB_CLS_ID];
					Sound->PlayAtPos(*l_pSound, this, P);
				}
			} else l_time += dt;
		}
		if(l_buzzer) {
			if(!m_buzzer.feedback) Sound->PlayAtPos(m_buzzer, this, P, true);
			if(m_buzzer.feedback) m_buzzer.feedback->SetPosition(P);
		} else if(m_buzzer.feedback) m_buzzer.feedback->Stop();
		if(l_maxPow > 0) {
			if(!m_noise.feedback) Sound->PlayAtPos(m_noise, this, P, true);
			if(m_noise.feedback) {
				l_maxPow = _max(logf(l_maxPow) / 10.f + 1.f, .0f);
				m_noise.feedback->SetVolume(l_maxPow);

				m_noise.feedback->SetPosition(P);
			}
		} else if(m_noise.feedback) m_noise.feedback->Stop();
		
	}
}

void CCustomDetector::UpdateCL() {
	inherited::UpdateCL();
	f32 l_zonePow = 0;
	list<CCustomZone*>::iterator l_it;
	for(l_it = m_zones.begin(); l_it != m_zones.end(); l_it++) l_zonePow = _max(l_zonePow, (*l_it)->Power((*l_it)->Position().distance_to(vPosition)));
	/*
	CGameFont* H		= HUD().pFontMedium;
	H->SetColor			(0xf0ffffff); 
	H->Out				(550,500,"Anomaly force: %.0f", l_zonePow);
	*/
}

void CCustomDetector::feel_touch_new(CObject* O) {
	CCustomZone *l_pZ = dynamic_cast<CCustomZone*>(O);
	if(l_pZ) {
		if(bDebug) HUD().outMessage(0xffffffff,cName(),"started to feel a zone.");
		m_zones.push_back(l_pZ);
		m_times[l_pZ] = 0;
	}
}

void CCustomDetector::feel_touch_delete(CObject* O) {
	CCustomZone *l_pZ = dynamic_cast<CCustomZone*>(O);
	if(l_pZ) {
		if(bDebug) HUD().outMessage(0xffffffff,cName(),"stoped to feel a zone.");
		m_zones.erase(find(m_zones.begin(), m_zones.end(), l_pZ));
	}
}

BOOL CCustomDetector::feel_touch_contact(CObject* O) {
	return dynamic_cast<CCustomZone*>(O) != NULL;
}

void CCustomDetector::SoundCreate(sound& dest, LPCSTR s_name, int iType, BOOL bCtrlFreq) {
	string256 temp;
	if (FS.exist(temp,Path.Sounds,s_name)) {
		Sound->Create(dest,TRUE,s_name,bCtrlFreq,iType);
		return;
	}
	Debug.fatal	("Can't find sound '%s'",s_name,cName());
}

void CCustomDetector::SoundDestroy(sound& dest) {
	Sound->Delete			(dest);
}

void CCustomDetector::OnH_A_Chield() {
	inherited::OnH_A_Chield		();
//	setVisible					(false);
	setEnabled					(true);
}

void CCustomDetector::OnH_B_Independent() {
	inherited::OnH_B_Independent();
//	setVisible					(true);
	setEnabled					(false);
	if(m_buzzer.feedback) m_buzzer.feedback->Stop();
	if(m_noise.feedback) m_noise.feedback->Stop();
	//NET_Packet			P;
	//u_EventGen			(P,GE_DESTROY,ID());
	//u_EventSend			(P);
}

void CCustomDetector::OnRender() {
	if(!bDebug) return;
	RCache.OnFrameEnd();
	Fmatrix l_ball;
	l_ball.scale(m_buzzer_radius, m_buzzer_radius, m_buzzer_radius);
	Fvector l_p; l_p.set(0, 1.f, 0); clTransform.transform(l_p, l_p);
	l_ball.translate_add(l_p);
	RCache.dbg_DrawEllipse(l_ball, D3DCOLOR_XRGB(255,0,255));
}
