////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_crow.h
//	Created 	: 13.05.2002
//  Modified 	: 13.05.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Crow"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_CROW__
#define __XRAY_AI_CROW__

#include "..\\..\\CustomMonster.h"
#include "..\\..\\group.h"

class CAI_Crow : public CGameObject
{
	enum ECrowStates 	{
		aiCrowDie = 0,
		aiCrowFreeFly,
	};
	typedef	CGameObject inherited;
	protected:
		ECrowStates		st_current;
		// parameters block
		Fvector			m_vGoalDir;
		Fvector			m_vCurrentDir;
		Fvector			m_vHPB;
		float			m_fDHeading;
	public:
						CAI_Crow();
		virtual			~CAI_Crow();
		virtual void	Load			( LPCSTR section );
		virtual BOOL	Spawn			( BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags );
		virtual BOOL	ShadowGenerate	()			{ return FALSE;	}
		virtual BOOL	ShadowReceive	()			{ return FALSE;	}
		virtual void	Update			(DWORD DT);
		virtual void	net_Export		(NET_Packet* P);
		virtual void	net_Import		(NET_Packet* P);
};
		
#endif