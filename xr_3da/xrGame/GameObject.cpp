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
#include "script_game_object.h"
#include "xrserver_objects_alife.h"
#include "game_cl_base.h"
#include "object_factory.h"

#define OBJECT_REMOVE_TIME 180000
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CGameObject::CGameObject		()
{
	init						();
	//-----------------------------------------
	m_bCrPr_Activated			= false;
	m_dwCrPr_ActivationStep		= 0;
}

CGameObject::~CGameObject		()
{
	VERIFY						(!m_ini_file);
	VERIFY						(!m_lua_game_object);
	VERIFY						(!m_spawned);
}

void CGameObject::init			()
{

	m_lua_game_object			= 0;
	m_dwFrameLoad				= u32(-1);
	m_dwFrameReload				= u32(-1);
	m_dwFrameReinit				= u32(-1);
	m_dwFrameSpawn				= u32(-1);
	m_dwFrameDestroy			= u32(-1);
	m_dwFrameClient				= u32(-1);
	m_dwFrameSchedule			= u32(-1);
	m_dwFrameBeforeChild		= u32(-1);
	m_dwFrameBeforeIndependent	= u32(-1);
	m_script_clsid				= -1;
	m_ini_file					= 0;
	m_spawned					= false;
}

void CGameObject::Load(LPCSTR section)
{
	if (!frame_check(m_dwFrameLoad))
		return;

	inherited::Load			(section);
	CPrefetchManager::Load	(section);
	ISpatial*		self				= dynamic_cast<ISpatial*> (this);
	if (self)	{
		// #pragma todo("to Dima: All objects are visible for AI ???")
		// self->spatial.type	|=	STYPE_VISIBLEFORAI;	
		self->spatial.type	&= ~STYPE_REACTTOSOUND;
	}
}

void CGameObject::reinit	()
{
	if (!frame_check(m_dwFrameReinit))
		return;

	m_visual_callback.clear		();
	CAI_ObjectLocation::reinit	();
	CScriptBinder::reinit		();
}

void CGameObject::reload	(LPCSTR section)
{
	if (!frame_check(m_dwFrameReload))
		return;

	m_script_clsid				= object_factory().script_clsid(SUB_CLS_ID);
	CScriptBinder::reload		(section);
}

void CGameObject::net_Destroy	()
{
	if (!frame_check(m_dwFrameDestroy))
		return;

	VERIFY					(m_spawned);

	xr_delete				(m_ini_file);

	m_script_clsid			= -1;
	if (Visual() && PKinematics(Visual()))
		PKinematics(Visual())->Callback	(0,0);

	inherited::net_Destroy						();
	setReady									(FALSE);
	g_pGameLevel->Objects.net_Unregister		(this);
	
	if (this == Level().CurrentEntity())
	{
		Level().SetEntity						(0);
		Level().SetControlEntity				(0);
	}

	if (UsedAI_Locations() && !H_Parent() && ai().get_level_graph() && ai().level_graph().valid_vertex_id(level_vertex_id()))
		ai().level_graph().ref_dec				(level_vertex_id());

	Parent = 0;



	//удалить партиклы из ParticlePlayer
	CParticlesPlayer::net_DestroyParticles	();
	
	CScriptBinder::net_Destroy				();

	xr_delete								(m_lua_game_object);
	m_spawned								= false;
}

void CGameObject::OnEvent		(NET_Packet& P, u16 type)
{
	switch (type)
	{
	case GE_HIT:
		{
			u16				id,weapon_id;
			Fvector			dir;
			float			power, impulse;
			s16				element;
			Fvector			position_in_bone_space;
			u16				hit_type;

			P.r_u16			(id);
			P.r_u16			(weapon_id);
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
			//Log			("-CL_destroy",*cName());
			setDestroy		(TRUE);
		}
		break;
	}
}

void __stdcall VisualCallback(CKinematics *tpKinematics);

