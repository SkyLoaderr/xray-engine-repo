// GameObject.cpp: implementation of the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameObject.h"
#include "..\fbasicvisual.h"
#include "PhysicsShell.h"
#include "ai_space.h"
#include "CustomMonster.h"
#include "physicobject.h"
#include "HangingLamp.h"
#include "PhysicsShell.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameObject::CGameObject		()
{
	AI_NodeID		= u32(-1);
	AI_Node			= 0;
	m_pPhysicsShell = NULL;
	//////////////////////////////////////
	// � ����� - ����!!!! (2-� ������� ����� �� ���� :-))))
	// ������ CGameObject �� ��� VISIBLEFORAI ???
	//////////////////////////////////////
	ISpatial*		self				= dynamic_cast<ISpatial*> (this);
	if (self)
		self->spatial.type	|= STYPE_VISIBLEFORAI;
	//////////////////////////////////////

	/******* Oles
#ifdef DEBUG
	Device.seqRender.Add	(this,REG_PRIORITY_LOW-999);
#endif
	*/
}

CGameObject::~CGameObject		()
{
	/******* Oles
#ifdef DEBUG
	Device.seqRender.Remove	(this);
#endif
	*/
}

void CGameObject::net_Destroy	()
{
	inherited::net_Destroy		();
	setReady									(FALSE);
	g_pGameLevel->Objects.net_Unregister		(this);
	if (this == Level().CurrentEntity())		Level().SetEntity(0);
	if (!H_Parent()) {
//		Msg										("REF_DEC (%s) %d = %d",cName(),AI_NodeID,getAI().q_mark[AI_NodeID] - 1);
		getAI().ref_dec							(AI_NodeID);
	}
	AI_NodeID									= u32(-1);
	AI_Node										= 0;
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
	setDestroy						(FALSE);	// @@@ WT

	CSE_Abstract*		E			= (CSE_Abstract*)DC;
	R_ASSERT						(E);

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

	// AI-DB connectivity
	CTimer		T; T.Start		();
	CSE_ALifeObject*		a_obj	= dynamic_cast<CSE_ALifeObject*>(E);
	CSE_Temporary			*l_tpTemporary = dynamic_cast<CSE_Temporary*>(E);
	u32						l_dwDesiredNodeID = a_obj ? a_obj->m_tNodeID : (l_tpTemporary ? l_tpTemporary->m_tNodeID : u32(-1));
	
	if (!getAI().bfCheckIfMapLoaded()) {
		AI_NodeID			= u32(-1);
		AI_Node				= 0;
	}
	else
		if (E->ID_Parent == 0xffff) {
			if (l_dwDesiredNodeID != u32(-1)) {
				CAI_Space&	AI		= getAI();
				R_ASSERT			(AI.bfCheckIfGraphLoaded());
				//Msg					("G2L : %f",getAI().m_tpaGraph[a_obj->m_tGraphID].tLocalPoint.distance_to(Position()));
				//		AI_NodeID			=	AI.q_Node	(getAI().m_tpaGraph[a_obj->m_tGraphID].tNodeID,Position());
				//		Msg					("G2L : %f",getAI().tfGetNodeCenter(a_obj->m_tNodeID).distance_to(Position()));
				//Msg("Net_spawn %s",cName());
				if ((l_dwDesiredNodeID < getAI().Header().count) && l_dwDesiredNodeID)
					AI_NodeID			=	AI.q_Node	(l_dwDesiredNodeID,Position());
				else
					AI_NodeID			=	AI.q_LoadSearch(Position());

				if (!AI_NodeID || (AI_NodeID == u32(-1))) {
					Msg					("! GameObject::NET_Spawn : Corresponding node hasn't been found for object %s",cName());
					R_ASSERT3			(!getAI().bfCheckIfMapLoaded(),"Cannot find a proper node for object ",cName());
					AI_NodeID			= u32(-1);
					AI_Node				= NULL;
				}
				else {
					AI_Node				= AI.Node(AI_NodeID);
					//Msg					("REF_ADD (%s) %d = %d",cName(),AI_NodeID,getAI().q_mark[AI_NodeID] + 1);
					getAI().ref_add		(AI_NodeID);
					CPhysicObject		*l_tpPhysicObject = dynamic_cast<CPhysicObject*>(this);
					if (!l_tpPhysicObject) {
						CHangingLamp	*l_tpHangingLamp = dynamic_cast<CHangingLamp*>(this);
						if (!l_tpHangingLamp)
							Position().y	= getAI().ffGetY(*AI_Node,Position().x,Position().z);
					}
				}
			}
			else {
				Fvector					nPos = Position();
				int node				= getAI().q_LoadSearch(nPos);

				if (node<=0)			{
					Msg					("! ERROR: AI node not found for object '%s'. (%f,%f,%f)",cName(),nPos.x,nPos.y,nPos.z);
					R_ASSERT3			(!getAI().bfCheckIfMapLoaded(),"Cannot find a proper node for object ",cName());
					AI_NodeID			= u32(-1);
					AI_Node				= NULL;
				}
				else {
					AI_NodeID			= u32(node);
					AI_Node				= getAI().Node(AI_NodeID);
					getAI().ref_add		(AI_NodeID);
					CPhysicObject		*l_tpPhysicObject = dynamic_cast<CPhysicObject*>(this);
					if (!l_tpPhysicObject) {
						CHangingLamp	*l_tpHangingLamp = dynamic_cast<CHangingLamp*>(this);
						if (!l_tpHangingLamp)
							Position().y	= getAI().ffGetY(*AI_Node,Position().x,Position().z);
					}
				}
			}
		}
		else {
			CGameObject* O	= dynamic_cast<CGameObject*>(H_Root());
			VERIFY						(O);
			Position().set				(O->Position());
			AI_NodeID					= O->AI_NodeID;
			AI_Node						= O->AI_Node;
		}

#pragma todo("Oles to Dima : Incorrect spawning, just hackery?")
#pragma todo("Dima to Oles : Parent will be assigned after object's net_spawn, though I need it filled correctly during net_spawn to prevent illegal node ref_add/ref_dec in the sector_detect")
	if ((E->ID_Parent != 0xffff) && !Parent) {
		Parent						= this;
		inherited::net_Spawn		(DC);
		Parent						= 0;
	}
	else
		inherited::net_Spawn		(DC);

	//Msg			("--spawn--ai-node: %f ms",1000.f*T.GetAsync());

	// Phantom
	// respawnPhantom			= E->ID_Phantom;
	
	return	TRUE;
}

