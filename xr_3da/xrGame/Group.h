#pragma once

class	CEntity;

enum	EGroupState
{
	gsHoldPosition=0,			//	удерживать позицию				
	gsNeedBackup,				//	идти ко мне (нужна помощь)		
	gsGoInThisDirection,		//	идти по направлению: "бесконечно" 
	gsGoToThatPosition,			//	идти в заданную позицию:			
	gsFreeHunting,				//	свободная охота					

	gsLast,
	gsForceDWORD = DWORD(-1)
};

enum	EGroupTriggers
{
	gtAgressive		= (1<<0),	// всегда при обнаружении врага / стрелять только для защиты
	gtQuiet			= (1<<1),	// тихо но более медленно пробираться / быстро (особо не прикрываясь) но более шумно
	gtForceDWORD	= DWORD(-1)
};

DEFINE_VECTOR(CEntity*,EntityVec,EntityIt);

typedef svector<Fvector,16>		MemberPlacement;
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
	const Fvector&				GetCentroid			();
};
