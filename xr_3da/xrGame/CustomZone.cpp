#include "stdafx.h"
#include "customzone.h"
#include "actor.h"
#include "hudmanager.h"
#include "..\xr_ioconsole.h"

CCustomZone::CCustomZone(void) {
	m_pA = NULL;
	m_maxPower = 100.f;
	m_attn = 1.f;
	m_period = 1000;
	m_time = 0;
}

CCustomZone::~CCustomZone(void) {}

BOOL CCustomZone::net_Spawn		(LPVOID DC)
{
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
		//m_maxPower = Z->m_maxPower;
		//m_attn = Z->m_attn;
		//m_period = Z->m_period;

//		setVisible(true);
		setEnabled(true);
	}

	return res;
}

void CCustomZone::net_Destroy(){
	inherited::net_Destroy();
}

void CCustomZone::Update(u32 dt) {
	inherited::Update	(dt);

	const Fsphere& s		= cfModel->getSphere();
	Fvector					P;
	clXFORM().transform_tiny(P,s.P);
	feel_touch_update		(P,s.R);

	m_time += dt;
	if(m_time > m_period) {
		while(m_time > m_period) m_time -= m_period;
		if(m_pA) {
			char l_pow[255]; sprintf(l_pow, "zone hit. %.1f", Power(m_pA->Position().distance_to(P)));
			Level().HUD()->outMessage(0xffffffff,m_pA->cName(), l_pow);
		}
	}
}


void CCustomZone::feel_touch_new			(CObject* O) {
	CActor *l_pA = dynamic_cast<CActor*>(O);
	Level().HUD()->outMessage(0xffffffff,l_pA->cName(),"entering a zone.");//Log("I feel a zone!!!!");
	//Console.Execute("rs_postprocess on");
	//Console.Execute("rs_supersample 1");
	::Render->getTarget()->set_blur(.5f);

	m_pA = l_pA;
}

void CCustomZone::feel_touch_delete		(CObject* O) {
	CActor *l_pA = dynamic_cast<CActor*>(O);
	Level().HUD()->outMessage(0xffffffff,l_pA->cName(),"leaving a zone.");//Log("I've left a zone!!!!");
	//Console.Execute("rs_postprocess off");
	//Console.Execute("rs_supersample 0");
	::Render->getTarget()->set_blur(1.f);

	m_pA = NULL;
}

BOOL CCustomZone::feel_touch_contact	(CObject* O) {
	CActor *l_pA = dynamic_cast<CActor*>(O); if(!l_pA || !l_pA->Local()) return false;
	return ((CCF_Shape*)cfModel)->Contact(O);
}

f32 CCustomZone::Power(f32 dist) {
	f32 l_r = cfModel->getRadius();
	return l_r < dist ? 0 : m_maxPower * (1.f - m_attn*dist/l_r);
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
