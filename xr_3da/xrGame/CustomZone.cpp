#include "stdafx.h"
#include "customzone.h"
#include "actor.h"
#include "hudmanager.h"
#include "..\xr_ioconsole.h"
#include "..\PGObject.h"

CCustomZone::CCustomZone(void) {
	m_maxPower = 100.f;
	m_attn = 1.f;
	m_period = 1000;
	m_ready = false;
	m_pLocalActor = NULL;
}

CCustomZone::~CCustomZone(void) {}

BOOL CCustomZone::net_Spawn(LPVOID DC) 
{
	BOOL res = inherited::net_Spawn(DC);

	if(res) {
		CCF_Shape *l_pShape			= xr_new<CCF_Shape>(this);
		collidable.model			= l_pShape;
		CSE_Abstract			*e	= (CSE_Abstract*)(DC);
		CSE_ALifeAnomalousZone				*Z	= dynamic_cast<CSE_ALifeAnomalousZone*>(e);
		for (u32 i=0; i < Z->shapes.size(); i++) {
			CSE_Shape::shape_def& S = Z->shapes[i];
			switch (S.type) {
				case 0 : l_pShape->add_sphere(S.data.sphere); break;
				case 1 : l_pShape->add_box(S.data.box); break;
			}
		}

		l_pShape->ComputeBounds						();
		g_pGameLevel->ObjectSpace.Object_Register	(this);
		CFORM()->OnMove								();
		m_maxPower = Z->m_maxPower;
		m_attn = Z->m_attn;
		m_period = Z->m_period;

		Fvector P; XFORM().transform_tiny			(P,CFORM()->getSphere().P);
		Sound->play_at_pos							(m_ambient,this, Position(), true);

//		setVisible(true);
		setEnabled(true);

		CPGObject* pStaticPG; s32 l_c = (int)m_effects.size();
		Fmatrix l_m; l_m.set(renderable.xform);
		for(s32 i = 0; i < l_c; i++) {
			Fvector c; c.set(l_m.c.x,l_m.c.y+EPS,l_m.c.z);
			IRender_Sector *l_pRS = ::Render->detectSector(c);
			pStaticPG = xr_new<CPGObject>(m_effects[i],l_pRS,false);
			pStaticPG->SetTransform(l_m);
			pStaticPG->Play();
			m_effectsPSs.push_back(pStaticPG);
		}
	}

	return res;
}

void CCustomZone::Load(LPCSTR section) {
	// verify class
	LPCSTR Class = pSettings->r_string(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	inherited::Load(section);

	LPCSTR l_PSnd = pSettings->r_string(section,"sound");
	SoundCreate(m_ambient, l_PSnd);


	strcpy(m_effectsSTR, pSettings->r_string(section,"effects"));
	char* l_effectsSTR = m_effectsSTR; R_ASSERT(l_effectsSTR);
	m_effects.clear(); m_effects.push_back(l_effectsSTR);
	while(*l_effectsSTR) {
		if(*l_effectsSTR == ',') {
			*l_effectsSTR = 0; l_effectsSTR++;
			while(*l_effectsSTR == ' ' || *l_effectsSTR == '\t') l_effectsSTR++;
			m_effects.push_back(l_effectsSTR);
		}
		l_effectsSTR++;
	}

// @@@ WT: !!!!!бпелеммн!!!!!
	//CRender_target*		T	= ::Render->getTarget();
	//T->set_duality_h		(0);
	//T->set_duality_v		(0);
	//T->set_noise			(0);
// @@@ WT

}

void CCustomZone::net_Destroy() {
	inherited::net_Destroy();
	SoundDestroy(m_ambient);
	while(m_effectsPSs.size()) { xr_delete(*(m_effectsPSs.begin())); m_effectsPSs.pop_front(); }
}

//void CCustomZone::Update(u32 dt) {
	//inherited::Update	(dt);
void CCustomZone::UpdateCL() {
	//u32 dt = Device.dwTimeDelta;
	inherited::UpdateCL();

	const Fsphere& s		= CFORM()->getSphere();
	Fvector					P;
	XFORM().transform_tiny(P,s.P);
	//feel_touch.clear(); m_inZone.clear();
	feel_touch_update		(P,s.R);

	if(m_ready) {
		xr_set<CObject*>::iterator l_it;
		for(l_it = m_inZone.begin(); l_it != m_inZone.end(); l_it++) {
			Affect(*l_it);
		}
		m_ready = false;
	}
}


void CCustomZone::feel_touch_new(CObject* O) {
	if(bDebug) HUD().outMessage(0xffffffff,O->cName(),"entering a zone.");
	m_inZone.insert(O);
	if(dynamic_cast<CActor*>(O) && O == Level().CurrentEntity()) m_pLocalActor = dynamic_cast<CActor*>(O);
}

void CCustomZone::feel_touch_delete(CObject* O) {
	if(bDebug) HUD().outMessage(0xffffffff,O->cName(),"leaving a zone.");
	m_inZone.erase(O);
	if(dynamic_cast<CActor*>(O)) m_pLocalActor = NULL;
}

BOOL CCustomZone::feel_touch_contact(CObject* O) {
	if(O == this) return false;
	if(!O->Local() || !((CGameObject*)O)->IsVisibleForZones()) return false;
	return ((CCF_Shape*)CFORM())->Contact(O);
}

f32 CCustomZone::Power(f32 dist) {
	f32 l_r = CFORM()->getRadius();
//	return l_r < dist ? 0 : m_maxPower * (1.f - m_attn*dist/l_r);
	f32 l_pow = l_r < dist ? 0 : m_maxPower * (1.f - m_attn*(dist/l_r)*(dist/l_r));
	return l_pow < 0 ? 0 : l_pow;
}

void CCustomZone::SoundCreate(sound& dest, LPCSTR s_name, int iType, BOOL bCtrlFreq) {
	string256 temp;
	if (FS.exist(temp,"$game_sounds$",s_name)) {
		Sound->create(dest,TRUE,s_name,iType);
		return;
	}
	Debug.fatal	("Can't find sound '%s'",s_name,cName());
}

void CCustomZone::SoundDestroy(sound& dest) {
	Sound->destroy		(dest);
}

//#ifdef DEBUG
void CCustomZone::OnRender() {
	if(!bDebug) return;
	RCache.OnFrameEnd();
	Fvector l_half; l_half.set(.5f, .5f, .5f);
	Fmatrix l_ball, l_box;
	xr_vector<CCF_Shape::shape_def> &l_shapes = ((CCF_Shape*)CFORM())->Shapes();
	xr_vector<CCF_Shape::shape_def>::iterator l_pShape;
	for(l_pShape = l_shapes.begin(); l_pShape != l_shapes.end(); l_pShape++) {
		switch(l_pShape->type) {
			case 0 : {
				Fsphere &l_sphere = l_pShape->data.sphere;
				l_ball.scale(l_sphere.R, l_sphere.R, l_sphere.R);
				//l_ball.scale(1.f, 1.f, 1.f);
				Fvector l_p; XFORM().transform(l_p, l_sphere.P);
				l_ball.translate_add(l_p);
				//l_ball.mul(XFORM(), l_ball);
				//l_ball.mul(l_ball, XFORM());
				RCache.dbg_DrawEllipse(l_ball, D3DCOLOR_XRGB(0,255,255));
			} break;
			case 1 : {
				l_box.mul(XFORM(), l_pShape->data.box);
				RCache.dbg_DrawOBB(l_box, l_half, D3DCOLOR_XRGB(0,255,255));
			} break;
		}
	}
}
//#endif
