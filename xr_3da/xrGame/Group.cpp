#include "stdafx.h"
#include "level.h"
#include "entity.h"
#include "custommonster.h"

CGroup::CGroup()
{
	SetState	(gsFollowMe);
	SetFlag		(gtAgressive,	true	);
	SetFlag		(gtQuiet,		false	);

	vTargetDirection.set(0,0,1);
	vTargetPosition.set	(0,0,0);
	vCentroid.set		(0,0,0);
	m_bLeaderViewsEnemy = false;
	m_dwLeaderChangeCount = 0;
	m_tLastHitDirection.set(1,0,0);
	m_tHitPosition.set(1,0,0);
	m_dwLastHitTime = 0;
	m_dwFiring = 0;
	m_bEnemyNoticed = false;
	m_bLessCoverLook = false;
}

const Fvector& CGroup::GetCentroid()
{
	vCentroid.set	(0,0,0);
	for (DWORD I=0; I<Members.size(); I++) 
		vCentroid.add	(Members[I]->Position());
	vCentroid.div	(float(Members.size()));
	return vCentroid;
}

void CGroup::Member_Add(CEntity* E){
	Members.push_back(E);
}

void CGroup::Member_Remove(CEntity* E){
	EntityIt it = find(Members.begin(),Members.end(),E);
	if (it!=Members.end()) Members.erase(it);
}

void CGroup::GetMemberPlacement(MemberPlacement& MP, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	vCentroid.set	(0,0,0);
	for (DWORD I=0; I<Members.size(); I++) 
	{
		CEntity*		E = Members[I];
		const Fvector&	P = E->Position();
		vCentroid.add	(P);
		if (E!=Me)	MP.push_back(P);
	}
	vCentroid.div	(float(Members.size()));
}

void CGroup::GetMemberPlacementN(MemberNodes& MP, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	for (DWORD I=0; I<Members.size(); I++) 
	{
		CEntity*	E = Members[I];
		if (E!=Me)	MP.push_back(E->AI_NodeID);
	}
}

void CGroup::GetMemberDedication(MemberPlacement& MP, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	vCentroid.set	(0,0,0);
	for (DWORD I=0; I<Members.size(); I++) {
		if (Members[I]->SUB_CLS_ID == CLSID_OBJECT_ACTOR) {
			CEntity*		E = Members[I];
			const Fvector&	P = E->Position();
			MP.push_back				(P);
		}
		else {
			CEntity*		E = Members[I];
			const Fvector&	P = E->Position();
			vCentroid.add	(P);
			if (E!=Me)	{
				CCustomMonster* M = dynamic_cast<CCustomMonster*>(E);
				if (M) {
					Fvector	P1,P2,C;

					NodeCompressed*	NC			= Level().AI.Node(M->AI_Path.DestNode);
					Level().AI.UnpackPosition	(P1,NC->p0);
					Level().AI.UnpackPosition	(P2,NC->p1);
					C.lerp						(P1,P2,.5f);

					MP.push_back				(C);
				}
			}
		}
	}
	vCentroid.div	(float(Members.size()));
}

void CGroup::GetMemberDedicationN(MemberNodes& MP, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	for (DWORD I=0; I<Members.size(); I++) {
		if (Members[I]->SUB_CLS_ID == CLSID_OBJECT_ACTOR)
			MP.push_back(Members[I]->AI_NodeID);
		else {
			CEntity*		E = Members[I];
			if (E!=Me)	{
				CCustomMonster* M = dynamic_cast<CCustomMonster*>(E);
				if (M)
					MP.push_back		(M->AI_Path.DestNode);
			}
		}
	}
}

