// GameObject.cpp: implementation of the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameObject.h"
#include "..\fbasicvisual.h"
#include "PhysicsShell.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameObject::CGameObject		()
{
	AI_NodeID	= 0;
	AI_Node		= 0;
	setActive	(FALSE);
	m_pPhysicsShell = NULL;
#ifdef DEBUG
	Device.seqRender.Add	(this,REG_PRIORITY_LOW-999);
#endif
}

CGameObject::~CGameObject		()
{
#ifdef DEBUG
	Device.seqRender.Remove	(this);
#endif
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
			Log			("-CL_destroy",cName());
			setDestroy	(TRUE);
		}
		break;
	}
}

BOOL CGameObject::net_Spawn		(LPVOID	DC)
{
	xrServerEntity*		E		= (xrServerEntity*)DC;
	R_ASSERT					(E);

	// Naming
	cName_set					(E->s_name);
	cNameSect_set				(E->s_name);
	if (E->s_name_replace[0])	cName_set		(E->s_name_replace);

	// XForm
	vPosition.set				(E->o_Position);
	mRotate.setXYZ				(E->o_Angle);
	UpdateTransform				();

	// Adapt to sphere
	/*
	Fvector						svC;
	float						svR		= Radius();
	svCenter					(svC);
	if ((svC.y-svR)<vPosition.y)
	{
		float diff			=	vPosition.y - (svC.y-svR);
		vPosition.y			+=	diff;
		UpdateTransform		();
	}
	*/

	// Net params
	setLocal						(E->s_flags.is(M_SPAWN_OBJECT_LOCAL));
	setReady						(TRUE);
	setID							(E->ID);
	pCreator->Objects.net_Register	(this);

	// AI-DB connectivity
	Fvector				nPos	= vPosition;
	int node			= Level().AI.q_LoadSearch(nPos);
	if (node<0)			{
		Msg					("! ERROR: AI node not found for object '%s'. (%f,%f,%f)",cName(),nPos.x,nPos.y,nPos.z);
		AI_NodeID			= u32(-1);
		AI_Node				= NULL;
	} else {
		AI_NodeID			= u32(node);
		AI_Node				= Level().AI.Node(AI_NodeID);
		Level().AI.ref_add  (AI_NodeID);
	}

	// Phantom
	respawnPhantom			= E->ID_Phantom;
	
	return	TRUE;
}

void CGameObject::net_Export(NET_Packet &tNetPacket)
{
	if (Level().game.type != GAME_SINGLE)
		R_ASSERT			(Local());

	tNetPacket.w				(&m_tGraphID,sizeof(m_tGraphID));
	tNetPacket.w_float			(m_fDistance);
}

void CGameObject::net_Import(NET_Packet &tNetPacket)
{
	if (Level().game.type != GAME_SINGLE)
		R_ASSERT				(Remote());
	
	tNetPacket.r				(&m_tGraphID,sizeof(m_tGraphID));
	tNetPacket.r_float			(m_fDistance);
	
	setVisible					(TRUE);
	setEnabled					(TRUE);
}

void CGameObject::Sector_Detect	()
{
	if (H_Parent())
	{
		// Use parent information
		CGameObject* O	= dynamic_cast<CGameObject*>(H_Root());
		VERIFY						(O);
		CAI_Space&	AI				= Level().AI;
		AI.ref_dec					(AI_NodeID);
		AI_NodeID					= O->AI_NodeID;
		AI.ref_add					(AI_NodeID);
		AI_Node						= O->AI_Node;
		// Sector_Move	(O->Sector());
	} else {
		// We was moved - so find _new AI-Node
		if (AI_Node)
		{
			Fvector		Pos	= pVisual->bv_Position;
			Pos.add		(vPosition);
			CAI_Space&	AI = Level().AI;

			AI.ref_dec  (AI_NodeID);
			AI_NodeID	= AI.q_Node	(AI_NodeID,vPosition);
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

void CGameObject::Hit(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse){
	if(m_pPhysicsShell) m_pPhysicsShell->applyImpulseTrace(p_in_object_space,dir,impulse);
}

#ifdef DEBUG
void CGameObject::OnRender()
{
	if (bDebug && Visual()){
		Fvector bc,bd; 
		Visual()->bv_BBox.get_CD(bc,bd);
		Fmatrix	M = clXFORM();	M.c.add (bc);
		Device.Primitive.dbg_DrawOBB (M,bd,D3DCOLOR_RGBA(0,0,255,255));
	}
}
#endif