// GameObject.cpp: implementation of the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameObject.h"
#include "..\fbasicvisual.h"
#include "PhysicsShell.h"
#include "ai_space.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameObject::CGameObject		()
{
	AI_NodeID	= 0;
	AI_Node		= 0;
	setActive	(FALSE);
	m_pPhysicsShell = NULL;
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
	if (this == Level().CurrentEntity())	Level().SetEntity(0);
}

void CGameObject::net_Destroy	()
{
	setReady									(FALSE);
	pCreator->Objects.net_Unregister			(this);
	pCreator->ObjectSpace.Object_Unregister		(this);
	shedule_Unregister							();
	Sector_Move									(0);
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
			P.r_u16			(id);
			P.r_dir			(dir);
			P.r_float		(power);
			P.r_s16			(element);
			P.r_vec3		(position_in_bone_space);
			P.r_float		(impulse);
			Hit				(power,dir,Level().Objects.net_Find(id),element,position_in_bone_space, impulse);
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
	inherited::net_Spawn			(DC);

	setDestroy	(FALSE);	// @@@ WT

	CSE_Abstract*		E			= (CSE_Abstract*)DC;
	R_ASSERT						(E);

	// Naming
	cName_set						(E->s_name);
	cNameSect_set					(E->s_name);
	if (E->s_name_replace[0])		cName_set		(E->s_name_replace);

	// XForm
	vPosition.set					(E->o_Position);
	mRotate.setXYZ					(E->o_Angle);
	UpdateTransform					();

	// Net params
	setLocal						(E->s_flags.is(M_SPAWN_OBJECT_LOCAL));
	setReady						(TRUE);
	setID							(E->ID);
	pCreator->Objects.net_Register	(this);

	// AI-DB connectivity
	CTimer		T; T.Start		();
	CSE_ALifeObject*		a_obj	= dynamic_cast<CSE_ALifeObject*>(E);
	if (a_obj)
	{
		CAI_Space&	AI		= getAI();
		R_ASSERT			(AI.bfCheckIfGraphLoaded());
		//Msg					("G2L : %f",getAI().m_tpaGraph[a_obj->m_tGraphID].tLocalPoint.distance_to(vPosition));
//		AI_NodeID			=	AI.q_Node	(getAI().m_tpaGraph[a_obj->m_tGraphID].tNodeID,vPosition);
//		Msg					("G2L : %f",getAI().tfGetNodeCenter(a_obj->m_tNodeID).distance_to(vPosition));
		if (a_obj->m_tNodeID < getAI().Header().count)
			AI_NodeID			=	AI.q_Node	(a_obj->m_tNodeID,vPosition);
		else
			AI_NodeID			=	AI.q_LoadSearch(vPosition);
		
		if (!AI_NodeID || (AI_NodeID == u32(-1))) {
			Msg("! GameObject::NET_Spawn : Corresponding node hasn't been found for object %s",cName());
			AI_NodeID			= u32(-1);
			AI_Node				= NULL;
		}
		else {
			AI_Node				=	AI.Node		(AI_NodeID);
			getAI().ref_add		(AI_NodeID);
		}
	}
	else 
	{
		Fvector				nPos	= vPosition;
		int node					= getAI().q_LoadSearch(nPos);

		if (node<0)			{
			Msg					("! ERROR: AI node not found for object '%s'. (%f,%f,%f)",cName(),nPos.x,nPos.y,nPos.z);
			AI_NodeID			= u32(-1);
			AI_Node				= NULL;
		} else {
			AI_NodeID			= u32(node);
			AI_Node				= getAI().Node(AI_NodeID);
			getAI().ref_add  (AI_NodeID);
		}
	}
	//Msg			("--spawn--ai-node: %f ms",1000.f*T.GetAsync());

	// Phantom
	// respawnPhantom			= E->ID_Phantom;
	
	return	TRUE;
}

void CGameObject::Sector_Detect	()
{
	if (H_Parent())
	{
		// Use parent information
		CGameObject* O	= dynamic_cast<CGameObject*>(H_Root());
		VERIFY						(O);
		CAI_Space&	AI				= getAI();
		AI.ref_dec					(AI_NodeID);
		AI_NodeID					= O->AI_NodeID;
		AI.ref_add					(AI_NodeID);
		AI_Node						= O->AI_Node;
		// Sector_Move	(O->Sector());
	} else {
		// We was moved - so find _new AI-Node
		if ((AI_Node) && (pVisual)) {
			Fvector		Pos	= pVisual->vis.sphere.P;		  
			Pos.add		(vPosition);
			CAI_Space&	AI = getAI();

			AI.ref_dec  (AI_NodeID);
			AI_NodeID	= AI.q_Node	(AI_NodeID,vPosition);
			
			if (!AI_NodeID)
				Msg("! GameObject::Sector_Detect : Corresponding node hasn't been found for monster %s",cName());

			AI.ref_add  (AI_NodeID);
			AI_Node		= AI.Node	(AI_NodeID);
		}

		// Perform sector detection
		inherited::Sector_Detect	();
	}
}

void CGameObject::OnVisible	()
{
	inherited::OnVisible			();
	::Render->set_Transform		(&clTransform);
	::Render->add_Visual		(Visual());
}

float CGameObject::Ambient	()
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

void CGameObject::Hit(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse)
{
	if(m_pPhysicsShell) m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,impulse);
}

f32 CGameObject::ExplosionEffect(const Fvector &expl_centre, const f32 expl_radius, xr_list<s16> &elements, xr_list<Fvector> &bs_positions) {
	Collide::ray_query RQ;
	Fvector l_pos; clCenter(l_pos);
	Fvector l_dir; l_dir.sub(l_pos, expl_centre); l_dir.normalize();
	if(!Level().ObjectSpace.RayPick(expl_centre, l_dir, expl_radius, RQ)) return 0;
	if(RQ.O != this) return 0;
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

void CGameObject::PHSetPushOut()
{
	if(m_pPhysicsShell)
		m_pPhysicsShell->set_PushOut(5000,PushOutCallback1);
}
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
	if (bDebug && Visual()){
		Fvector bc,bd; 
		Visual()->vis.box.get_CD	(bc,bd);
		Fmatrix	M = clXFORM();	M.c.add (bc);
		RCache.dbg_DrawOBB (M,bd,D3DCOLOR_RGBA(0,0,255,255));
	}
}
#endif