void CGroup::GetMemberInfo(MemberPlacement& P0, MemberNodes& P1, MemberPlacement& P2, MemberNodes& P3, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	P0.clear();
	P1.clear();
	P2.clear();
	P3.clear();
	for (DWORD I=0; I<Members.size(); I++) {
		if (Members[I]->SUB_CLS_ID == CLSID_OBJECT_ACTOR) {
			CEntity*E = Members[I];
			const Fvector&	P = E->Position();
			P0.push_back(P);
			P1.push_back(E->AI_NodeID);
			P2.push_back(P);
			P3.push_back(E->AI_NodeID);
		}
		else {
			CEntity*E = Members[I];
			if (E!=Me) {
				CCustomMonster* M = dynamic_cast<CCustomMonster*>(E);
				if (M) {
					const Fvector&	P = E->Position();
					Fvector	_P1,_P2,C;
					NodeCompressed*	NC = Level().AI.Node(M->AI_Path.DestNode);
					Level().AI.UnpackPosition(_P1,NC->p0);
					Level().AI.UnpackPosition(_P2,NC->p1);
					C.lerp(_P1,_P2,.5f);
					P0.push_back(P);
					P1.push_back(M->AI_NodeID);
					P2.push_back(C);
					P3.push_back(M->AI_Path.DestNode);
				}
			}
		}
	}
}

void CGroup::GetAliveMemberPlacement(MemberPlacement& MP, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	vCentroid.set	(0,0,0);
	for (DWORD I=0; I<Members.size(); I++) 
		if (Members[I]->g_Health() > 0) {
			CEntity*		E = Members[I];
			const Fvector&	P = E->Position();
			vCentroid.add	(P);
			if (E!=Me)	MP.push_back(P);
		}
	vCentroid.div	(float(Members.size()));
}

void CGroup::GetAliveMemberPlacementN(MemberNodes& MP, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	for (DWORD I=0; I<Members.size(); I++) 
		if (Members[I]->g_Health() > 0) {
			CEntity*	E = Members[I];
			if (E!=Me)	MP.push_back(E->AI_NodeID);
		}
}

void CGroup::GetAliveMemberDedication(MemberPlacement& MP, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	vCentroid.set	(0,0,0);
	for (DWORD I=0; I<Members.size(); I++) 
		if (Members[I]->g_Health() > 0) {
			if (Members[I]->SUB_CLS_ID == CLSID_OBJECT_ACTOR) {
				CEntity*		E = Members[I];
				const Fvector&	P = E->Position();
				MP.push_back				(P);
			}
			else {
				CEntity*		E = Members[I];
				const Fvector&	P = E->Position();
				vCentroid.add	(P);
				if (E!=Me)	{
					CCustomMonster* M = dynamic_cast<CCustomMonster*>(E);
					if (M) {
						Fvector	P1,P2,C;

						NodeCompressed*	NC			= Level().AI.Node(M->AI_Path.DestNode);
						Level().AI.UnpackPosition	(P1,NC->p0);
						Level().AI.UnpackPosition	(P2,NC->p1);
						C.lerp						(P1,P2,.5f);

						MP.push_back				(C);
					}
				}
			}
		}
	vCentroid.div	(float(Members.size()));
}

void CGroup::GetAliveMemberDedicationN(MemberNodes& MP, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	for (DWORD I=0; I<Members.size(); I++) 
		if (Members[I]->g_Health() > 0) {
			if (Members[I]->SUB_CLS_ID == CLSID_OBJECT_ACTOR)
				MP.push_back(Members[I]->AI_NodeID);
			else {
				CEntity*		E = Members[I];
				if (E!=Me)	{
					CCustomMonster* M	= dynamic_cast<CCustomMonster*>(E);
					if (M)
						MP.push_back		(M->AI_Path.DestNode);
				}
			}
		}
}

