// RemoteActor.cpp: implementation of the CRemoteActor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\..\xr_creator.h"
#include "..\..\networkclient.h"
#include "..\..\net_messages.h"
#include "..\..\bodyinstance.h"
#include "..\..\std_classes.h"
#include "RemoteActor.h"

#define A_NET_PING	0 //(A_NET_EXPORTTIME/4.f)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRemoteActor::CRemoteActor(void *params) : CCustomActor(params)
{
	// net
	imported.vPos.set	(0,0,0);
	imported.vDir.set	(0,0,0);
	imported.MState		= 0;
	fTimeElapsed		= A_NET_PING;
}

CRemoteActor::~CRemoteActor()
{

}

void CRemoteActor::Die				()
{
	inherited::Die();
	bVisible = false;
}

void CRemoteActor::HitAnother		(CCustomActor* pActor, int Power )
{
	R_ASSERT(0=="Invalid call");
}
void CRemoteActor::NetworkExport	()
{
	R_ASSERT(0=="Invalid call");
}
void CRemoteActor::DoMove			(EMoveCommand move_type, BOOL bSet)
{
	R_ASSERT(0=="Invalid call");
}

BOOL CRemoteActor::TakeItem			(DWORD CID)
{
	return inherited::TakeItem(CID);
}

void CRemoteActor::OnNetworkMessage	()
{
	DWORD dwTimeStamp;
	switch (pNetwork->GetMsg(&dwTimeStamp)) {
	case M_ACTOR_UPDATE:
		{
			A_NET_export* tmp = (A_NET_export*)pNetwork->GetMsgData();
			imported = *tmp;
			imported.vDir.div(2);
			fTimeElapsed = A_NET_PING;
		}
		break;
	case M_ACTOR_WEAPON_CHANGE:
		R_ASSERT(0=="Invalid call");
		break;
	case M_ACTOR_FIRE_START:
		{
			Fvector* tmp = (Fvector *)pNetwork->GetMsgData();
			firePos.set(tmp[0]);
			fireDir.set(tmp[1]);
			FireStart();
		}
		break;
	case M_ACTOR_FIRE_STOP:
		FireEnd();
		break;
	case M_ACTOR_HIT_ANOTHER:
		{
			struct AHA {
				DWORD	dpID;
				int		Power;
			};
			AHA *A = (AHA *)pNetwork->GetMsgData();
			CCustomActor* pAnother = pCreator->GetRemoteByID(A->dpID);
			pAnother->Hit(A->Power);
		}
		break;
	case M_ACTOR_RESPAWN:
		{
			Fvector* tmp = (Fvector *)pNetwork->GetMsgData();
			vPosition.set(*tmp);
			imported.vPos.set(*tmp);
			imported.vDir.set(0,0,0);
			Respawn();
			bVisible = true;
		}
		break;
	case M_ACTOR_TAKE_ITEM:
		{
			DWORD* pCID = (DWORD*)pNetwork->GetMsgData();
			TakeItem(*pCID);
		}
		break;
	}
}

void CRemoteActor::GetFireParams(Fvector &pos, Fvector &dir)
{
	pos.direct	(firePos,imported.vDir,fTimeElapsed);
	dir.set		(fireDir);
}

/*
BOOL CRemoteActor::OnCollide( CCFModel* target )
{
	return FALSE;
}
*/

void CRemoteActor::OnMoveRelevant()
{
	fTimeElapsed+=Device.fTimeDelta;
	vPosition.direct(imported.vPos,imported.vDir,fTimeElapsed);

	Fvector D,N;
	D.set(-sinf(imported.yaw),0,-cosf(imported.yaw));
	N.set(0,1,0);
	mRotate.rotation	(D,N);

	UpdateTransform();

	inherited::OnMoveRelevant();

	Weapon->UpdatePosition();
}
