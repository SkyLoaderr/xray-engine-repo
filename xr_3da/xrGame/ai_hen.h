////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_hen.h
//	Created 	: 05.04.2002
//  Modified 	: 05.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Hen"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_HEN__
#define __XRAY_AI_HEN__

#include "CustomMonster.h"

namespace AI {
	// 		aiHenDie
	class _HenDie					: public State
	{
	public:
		_HenDie()					: State(aiHenDie)
		{
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	// 		aiHenUnderFire
	class _HenUnderFire					: public State
	{
	public:
		_HenUnderFire()					: State(aiHenUnderFire)
		{
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	// 		aiHenGoInThisDirection
	class _HenGoDirection			: public State
	{
	public:
		Fvector					direction;

		_HenGoDirection(Fvector& D) : State(aiHenGoInThisDirection)
		{
			direction.set		(D);
		}
		_HenGoDirection()			: State(aiHenGoInThisDirection)
		{
			direction.set		(0,0,0);
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	// 		aiHenGoToThatPosition
	class _HenGoPosition			: public State
	{
	public:
		Fvector					position;
		DWORD					nodeID;

		_HenGoPosition(Fvector& P) : State(aiHenGoToThatPosition)
		{
			position.set		(P);
		}
		_HenGoPosition()			: State(aiHenGoToThatPosition)
		{
			position.set		(0,0,0);
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	// 		aiHenHoldition
	class _HenHoldPosition			: public State
	{
	public:
		Fvector					position;
		DWORD					nodeID;

		_HenHoldPosition(Fvector& P) : State(aiHenHoldPosition)
		{
			position.set		(P);
		}
		_HenHoldPosition()			: State(aiHenHoldPosition)
		{
			position.set		(0,0,0);
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	// 		aiHenHoldUnderFire
	class _HenHoldUnderFire			: public State
	{
	public:
		Fvector					position;
		DWORD					nodeID;

		_HenHoldUnderFire(Fvector& P) : State(aiHenHoldUnderFire)
		{
			position.set		(P);
		}
		_HenHoldUnderFire()			: State(aiHenHoldUnderFire)
		{
			position.set		(0,0,0);
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	// 		aiHenFreeHunting,			
	class _HenFreeHunting			: public State
	{
	public:
		_HenFreeHunting()			: State(aiHenFreeHunting)
		{
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	//		aiHenFollowMe
	class _HenFollowMe			: public State
	{
		Fvector					vLook;

		DWORD					hitTime;
		Fvector					hitDir;
	public:
		_HenFollowMe()			: State(aiHenFollowMe)
		{
			hitTime				= 0;
			vLook.set			(0,0,1);
			hitDir.set			(0,0,1);
		}

		void			u_orientate	(DWORD node, Fvector& look);

		virtual BOOL	Parse		(CCustomMonster* Me);
		virtual void	Hit			(Fvector &dir);
	};
	//		aiHenAttack
	class _HenAttack				: public State
	{
		BOOL					bBuildPathToLostEnemy;
	public:
		CEntity*				EnemySaved;			// жертва
		_HenAttack()				: State(aiHenAttack)
		{
			EnemySaved			= 0;
		}
		virtual BOOL	Parse		(CCustomMonster* Me);
	};
	//		aiHenDefend
	class _HenDefend				: public State
	{
		BOOL					bBuildPathToLostEnemy;
	public:
		CEntity*				EnemySaved;			// жертва
		_HenDefend()				: State(aiHenDefend)
		{
			EnemySaved			= 0;
		}
		virtual BOOL	Parse		(CCustomMonster* Me);
	};
	//		aiHenPusuit
	class _HenPursuit				: public State
	{
	public:
		CEntity*				victim;				// жертва
		Fvector					PositionPredicted;
		Fvector					savedPosition;
		DWORD					savedTime;
		DWORD					savedNode;
		BOOL					bDirectPathBuilded;

		_HenPursuit(CEntity* E);
		_HenPursuit();
		virtual BOOL	Parse		(CCustomMonster* Me);
	};
	//		aiHenRetreat
	class _HenRetreat			: public State
	{
	public:
		_HenRetreat()			: State(aiHenRetreat)
		{
		}
		virtual BOOL	Parse		(CCustomMonster* Me);
	};
	//----------------------------------------------------------------------------------
};


class CAI_Hen : public CCustomMonster  
{
	enum ESoundCcount {
		SND_HIT_COUNT=8,
		SND_DIE_COUNT=4
	};
	typedef	CCustomMonster inherited;
	friend	class AI::State;
protected:
	// media
	sound3D				sndHit[SND_HIT_COUNT];
	sound3D				sndDie[SND_DIE_COUNT];
public:
	virtual void		Update					(DWORD DT);
	virtual void		HitSignal				(int amount, Fvector& vLocalDir, CEntity* who);
	virtual void		Die						();
	virtual void		Load					( CInifile* ini, const char* section );

	CAI_Hen			();
	virtual ~CAI_Hen	();
};

#endif