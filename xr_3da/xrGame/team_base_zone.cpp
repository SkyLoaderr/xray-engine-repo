////////////////////////////////////////////////////////////////////////////
//	Module 		: team_base_zone.h
//	Created 	: 27.04.2004
//  Modified 	: 27.04.2004
//	Author		: Dmitriy Iassenev
//	Description : Team base zone object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "team_base_zone.h"
#include "xrserver_objects_alife_monsters.h"
#include "Actor.h"
#include "HUDManager.h"
#include "level.h"
#include "xrserver.h"

CTeamBaseZone::CTeamBaseZone		()
{
}

CTeamBaseZone::~CTeamBaseZone		()
{
}

void CTeamBaseZone::reinit			()
{
	inherited::reinit		();
}

void CTeamBaseZone::Center			(Fvector &C) const
{
	XFORM().transform_tiny	(C,CFORM()->getSphere().P);
}

float CTeamBaseZone::Radius			() const
{
	return						(CFORM()->getRadius());
}

BOOL CTeamBaseZone::net_Spawn	(LPVOID DC) 
{
	CCF_Shape					*l_pShape = xr_new<CCF_Shape>(this);
	collidable.model			= l_pShape;

	CSE_Abstract				*l_tpAbstract = (CSE_Abstract*)(DC);
	CSE_ALifeTeamBaseZone		*l_tpALifeScriptZone = dynamic_cast<CSE_ALifeTeamBaseZone*>(l_tpAbstract);
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

	m_Team = l_tpALifeScriptZone->m_team;

	BOOL						bOk = inherited::net_Spawn(DC);
	if (bOk) {
		l_pShape->ComputeBounds	();
		Fvector					P;
		XFORM().transform_tiny	(P,CFORM()->getSphere().P);
		setEnabled				(true);
	}

	return						(bOk);
}

void CTeamBaseZone::shedule_Update(u32 dt)
{
	inherited::shedule_Update	(dt);
	
	const Fsphere				&s = CFORM()->getSphere();
	Fvector						P;
	XFORM().transform_tiny		(P,s.P);
	feel_touch_update			(P,s.R);
}

void CTeamBaseZone::feel_touch_new	(CObject *tpObject)
{
	HUD().GetUI()->UIGame()->OnObjectEnterTeamBase(tpObject, this);
	if (OnServer()) Level().Server->game->OnObjectEnterTeamBase(tpObject->ID(), ID());
	
/*	NET_Packet			P;
	P.w_begin			(M_GAMEMESSAGE);
	P.w_u32				(GMSG_TEAM_BASE);
	P.w_u8				(1);
	P.w_u8				( GetZoneTeam() );
	P.w_u16				(tpObject->ID() );
	u_EventSend			(P);
*/
}

void CTeamBaseZone::feel_touch_delete	(CObject *tpObject)
{
	HUD().GetUI()->UIGame()->OnObjectLeaveTeamBase(tpObject, this);		// 
	if (OnServer()) Level().Server->game->OnObjectLeaveTeamBase(tpObject->ID(), ID());

/*	
if (OnServer())
	NET_Packet			P;
	P.w_begin			(M_GAMEMESSAGE);
	P.w_u32				(GMSG_TEAM_BASE);
	P.w_u8				(0);
	P.w_u8				( GetZoneTeam() );
	P.w_u16				(tpObject->ID() );
	u_EventSend			(P);
*/
}

BOOL CTeamBaseZone::feel_touch_contact	(CObject* O)
{
	CActor*	pActor = dynamic_cast<CActor*>(O);
	if (!pActor) return (FALSE);
	return ((CCF_Shape*)CFORM())->Contact(O);
}

#ifdef DEBUG
extern	Flags32	dbg_net_Draw_Flags;
void CTeamBaseZone::OnRender() 
{
	if(!bDebug) return;
	if (!(dbg_net_Draw_Flags.is_any((1<<3)))) return;
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
