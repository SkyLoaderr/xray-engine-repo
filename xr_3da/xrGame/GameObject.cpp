// GameObject.cpp: implementation of the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameObject.h"
#include "../fbasicvisual.h"
#include "PhysicsShell.h"
#include "ai_space.h"
#include "CustomMonster.h"
#include "physicobject.h"
#include "HangingLamp.h"
#include "PhysicsShell.h"
#include "game_sv_single.h"
#include "level_graph.h"
#include "game_level_cross_table.h"
#include "ph_shell_interface.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGameObject::CGameObject		()
{
	Init					();
}

CGameObject::~CGameObject		()
{
}

void CGameObject::Init		()
{
	m_dwNetSpawnFrame			= u32(-1);
	m_pPhysicsShell				= NULL;
}

void CGameObject::Load(LPCSTR section)
{
	inherited::Load			(section);
	CPrefetchManager::Load	(section);
	//////////////////////////////////////
	// С Олеся - ПИВО!!!! (2-я бутылка опять же Диме :-))))
	// Надо что-то придумать, чтобы флаги правильно 
	// выставлялись в конструкторе
	//////////////////////////////////////
	ISpatial*		self				= dynamic_cast<ISpatial*> (this);
	if (self)	{
		self->spatial.type	|=	STYPE_VISIBLEFORAI;
		self->spatial.type	&= ~STYPE_REACTTOSOUND;
	}
	//////////////////////////////////////
}

void CGameObject::reinit	()
{
	if (Device.dwFrame == m_dwNetSpawnFrame)
		return;

	m_dwNetSpawnFrame				= Device.dwFrame;

	CAI_ObjectLocation::reinit	();
//	CPrefetchManager::reinit	();
	m_tpALife					= 0;
}

void CGameObject::reload	(LPCSTR section)
{
//	CAI_ObjectLocation::reload	(section);
//	CPrefetchManager::reload	(section);
}

void CGameObject::net_Destroy	()
{
	if (Device.dwFrame == m_dwNetSpawnFrame)
		return;

	m_dwNetSpawnFrame				= Device.dwFrame;

	inherited::net_Destroy						();
	setReady									(FALSE);
	g_pGameLevel->Objects.net_Unregister		(this);
	
	if (this == Level().CurrentEntity())
		Level().SetEntity						(0);

	if (!H_Parent() && ai().get_level_graph() && ai().level_graph().valid_vertex_id(level_vertex_id()))
		ai().level_graph().ref_dec				(level_vertex_id());

	xr_delete									(m_pPhysicsShell);
}

void CGameObject::OnEvent		(NET_Packet& P, u16 type)
{
	switch (type)
	{
	case GE_HIT:
		{
			u16				id;
			Fvector			dir;
			float			power, impulse;
			s16				element;
			Fvector			position_in_bone_space;
			u16		hit_type;

			P.r_u16			(id);
			P.r_dir			(dir);
			P.r_float		(power);
			P.r_s16			(element);
			P.r_vec3		(position_in_bone_space);
			P.r_float		(impulse);
			P.r_u16			(hit_type);	//hit type
			Hit				(power,dir,Level().Objects.net_Find(id),element,
							 position_in_bone_space, impulse, (ALife::EHitType)hit_type);
		}
		break;
	case GE_DESTROY:
		{
			//Log			("-CL_destroy",cName());
			setDestroy	(TRUE);
		}
		break;
	}
}