void CGroup::GetAliveMemberInfo(MemberPlacement& P0, MemberNodes& P1, MemberPlacement& P2, MemberNodes& P3, CEntity* Me)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	P0.clear();
	P1.clear();
	P2.clear();
	P3.clear();
	for (DWORD I=0; I<Members.size(); I++)
		if (Members[I]->g_Health() > 0) {
			if (Members[I]->SUB_CLS_ID == CLSID_OBJECT_ACTOR) {
				CEntity*E = Members[I];
				const Fvector&	P = E->Position();
				P0.push_back(P);
				P1.push_back(E->AI_NodeID);
				P2.push_back(P);
				P3.push_back(E->AI_NodeID);
			}
			else {
				CEntity*E = Members[I];
				if (E!=Me) {
					CCustomMonster* M = dynamic_cast<CCustomMonster*>(E);
					if (M) {
						const Fvector&	P = E->Position();
						Fvector	_P1,_P2,C;
						NodeCompressed*	NC = Level().AI.Node(M->AI_Path.DestNode);
						Level().AI.UnpackPosition(_P1,NC->p0);
						Level().AI.UnpackPosition(_P2,NC->p1);
						C.lerp(_P1,_P2,.5f);
						P0.push_back(P);
						P1.push_back(M->AI_NodeID);
						P2.push_back(C);
						P3.push_back(M->AI_Path.DestNode);
					}
				}
			}
		}
}

void CGroup::GetAliveMemberPlacementWithLeader(MemberPlacement& MP, CEntity* Me, CEntity* Leader)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	vCentroid.set	(0,0,0);
	for (DWORD I=0; I<Members.size(); I++) 
		if (Members[I]->g_Health() > 0) {
			CEntity*		E = Members[I];
			const Fvector&	P = E->Position();
			vCentroid.add	(P);
			if (E!=Me)	MP.push_back(P);
		}
	if (Leader->g_Health() > 0) {
		CEntity*		E = Leader;
		const Fvector&	P = E->Position();
		vCentroid.add	(P);
		if (E!=Me)	MP.push_back(P);
	}
	vCentroid.div	(float(Members.size()));
}

void CGroup::GetAliveMemberPlacementNWithLeader(MemberNodes& MP, CEntity* Me, CEntity* Leader)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	for (DWORD I=0; I<Members.size(); I++) 
		if (Members[I]->g_Health() > 0)
			if (Members[I]!=Me)	
				MP.push_back(Members[I]->AI_NodeID);
	
	if (Leader->g_Health() > 0)
		if (Leader!=Me)	
			MP.push_back(Leader->AI_NodeID);
}

void CGroup::GetAliveMemberDedicationWithLeader(MemberPlacement& MP, CEntity* Me, CEntity* Leader)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	vCentroid.set	(0,0,0);
	for (DWORD I=0; I<Members.size(); I++) 
		if (Members[I]->g_Health() > 0) {
			if (Members[I]->SUB_CLS_ID == CLSID_OBJECT_ACTOR) {
				CEntity*		E = Members[I];
				const Fvector&	P = E->Position();
				MP.push_back				(P);
			}
			else {
				CEntity*		E = Members[I];
				const Fvector&	P = E->Position();
				vCentroid.add	(P);
				if (E!=Me)	{
					CCustomMonster* M			= dynamic_cast<CCustomMonster*>(E);
					if (M) {
						Fvector	P1,P2,C;

						NodeCompressed*	NC			= Level().AI.Node(M->AI_Path.DestNode);
						Level().AI.UnpackPosition	(P1,NC->p0);
						Level().AI.UnpackPosition	(P2,NC->p1);
						C.lerp						(P1,P2,.5f);

						MP.push_back				(C);
					}
				}
			}
		}
	if (Leader->g_Health() > 0) {
		if (Leader->SUB_CLS_ID == CLSID_OBJECT_ACTOR) {
			CEntity*		E = Leader;
			const Fvector&	P = E->Position();
			MP.push_back	(P);
		}
		else {
			CEntity*		E = Leader;
			const Fvector&	P = E->Position();
			vCentroid.add	(P);
			if (E!=Me)	{
				CCustomMonster* M			= dynamic_cast<CCustomMonster*>(E);
				if (M) {
					Fvector	P1,P2,C;

					NodeCompressed*	NC			= Level().AI.Node(M->AI_Path.DestNode);
					Level().AI.UnpackPosition	(P1,NC->p0);
					Level().AI.UnpackPosition	(P2,NC->p1);
					C.lerp						(P1,P2,.5f);

					MP.push_back				(C);
				}
			}
		}
	}
	vCentroid.div	(float(Members.size()));
}

