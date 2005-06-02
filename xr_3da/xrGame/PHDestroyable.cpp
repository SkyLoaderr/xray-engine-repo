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
#include "PHCollideValidator.h"
#include "PHShell.h"

/*
[impulse_transition]
random_min              =1       ; х массу объекта = величина случайно направленного импульса 
; с случайно выбранной точкой приложения в пределах нового обекта
random_hit_imp         =0.1     ; х величена хит - импульса =............

;ref_bone                       ; кость из по которой определяется скорость для частей у который связь не задана по умолчанию рут
imp_transition_factor  =0.1     ; фактор с которым прикладывается хит по исходному объекту ко всем частям кроме взрывов
lv_transition_factor   =1       ; коэффициент передачи линейной скорости
av_transition_factor   =1       ; коэффициент передачи угловой скорости


[physics\box\prt\box_wood_01_prt1]
source_bone            =-1      ;-1- ref_bone
imp_transition_factor  =1       ; коэффициент передачи импульса     
lv_transition_factor   =1       ; коэффициент передачи линейной скорости 
av_transition_factor   =1       ; коэффициент передачи угловой скорости

*/
CPHDestroyable::CPHDestroyable()
{
	m_flags.flags=0;
	m_flags.set(fl_released,TRUE);
	m_depended_objects=0;
}
/////////spawn object representing destroyed item//////////////////////////////////////////////////////////////////////////////////
void CPHDestroyable::GenSpawnReplace(u16 ref_id,LPCSTR section,shared_str visual_name)
{

	CSE_Abstract				*D	= F_entity_Create(section);//*cNameSect()
	VERIFY						(D);
	CSE_Visual					*V  =smart_cast<CSE_Visual*>(D);
	V->set_visual				(*visual_name);
	CSE_PHSkeleton				*l_tpPHSkeleton = smart_cast<CSE_PHSkeleton*>(D);
	VERIFY						(l_tpPHSkeleton);
	l_tpPHSkeleton->source_id	= ref_id;
	//init
	InitServerObject			(D);
	// Send
	D->s_name			= section;//*cNameSect()
	D->ID_Parent		= u16(-1);
	if (OnServer())
	{
		NET_Packet			P;
		D->Spawn_Write		(P,TRUE);
		Level().Send		(P,net_flags(TRUE));
		// Destroy
		F_entity_Destroy	(D);
		m_depended_objects++;
	};
};

void CPHDestroyable::InitServerObject(CSE_Abstract* D)
{
	CPhysicsShellHolder	*obj	=PPhysicsShellHolder()		;
	CSE_ALifeDynamicObjectVisual	*l_tpALifeDynamicObject = smart_cast<CSE_ALifeDynamicObjectVisual*>(D);
	VERIFY							(l_tpALifeDynamicObject);
	

	l_tpALifeDynamicObject->m_tGraphID	=obj->ai_location().game_vertex_id();
	l_tpALifeDynamicObject->m_tNodeID	= obj->ai_location().level_vertex_id();


	//	l_tpALifePhysicObject->startup_animation=m_startup_anim;
	
	D->set_name_replace	("");
	D->s_gameid			=	u8(GameID());
	D->s_RP				=	0xff;
	D->ID				=	0xffff;

	D->ID_Phantom		=	0xffff;
	D->o_Position		=	obj->Position();
	if (ai().get_alife())
		l_tpALifeDynamicObject->m_tGraphID = ai().game_graph().current_level_vertex();
	else
		l_tpALifeDynamicObject->m_tGraphID = 0xffff;
	obj->XFORM().getXYZ	(D->o_Angle);
	D->s_flags.assign	(M_SPAWN_OBJECT_LOCAL);
	D->RespawnTime		=	0;
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
	}
	else
	{
		//obj->PPhysicsShell()->Deactivate();
		obj->PPhysicsShell()->PureStep();
		obj->PPhysicsShell()->Disable();
		obj->PPhysicsShell()->DisableCollision();
		
	}

	obj->setVisible(false);
	obj->setEnabled(false);
	obj->processing_activate();
	if(source_id==obj->ID())
	{
		m_flags.set(fl_released,FALSE);
	}
	xr_vector<shared_str>::iterator i=m_destroyed_obj_visual_names.begin(),e=m_destroyed_obj_visual_names.end();

	if (GameID() == GAME_SINGLE)
	{
		for(;e!=i;i++)
			GenSpawnReplace(source_id,section,*i);
	};	
