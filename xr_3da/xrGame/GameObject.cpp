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
#include "level_navigation_graph.h"
#include "game_level_cross_table.h"
#include "ph_shell_interface.h"
#include "script_game_object.h"
#include "xrserver_objects_alife.h"
#include "game_cl_base.h"
#include "object_factory.h"
#include "../skeletoncustom.h"
#include "ai_object_location_impl.h"
#include "game_graph.h"
#include "ai_debug.h"
#include "../igame_level.h"
#include "level.h"
#include "net_utils.h"
#include "script_callback_ex.h"

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
	m_spawn_time				= 0;
	m_ai_location				= xr_new<CAI_ObjectLocation>();
	m_server_flags.one			();

	m_callbacks					= xr_new<CALLBACK_MAP>();
}

CGameObject::~CGameObject		()
{
	VERIFY						(!m_ini_file);
	VERIFY						(!m_lua_game_object);
	VERIFY						(!m_spawned);
	xr_delete					(m_ai_location);
	xr_delete					(m_callbacks);
}

void CGameObject::init			()
{
	m_lua_game_object			= 0;
	m_script_clsid				= -1;
	m_ini_file					= 0;
	m_spawned					= false;
}

void CGameObject::Load(LPCSTR section)
{
	inherited::Load			(section);
	CPrefetchManager::Load	(section);
	ISpatial*		self				= smart_cast<ISpatial*> (this);
	if (self)	{
		// #pragma todo("to Dima: All objects are visible for AI ???")
		// self->spatial.type	|=	STYPE_VISIBLEFORAI;	
		self->spatial.type	&= ~STYPE_REACTTOSOUND;
	}
}

void CGameObject::reinit	()
{
	m_visual_callback.clear	();
	ai_location().reinit	();

	// clear callbacks	
	for (CALLBACK_MAP_IT it = m_callbacks->begin(); it != m_callbacks->end(); ++it) it->second.clear();
}

void CGameObject::reload	(LPCSTR section)
{
	m_script_clsid				= object_factory().script_clsid(CLS_ID);
}

