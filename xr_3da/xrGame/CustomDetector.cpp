#include "stdafx.h"
#include "customdetector.h"
#include "hudmanager.h"
#include "artifact.h"

CCustomDetector::CCustomDetector(void) 
{
//	m_belt = true;
}

CCustomDetector::~CCustomDetector(void) 
{
}

BOOL CCustomDetector::net_Spawn(LPVOID DC) 
{
	BOOL res = inherited::net_Spawn(DC);

	CKinematics* V = PKinematics(Visual());
	if(V) V->PlayCycle("idle");

	if (m_pPhysicsShell == NULL) 
	{
		// Physics (Box)
		Fobb obb; Visual()->vis.box.get_CD(obb.m_translate,obb.m_halfsize); 
		obb.m_rotate.identity();
		// Physics (Elements)
		CPhysicsElement* E = P_create_Element(); 
		R_ASSERT(E); 
		E->add_Box(obb);
		// Physics (Shell)
		m_pPhysicsShell = P_create_Shell(); 
		R_ASSERT(m_pPhysicsShell);
		m_pPhysicsShell->add_Element(E);
		m_pPhysicsShell->setDensity(2000.f);
		CSE_Abstract *l_pE = (CSE_Abstract*)DC;
		if(l_pE->ID_Parent==0xffff) m_pPhysicsShell->Activate(XFORM(),0,XFORM());
		m_pPhysicsShell->mDesired.identity();
		m_pPhysicsShell->fDesiredStrength = 0.f;
	}
//	CCF_Shape*	shape			= xr_new<CCF_Shape>	(this);
//	CFORM()						= shape;
//	Fsphere S;	S.P.set			(0,1.f,0); S.R = m_buzzer_radius;
//	shape->add_sphere			(S);
//	shape->ComputeBounds						();
//#pragma todo("@@@ WT: Check if detector will work w/o registration.")
//	g_pGameLevel->ObjectSpace.Object_Register		(this);
//	CFORM()->OnMove								();

	setVisible(true);
	setEnabled(true);

	return res;
}

