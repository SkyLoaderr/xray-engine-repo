#pragma once

class CCustomMonster;

namespace AI {
	enum AI_States 
	{
		aiDie					=0,
			
		aiGoInThisDirection,	// идти по направлению: "бесконечно" 
		aiGoToThatPosition,		// идти в заданную позицию:			
		aiHoldPosition,			// удерживать позицию				
		aiFreeHunting,			// свободная охота					
		aiFollowBackup,			// идти со мной / ко мне (нужна помощь)		

		aiAttack,				// attack mode
		aiPursuit,				// преследование после потери цели
		aiRetreating,			// retreating
			
		aiLAST,
		aiFORCEDWORD			= DWORD(-1)
	};
	class State
	{
	public:
		AI_States				Type;
		DWORD					svTime_Create;
		
		State(AI_States T) : Type(T)	{ svTime_Create = Level().timeServer(); };
		
		static State*	Create	(DWORD type);

		virtual BOOL	Parse	(CCustomMonster* Me)=0;
		virtual void	Hit		(Fvector &dir)	{};
	};
	//----------------------------------------------------------------------------------
	class _Die					: public State
	{
	public:
		_Die()					: State(aiDie)
		{
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	//----------------------------------------------------------------------------------
	class _GoDirection			: public State
	{
	public:
		Fvector					direction;

		_GoDirection(Fvector& D) : State(aiGoInThisDirection)
		{
			direction.set		(D);
		}
		_GoDirection()			: State(aiGoInThisDirection)
		{
			direction.set		(0,0,0);
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	//----------------------------------------------------------------------------------
	class _GoPosition			: public State
	{
	public:
		Fvector					position;
		DWORD					nodeID;

		_GoPosition(Fvector& P) : State(aiGoToThatPosition)
		{
			position.set		(P);
		}
		_GoPosition()			: State(aiGoToThatPosition)
		{
			position.set		(0,0,0);
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	//----------------------------------------------------------------------------------
	class _HoldPosition			: public State
	{
	public:
		Fvector					position;
		DWORD					nodeID;

		_HoldPosition(Fvector& P) : State(aiHoldPosition)
		{
			position.set		(P);
		}
		_HoldPosition()			: State(aiHoldPosition)
		{
			position.set		(0,0,0);
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	//----------------------------------------------------------------------------------
	class _FreeHunting			: public State
	{
	public:
		_FreeHunting()			: State(aiFreeHunting)
		{
		}
		virtual BOOL	Parse	(CCustomMonster* Me);
	};
	//----------------------------------------------------------------------------------
	const DWORD _FB_hit_RelevantTime	= 10;
	const float _FB_look_speed			= PI;
	const float _FB_invisible_hscale	= 2.f;
	class _FollowBackup			: public State
	{
		Fvector					vLook;

		DWORD					hitTime;
		Fvector					hitDir;
	public:
		_FollowBackup()			: State(aiFollowBackup)
		{
			hitTime				= 0;
			vLook.set			(0,0,1);
			hitDir.set			(0,0,1);
		}

		void			u_orientate	(DWORD node, Fvector& look);

		virtual BOOL	Parse		(CCustomMonster* Me);
		virtual void	Hit			(Fvector &dir);
	};
	//----------------------------------------------------------------------------------
	const float _A_look_speed			= PI;
	class _Attack				: public State
	{
		BOOL					bBuildPathToLostEnemy;
	public:
		CEntity*				EnemySaved;			// жертва
		_Attack()				: State(aiAttack)
		{
			EnemySaved			= 0;
		}
		virtual BOOL	Parse		(CCustomMonster* Me);
	};
	//----------------------------------------------------------------------------------
	class _Pursuit				: public State
	{
	public:
		CEntity*				victim;				// жертва
		Fvector					PositionPredicted;
		Fvector					savedPosition;
		DWORD					savedTime;
		DWORD					savedNode;
		BOOL					bDirectPathBuilded;

		_Pursuit(CEntity* E);
		_Pursuit();
		virtual BOOL	Parse		(CCustomMonster* Me);
	};
	//----------------------------------------------------------------------------------
	class _Retreating			: public State
	{
	public:
		_Retreating()			: State(aiRetreating)
		{
		}
		virtual BOOL	Parse		(CCustomMonster* Me);
	};
	//----------------------------------------------------------------------------------
};
