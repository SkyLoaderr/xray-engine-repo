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
	m_tpCurrentBlend				= 0;
	m_bPlaying						= false;
	m_dwCurrentAnimationIndex		= 0;
}

CAI_Idol::~CAI_Idol					()
{
}

void CAI_Idol::Load					(LPCSTR section)
{
	setEnabled						(false);
	inherited::Load					(section);
}

BOOL CAI_Idol::net_Spawn			(LPVOID DC)
{
	xrSE_Idol						*tpIdol = (xrSE_Idol*)(DC);
	R_ASSERT						(tpIdol);
	cNameVisual_set					(tpIdol->caModel);
	
	if (!inherited::net_Spawn(DC))	return FALSE;
	
	m_dwAnyPlayType					= tpIdol->m_dwAniPlayType;
	m_tpaAnims.clear				();
	u32								N = _GetItemCount(tpIdol->m_caAnimations);
	string16						I;
	for (u32 i=0; i<N; i++)
		m_tpaAnims.push_back		(PKinematics(pVisual)->ID_Cycle(_GetItem(tpIdol->m_caAnimations,i,I)));

	return							TRUE;
}

void CAI_Idol::SelectAnimation		(const Fvector& _view, const Fvector& _move, float speed)
{
	R_ASSERT						(!m_tpaAnims.empty());
	if (g_Alive()) {
		switch (m_dwAnyPlayType) {
			case 0 : {
				if (!m_bPlaying) {
					m_tpCurrentBlend		= PKinematics(pVisual)->PlayCycle	(m_tpaAnims[::Random.randI(0,m_tpaAnims.size())],TRUE,AnimCallback,this);
					m_bPlaying				= true;
				}
				break;
			}
			case 1 : {
				if (!m_bPlaying) {
					m_tpCurrentBlend		= PKinematics(pVisual)->PlayCycle	(m_tpaAnims[m_dwCurrentAnimationIndex],TRUE,AnimCallback,this);
					m_bPlaying				= true;
					m_dwCurrentAnimationIndex = (m_dwCurrentAnimationIndex + 1) % m_tpaAnims.size();
				}
				break;
			}
			case 2 : {
				if (!m_bPlaying) {
					m_tpCurrentBlend		= PKinematics(pVisual)->PlayCycle	(m_tpaAnims[m_dwCurrentAnimationIndex],TRUE,AnimCallback,this);
					m_bPlaying				= true;
					if (m_dwCurrentAnimationIndex < m_tpaAnims.size() - 1)
						m_dwCurrentAnimationIndex++;
				}
				break;
			}
			default : NODEFAULT;
		}
	}
}