void CGameObject::spatial_move		()
{
	if (H_Parent()) {
//		// Use parent information
		CGameObject* O	= dynamic_cast<CGameObject*>(H_Root());
		VERIFY						(O);
//		CAI_Space&	AI				= getAI();
//		Msg							("REF_DEC (%s) %d = %d",cName(),AI_NodeID,getAI().q_mark[AI_NodeID] - 1);
//		AI.ref_dec					(AI_NodeID);

		Position().set				(O->Position());
		AI_NodeID					= O->AI_NodeID;
//		Msg							("REF_ADD (%s) %d = %d",cName(),AI_NodeID,getAI().q_mark[AI_NodeID] + 1);
//		AI.ref_add					(AI_NodeID);
		AI_Node						= O->AI_Node;
		// Sector_Move	(O->Sector());
	} else {
		// We was moved - so find _new AI-Node
//		if ((AI_Node) && (Visual())) {
		if (Visual() && getAI().bfCheckIfMapLoaded()) {
			Fvector		Pos	= Visual()->vis.sphere.P;		  
			Pos.add		(Position());
			CAI_Space&	AI = getAI();

//			Msg						("REF_DEC (%s) %d = %d",cName(),AI_NodeID,getAI().q_mark[AI_NodeID] - 1);
			if ((int(AI_NodeID) > 0) && (getAI().Header().count))
				AI.ref_dec			(AI_NodeID);
//			Msg("Spatial move %s",cName());
			AI_NodeID	= AI.q_Node	(AI_NodeID,Position());

//			Msg						("REF_ADD (%s) %d = %d",cName(),AI_NodeID,getAI().q_mark[AI_NodeID] + 1);
			AI.ref_add				(AI_NodeID);
			AI_Node					= AI.Node(AI_NodeID);
//			if (!getAI().bfInsideNode(AI_Node,Position())) {
//				AI_NodeID	= AI_NodeID;
//			}

		}

		// Perform sector detection
	}
	inherited::spatial_move();
}

