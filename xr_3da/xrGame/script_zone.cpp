////////////////////////////////////////////////////////////////////////////
//	Module 		: script_zone.cpp
//	Created 	: 10.10.2003
//  Modified 	: 10.10.2003
//	Author		: Dmitriy Iassenev
//	Description : Script zone object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_zone.h"
#include "ai_script_classes.h"
#include "xrserver_objects_alife_monsters.h"

CScriptZone::CScriptZone		()
{
}

CScriptZone::~CScriptZone		()
{
}

void CScriptZone::reinit		()
{
	inherited::reinit		();
	
	m_tpOnEnter.clear		();
	m_tpOnExit.clear		();
}

void CScriptZone::Center(Fvector& C) const
{
	XFORM().transform_tiny		(C,CFORM()->getSphere().P);

}

float CScriptZone::Radius() const
{
	return						(CFORM()->getRadius());
}

BOOL CScriptZone::net_Spawn	(LPVOID DC) 
{
	CCF_Shape					*l_pShape = xr_new<CCF_Shape>(this);
	collidable.model			= l_pShape;

	CSE_Abstract				*l_tpAbstract = (CSE_Abstract*)(DC);
	CSE_ALifeScriptZone			*l_tpALifeScriptZone = dynamic_cast<CSE_ALifeScriptZone*>(l_tpAbstract);
	R_ASSERT					(l_tpALifeScriptZone);

	feel_touch.clear			();

	for (u32 i=0; i < l_tpALifeScriptZone->shapes.size(); ++i) {
		CSE_Shape::shape_def	&S = l_tpALifeScriptZone->shapes[i];
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

void CScriptZone::net_Destroy	()
{
	m_tpOnEnter.clear();
	m_tpOnExit.clear();
}

void CScriptZone::UpdateCL	()
{
	inherited::UpdateCL			();
	const Fsphere				&s = CFORM()->getSphere();
	Fvector						P;
	XFORM().transform_tiny		(P,s.P);
	feel_touch_update			(P,s.R);
}

void CScriptZone::feel_touch_new	(CObject *tpObject)
{
	CGameObject					*l_tpGameObject = dynamic_cast<CGameObject*>(tpObject);
	if (!l_tpGameObject)
		return;
	
	SCRIPT_CALLBACK_EXECUTE_2(m_tpOnEnter, lua_game_object(),l_tpGameObject->lua_game_object());
}

void CScriptZone::feel_touch_delete	(CObject *tpObject)
{
	CGameObject					*l_tpGameObject = dynamic_cast<CGameObject*>(tpObject);
	if (!l_tpGameObject)
		return;
	
	SCRIPT_CALLBACK_EXECUTE_2(m_tpOnExit, lua_game_object(),l_tpGameObject->lua_game_object());
}

BOOL CScriptZone::feel_touch_contact	(CObject* O)
{
	return						(TRUE);
}

void CScriptZone::set_callback	(const luabind::object &lua_object, LPCSTR method, bool bOnEnter)
{
	CScriptCallback				&callback = bOnEnter ? m_tpOnEnter : m_tpOnExit;
	callback.set				(lua_object, method);
}

void CScriptZone::set_callback	(const luabind::functor<void> &lua_function, bool bOnEnter)
{
	CScriptCallback				&callback = bOnEnter ? m_tpOnEnter : m_tpOnExit;
	callback.set				(lua_function);
}

void CScriptZone::clear_callback(bool bOnEnter)
{
	CScriptCallback				&callback = bOnEnter ? m_tpOnEnter : m_tpOnExit;
	callback.clear				();
}

#ifdef DEBUG
void CScriptZone::OnRender() 
{
	if(!bDebug) return;
	RCache.OnFrameEnd();
	Fvector l_half; l_half.set(.5f, .5f, .5f);
	Fmatrix l_ball, l_box;
	xr_vector<CCF_Shape::shape_def> &l_shapes = ((CCF_Shape*)CFORM())->Shapes();
	xr_vector<CCF_Shape::shape_def>::iterator l_pShape;
	
	for(l_pShape = l_shapes.begin(); l_shapes.end() != l_pShape; ++l_pShape) 
	{
		switch(l_pShape->type)
		{
		case 0:
			{
                Fsphere &l_sphere = l_pShape->data.sphere;
				l_ball.scale(l_sphere.R, l_sphere.R, l_sphere.R);
				Fvector l_p; XFORM().transform(l_p, l_sphere.P);
				l_ball.translate_add(l_p);
				RCache.dbg_DrawEllipse(l_ball, D3DCOLOR_XRGB(0,255,255));
			}
			break;
		case 1:
			{
				l_box.mul(XFORM(), l_pShape->data.box);
				RCache.dbg_DrawOBB(l_box, l_half, D3DCOLOR_XRGB(0,255,255));
			}
			break;
		}
	}
}
#endif
