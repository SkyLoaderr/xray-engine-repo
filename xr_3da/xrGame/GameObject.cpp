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
	bActive		= FALSE;
}

CGameObject::~CGameObject()
{
	
}

BOOL CGameObject::net_Spawn	(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	BOOL bResult		= CObject::net_Spawn(bLocal,server_id,o_pos,o_angle,P,flags);
	
	// AI-DB connectivity
	Fvector				nPos = vPosition;
	nPos.y				+= .1f;
	int node			= Level().AI.q_LoadSearch(nPos);
	if (node<0)			{
		Msg					("! ERROR: AI node not found for monster %s. (%f,%f,%f)",cName(),nPos.x,nPos.y,nPos.z);
		// R_ASSERT		(node>=0);
		AI_NodeID			= DWORD(-1);
		AI_Node				= NULL;
	} else {
		AI_NodeID			= DWORD(node);
		AI_Node				= Level().AI.Node(AI_NodeID);
		Level().AI.ref_add  (AI_NodeID);
	}
	
	return	bResult;
}

void CGameObject::Sector_Detect	()
{
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
	CObject::Sector_Detect	();
}

void CGameObject::OnVisible	()
{
	CObject::OnVisible			();
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
	CObject::SavedPosition	SP	=	PositionStack[ID];
	SP.dwTime					+=	Level().timeServer_Delta();
	return SP;
}
