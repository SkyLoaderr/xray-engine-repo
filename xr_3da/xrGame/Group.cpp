#include "stdafx.h"
#include "level.h"
#include "entity.h"
#include "custommonster.h"

CGroup::CGroup()
{
	SetState	(gsNeedBackup);
	SetFlag		(gtAgressive,	true	);
	SetFlag		(gtQuiet,		false	);

	vTargetDirection.set(0,0,1);
	vTargetPosition.set	(0,0,0);
	vCentroid.set		(0,0,0);
}

void CGroup::GetMemberPlacement(MemberPlacement& MP, CEntity* Me)
{
	R_ASSERT		(Members.size()<16);
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
void CGroup::GetMemberDedication(MemberPlacement& MP, CEntity* Me)
{
	R_ASSERT		(Members.size()<16);
	MP.clear		();
	vCentroid.set	(0,0,0);
	for (DWORD I=0; I<Members.size(); I++) 
	{
		CEntity*		E = Members[I];
		const Fvector&	P = E->Position();
		vCentroid.add	(P);
		if (E!=Me)	{
			CCustomMonster* M = (CCustomMonster*)E;
			Fvector	P1,P2,C;

			NodeCompressed*	NC			= Level().AI.Node(M->AI_Path.DestNode);
			Level().AI.UnpackPosition	(P1,NC->p0);
			Level().AI.UnpackPosition	(P2,NC->p1);
			C.lerp						(P1,P2,.5f);

			MP.push_back				(C);
		}
	}
	vCentroid.div	(float(Members.size()));
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