BOOL CGameObject::net_Spawn		(LPVOID	DC)
{
	if (!frame_check(m_dwFrameSpawn))
		return						(TRUE);

	VERIFY							(!m_spawned);
	m_spawned						= true;
	CSE_Abstract*		E			= (CSE_Abstract*)DC;
	VERIFY							(E);

	const CSE_Visual				*l_tpVisual = dynamic_cast<const CSE_Visual*>(E);
	if (l_tpVisual) 
		cNameVisual_set				(l_tpVisual->get_visual());

	// Naming
	cName_set						(E->s_name);
	cNameSect_set					(E->s_name);
	if (E->s_name_replace[0])
		cName_set					(E->s_name_replace);

	setID							(E->ID);
	
	// XForm
	XFORM().setXYZ					(E->o_Angle);
	Position().set					(E->o_Position);
	VERIFY							(_valid(renderable.xform));
	VERIFY							(!fis_zero(DET(renderable.xform)));
	CSE_ALifeObject					*O = dynamic_cast<CSE_ALifeObject*>(E);
	if (O && xr_strlen(O->m_ini_string)) {
#pragma warning(push)
#pragma warning(disable:4238)
		m_ini_file					= xr_new<CInifile>(
			&IReader				(
				(void*)(*(O->m_ini_string)),
				O->m_ini_string.size()
			)
		);
#pragma warning(pop)
	}

	// Net params
	setLocal						(E->s_flags.is(M_SPAWN_OBJECT_LOCAL));
	setReady						(TRUE);
	g_pGameLevel->Objects.net_Register	(this);

	if (O) {
		if (O->m_flags.is(CSE_ALifeObject::flVisibleForAI))
			spatial.type				|= STYPE_VISIBLEFORAI;
		else
			spatial.type				= (spatial.type | STYPE_VISIBLEFORAI) ^ STYPE_VISIBLEFORAI;
	}

	reload						(*cNameSect());
	reinit						();

	//load custom user data from server
	if(!E->client_data.empty())
	{	
		IReader ireader = IReader(&*E->client_data.begin(), E->client_data.size());
		load(ireader);
	}

	// if we have a parent
	if (0xffff != E->ID_Parent) {
		
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

			if (l_tpALifeObject && ai().game_graph().valid_vertex_id(l_tpALifeObject->m_tGraphID))
				set_game_vertex		(l_tpALifeObject->m_tGraphID);

			validate_ai_locations	(false);

			// validating position
			if (UsedAI_Locations())
				Position().y		= EPS_L + ai().level_graph().vertex_plane_y(*level_vertex(),Position().x,Position().z);
		
		}
 		inherited::net_Spawn	(DC);
	}

	m_bObjectRemoved			= false;

	spawn_supplies				();

	return						(CScriptBinder::net_Spawn(DC));
}

void CGameObject::net_Save		(NET_Packet &net_packet)
{
	u32	position;
	net_packet.w_chunk_open8(position);
	save(net_packet);
	net_packet.w_chunk_close8(position);
}

void CGameObject::save		(NET_Packet &output_packet) {}
void CGameObject::load		(IReader &input_packet) {}

void CGameObject::spawn_supplies()
{
	if (!spawn_ini() || ai().get_alife())
		return;

	if (!spawn_ini()->section_exist("spawn"))
		return;

	LPCSTR					N,V;
	for (u32 k = 0, j; spawn_ini()->r_line("spawn",k,&N,&V); k++) {
		VERIFY				(xr_strlen(N));
		j					= 1;
		if (V && xr_strlen(V)) {
			j				= atoi(V);
			if (!j)
				j			= 1;
		}
		for (u32 i=0; i<j; ++i)
			Level().spawn_item	(N,Position(),level_vertex_id(),ID());
	}
}

void CGameObject::setup_parent_ai_locations(bool assign_position)
{
	CGameObject				*l_tpGameObject	= dynamic_cast<CGameObject*>(H_Root());
	VERIFY					(l_tpGameObject);
	// get parent's position
	if (assign_position && use_parent_ai_locations())
		Position().set		(l_tpGameObject->Position());

	// setup its ai locations
	if (!UsedAI_Locations() || !ai().get_level_graph())
		return;

	if (ai().level_graph().valid_vertex_id(l_tpGameObject->level_vertex_id()))
		set_level_vertex		(l_tpGameObject->level_vertex_id());
	else
		validate_ai_locations	(false);

	if (ai().game_graph().valid_vertex_id(l_tpGameObject->game_vertex_id()))
		set_game_vertex			(l_tpGameObject->game_vertex_id());
	else
		set_game_vertex			(ai().cross_table().vertex(level_vertex_id()).game_vertex_id());
}