BOOL CGameObject::net_Spawn		(LPVOID	DC)
{
	if (Device.dwFrame == m_dwNetSpawnFrame)
		return						(TRUE);

	m_dwNetSpawnFrame				= Device.dwFrame;

	CSE_Abstract*		E			= (CSE_Abstract*)DC;
	R_ASSERT						(E);

	const CSE_Visual				*l_tpVisual = dynamic_cast<const CSE_Visual*>(E);
	if (l_tpVisual)
		cNameVisual_set				(l_tpVisual->get_visual());

	reinit							();
	reload							(cNameSect());

	setDestroy						(FALSE);	// @@@ WT

	// Naming
	cName_set						(E->s_name);
	cNameSect_set					(E->s_name);
	if (E->s_name_replace[0])		cName_set		(E->s_name_replace);

	// XForm
	XFORM().setXYZ					(E->o_Angle);
	Position().set					(E->o_Position);

	// Net params
	setLocal						(E->s_flags.is(M_SPAWN_OBJECT_LOCAL));
	setReady						(TRUE);
	setID							(E->ID);
	g_pGameLevel->Objects.net_Register	(this);

	// if we have a parent
	if (0xffff != E->ID_Parent) {
		
		setup_parent_ai_locations();

		if (!Parent) {
			// we need this to prevent illegal ref_dec/ref_add
			Parent				= this;
			inherited::net_Spawn(DC);
			Parent				= 0;
		}
		else
			inherited::net_Spawn(DC);
	}
	else {
		if (ai().get_level_graph()) {
			CSE_ALifeObject			*l_tpALifeObject = dynamic_cast<CSE_ALifeObject*>(E);
			CSE_Temporary			*l_tpTemporary	= dynamic_cast<CSE_Temporary*>	(E);
			if (l_tpALifeObject && ai().level_graph().valid_vertex_id(l_tpALifeObject->m_tNodeID))
				set_level_vertex	(l_tpALifeObject->m_tNodeID);
			else
				if (l_tpTemporary  && ai().level_graph().valid_vertex_id(l_tpTemporary->m_tNodeID))
					set_level_vertex(l_tpTemporary->m_tNodeID);

			validate_ai_locations	(false);

			// validating position
			if (UsedAI_Locations())
				Position().y		= EPS_L + ai().level_graph().vertex_plane_y(*level_vertex(),Position().x,Position().z);
		
		}
		inherited::net_Spawn	(DC);
	}
	IPhysicShellCreator* shell_creator=dynamic_cast<IPhysicShellCreator*>(this);
	if(shell_creator)shell_creator->CreatePhysicsShell();
	return						(TRUE);
}

void CGameObject::setup_parent_ai_locations()
{
	CGameObject				*l_tpGameObject	= dynamic_cast<CGameObject*>(H_Root());
	VERIFY					(l_tpGameObject);
	// get parent's position
	if (use_parent_ai_locations())
		Position().set		(l_tpGameObject->Position());

	// setup its ai locations
	if (!UsedAI_Locations() || !ai().get_level_graph())
		return;

	if (ai().level_graph().valid_vertex_id(l_tpGameObject->level_vertex_id()))
		set_level_vertex		(l_tpGameObject->level_vertex_id());
	if (ai().game_graph().valid_vertex_id(l_tpGameObject->game_vertex_id()))
		set_game_vertex			(l_tpGameObject->game_vertex_id());
}

void CGameObject::validate_ai_locations			(bool decrement_reference)
{
	if (!UsedAI_Locations() || !ai().get_level_graph())
		return;

	u32		l_dwNewLevelVertexID	= ai().level_graph().vertex(level_vertex_id(),Position());

	if (decrement_reference && (level_vertex_id() == l_dwNewLevelVertexID))
		return;

	if (!ai().level_graph().valid_vertex_id(l_dwNewLevelVertexID)) {
		Msg							("! GameObject : Corresponding vertex hasn't been found for object %s",cName());
		R_ASSERT3					(!ai().get_level_graph(),"Cannot find a proper vertex for object ",cName());
		return;
	}
	
	if (decrement_reference && ai().level_graph().valid_vertex_id(level_vertex_id()))
		ai().level_graph().ref_dec	(level_vertex_id());

	set_level_vertex				(l_dwNewLevelVertexID);

	ai().level_graph().ref_add		(level_vertex_id());

	if (ai().get_game_graph() && ai().get_cross_table()) {
		set_game_vertex				(ai().cross_table().vertex(level_vertex_id()).game_vertex_id());
		VERIFY						(ai().game_graph().valid_vertex_id(game_vertex_id()));
	}
}

void CGameObject::spatial_move	()
{
	if (H_Parent())
		setup_parent_ai_locations	();
	else
		if (Visual())
			validate_ai_locations	();

	inherited::spatial_move			();
}

void CGameObject::renderable_Render	()
{
	inherited::renderable_Render();
	::Render->set_Transform		(&XFORM());
	::Render->add_Visual		(Visual());
}

float CGameObject::renderable_Ambient	()
{
	return (ai().get_level_graph() && ai().level_graph().valid_vertex_id(level_vertex_id()) ? float(level_vertex()->light()/15.f) : 1.f);
}

CObject::SavedPosition CGameObject::ps_Element(u32 ID)
{
	VERIFY(ID<ps_Size());
	inherited::SavedPosition	SP	=	PositionStack[ID];
	SP.dwTime					+=	Level().timeServer_Delta();
	return SP;
}

void CGameObject::UpdateCL	()
{
	//обновить присоединенные партиклы
	UpdateParticles	();

	inherited::UpdateCL	();
}

