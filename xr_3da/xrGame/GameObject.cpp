// GameObject.cpp: implementation of the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameObject.h"
#include "..\fbasicvisual.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameObject::CGameObject()
{
	AI_NodeID	= 0;
	AI_Node		= 0;
	setActive	(FALSE);
}

CGameObject::~CGameObject()
{
	
}

void CGameObject::net_Destroy	()
{
	net_Ready									= FALSE;
	pCreator->Objects.net_Unregister			(this);
	pCreator->ObjectSpace.Object_Unregister		(this);
	Sector_Move									(0);
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
	Fvector						svC;
	float						svR		= Radius();
	svCenter					(svC);
	if ((svC.y-svR)<vPosition.y)
	{
		float diff			=	vPosition.y - (svC.y-svR);
		vPosition.y			+=	diff;
		UpdateTransform		();
	}

	// Net params
	net_Local					= (E->s_flags&M_SPAWN_OBJECT_LOCAL);
	net_ID						= E->ID;
	net_Ready					= TRUE;
	pCreator->Objects.net_Register	(this);

	// Sector detection
	Sector_Detect				();

	// AI-DB connectivity
	Fvector				nPos	= vPosition;
	int node			= Level().AI.q_LoadSearch(nPos);
	if (node<0)			{
		Msg					("! ERROR: AI node not found for object '%s'. (%f,%f,%f)",cName(),nPos.x,nPos.y,nPos.z);
		AI_NodeID			= DWORD(-1);
		AI_Node				= NULL;
	} else {
		AI_NodeID			= DWORD(node);
		AI_Node				= Level().AI.Node(AI_NodeID);
		Level().AI.ref_add  (AI_NodeID);
	}
	
	return	TRUE;
}

void CGameObject::Sector_Detect	()
{
	if (H_Parent())
	{
		// Use parent information
		CGameObject* O	= dynamic_cast<CGameObject*>(H_Root());
		CAI_Space&	AI	= Level().AI;
		AI.ref_dec		(AI_NodeID);
		AI_NodeID		= O->AI_NodeID;
		AI.ref_add		(AI_NodeID);
		AI_Node			= O->AI_Node;
		// Sector_Move	(O->Sector());
	} else {
		// We was moved - so find new AI-Node
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

CObject::SavedPosition CGameObject::ps_Element(DWORD ID)
{
	VERIFY(ID<ps_Size());
	inherited::SavedPosition	SP	=	PositionStack[ID];
	SP.dwTime					+=	Level().timeServer_Delta();
	return SP;
}

void CGameObject::net_Event	(NET_Packet& P)
{
	net_Events.insert		(P);
}

void CGameObject::UpdateCL	()
{
	inherited::UpdateCL	();

	NET_Packet			P;
	DWORD svT			= Level().timeServer()-NET_Latency;
	while	(net_Events.available(svT))
	{
		u16	type		= net_Events.get(P);
		OnEvent			(P,type);
	}
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
