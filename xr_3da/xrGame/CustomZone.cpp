#include "stdafx.h"
#include "customzone.h"
#include "actor.h"
#include "hudmanager.h"
#include "..\xr_ioconsole.h"

CCustomZone::CCustomZone(void) {
	m_maxPower = 100.f;
	m_attn = 1.f;
	m_period = 1000;
	m_ready = false;
}

CCustomZone::~CCustomZone(void) {}

BOOL CCustomZone::net_Spawn(LPVOID DC) {
	BOOL res = inherited::net_Spawn(DC);

	if(res) {
		CCF_Shape *l_pShape = xr_new<CCF_Shape>(this);
		cfModel = l_pShape;
		xrSE_Zone* Z		= (xrSE_Zone*)DC;
		for (u32 i=0; i < Z->shapes.size(); i++) {
			xrSE_CFormed::shape_def& S = Z->shapes[i];
			switch (S.type) {
				case 0 : l_pShape->add_sphere(S.data.sphere); break;
				case 1 : l_pShape->add_box(S.data.box); break;
			}
		}

		l_pShape->ComputeBounds();
		pCreator->ObjectSpace.Object_Register(this);
		cfModel->OnMove();
		m_maxPower = Z->m_maxPower;
		m_attn = Z->m_attn;
		m_period = Z->m_period;

		Fvector P; clXFORM().transform_tiny(P,cfModel->getSphere().P);
		Sound->PlayAtPos(m_ambient,this, vPosition, true);

//		setVisible(true);
		setEnabled(true);
	}

	return res;
}

void CCustomZone::Load(LPCSTR section) {
	// verify class
	LPCSTR Class = pSettings->ReadSTRING(section,"class");
	CLASS_ID load_cls = TEXT2CLSID(Class);
	R_ASSERT(load_cls==SUB_CLS_ID);

	inherited::Load(section);

	LPCSTR l_PSnd = pSettings->ReadSTRING(section,"sound");
	SoundCreate(m_ambient, l_PSnd);

}

void CCustomZone::net_Destroy() {
	inherited::net_Destroy();
	SoundDestroy(m_ambient);
}

void CCustomZone::Update(u32 dt) {
	inherited::Update	(dt);

	const Fsphere& s		= cfModel->getSphere();
	Fvector					P;
	clXFORM().transform_tiny(P,s.P);
	feel_touch_update		(P,s.R);

	if(m_ready) {
		set<CObject*>::iterator l_it;
		for(l_it = m_inZone.begin(); l_it != m_inZone.end(); l_it++) {
			Affect(*l_it);
		}
		m_ready = false;
	}
}


void CCustomZone::feel_touch_new(CObject* O) {
	Level().HUD()->outMessage(0xffffffff,O->cName(),"entering a zone.");
	m_inZone.insert(O);
}

void CCustomZone::feel_touch_delete(CObject* O) {
	Level().HUD()->outMessage(0xffffffff,O->cName(),"leaving a zone.");
	m_inZone.erase(O);
}

BOOL CCustomZone::feel_touch_contact(CObject* O) {
	if(!O->Local()) return false;
	return ((CCF_Shape*)cfModel)->Contact(O);
}

f32 CCustomZone::Power(f32 dist) {
	f32 l_r = cfModel->getRadius();
	return l_r < dist ? 0 : m_maxPower * (1.f - m_attn*dist/l_r);
}

void CCustomZone::SoundCreate(sound& dest, LPCSTR s_name, int iType, BOOL bCtrlFreq) {
	string256 temp;
	if (Engine.FS.Exist(temp,Path.Sounds,s_name)) {
		Sound->Create(dest,TRUE,s_name,bCtrlFreq,iType);
		return;
	}
	Debug.fatal	("Can't find sound '%s'",s_name,cName());
}

void CCustomZone::SoundDestroy(sound& dest) {
	Sound->Delete			(dest);
}

//#ifdef DEBUG
void CCustomZone::OnRender() {
	if(!bDebug) return;
	Device.Shader.OnFrameEnd();
	Fvector l_half; l_half.set(.5f, .5f, .5f);
	Fmatrix l_ball, l_box;
	vector<CCF_Shape::shape_def> &l_shapes = ((CCF_Shape*)cfModel)->Shapes();
	vector<CCF_Shape::shape_def>::iterator l_pShape;
	for(l_pShape = l_shapes.begin(); l_pShape != l_shapes.end(); l_pShape++) {
		switch(l_pShape->type) {
			case 0 : {
				Fsphere &l_sphere = l_pShape->data.sphere;
				l_ball.scale(l_sphere.R, l_sphere.R, l_sphere.R); l_ball.translate_add(l_sphere.P);
				l_ball.mul(clTransform, l_ball);
				Device.Primitive.dbg_DrawEllipse(l_ball, D3DCOLOR_XRGB(0,255,255));
			} break;
			case 1 : {
				l_box.mul(clTransform, l_pShape->data.box);
				Device.Primitive.dbg_DrawOBB(l_box, l_half, D3DCOLOR_XRGB(0,255,255));
			} break;
		}
	}
}
//#endif
