#include "stdafx.h"
#include "phdestroyable.h"
#include "physicsshellholder.h"
#include "xrMessages.h"
#include "object_factory.h"
#include "xrServer_Objects_ALife.h"
#include "Level.h"
	CPHDestroyable::CPHDestroyable()
	{
		b_canbe_destroyed=false;
	}

void CPHDestroyable::Destroy()
{
	if(!b_canbe_destroyed)return;
//////////send destroy to self //////////////////////////////////////////////////////////////////
	CPhysicsShellHolder* obj=PPhysicsShellHolder();

	{
		NET_Packet			P;
		obj->u_EventGen			(P,GE_DESTROY,obj->ID());
		//	Msg					("ge_destroy: [%d] - %s",ID(),*cName());
		if (obj->Local()) obj->u_EventSend			(P);
	}
/////////spawn object representing destroyed item//////////////////////////////////////////////////////////////////////////////////
	{
		CSE_Abstract*				D	= F_entity_Create("ph_skeleton_object");//*cNameSect()
		R_ASSERT					(D);

		CSE_ALifeDynamicObject		*l_tpALifeDynamicObject = dynamic_cast<CSE_ALifeDynamicObject*>(D);
		R_ASSERT					(l_tpALifeDynamicObject);
		CSE_ALifePHSkeletonObject		*l_tpALifePhysicObject = dynamic_cast<CSE_ALifePHSkeletonObject*>(D);
		R_ASSERT					(l_tpALifePhysicObject);

		l_tpALifePhysicObject->m_tGraphID	=obj->game_vertex_id();
		l_tpALifeDynamicObject->m_tNodeID	= obj->level_vertex_id();
		l_tpALifePhysicObject->set_visual	(*obj->cNameVisual());

		l_tpALifePhysicObject->source_id	= u16(-1);
	//	l_tpALifePhysicObject->startup_animation=m_startup_anim;
		strcpy				(D->s_name,"ph_skeleton_object");//*cNameSect()
		strcpy				(D->s_name_replace,"");
		D->s_gameid			=	u8(GameID());
		D->s_RP				=	0xff;
		D->ID				=	0xffff;
		D->ID_Parent		=	0xffff;//u16(ID());//
		D->ID_Phantom		=	0xffff;
		D->o_Position		=	obj->Position();
		obj->XFORM()			.getHPB(D->o_Angle);
		D->s_flags.assign	(M_SPAWN_OBJECT_LOCAL);
		D->RespawnTime		=	0;
		// Send
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
	}
///////////////////////////////////////////////////////////////////////////
	b_canbe_destroyed=false;
}

void CPHDestroyable::Load(LPCSTR section)
{
	if(pSettings->line_exist(section,"destroyed_vis_name"))
	{
		b_canbe_destroyed=true;
		m_destroyed_obj_visual_name=pSettings->r_string(section,"destroyed_vis_name");
	}
}

void CPHDestroyable::Init()
{
	if(!b_canbe_destroyed)return;
	CPhysicsShellHolder * shell_holder=PPhysicsShellHolder();
	ref_str visual_name;
	visual_name=shell_holder->cNameVisual();
	shell_holder->cNameVisual_set(m_destroyed_obj_visual_name);
	shell_holder->cNameVisual_set(visual_name);
}

