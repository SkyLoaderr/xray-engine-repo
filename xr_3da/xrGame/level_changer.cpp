////////////////////////////////////////////////////////////////////////////
//	Module 		: level_changer.cpp
//	Created 	: 10.07.2003
//  Modified 	: 10.07.2003
//	Author		: Dmitriy Iassenev
//	Description : Level change object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_changer.h"

CLevelChanger::~CLevelChanger	()
{
}

void CLevelChanger::spatial_register()
{
	R_ASSERT2					(CFORM(),"Invalid or no CForm!");
	spatial.center.set			(CFORM()->getSphere().P);
	spatial.radius				= CFORM()->getRadius();
	ISpatial::spatial_register	();
}

void CLevelChanger::spatial_move()
{
	R_ASSERT2					(CFORM(),"Invalid or no CForm!");
	spatial.center.set			(CFORM()->getSphere().P);
	spatial.radius				= CFORM()->getRadius();
	ISpatial::spatial_move		();
}

BOOL CLevelChanger::net_Spawn	(LPVOID DC) 
{
	CCF_Shape *l_pShape			= xr_new<CCF_Shape>(this);
	collidable.model			= l_pShape;
	
	CSE_Abstract				*l_tpAbstract = (CSE_Abstract*)(DC);
	CSE_ALifeLevelChanger		*l_tpALifeLevelChanger = dynamic_cast<CSE_ALifeLevelChanger*>(l_tpAbstract);
	R_ASSERT					(l_tpALifeLevelChanger);

	strcpy						(m_caLevelToChange,l_tpALifeLevelChanger->m_caLevelToChange);
	
	for (u32 i=0; i < l_tpALifeLevelChanger->shapes.size(); i++) {
		CSE_Shape::shape_def	&S = l_tpALifeLevelChanger->shapes[i];
		switch (S.type) {
			case 0 : {
				l_pShape->add_sphere(S.data.sphere);
				break;
			}
			case 1 : {
				l_pShape->add_box(S.data.box);
				break;
			}
		}
	}

	BOOL						bOk = inherited::net_Spawn(DC);
	if (bOk) {
		l_pShape->ComputeBounds	();
		Fvector					P;
		XFORM().transform_tiny	(P,CFORM()->getSphere().P);
		setEnabled				(true);
	}

	return						(bOk);
}

void CLevelChanger::feel_touch_new	(CObject *tpObject)
{
	CActor				*l_tpActor = dynamic_cast<CActor*>(tpObject);
	if (l_tpActor) {
		strconcat(Level().m_caServerOptions,m_caLevelToChange,"/single");
		Level().IR_OnKeyboardPress(DIK_F7);
	}
}