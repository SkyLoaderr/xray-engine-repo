#include "stdafx.h"
#include "phdestroyable.h"
#include "physicsshellholder.h"
#include "xrMessages.h"
#include "object_factory.h"
#include "xrServer_Objects_ALife.h"
#include "Level.h"
#include "PhysicsShell.h"
#include "Actor.h"
#include "CharacterPhysicsSupport.h"
#include "ai_object_location.h"
#include "ai_space.h"
#include "game_graph.h"

CPHDestroyable::CPHDestroyable()
{
	m_flags.flags=0;
}
/////////spawn object representing destroyed item//////////////////////////////////////////////////////////////////////////////////
void CPHDestroyable::GenSpawnReplace(u16 source_id,LPCSTR section)
{
	CPhysicsShellHolder	*obj	=PPhysicsShellHolder()		;
	CSE_Abstract*				D	= F_entity_Create(section);//*cNameSect()
	VERIFY						(D);

	CSE_ALifeDynamicObjectVisual	*l_tpALifeDynamicObject = smart_cast<CSE_ALifeDynamicObjectVisual*>(D);
	VERIFY							(l_tpALifeDynamicObject);
	CSE_PHSkeleton					*l_tpPHSkeleton = smart_cast<CSE_PHSkeleton*>(D);
	VERIFY							(l_tpPHSkeleton);

	l_tpALifeDynamicObject->m_tGraphID	=obj->ai_location().game_vertex_id();
	l_tpALifeDynamicObject->m_tNodeID	= obj->ai_location().level_vertex_id();
	l_tpALifeDynamicObject->set_visual	(*m_destroyed_obj_visual_name);

	l_tpPHSkeleton->source_id	= u16(-1);
	//	l_tpALifePhysicObject->startup_animation=m_startup_anim;
	D->s_name			= section;//*cNameSect()
	D->set_name_replace	("");
	D->s_gameid			=	u8(GameID());
	D->s_RP				=	0xff;
	D->ID				=	0xffff;
	D->ID_Parent		=	source_id;
	D->ID_Phantom		=	0xffff;
	D->o_Position		=	obj->Position();
	if (ai().get_alife())
		l_tpALifeDynamicObject->m_tGraphID = ai().game_graph().current_level_vertex();
	else
		l_tpALifeDynamicObject->m_tGraphID = 0xffff;
	obj->XFORM().getHPB	(D->o_Angle);
	D->s_flags.assign	(M_SPAWN_OBJECT_LOCAL);
	D->RespawnTime		=	0;
	// Send
	NET_Packet			P;
	D->Spawn_Write		(P,TRUE);
	Level().Send		(P,net_flags(TRUE));
	// Destroy
	F_entity_Destroy	(D);
}
void CPHDestroyable::Destroy(u16 source_id/*=u16(-1)*/,LPCSTR section/*="ph_skeleton_object"*/)
{
	
	if(!CanDestroy())return ;
//////////send destroy to self //////////////////////////////////////////////////////////////////
	CPhysicsShellHolder	*obj	=PPhysicsShellHolder()		;
	CActor				*A		=smart_cast<CActor*>(obj)	;
	if(A)
	{
		A->character_physics_support()->SetRemoved();
		obj->setVisible(FALSE);
		obj->setEnabled(FALSE);
	}
	else
	{
		NET_Packet			P;
		obj->u_EventGen			(P,GE_DESTROY,obj->ID());
		//	Msg					("ge_destroy: [%d] - %s",ID(),*cName());
		if (obj->Local()) obj->u_EventSend			(P);
	}
	GenSpawnReplace(source_id,section);
///////////////////////////////////////////////////////////////////////////
	m_flags.set(fl_destroyed,TRUE);
	return;
}

void CPHDestroyable::Load(CInifile* ini,LPCSTR section)
{
	if(ini->line_exist(section,"destroyed_vis_name"))
	{
		m_flags.set(fl_destroyable,TRUE);
		m_destroyed_obj_visual_name=ini->r_string(section,"destroyed_vis_name");
		CPhysicsShellHolder * shell_holder=PPhysicsShellHolder();
		shared_str visual_name;
		visual_name=shell_holder->cNameVisual();
		shell_holder->cNameVisual_set(m_destroyed_obj_visual_name);
		shell_holder->cNameVisual_set(visual_name);
	}
}
void CPHDestroyable::Load(LPCSTR section)
{
	Load(pSettings,section);
}

void CPHDestroyable::Init()
{
	if(!m_flags.test(fl_destroyable))return;
}

void CPHDestroyable::RespawnInit()
{
	m_flags.set(fl_destroyed,FALSE);
}
