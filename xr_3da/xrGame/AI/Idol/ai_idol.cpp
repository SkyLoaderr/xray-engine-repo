////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.cpp
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ai_idol.h"

CAI_Idol::CAI_Idol					()
{
}

CAI_Idol::~CAI_Idol					()
{
}

void CAI_Idol::Load					(LPCSTR section)
{
	inherited::Load					(section);
	m_tpaAnims.clear				();


	LPCSTR							S = pSettings->r_string(section,"animations");
	u32								N = _GetItemCount(S);
	string16						I;
	for (u32 i=0; i<N;)
		m_tpaAnims.push_back		(PKinematics(pVisual)->ID_Cycle(_GetItem(S,i,I)));
}

BOOL CAI_Idol::net_Spawn			(LPVOID DC)
{
	BOOL							R = inherited::net_Spawn(DC);
	xrSE_Idol						*tpIdol = (xrSE_Idol*)(DC);
	R_ASSERT						(tpIdol);
	cNameVisual_set					(tpIdol->caModel);
	return							R;
}

void CAI_Idol::net_Export			(NET_Packet& P)
{
}

void CAI_Idol::net_Import			(NET_Packet& P)
{
}

void CAI_Idol::SelectAnimation		(const Fvector& _view, const Fvector& _move, float speed)
{
	R_ASSERT						(!m_tpaAnims.empty());
	if (g_Alive()) {
		if (!m_tpCurrentBlend || !m_tpCurrentBlend->playing)
			m_tpCurrentBlend		= PKinematics(pVisual)->PlayCycle	(m_tpaAnims[::Random.randI(0,m_tpaAnims.size())]);
	}
}