void CCustomDetector::Load(LPCSTR section) 
{
	// verify class
	LPCSTR Class = pSettings->r_string(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	inherited::Load(section);

	m_radius = pSettings->r_float(section,"radius");
	m_buzzer_radius = pSettings->r_float(section,"buzzer_radius");

	LPCSTR l_soundName = pSettings->r_string(section,"noise");
	SoundCreate(m_noise, l_soundName);
	l_soundName = pSettings->r_string(section,"buzzer");
	SoundCreate(m_buzzer, l_soundName);
	u32 i = 1;
	string256 temp;

	//загрузить звуки для обозначения различных типов зон
	do 
	{
		sprintf(temp, "zone_class_%d", i);
		if(pSettings->line_exist(section,temp)) 
		{
			LPCSTR z_Class = pSettings->r_string(section,temp);
			CLASS_ID zone_cls = TEXT2CLSID(pSettings->r_string(z_Class,"class"));
			sprintf(temp, "zone_sound_%d", i);
			l_soundName = pSettings->r_string(section,temp);
			ref_sound *l_pSound = xr_new<ref_sound>();
			SoundCreate(*l_pSound, l_soundName);
			m_sounds[zone_cls] = l_pSound;
			i++;
		} else break;
	} while(true);
}

void CCustomDetector::net_Destroy() 
{
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
	xr_delete(m_pPhysicsShell);
	inherited::net_Destroy();
	SoundDestroy(m_noise);
	xr_map<CLASS_ID, ref_sound*>::iterator l_it;
	for(l_it = m_sounds.begin(); l_it != m_sounds.end(); l_it++) SoundDestroy(*l_it->second);
}

void CCustomDetector::shedule_Update(u32 dt) 
{
	inherited::shedule_Update	(dt);

	if(!H_Parent()) return;

	Position().set(H_Parent()->Position());

	//const Fsphere& s		= CFORM()->getSphere();
	Fvector					P; 
	P.set(H_Parent()->Position());
	//H_Parent()->XFORM().transform_tiny(P,P);
	feel_touch_update		(P,m_radius);

	f32 l_maxPow = 0;
	BOOL l_buzzer = false;
	xr_list<CCustomZone*>::iterator l_it;
		
	///////////////////////////////////
	//звуки обнаружения аномальных зон
	///////////////////////////////////
	for(l_it = m_zones.begin(); l_it != m_zones.end(); l_it++) 
	{
		CCustomZone *l_pZ = *l_it;
		u32 &l_time = m_times[l_pZ];
		//Fvector ZP; ZP.set(0,0,0); l_pZ->XFORM().transform_tiny(ZP,ZP);
		f32 l_dst = P.distance_to(l_pZ->Position()); 
		
		/*if(l_dst > m_radius)
			l_dst -= m_radius;
        else
			l_dst = 0;*/

		f32 l_relPow = l_pZ->Power(l_dst) / l_pZ->m_maxPower;
		if(l_relPow > 0 && l_pZ->feel_touch_contact(this)) l_buzzer = true;
		l_maxPow = _max(l_maxPow, l_relPow);
		l_relPow = 1.f - l_relPow;
		
		//определить пришло ли время играть очередной звук щелчка
		if((f32)l_time > 100.f + 5000.f * (l_relPow/**l_relPow*l_relPow*l_relPow*/)) 
		{
			l_time = 0;
			if(m_sounds.find(l_pZ->SUB_CLS_ID) != m_sounds.end()) 
			{
				ref_sound *l_pSound = m_sounds[l_pZ->SUB_CLS_ID];
				Sound->play_at_pos(*l_pSound, this, P);
			}
		} 
		else l_time += dt;
	}
	
	if(l_buzzer) 
	{
		if(!m_buzzer.feedback) m_buzzer.play_at_pos(this, P, true);
		if(m_buzzer.feedback) m_buzzer.set_position(P);
	} 
	else 
	{
		m_buzzer.stop();
	}
		if(l_maxPow > 0) 
	{
		if(!m_noise.feedback) Sound->play_at_pos(m_noise, this, P, true);
		if(m_noise.feedback) 
		{
			//l_maxPow = _max(logf(l_maxPow) / 10.f + 1.f, .0f);
			m_noise.set_volume	(l_maxPow);
			m_noise.set_position(P);
		}
	} 
	else if(m_noise.feedback) 
		m_noise.stop();
	
	//////////////////////////////////
	//Звуки обнаружения артефактов
	//////////////////////////////////
/*	
	xr_set<CArtifact*>::iterator l_it2;
	for(l_it2 = CArtifact::m_all.begin(); l_it2 != CArtifact::m_all.end(); l_it2++) 
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
	/*
	f32 l_zonePow = 0;
	xr_list<CCustomZone*>::iterator l_it;
	for(l_it = m_zones.begin(); l_it != m_zones.end(); l_it++) l_zonePow = _max(l_zonePow, (*l_it)->Power((*l_it)->Position().distance_to(Position())));
	CGameFont* H		= HUD().pFontMedium;
	H->SetColor			(0xf0ffffff); 
	H->Out				(550,500,"Anomaly force: %.0f", l_zonePow);
	*/
	if(getVisible() && m_pPhysicsShell) 
	{
		m_pPhysicsShell->Update	();
		XFORM().set				(m_pPhysicsShell->mXFORM);
		Position().set			(XFORM().c);
	}
}

void CCustomDetector::feel_touch_new(CObject* O) 
{
	CCustomZone *l_pZ = dynamic_cast<CCustomZone*>(O);
	if(l_pZ) 
	{
		if(bDebug) HUD().outMessage(0xffffffff,cName(),"started to feel a zone.");
		m_zones.push_back(l_pZ);
		m_times[l_pZ] = 0;
	}
}

void CCustomDetector::feel_touch_delete(CObject* O)
{
	CCustomZone *l_pZ = dynamic_cast<CCustomZone*>(O);
	if(l_pZ)
	{
		if(bDebug) HUD().outMessage(0xffffffff,cName(),"stoped to feel a zone.");
		m_zones.erase(std::find(m_zones.begin(), m_zones.end(), l_pZ));
	}
}

BOOL CCustomDetector::feel_touch_contact(CObject* O) 
{
	return dynamic_cast<CCustomZone*>(O) != NULL;
}

void CCustomDetector::SoundCreate(ref_sound& dest, LPCSTR s_name, int iType, BOOL bCtrlFreq) 
{
	string256 temp;
	if (FS.exist(temp,"$game_sounds$",s_name)) 
	{
		Sound->create(dest,TRUE,s_name,iType);
		return;
	}
	Debug.fatal	("Can't find ref_sound '%s'",s_name,cName());
}

void CCustomDetector::SoundDestroy(ref_sound& dest) 
{
	::Sound->destroy			(dest);
}

void CCustomDetector::OnH_A_Chield() 
{
	inherited::OnH_A_Chield		();
	setVisible					(false);
	setEnabled					(false);
	if(m_pPhysicsShell) m_pPhysicsShell->Deactivate();
}

void CCustomDetector::OnH_B_Independent() 
{
	inherited::OnH_B_Independent();
	
	//выключить
	m_buzzer.stop				();
	m_noise.stop				();

	setVisible(true);
	setEnabled(true);
	
	CObject* E = dynamic_cast<CObject*>(H_Parent()); 
	R_ASSERT(E);
	
	XFORM().set(E->XFORM());
	Position().set(XFORM().c);
	
	if(m_pPhysicsShell) 
	{
		Fmatrix trans;
		Level().Cameras.unaffected_Matrix(trans);
		Fvector l_fw; l_fw.set(trans.k);
		Fvector l_up; l_up.set(XFORM().j); l_up.mul(2.f);
		Fmatrix l_p1, l_p2;
		l_p1.set(XFORM()); l_p1.c.add(l_up); l_up.mul(1.2f); 
		l_p2.set(XFORM()); l_p2.c.add(l_up); l_fw.mul(3.f); l_p2.c.add(l_fw);
		m_pPhysicsShell->Activate(l_p1, 0, l_p2);
		XFORM().set(l_p1);
		Position().set(XFORM().c);
	}


	//NET_Packet			P;
	//u_EventGen			(P,GE_DESTROY,ID());
	//u_EventSend			(P);
}

void CCustomDetector::OnRender() 
{
	if(!bDebug) return;
	RCache.OnFrameEnd();
	Fmatrix l_ball;
	l_ball.scale(m_buzzer_radius, m_buzzer_radius, m_buzzer_radius);
	Fvector l_p;
	l_p.set(0, 1.f, 0);
	XFORM().transform(l_p, l_p);
	l_ball.translate_add(l_p);
	RCache.dbg_DrawEllipse(l_ball, D3DCOLOR_XRGB(255,0,255));
}

void CCustomDetector::renderable_Render()
{
	if(getVisible() && !H_Parent())
	{
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
}