void CGameObject::net_Destroy	()
{
#ifdef DEBUG
	if (psAI_Flags.test(aiDestroy))
		Msg					("Destroying client object [%d][%s][%x]",ID(),*cName(),this);
#endif

	VERIFY					(m_spawned);

	xr_delete				(m_ini_file);

	m_script_clsid			= -1;
	if (Visual() && smart_cast<CKinematics*>(Visual()))
		smart_cast<CKinematics*>(Visual())->Callback	(0,0);

	inherited::net_Destroy						();
	setReady									(FALSE);
	g_pGameLevel->Objects.net_Unregister		(this);
	shedule_unregister							();

	
	if (this == Level().CurrentEntity())
	{
		Level().SetEntity						(0);
		Level().SetControlEntity				(0);
	}

	if (UsedAI_Locations() && !H_Parent() && ai().get_level_graph() && ai().level_graph().valid_vertex_id(ai_location().level_vertex_id()))
		ai().level_graph().ref_dec				(ai_location().level_vertex_id());

	Parent = 0;


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

			CObject*	Hitter = Level().Objects.net_Find(id);
			CObject*	Weapon = Level().Objects.net_Find(weapon_id);

			if (power>0)
				SetHitInfo(Hitter, Weapon, element);
			Hit				(power,dir,Hitter,element,
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

BOOL CGameObject::net_Spawn		(CSE_Abstract*	DC)
{
	VERIFY							(!m_spawned);
	m_spawned						= true;
	m_spawn_time					= Device.dwFrame;
	CSE_Abstract*		E			= (CSE_Abstract*)DC;
	VERIFY							(E);

	const CSE_Visual				*l_tpVisual = smart_cast<const CSE_Visual*>(E);
	if (l_tpVisual) 
		cNameVisual_set				(l_tpVisual->get_visual());

	// Naming
	cName_set						(E->s_name);
	cNameSect_set					(E->s_name);
	if (E->name_replace()[0])
		cName_set					(E->name_replace());

	setID							(E->ID);
	
	// XForm
	XFORM().setXYZ					(E->o_Angle);
	Position().set					(E->o_Position);
	VERIFY							(_valid(renderable.xform));
	VERIFY							(!fis_zero(DET(renderable.xform)));
	CSE_ALifeObject					*O = smart_cast<CSE_ALifeObject*>(E);
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

	m_server_flags.one				();
	if (O) {
		m_server_flags					= O->m_flags;
		if (O->m_flags.is(CSE_ALifeObject::flVisibleForAI))
			spatial.type				|= STYPE_VISIBLEFORAI;
		else
			spatial.type				= (spatial.type | STYPE_VISIBLEFORAI) ^ STYPE_VISIBLEFORAI;
	}

	reload						(*cNameSect());
	CScriptBinder::reload		(*cNameSect());
	
	reinit						();
	CScriptBinder::reinit		();

	//load custom user data from server
	if(!E->client_data.empty())
	{	
		IReader			ireader = IReader(&*E->client_data.begin(), E->client_data.size());
		net_Load		(ireader);
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
			CSE_ALifeObject			*l_tpALifeObject = smart_cast<CSE_ALifeObject*>(E);
			CSE_Temporary			*l_tpTemporary	= smart_cast<CSE_Temporary*>	(E);
			if (l_tpALifeObject && ai().level_graph().valid_vertex_id(l_tpALifeObject->m_tNodeID))
				ai_location().level_vertex	(l_tpALifeObject->m_tNodeID);
			else
				if (l_tpTemporary && ai().level_graph().valid_vertex_id(l_tpTemporary->m_tNodeID))
					ai_location().level_vertex	(l_tpTemporary->m_tNodeID);

			if (l_tpALifeObject && ai().game_graph().valid_vertex_id(l_tpALifeObject->m_tGraphID))
				ai_location().game_vertex			(l_tpALifeObject->m_tGraphID);

			validate_ai_locations	(false);

			// validating position
			if (UsedAI_Locations())
				Position().y		= EPS_L + ai().level_graph().vertex_plane_y(*ai_location().level_vertex(),Position().x,Position().z);
		
		}
 		inherited::net_Spawn	(DC);
	}

	m_bObjectRemoved			= false;

	spawn_supplies				();

	return						(CScriptBinder::net_Spawn(DC));
}

void CGameObject::net_Save		(NET_Packet &net_packet)
{
	u32							position;
	net_packet.w_chunk_open16	(position);
	save						(net_packet);

	// Script Binder Save ---------------------------------------
#ifdef DEBUG	
	if (psAI_Flags.test(aiSerialize))	{
		Msg(">> **** Save script object [%s] *****", *cName());
		Msg(">> Before save :: packet position = [%u]", net_packet.w_tell());
	}

#endif

	CScriptBinder::save			(net_packet);

#ifdef DEBUG	

	if (psAI_Flags.test(aiSerialize))	{
		Msg(">> After save :: packet position = [%u]", net_packet.w_tell());
	}
#endif

	// ----------------------------------------------------------

	net_packet.w_chunk_close16	(position);
}

void CGameObject::net_Load		(IReader &ireader)
{
	load					(ireader);

	// Script Binder Load ---------------------------------------
#ifdef DEBUG	
	if (psAI_Flags.test(aiSerialize))	{
		Msg(">> **** Load script object [%s] *****", *cName());
		Msg(">> Before load :: reader position = [%i]", ireader.tell());
	}

#endif

	CScriptBinder::load		(ireader);


#ifdef DEBUG	

	if (psAI_Flags.test(aiSerialize))	{
		Msg(">> After load :: reader position = [%i]", ireader.tell());
	}
#endif
	// ----------------------------------------------------------

}

void CGameObject::save			(NET_Packet &output_packet) 
{
}

void CGameObject::load			(IReader &input_packet)
{
}

void CGameObject::spawn_supplies()
{
	if (!spawn_ini() || ai().get_alife())
		return;

	if (!spawn_ini()->section_exist("spawn"))
		return;

	LPCSTR					N,V;
	float					p;
	for (u32 k = 0, j; spawn_ini()->r_line("spawn",k,&N,&V); k++) {
		VERIFY				(xr_strlen(N));
		j					= 1;
		p					= 1.f;
		
		if (V && xr_strlen(V)) {
			int				n = _GetItemCount(V);
			string16		temp;
			if (n > 0)
				j			= atoi(_GetItem(V,0,temp));
			
#if 0
			if (n > 1)
				p			= (float)atof(_GetItem(V,1,temp));
#endif
			
			if (!j)
				j			= 1;

			if (fis_zero(p))
				p			= 1.f;
		}
		
		for (u32 i=0; i<j; ++i)
			if (::Random.randF(1.f) < p)
				Level().spawn_item	(N,Position(),ai_location().level_vertex_id(),ID());
	}
}

void CGameObject::setup_parent_ai_locations(bool assign_position)
{
	CGameObject				*l_tpGameObject	= static_cast<CGameObject*>(H_Root());
	VERIFY					(l_tpGameObject);
	// get parent's position
	if (assign_position && use_parent_ai_locations())
		Position().set		(l_tpGameObject->Position());

	// setup its ai locations
	if (!UsedAI_Locations() || !ai().get_level_graph())
		return;

	if (ai().level_graph().valid_vertex_id(l_tpGameObject->ai_location().level_vertex_id()))
		ai_location().level_vertex	(l_tpGameObject->ai_location().level_vertex_id());
	else
		validate_ai_locations	(false);

	if (ai().game_graph().valid_vertex_id(l_tpGameObject->ai_location().game_vertex_id()))
		ai_location().game_vertex	(l_tpGameObject->ai_location().game_vertex_id());
	else
		ai_location().game_vertex	(ai().cross_table().vertex(ai_location().level_vertex_id()).game_vertex_id());
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
	u32								l_dwNewLevelVertexID = ai().level_graph().vertex(ai_location().level_vertex_id(),center);
//	u64								stop = CPU::GetCycleCount();

#ifdef _DEBUG
//	Msg								("%6d Searching for node for object %s (%.5f seconds)",Device.dwTimeGlobal,*cName(),float(s64(stop - start))*CPU::cycles2seconds);
#endif
	VERIFY							(ai().level_graph().valid_vertex_id(l_dwNewLevelVertexID));

#if 0
	if (decrement_reference && (ai_location().level_vertex_id() != l_dwNewLevelVertexID)) {
		Fvector						new_position = ai().level_graph().vertex_position(l_dwNewLevelVertexID);
		if (Position().y - new_position.y >= 1.5f) {
			u32						new_vertex_id = ai().level_graph().vertex(ai_location().level_vertex_id(),center);
			new_vertex_id			= new_vertex_id;
		}
	}
#endif

	if (decrement_reference && (ai_location().level_vertex_id() == l_dwNewLevelVertexID))
		return;

	if (decrement_reference && ai().level_graph().valid_vertex_id(ai_location().level_vertex_id()))
		ai().level_graph().ref_dec	(ai_location().level_vertex_id());

	ai_location().level_vertex		(l_dwNewLevelVertexID);

	ai().level_graph().ref_add		(ai_location().level_vertex_id());

	if (ai().get_game_graph() && ai().get_cross_table()) {
		ai_location().game_vertex	(ai().cross_table().vertex(ai_location().level_vertex_id()).game_vertex_id());
		VERIFY						(ai().game_graph().valid_vertex_id(ai_location().game_vertex_id()));
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

/*
float CGameObject::renderable_Ambient	()
{
	return (ai().get_level_graph() && ai().level_graph().valid_vertex_id(level_vertex_id()) ? float(level_vertex()->light()/15.f) : 1.f);
}
*/

CObject::SavedPosition CGameObject::ps_Element(u32 ID) const
{
	VERIFY(ID<ps_Size());
	inherited::SavedPosition	SP	=	PositionStack[ID];
	SP.dwTime					+=	Level().timeServer_Delta();
	return SP;
}

void CGameObject::UpdateCL	()
{
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




#include "bolt.h"
void CGameObject::OnH_B_Chield()
{
	inherited::OnH_B_Chield();
	///PHSetPushOut();????
	if (UsedAI_Locations() && ai().get_level_graph() && ai().level_graph().valid_vertex_id(ai_location().level_vertex_id()))
		ai().level_graph().ref_dec(ai_location().level_vertex_id());
}

void CGameObject::OnH_B_Independent()
{
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
	return					(m_server_flags.test(CSE_ALifeObject::flUsedAI_Locations));
}

void CGameObject::add_visual_callback		(visual_callback *callback)
{
	VERIFY						(smart_cast<CKinematics*>(Visual()));
	CALLBACK_VECTOR_IT			I = std::find(visual_callbacks().begin(),visual_callbacks().end(),callback);
	VERIFY						(I == visual_callbacks().end());

	if (m_visual_callback.empty())	SetKinematicsCallback(true);
//		smart_cast<CKinematics*>(Visual())->Callback(VisualCallback,this);
	m_visual_callback.push_back	(callback);
}

void CGameObject::remove_visual_callback	(visual_callback *callback)
{
	CALLBACK_VECTOR_IT			I = std::find(m_visual_callback.begin(),m_visual_callback.end(),callback);
	VERIFY						(I != m_visual_callback.end());
	m_visual_callback.erase		(I);
	if (m_visual_callback.empty())	SetKinematicsCallback(false);
//		smart_cast<CKinematics*>(Visual())->Callback(0,0);
}

void CGameObject::SetKinematicsCallback		(bool set)
{
	if (set)
		smart_cast<CKinematics*>(Visual())->Callback(VisualCallback,this);
	else
		smart_cast<CKinematics*>(Visual())->Callback(0,0);
};

void __stdcall VisualCallback(CKinematics *tpKinematics)
{
	CGameObject						*game_object = static_cast<CGameObject*>(static_cast<CObject*>(tpKinematics->Update_Callback_Param));
	VERIFY							(game_object);
	
	CGameObject::CALLBACK_VECTOR_IT	I = game_object->visual_callbacks().begin();
	CGameObject::CALLBACK_VECTOR_IT	E = game_object->visual_callbacks().end();
	for ( ; I != E; ++I)
		(*I)						(tpKinematics);
}

CScriptGameObject *CGameObject::lua_game_object		() const
{
#ifdef DEBUG
	if (!m_spawned)
		Msg							("! you are trying to use a destroyed object [%x]",this);
#endif
	THROW							(m_spawned);
	if (!m_lua_game_object)
		m_lua_game_object			= xr_new<CScriptGameObject>(const_cast<CGameObject*>(this));
	return							(m_lua_game_object);
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
	//Msg					("DestroyObject: ge_destroy: [%d] - %s",ID(),*cName());
	if (Local())		u_EventSend		(P);
}

void CGameObject::shedule_Update	(u32 dt)
{
	//уничтожить
	if(NeedToDestroyObject())
		DestroyObject();

	// Msg							("-SUB-:[%x][%s] CGameObject::shedule_Update",smart_cast<void*>(this),*cName());
	inherited::shedule_Update	(dt);
	CScriptBinder::shedule_Update(dt);
}

BOOL CGameObject::net_SaveRelevant	()
{
	return	(CScriptBinder::net_SaveRelevant());
}

//игровое имя объекта
LPCSTR CGameObject::Name () const
{
	return	(*cName());
}

void CGameObject::on_reguested_spawn	(CObject *object)
{
}

u32	CGameObject::ef_creature_type		() const
{
	string16	temp; CLSID2TEXT(CLS_ID,temp);
	R_ASSERT3	(false,"Invalid creature type request, virtual function is not properly overridden!",temp);
	return		(u32(-1));
}

u32	CGameObject::ef_equipment_type		() const
{
	string16	temp; CLSID2TEXT(CLS_ID,temp);
	R_ASSERT3	(false,"Invalid equipment type request, virtual function is not properly overridden!",temp);
	return		(u32(-1));
//	return		(6);
}

u32	CGameObject::ef_main_weapon_type	() const
{
	string16	temp; CLSID2TEXT(CLS_ID,temp);
	R_ASSERT3	(false,"Invalid main weapon type request, virtual function is not properly overridden!",temp);
	return		(u32(-1));
//	return		(5);
}

u32	CGameObject::ef_anomaly_type		() const
{
	string16	temp; CLSID2TEXT(CLS_ID,temp);
	R_ASSERT3	(false,"Invalid anomaly type request, virtual function is not properly overridden!",temp);
	return		(u32(-1));
}

u32	CGameObject::ef_weapon_type			() const
{
	string16	temp; CLSID2TEXT(CLS_ID,temp);
	R_ASSERT3	(false,"Invalid weapon type request, virtual function is not properly overridden!",temp);
	return		(u32(-1));
//	return		(u32(0));
}

u32	CGameObject::ef_detector_type		() const
{
	string16	temp; CLSID2TEXT(CLS_ID,temp);
	R_ASSERT3	(false,"Invalid detector type request, virtual function is not properly overridden!",temp);
	return		(u32(-1));
}

void CGameObject::net_Relcase			(CObject* O)
{
	inherited::net_Relcase	(O);
}

CGameObject::CScriptCallbackExVoid &CGameObject::callback(GameObject::ECallbackType type) const
{
	return ((*m_callbacks)[type]);
}