void CGameObject::validate_ai_locations			(bool decrement_reference)
{
	if (!ai().get_level_graph())
		return;

	if (!UsedAI_Locations()) {
//		if (ai().get_game_graph() && ai().get_cross_table())
//			set_game_vertex		(ai().cross_table().vertex(level_vertex_id()).game_vertex_id());
		return;
	}

//	u64								start = CPU::GetCycleCount();
	Fvector							center;
	Center							(center);
	center.x						= Position().x;
	center.z						= Position().z;
	u32								l_dwNewLevelVertexID = ai().level_graph().vertex(level_vertex_id(),center);
//	u64								stop = CPU::GetCycleCount();

#ifdef _DEBUG
//	Msg								("%6d Searching for node for object %s (%.5f seconds)",Level().timeServer(),*cName(),float(s64(stop - start))*CPU::cycles2seconds);
#endif
	VERIFY							(ai().level_graph().valid_vertex_id(l_dwNewLevelVertexID));

	if (decrement_reference && (level_vertex_id() == l_dwNewLevelVertexID))
		return;

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

CObject::SavedPosition CGameObject::ps_Element(u32 ID) const
{
	VERIFY(ID<ps_Size());
	inherited::SavedPosition	SP	=	PositionStack[ID];
	SP.dwTime					+=	Level().timeServer_Delta();
	return SP;
}

void CGameObject::UpdateCL	()
{
	if (!frame_check(m_dwFrameClient))
		return;

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


#include "bolt.h"
void CGameObject::OnH_B_Chield()
{
	if (!frame_check(m_dwFrameBeforeChild))
		return;

	inherited::OnH_B_Chield();
	///PHSetPushOut();????
	if (UsedAI_Locations() && ai().get_level_graph() && ai().level_graph().valid_vertex_id(level_vertex_id()))
		ai().level_graph().ref_dec(level_vertex_id());
}

void CGameObject::OnH_B_Independent()
{
	if (!frame_check(m_dwFrameBeforeIndependent))
		return;

	inherited::OnH_B_Independent();
	setup_parent_ai_locations	(false);
	validate_ai_locations		(false);
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

void CGameObject::add_visual_callback		(visual_callback *callback)
{
	VERIFY						(PKinematics(Visual()));
	CALLBACK_VECTOR_IT			I = std::find(visual_callbacks().begin(),visual_callbacks().end(),callback);
	VERIFY						(I == visual_callbacks().end());

	if (m_visual_callback.empty())	SetKinematicsCallback(true);
//		PKinematics(Visual())->Callback(VisualCallback,this);
	m_visual_callback.push_back	(callback);
}

void CGameObject::remove_visual_callback	(visual_callback *callback)
{
	CALLBACK_VECTOR_IT			I = std::find(m_visual_callback.begin(),m_visual_callback.end(),callback);
	VERIFY						(I != m_visual_callback.end());
	m_visual_callback.erase		(I);
	if (m_visual_callback.empty())	SetKinematicsCallback(false);
//		PKinematics(Visual())->Callback(0,0);
}

void CGameObject::SetKinematicsCallback		(bool set)
{
	if (set)
		PKinematics(Visual())->Callback(VisualCallback,this);
	else
		PKinematics(Visual())->Callback(0,0);
};

void __stdcall VisualCallback(CKinematics *tpKinematics)
{
	CGameObject						*game_object = dynamic_cast<CGameObject*>(static_cast<CObject*>(tpKinematics->Update_Callback_Param));
	VERIFY							(game_object);
	
	CGameObject::CALLBACK_VECTOR_IT	I = game_object->visual_callbacks().begin();
	CGameObject::CALLBACK_VECTOR_IT	E = game_object->visual_callbacks().end();
	for ( ; I != E; ++I)
		(*I)						(tpKinematics);
}



CScriptGameObject *CGameObject::lua_game_object		() const
{
	VERIFY							(m_spawned);
	if (!m_lua_game_object)
		m_lua_game_object			= xr_new<CScriptGameObject>(const_cast<CGameObject*>(this));
	return							(m_lua_game_object);
}

bool CGameObject::frame_check(u32 &frame)
{
	if (Device.dwFrame == frame)
		return		(false);

	frame			= Device.dwFrame;
	return			(true);
}



bool CGameObject::NeedToDestroyObject()	const
{
	if (GameID() == GAME_SINGLE) return false;
	return false;
}

void CGameObject::DestroyObject()			
{
	if(m_bObjectRemoved) return;
	m_bObjectRemoved = true;

	NET_Packet			P;
	u_EventGen			(P,GE_DESTROY,ID());
//	Msg					("DestroyObject: ge_destroy: [%d] - %s",ID(),*cName());
	if (Local())		u_EventSend		(P);
}

void CGameObject::shedule_Update	(u32 dt)
{
	if (!frame_check(m_dwFrameSchedule))
		return;
	//уничтожить
	if(NeedToDestroyObject())
		DestroyObject();

	// Msg							("-SUB-:[%x][%s] CGameObject::shedule_Update",dynamic_cast<void*>(this),*cName());
	inherited::shedule_Update	(dt);
	CScriptBinder::shedule_Update(dt);
}

BOOL CGameObject::net_SaveRelevant	()
{
	return						(false);
}

//игровое имя объекта
LPCSTR CGameObject::Name () const
{
	return *cName();
}