void CGameObject::renderable_Render	()
{
	inherited::renderable_Render();
	::Render->set_Transform		(&XFORM());
	::Render->add_Visual		(Visual());
}

float CGameObject::renderable_Ambient	()
{
	return AI_Node?float(AI_Node->light):255;
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
	inherited::UpdateCL	();
}

void CGameObject::u_EventGen(NET_Packet& P, u32 type, u32 dest)
{
	P.w_begin	(M_EVENT);
	P.w_u32		(Level().timeServer());
	P.w_u16		(u16(type&0xffff));
	P.w_u16		(u16(dest&0xffff));
}

void CGameObject::u_EventSend(NET_Packet& P, BOOL sync)
{
	Level().Send(P,net_flags(TRUE,TRUE));
}

void CGameObject::Hit(float P, Fvector &dir, CObject* who, s16 element,
					  Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
{
	if(impulse>0)
		if(m_pPhysicsShell) m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,impulse);
}

//�������� �� ��������� "��������" �� �������
f32 CGameObject::ExplosionEffect(const Fvector &expl_centre, const f32 expl_radius, xr_list<s16> &elements, xr_list<Fvector> &bs_positions) 
{
	Collide::ray_query RQ;
	Fvector l_pos; 
	Center(l_pos);
	Fvector l_dir; 
	l_dir.sub(l_pos, expl_centre); 
	l_dir.normalize();
	if(!Level().ObjectSpace.RayPick(expl_centre, l_dir, expl_radius, RQ)) return 0;
	//������� �� ����� ��� �����, �� �� �� ���
	if(RQ.O != this) return 0;
	
/*	//�������������� ���������
	if((s16)RQ.element != -1)
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

void CGameObject::PHSetMaterial(u32 m)
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

void CGameObject::OnH_B_Chield()
{
	inherited::OnH_B_Chield();
	PHSetPushOut();
}

void CGameObject::OnH_B_Independent()
{
	inherited::OnH_B_Independent();
	if (!getAI().bfCheckIfMapLoaded())
		return;

	if ((AI_NodeID < getAI().Header().count) && AI_NodeID)
		AI_NodeID			=	getAI().q_Node	(AI_NodeID,Position());
	else
		AI_NodeID			=	getAI().q_LoadSearch(Position());

	if (!AI_NodeID || (AI_NodeID == u32(-1))) {
		Msg					("! GameObject::NET_Spawn : Corresponding node hasn't been found for object %s",cName());
		R_ASSERT3			(!getAI().bfCheckIfMapLoaded(),"Cannot find a proper node for object ",cName());
		AI_NodeID			= u32(-1);
		AI_Node				= NULL;
	}
	else
		AI_Node				= getAI().Node(AI_NodeID);

	getAI().ref_add			(AI_NodeID);
}

void CGameObject::PHSetPushOut(u32 time /* = 5000 */)
{
	if(m_pPhysicsShell)
		m_pPhysicsShell->set_PushOut(time,PushOutCallback1);
}

f32 CGameObject::GetMass() { return m_pPhysicsShell?m_pPhysicsShell->getMass():0; }
///void CGameObject::OnH_A_Independent()
//{
//	if(m_pPhysicsShell)
//		m_pPhysicsShell->set_PushOut(50000);
		//m_pPhysicsShell->SetMaterial("objects\\soft_object");

//	inherited::OnH_A_Independent();
//}

#ifdef DEBUG
void CGameObject::OnRender()
{
	if (bDebug && Visual())
	{
		Fvector bc,bd; 
		Visual()->vis.box.get_CD	(bc,bd);
		Fmatrix	M = XFORM();		M.c.add (bc);
		RCache.dbg_DrawOBB			(M,bd,D3DCOLOR_RGBA(0,0,255,255));
	}
}
#endif