void CGameObject::u_EventGen(NET_Packet& P, u32 type, u32 dest)
{
	P.w_begin	(M_EVENT);
	P.w_u32		(Level().timeServer());
	P.w_u16		(u16(type&0xffff));
	P.w_u16		(u16(dest&0xffff));
}

void CGameObject::u_EventSend(NET_Packet& P, BOOL /**sync/**/)
{
	Level().Send(P,net_flags(TRUE,TRUE));
}

void CGameObject::Hit(float /**P/**/, Fvector &dir, CObject* /**who/**/, s16 /**element/**/,
					  Fvector p_in_object_space, float impulse, ALife::EHitType /**hit_type/**/)
{
	if(impulse>0)
		if(m_pPhysicsShell) m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,impulse);
}

//проверка на попадание "осколком" по объекту
f32 CGameObject::ExplosionEffect(const Fvector &expl_centre, const f32 expl_radius, xr_list<s16> &elements, xr_list<Fvector> &bs_positions) 
{
	Collide::rq_result RQ;
	Fvector l_pos; 
	Center(l_pos);
	Fvector l_dir; 
	l_dir.sub(l_pos, expl_centre); 
	l_dir.normalize();
	if(!Level().ObjectSpace.RayPick(expl_centre, l_dir, expl_radius, Collide::rqtBoth, RQ)) return 0;
	//осколок не попал или попал, но не по нам
	if(this != RQ.O) return 0;
	
/*	//предотвращение вылетания
	if(-1 != (s16)RQ.element)
	{
		elements.push_back((s16)RQ.element);
	}
	else
	{
		elements.push_back(0);
	}*/
	elements.push_back((s16)RQ.element);
	
	l_pos.set(0, 0, 0);
	bs_positions.push_back(l_pos);
	return 1.f;
}

void CGameObject::PHSetMaterial(u16 m)
{
	if(m_pPhysicsShell)
		m_pPhysicsShell->SetMaterial(m);
}

void CGameObject::PHSetMaterial(LPCSTR m)
{
	if(m_pPhysicsShell)
		m_pPhysicsShell->SetMaterial(m);
}

void CGameObject::PHGetLinearVell		(Fvector& velocity)
{
	if(!m_pPhysicsShell)
	{
		velocity.set(0,0,0);
		return;
	}
	m_pPhysicsShell->get_LinearVel(velocity);
}

void CGameObject::PHSetLinearVell(Fvector& velocity)
{
	if(!m_pPhysicsShell)
	{
		return;
	}
	m_pPhysicsShell->set_LinearVel(velocity);
}

void CGameObject::OnH_B_Chield()
{
	inherited::OnH_B_Chield();
	PHSetPushOut();
	if (UsedAI_Locations() && ai().get_level_graph() && ai().level_graph().valid_vertex_id(level_vertex_id()))
		ai().level_graph().ref_dec(level_vertex_id());
}

void CGameObject::OnH_B_Independent()
{
	inherited::OnH_B_Independent();
	setup_parent_ai_locations	();
	validate_ai_locations		(false);
}

void CGameObject::PHSetPushOut(u32 time /* = 5000 */)
{
	if(m_pPhysicsShell)
		m_pPhysicsShell->set_PushOut(time,PushOutCallback1);
}


f32 CGameObject::GetMass()
{
	return m_pPhysicsShell ? m_pPhysicsShell->getMass() : 0;
}

u16	CGameObject::PHGetSyncItemsNumber()
{
	if(m_pPhysicsShell)	return m_pPhysicsShell->get_ElementsNumber();
	else				return 0;
}
CPHSynchronize*	CGameObject::PHGetSyncItem	(u16 item)
{
	if(m_pPhysicsShell) return m_pPhysicsShell->get_ElementSync(item);
	else				return 0;
}
void	CGameObject::PHUnFreeze	()
{
	if(m_pPhysicsShell) m_pPhysicsShell->UnFreeze();
}
void	CGameObject::PHFreeze()
{
	if(m_pPhysicsShell) m_pPhysicsShell->Freeze();
}

#ifdef DEBUG
void CGameObject::OnRender()
{
	if (bDebug && Visual())
	{
		Fvector bc,bd; 
		Visual()->vis.box.get_CD	(bc,bd);
		Fmatrix	M = XFORM();		M.c.add (bc);
		RCache.dbg_DrawOBB			(M,bd,color_rgba(0,0,255,255));
	}
}
#endif

BOOL CGameObject::UsedAI_Locations()
{
	return					(TRUE);
}
