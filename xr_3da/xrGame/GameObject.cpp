// GameObject.cpp: implementation of the CGameObject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GameObject.h"
#include "..\render.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGameObject::CGameObject()
{
	AI_NodeID	= 0;
	AI_Node		= 0;
	AI_Lighting	= 0;
	bActive		= FALSE;
}

CGameObject::~CGameObject()
{
	
}

BOOL CGameObject::Spawn	(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	BOOL bResult		= CObject::Spawn(bLocal,server_id,o_pos,o_angle,P,flags);
	
	// AI-DB connectivity
	Fvector				nPos = vPosition;
	nPos.y				+= .1f;
	int node			= Level().AI.q_LoadSearch(nPos);
	if (node<0)			{
		Msg				("! ERROR: AI node not found. (%f,%f,%f)",nPos.x,nPos.y,nPos.z);
		R_ASSERT		(node>=0);
	}
	AI_NodeID			= DWORD(node);
	AI_Node				= Level().AI.Node(AI_NodeID);
	if (AI_Node)	{
		AI_Lighting			= float(AI_Node->light);
		Level().AI.ref_add  (AI_NodeID);
	} else {
		AI_Lighting			= 255.f;
	}
	
	return	bResult;
}

void CGameObject::Sector_Detect	()
{
	// We was moved - so find new AI-Node
	if (AI_Node)
	{
		Fvector		Pos;
		pVisual->bv_BBox.getcenter	(Pos);
		Pos.add		(vPosition);
		CAI_Space&	AI = Level().AI;

		AI.ref_dec  (AI_NodeID);
		AI_NodeID	= AI.q_Node	(AI_NodeID,vPosition);
		AI.ref_add  (AI_NodeID);
		AI_Node		= AI.Node	(AI_NodeID);
	}
	
	// Perform sector detection
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

void CGameObject::OnVisible	()
{
	float	l_f		= Device.fTimeDelta*fLightSmoothFactor;
	float	l_i		= 1.f-l_f;
	float&	LL		= AI_Lighting;
	float	CL		= AI_Node?float(AI_Node->light):255;
	LL				= l_i*LL + l_f*CL;
	::Render.set_Transform		(&clTransform);
	::Render.set_LightLevel		(iFloor(LL));
	::Render.add_leafs_Dynamic	(Visual());
}

CObject::SavedPosition CGameObject::ps_Element(DWORD ID)
{
	VERIFY(ID<ps_Size());
	CObject::SavedPosition	SP	=	PositionStack[ID];
	SP.dwTime					+=	Level().timeServer_Delta();
	return SP;
}