void CGroup::GetAliveMemberDedicationNWithLeader(MemberNodes& MP, CEntity* Me, CEntity* Leader)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	MP.clear		();
	for (DWORD I=0; I<Members.size(); I++) 
		if (Members[I]->g_Health() > 0) {
			if (Members[I]->SUB_CLS_ID == CLSID_OBJECT_ACTOR)
				MP.push_back(Members[I]->AI_NodeID);
			else {
				CEntity*		E = Members[I];
				if (E!=Me)	{
					CCustomMonster*	M	= dynamic_cast<CCustomMonster*>(E);
					if (M)
						MP.push_back		(M->AI_Path.DestNode);
				}
			}
		}
	if (Leader->g_Health() > 0) {
		if (Leader->SUB_CLS_ID == CLSID_OBJECT_ACTOR)
			MP.push_back(Leader->AI_NodeID);
		else {
			CEntity*		E = Leader;
			if (E!=Me)	{
				CCustomMonster* M	= dynamic_cast<CCustomMonster*>(E);
				if (M)
					MP.push_back		(M->AI_Path.DestNode);
			}
		}
	}
}

void CGroup::GetAliveMemberInfoWithLeader(MemberPlacement& P0, MemberNodes& P1, MemberPlacement& P2, MemberNodes& P3, CEntity* Me, CEntity* Leader)
{
	R_ASSERT		(Members.size()<MAX_GROUP_SIZE);
	P0.clear();
	P1.clear();
	P2.clear();
	P3.clear();
	for (DWORD I=0; I<Members.size(); I++)
		if (Members[I]->g_Health() > 0) {
			if (Members[I]->SUB_CLS_ID == CLSID_OBJECT_ACTOR) {
				CEntity*E = Members[I];
				const Fvector&	P = E->Position();
				P0.push_back(P);
				P1.push_back(E->AI_NodeID);
				P2.push_back(P);
				P3.push_back(E->AI_NodeID);
			}
			else {
				CEntity*E = Members[I];
				if (E!=Me) {
					CCustomMonster* M = dynamic_cast<CCustomMonster*>(E);
					if (M) {
						const Fvector&	P = E->Position();
						Fvector	_P1,_P2,C;
						NodeCompressed*	NC = Level().AI.Node(M->AI_Path.DestNode);
						Level().AI.UnpackPosition(_P1,NC->p0);
						Level().AI.UnpackPosition(_P2,NC->p1);
						C.lerp(_P1,_P2,.5f);
						P0.push_back(P);
						P1.push_back(M->AI_NodeID);
						P2.push_back(C);
						P3.push_back(M->AI_Path.DestNode);
					}
				}
			}
		}
		
	if (Leader->g_Health() > 0) {
		if (Leader->SUB_CLS_ID == CLSID_OBJECT_ACTOR) {
			CEntity*E = Leader;
			const Fvector&	P = E->Position();
			P0.push_back(P);
			P1.push_back(E->AI_NodeID);
			P2.push_back(P);
			P3.push_back(E->AI_NodeID);
		}
		else {
			CEntity*E = Leader;
			if (E!=Me) {
				CCustomMonster* M = dynamic_cast<CCustomMonster*>(E);
				if (M) {
					const Fvector&	P = E->Position();
					Fvector	_P1,_P2,C;
					NodeCompressed*	NC = Level().AI.Node(M->AI_Path.DestNode);
					Level().AI.UnpackPosition(_P1,NC->p0);
					Level().AI.UnpackPosition(_P2,NC->p1);
					C.lerp(_P1,_P2,.5f);
					P0.push_back(P);
					P1.push_back(M->AI_NodeID);
					P2.push_back(C);
					P3.push_back(M->AI_Path.DestNode);
				}
			}
		}
	}
}
