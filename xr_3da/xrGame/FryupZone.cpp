#include "stdafx.h"
#include "FryupZone.h"

CFryupZone::CFryupZone()
{
}
CFryupZone::~CFryupZone()
{
}

#ifdef DEBUG
void CFryupZone::OnRender()
{
}
#endif

//BOOL CCustomZone::net_Spawn(LPVOID DC) 
//{
//	CCF_Shape *l_pShape			= xr_new<CCF_Shape>(this);
//	collidable.model			= l_pShape;
//	CSE_Abstract				*e = (CSE_Abstract*)(DC);
//	CSE_ALifeAnomalousZone		*Z = smart_cast<CSE_ALifeAnomalousZone*>(e);
//
//	for (u32 i=0; i < Z->shapes.size(); ++i) 
//	{
//		CSE_Shape::shape_def	&S = Z->shapes[i];
//		switch (S.type) 
//		{
//		case 0 : l_pShape->add_sphere(S.data.sphere); break;
//		case 1 : l_pShape->add_box(S.data.box); break;
//		}
//	}
//
//	BOOL bOk = inherited::net_Spawn(DC);
//
//	if (bOk) 
//	{
//		l_pShape->ComputeBounds		();
//		m_maxPower					= Z->m_fMaxPower;
//		m_fAttenuation						= Z->m_attn;
//		m_dwPeriod					= Z->m_period;
//
//		Fvector						P;
//		XFORM().transform_tiny		(P,CFORM()->getSphere().P);
//		Sound->play_at_pos			(m_ambient,this, Position(), true);
//
//		//	setVisible(true);
//		setEnabled(true);
//
//		CParticlesObject* pStaticPG; s32 l_c = (int)m_effects.size();
//		Fmatrix l_m; l_m.set(renderable.xform);
//		Fvector zero_vel = {0.f,0.f,0.f};
//		for(s32 i = 0; i < l_c; ++i) {
//			Fvector c; c.set(l_m.c.x,l_m.c.y+EPS,l_m.c.z);
//			IRender_Sector *l_pRS = ::Render->detectSector(c);
//			pStaticPG = xr_new<CParticlesObject>(*m_effects[i],l_pRS,false);
//			pStaticPG->UpdateParent(l_m,zero_vel);
//			pStaticPG->Play();
//			m_effectsPSs.push_back(pStaticPG);
//		}
//	}
//
//	return bOk;
//}


