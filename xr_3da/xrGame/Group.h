#pragma once

class	CEntity;

enum	EGroupState
{
	gsHoldPosition=0,			//	���������� �������				
	gsNeedBackup,				//	���� �� ��� (����� ������)		
	gsGoInThisDirection,		//	���� �� �����������: "����������" 
	gsGoToThatPosition,			//	���� � �������� �������:			
	gsFreeHunting,				//	��������� �����					

	gsLast,
	gsForceDWORD = DWORD(-1)
};

enum	EGroupTriggers
{
	gtAgressive		= (1<<0),	// ������ ��� ����������� ����� / �������� ������ ��� ������
	gtQuiet			= (1<<1),	// ���� �� ����� �������� ����������� / ������ (����� �� �����������) �� ����� �����
	gtForceDWORD	= DWORD(-1)
};

DEFINE_VECTOR(CEntity*,EntityVec,EntityIt);

typedef svector<Fvector,16>		MemberPlacement;
typedef svector<DWORD,16>		MemberNodes;
class CGroup
{
public:
	EGroupState					State;
	DWORD						Flags;

	Fvector						vTargetDirection;
	Fvector						vTargetPosition;

	EntityVec					Members;
	Fvector						vCentroid;

public:
	// Constructor
								CGroup				();

	void						SetState			(EGroupState S)				{ State=S; }
	void						SetFlag				(EGroupTriggers T, BOOL f)	{ if (f) Flags|=DWORD(T); else Flags &= ~DWORD(T);}
	void						InvertFlag			(EGroupTriggers T)			{ if (Flags&DWORD(T)) Flags&=~DWORD(T); else Flags|=DWORD(T);}

	void						Member_Add			(CEntity* E);
	void						Member_Remove		(CEntity* E);

	bool						Empty				(){return Members.empty();}
	int							Size				(){return Members.size();}

	// Internal communication
	void						GetMemberPlacement	(MemberPlacement& P, CEntity* Me);
	void						GetMemberDedication	(MemberPlacement& P, CEntity* Me);
	void						GetMemberPlacementN	(MemberNodes& P, CEntity* Me);
	void						GetMemberDedicationN(MemberNodes& P, CEntity* Me);
	const Fvector&				GetCentroid			();
};
