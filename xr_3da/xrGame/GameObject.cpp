// GameObject.cpp: implementation of the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameObject::CGameObject()
{
	AI_NodeID	= 0;
	AI_Node		= 0;
	AI_Lighting	= 0;
}

CGameObject::~CGameObject()
{
	
}

BOOL CGameObject::Spawn	(BOOL bLocal, int server_id, Fvector4& o_pos)
{
	BOOL bResult		= CObject::Spawn(bLocal,server_id,o_pos);
	
	// AI-DB connectivity
	Fvector				nPos = vPosition;
	nPos.y				+= .1f;
	int node			= pCreator->AI.q_LoadSearch(nPos);
	if (node<0)			{
		Msg				("! ERROR: AI node not found. (%f,%f,%f)",nPos.x,nPos.y,nPos.z);
		R_ASSERT		(node>=0);
	}
	AI_NodeID			= DWORD(node);
	AI_Node				= pCreator->AI.Node(AI_NodeID);
	AI_Lighting			= (AI_Node?float(AI_Node->light):255);
}

void CGameObject::Sector_Detect	()
{
	if (0==AI_Node) {
		CObject::Sector_Detect();	// undefined sector
	} else {
		if (AI_Node->sector == 255)	CObject::Sector_Detect();	// undefined sector
		else	{
			CSector*	P = ::Render.getSector(AI_Node->sector);
			Sector_Move	(P);
		}
	}
}
