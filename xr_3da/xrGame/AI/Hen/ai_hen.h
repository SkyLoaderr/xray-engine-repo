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
//******************************************************************
// Mode "Die"
//******************************************************************
	class _HenDie					: public State
	{
		// hit data
		DWORD					hitTime;
		Fvector					hitDir;
		// sense data
		DWORD					senseTime;
		Fvector					senseDir;
	public:
		_HenDie()					: State(aiHenDie)
		{
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
//******************************************************************
// Mode "Under Fire!"
//******************************************************************
	class _HenUnderFire					: public State
	{
		// hit data
		DWORD					hitTime;
		Fvector					hitDir;
		// sense data
		DWORD					senseTime;
		Fvector					senseDir;
	public:
		_HenUnderFire();
		virtual BOOL Parse(CCustomMonster* Me);
		virtual void Hit(Fvector &dir);
		virtual void Sense(Fvector &dir);
	};
//******************************************************************
// Mode "Sense Something"
//******************************************************************
	class _HenSenseSomething			: public State
	{
		// hit data
		DWORD					hitTime;
		Fvector					hitDir;
		// sense data
		DWORD					senseTime;
		Fvector					senseDir;
	public:
		_HenSenseSomething();
		virtual BOOL Parse(CCustomMonster* Me);
		virtual void Hit(Fvector &dir);
		virtual void Sense(Fvector &dir);
	};
//******************************************************************
// Mode "Follow me"
//******************************************************************
	class _HenFollowMe			: public State
	{
		// hit data
		DWORD					hitTime;
		Fvector					hitDir;
		// sense data
		DWORD					senseTime;
		Fvector					senseDir;
	public:
		_HenFollowMe();
		virtual BOOL Parse(CCustomMonster* Me);
		virtual void Hit(Fvector &dir);
		virtual void Sense(Fvector &dir);
	};
//******************************************************************
// Mode "Attack!"
//******************************************************************
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
//******************************************************************
// Mode "Free Hunting"
//******************************************************************
	class _HenFreeHunting			: public State
	{
		DWORD					hitTime;
		Fvector					hitDir;
	public:
		_HenFreeHunting()			: State(aiHenFreeHunting)
		{
			hitTime				= 0;
			hitDir.set			(0,0,1);
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
		virtual void	Hit			(Fvector &dir);
	};
//******************************************************************
// Mode "Pursuit"
//******************************************************************
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
//******************************************************************
// Mode "Retreat"
//******************************************************************
	class _HenRetreat			: public State
	{
	public:
		_HenRetreat()			: State(aiHenRetreat)
		{
		}
		virtual BOOL	Parse		(CCustomMonster* Me);
	};
//******************************************************************
// Mode "Go In This Direction"
//******************************************************************
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
//******************************************************************
// Mode "Go To That Position"
//******************************************************************
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
//******************************************************************
// Mode "Hold That Position"
//******************************************************************
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
//******************************************************************
// Mode "Under Fire Holding Position"
//******************************************************************
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
//******************************************************************
// Mode "Defend!"
//******************************************************************
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
	virtual void		SenseSignal				(int amount, Fvector& vLocalDir, CEntity* who);
	virtual void		Die						();
	virtual void		Load					( CInifile* ini, const char* section );

	CAI_Hen			();
	virtual ~CAI_Hen	();
};

#endif