////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_rat.h
//	Created 	: 05.04.2002
//  Modified 	: 05.04.2002
//	Author		: Dmitriy Iassenev
//	Description : AI Behaviour for monster "Rat"
////////////////////////////////////////////////////////////////////////////

#ifndef __XRAY_AI_RAT__
#define __XRAY_AI_RAT__

#include "CustomMonster.h"

namespace AI {
	// 		aiRatDie
	class _RatDie					: public State
	{
	public:
		_RatDie()					: State(aiRatDie)
		{
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	// 		aiRatUnderFire
	class _RatUnderFire					: public State
	{
	public:
		_RatUnderFire()					: State(aiRatUnderFire)
		{
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	// 		aiRatGoInThisDirection
	class _RatGoDirection			: public State
	{
	public:
		Fvector					direction;

		_RatGoDirection(Fvector& D) : State(aiRatGoInThisDirection)
		{
			direction.set		(D);
		}
		_RatGoDirection()			: State(aiRatGoInThisDirection)
		{
			direction.set		(0,0,0);
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	// 		aiRatGoToThatPosition
	class _RatGoPosition			: public State
	{
	public:
		Fvector					position;
		DWORD					nodeID;

		_RatGoPosition(Fvector& P) : State(aiRatGoToThatPosition)
		{
			position.set		(P);
		}
		_RatGoPosition()			: State(aiRatGoToThatPosition)
		{
			position.set		(0,0,0);
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	// 		aiRatHoldition
	class _RatHoldPosition			: public State
	{
	public:
		Fvector					position;
		DWORD					nodeID;

		_RatHoldPosition(Fvector& P) : State(aiRatHoldPosition)
		{
			position.set		(P);
		}
		_RatHoldPosition()			: State(aiRatHoldPosition)
		{
			position.set		(0,0,0);
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	// 		aiRatHoldUnderFire
	class _RatHoldUnderFire			: public State
	{
	public:
		Fvector					position;
		DWORD					nodeID;

		_RatHoldUnderFire(Fvector& P) : State(aiRatHoldUnderFire)
		{
			position.set		(P);
		}
		_RatHoldUnderFire()			: State(aiRatHoldUnderFire)
		{
			position.set		(0,0,0);
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	// 		aiRatFreeHunting,			
	class _RatFreeHunting			: public State
	{
	public:
		_RatFreeHunting()			: State(aiRatFreeHunting)
		{
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	//		aiRatFollowMe
	class _RatFollowMe			: public State
	{
		Fvector					vLook;

		DWORD					hitTime;
		Fvector					hitDir;
	public:
		_RatFollowMe()			: State(aiRatFollowMe)
		{
			hitTime				= 0;
			vLook.set			(0,0,1);
			hitDir.set			(0,0,1);
		}

		void			u_orientate	(DWORD node, Fvector& look);

		virtual BOOL	Parse		(CCustomMonster* Me);
		virtual void	Hit			(Fvector &dir);
	};
	//		aiRatAttack
	class _RatAttack				: public State
	{
		BOOL					bBuildPathToLostEnemy;
	public:
		CEntity*				EnemySaved;			// жертва
		_RatAttack()				: State(aiRatAttack)
		{
			EnemySaved			= 0;
		}
		virtual BOOL	Parse		(CCustomMonster* Me);
	};
	//		aiRatDefend
	class _RatDefend				: public State
	{
		BOOL					bBuildPathToLostEnemy;
	public:
		CEntity*				EnemySaved;			// жертва
		_RatDefend()				: State(aiRatDefend)
		{
			EnemySaved			= 0;
		}
		virtual BOOL	Parse		(CCustomMonster* Me);
	};
	//		aiRatPusuit
	class _RatPursuit				: public State
	{
	public:
		CEntity*				victim;				// жертва
		Fvector					PositionPredicted;
		Fvector					savedPosition;
		DWORD					savedTime;
		DWORD					savedNode;
		BOOL					bDirectPathBuilded;

		_RatPursuit(CEntity* E);
		_RatPursuit();
		virtual BOOL	Parse		(CCustomMonster* Me);
	};
	//		aiRatRetreat
	class _RatRetreat			: public State
	{
	public:
		_RatRetreat()			: State(aiRatRetreat)
		{
		}
		virtual BOOL	Parse		(CCustomMonster* Me);
	};
	//----------------------------------------------------------------------------------
};


class CAI_Rat : public CCustomMonster  
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

	CAI_Rat			();
	virtual ~CAI_Rat	();
};

#endif