///////////////////////////////////////////////////////////////////////////
	m_flags.set(fl_destroyed,TRUE);
	//obj->shedule_register();
	return;
}

void CPHDestroyable::Load(CInifile* ini,LPCSTR section)
{
	m_flags.set(fl_destroyable,FALSE);
	if(ini->line_exist(section,"destroyed_vis_name")){
		m_flags.set(fl_destroyable,TRUE);
		m_destroyed_obj_visual_names.push_back(ini->r_string(section,"destroyed_vis_name"));
	}else{
		CInifile::Sect& data		= ini->r_section(section);
		if(data.size()>0) m_flags.set(fl_destroyable,TRUE);
		for (CInifile::SectIt I=data.begin(); I!=data.end(); I++)
			if(I->first.size())		m_destroyed_obj_visual_names.push_back(I->first);
	}
}
void CPHDestroyable::Load(LPCSTR section)
{
	m_flags.set(fl_destroyable,FALSE);

	if(pSettings->line_exist(section,"destroyed_vis_name")){
		m_flags.set								(fl_destroyable,TRUE);
		m_destroyed_obj_visual_names.push_back	(pSettings->r_string(section,"destroyed_vis_name"));
	}
}

void CPHDestroyable::Init()
{
	m_depended_objects=0;
}

void CPHDestroyable::RespawnInit()
{
	m_flags.set(fl_destroyed,FALSE);
	m_flags.set(fl_released,TRUE);
	m_destroyed_obj_visual_names.clear();
	m_depended_objects=0;
}
void CPHDestroyable::SheduleUpdate(u32 dt)
{
	if(!m_flags.test(fl_destroyed)||!m_flags.test(fl_released)) return;
	CPhysicsShellHolder *obj=PPhysicsShellHolder();
	//CActor				*A		=smart_cast<CActor*>(obj)	;
	//if(A)
	//{
	//	A->character_physics_support()->SetRemoved();
	//	obj->setVisible(FALSE);
	//	obj->setEnabled(FALSE);
	//}
	//else
	if( CanRemoveObject() )
	{
		if (obj->Local())	obj->DestroyObject();
//		NET_Packet			P;
//		obj->u_EventGen			(P,GE_DESTROY,obj->ID());
//		//	Msg					("ge_destroy: [%d] - %s",ID(),*cName());
//		if (obj->Local()) obj->u_EventSend			(P);
	}

}

void CPHDestroyable::NotificateDestroy(CPHDestroyableNotificate *dn)
{
	VERIFY(m_depended_objects);
	m_depended_objects--;
	if(!m_depended_objects)m_flags.set(fl_released,TRUE);
	CPhysicsShell* own_shell=PPhysicsShellHolder()->PPhysicsShell();
	CPhysicsShell* new_shell=dn->PPhysicsShellHolder()->PPhysicsShell();
	
//////////////////////////////////////////////////////////////////////////////////	
	dBodyID own_body=own_shell->get_ElementByStoreOrder(0)->get_body();

	u16 new_el_number = new_shell->get_ElementsNumber();

	for(u16 i=0;i<new_el_number;++i)
	{
		CPhysicsElement* e=new_shell->get_ElementByStoreOrder(i);
		Fvector mc; mc.set(e->mass_Center());
		dVector3 res_vell;
		dBodyGetPointVel(own_body,mc.x,mc.y,mc.z,res_vell);
		e->set_LinearVel(*(Fvector*)&res_vell);
	}




	new_shell->Enable();
	if(own_shell->IsGroupObject())
							new_shell->RegisterToCLGroup(own_shell->CollideBits());

	

}