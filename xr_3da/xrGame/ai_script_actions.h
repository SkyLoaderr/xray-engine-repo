////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_actions.h
//	Created 	: 30.09.2003
//  Modified 	: 30.09.2003
//	Author		: Dmitriy Iassenev
//	Description : Script actions
////////////////////////////////////////////////////////////////////////////

#pragma once

class CAbstractAction {
public:
	bool							m_bCompleted;

							CAbstractAction		()
	{
		m_bCompleted		= true;
	}

	virtual					~CAbstractAction	()
	{
	}
};

class CLuaGameObject;

class CMovementAction : public CAbstractAction {
public:
	enum EGoalType {
		eGoalTypeObject = u32(0),
		eGoalTypePatrolPath,
		eGoalTypePosition,
		eGoalTypeDummy = u32(-1),
	};

	StalkerSpace::EBodyState		m_tBodyState;
	StalkerSpace::EMovementType		m_tMovementType;
	StalkerSpace::EPathType			m_tPathType;
	CObject							*m_tpObjectToGo;
	string32						m_caPatrolPathToGo;
	Fvector							m_tDestinationPosition;
	u32								m_tNodeID;
	EGoalType						m_tGoalType;

							CMovementAction		()
	{
		m_tBodyState		= eBodyStateStand;
		m_tMovementType		= eMovementTypeStand;
		m_tPathType			= ePathTypeStraight;

		m_tpObjectToGo		= 0;
		strcpy				(m_caPatrolPathToGo,"");
		m_tDestinationPosition.set(0,0,0);
		m_tGoalType			= eGoalTypeDummy;
	}

							CMovementAction		(StalkerSpace::EBodyState tBodyState, StalkerSpace::EMovementType tMovementType, StalkerSpace::EPathType tPathType, CLuaGameObject *tpObjectToGo);

							CMovementAction		(StalkerSpace::EBodyState tBodyState, StalkerSpace::EMovementType tMovementType, StalkerSpace::EPathType tPathType, LPCSTR caPatrolPathToGo)
	{
		SetBodyState		(tBodyState);
		SetMovementType		(tMovementType);
		SetPathType			(tPathType);
		SetPatrolPath		(caPatrolPathToGo);
	}

							CMovementAction		(StalkerSpace::EBodyState tBodyState, StalkerSpace::EMovementType tMovementType, StalkerSpace::EPathType tPathType, const Fvector &tPosition)
	{
		SetBodyState		(tBodyState);
		SetMovementType		(tMovementType);
		SetPathType			(tPathType);
		SetPosition			(tPosition);
	}

			void			SetBodyState		(const StalkerSpace::EBodyState tBodyState)
	{
		m_tBodyState		= tBodyState;
		m_bCompleted		= false;
	}

			void			SetMovementType		(const StalkerSpace::EMovementType tMovementType)
	{
		m_tMovementType		= tMovementType;
		m_bCompleted		= false;
	}

			void			SetPathType			(const StalkerSpace::EPathType tPathType)
	{
		m_tPathType			= tPathType;
		m_bCompleted		= false;
	}

			void			SetObjectToGo		(CLuaGameObject *tpObjectToGo);

			void			SetPatrolPath		(LPCSTR caPatrolPath)
	{
		strcpy				(m_caPatrolPathToGo,caPatrolPath);
		m_tGoalType			= eGoalTypePatrolPath;
		m_bCompleted		= false;
	}

			void			SetPosition			(const Fvector &tPosition)
	{
		m_tDestinationPosition = tPosition;
		m_tGoalType			= eGoalTypePosition;
		m_bCompleted		= false;
	}
};

class CWatchAction : public CAbstractAction {
public:
	enum EGoalType {
		eGoalTypeObject = u32(0),
		eGoalTypeWatchType,
		eGoalTypeDirection,
		eGoalTypeDummy = u32(-1),
	};
	CObject							*m_tpObjectToWatch;
	StalkerSpace::ELookType			m_tWatchType;
	EGoalType						m_tGoalType;
	Fvector							m_tWatchVector;

							CWatchAction		()
	{
		m_tpObjectToWatch	= 0;
		m_tWatchType		= eLookTypePathDirection;
		m_tWatchVector.set	(0,0,0);
		m_tGoalType			= eGoalTypeWatchType;
	}

							CWatchAction		(StalkerSpace::ELookType tWatchType)
	{
		SetWatchType		(tWatchType);
		m_tGoalType			= eGoalTypeWatchType;
	}

							CWatchAction		(StalkerSpace::ELookType tWatchType, const Fvector &tDirection)
	{
		SetWatchDirection	(tDirection);
		SetWatchType		(tWatchType);
	}

							CWatchAction		(StalkerSpace::ELookType tWatchType, CLuaGameObject *tpObjectToWatch)
	{
		SetWatchType		(tWatchType);
		SetWatchObject		(tpObjectToWatch);
	}

			void			SetWatchObject		(CLuaGameObject *tpObjectToWatch);

			void			SetWatchType		(StalkerSpace::ELookType tWatchType)
	{
		m_tWatchType		= tWatchType;
		m_bCompleted		= false;
	}

			void			SetWatchDirection	(const Fvector &tDirection)
	{
		m_tWatchVector		= tDirection;
		m_tGoalType			= eGoalTypeDirection;
		m_bCompleted		= false;
	}
};

class CAnimationAction : public CAbstractAction {
public:
	enum EGoalType {
		eGoalTypeAnimation = u32(0),
		eGoalTypeMental,
		eGoalTypeDummy = u32(-1),
	};
	string32						m_caAnimationToPlay;
	StalkerSpace::EMentalState		m_tMentalState;
	EGoalType						m_tGoalType;

							CAnimationAction	()
	{
		strcpy				(m_caAnimationToPlay,"");
		m_tMentalState		= eMentalStateDanger;
		m_tGoalType			= eGoalTypeMental;
		m_bCompleted		= false;
	}

							CAnimationAction	(LPCSTR caAnimationToPlay)
	{
		SetAnimation		(caAnimationToPlay);
	}

							CAnimationAction	(StalkerSpace::EMentalState tMentalState)
	{
		SetMentalState		(tMentalState);
	}

			void			SetAnimation		(LPCSTR caAnimationToPlay)
	{
		strcpy				(m_caAnimationToPlay,caAnimationToPlay);
		m_tMentalState		= eMentalStateDanger;
		m_tGoalType			= eGoalTypeAnimation;
		m_bCompleted		= false;
	}

			void			SetMentalState		(StalkerSpace::EMentalState tMentalState)
	{
		m_tMentalState		= tMentalState;
		m_tGoalType			= eGoalTypeMental;
		m_bCompleted		= false;
	}
};

class CSoundAction : public CAbstractAction {
public:
	enum EGoalType {
		eGoalTypeSoundAttached = u32(0),
		eGoalTypeSoundPosition,
		eGoalTypeDummy = u32(-1),
	};
	string32						m_caSoundToPlay;
	EGoalType						m_tGoalType;
	ref_sound						*m_tpSound;
	bool							m_bStartedToPlay;
	Fvector							m_tSoundPosition;

							CSoundAction		()
	{
		strcpy				(m_caSoundToPlay,"");
		m_tGoalType			= eGoalTypeDummy;
		m_bCompleted		= false;
		m_bStartedToPlay	= false;
		m_tpSound			= 0;
	}

							CSoundAction		(LPCSTR caSoundToPlay)
	{
		SetSound			(caSoundToPlay);
	}

							CSoundAction		(LPCSTR caSoundToPlay, const Fvector &tPosition)
	{
		SetSound			(caSoundToPlay);
		SetPosition			(tPosition);
	}

			void			SetSound			(LPCSTR caSoundToPlay)
	{
		strcpy				(m_caSoundToPlay,caSoundToPlay);
		m_tGoalType			= eGoalTypeSoundAttached;
		m_tpSound			= 0;
		m_bStartedToPlay	= false;
		string256			l_caFileName;
		if (FS.exist(l_caFileName,"$game_sounds$",m_caSoundToPlay,".wav")) {
			::Sound->create	(*(m_tpSound = xr_new<ref_sound>()),TRUE,m_caSoundToPlay,0);
			m_bStartedToPlay= true;
			m_bCompleted	= true;
		}
		else {
			Log				("* [LUA] File not found \"%s\"!",l_caFileName);
			m_tpSound		= 0;
			m_bCompleted	= false;
		}
	}
			void			SetPosition			(const Fvector &tPosition)
	{
		m_tSoundPosition	= tPosition;
		m_tGoalType			= eGoalTypeSoundPosition;
		m_bStartedToPlay	= false;
	}
};

class CObjectAction : public CAbstractAction {
public:
	enum EObjectActionType {
		eObjectActionTypeShow = u32(0),
		eObjectActionTypeHide,
		eObjectActionTypeUse1,
		eObjectActionTypeUse2,
		eObjectActionTypeDummy = u32(-1),
	};
	CObject							*m_tpObject;
	StalkerSpace::EObjectAction		m_tGoalType;

							CObjectAction		()
	{
		m_tpObject			= 0;
		m_tGoalType			= eObjectActionIdle;
		m_bCompleted		= false;
	}

							CObjectAction		(CLuaGameObject *tpLuaGameObject, StalkerSpace::EObjectAction tObjectActionType)
	{
		SetObject			(tpLuaGameObject);
		SetObjectAction		(tObjectActionType);
	}

			void			SetObject			(CLuaGameObject *tpLuaGameObject);

			void			SetObjectAction		(StalkerSpace::EObjectAction tObjectActionType)
	{
		m_tGoalType			= tObjectActionType;
		m_bCompleted		= false;
	}
};

class CActionCondition {
public:
	enum EActionFlags {
		MOVEMENT_FLAG	= u32(1 << 0),
		WATCH_FLAG		= u32(1 << 1),
		ANIMATION_FLAG	= u32(1 << 2),
		SOUND_FLAG		= u32(1 << 3),
		OBJECT_FLAG		= u32(1 << 4),
		TIME_FLAG		= u32(1 << 5),
	};
	u32								m_dwFlags;
	_TIME_ID						m_tLifeTime;
	_TIME_ID						m_tStartTime;
							CActionCondition		()
	{
		m_dwFlags			= 0;
		m_tLifeTime			= _TIME_ID(-1);
		m_tStartTime		= Level().GetGameTime();
	}

							CActionCondition		(u32 dwFlags, double dTime = -1)
	{
		m_dwFlags			= dwFlags;
		m_tLifeTime			= _TIME_ID(dTime);
		m_tStartTime		= Level().GetGameTime();
	}
};

class CEntityAction {
public:
	CMovementAction					m_tMovementAction;
	CWatchAction					m_tWatchAction;
	CAnimationAction				m_tAnimationAction;
	CSoundAction					m_tSoundAction;
	CObjectAction					m_tObjectAction;
	CActionCondition				m_tActionCondition;

							CEntityAction		()
	{
	}

	virtual					~CEntityAction		()
	{
	}

	template<typename T>
	IC		void			SetAction(const T &t, T &tt)
	{
		tt					= t;
	}

	IC		void			SetAction(const CMovementAction &tMovementAction)
	{
		SetAction			(tMovementAction,m_tMovementAction);
	}

	IC		void			SetAction(const CWatchAction &tWatchAction)
	{
		SetAction			(tWatchAction,m_tWatchAction);
	}

	IC		void			SetAction(const CAnimationAction &tAnimationAction)
	{
		SetAction			(tAnimationAction,m_tAnimationAction);
	}

	IC		void			SetAction(const CSoundAction &tSoundAction)
	{
		SetAction			(tSoundAction,m_tSoundAction);
	}

	IC		void			SetAction(const CObjectAction &tObjectAction)
	{
		SetAction			(tObjectAction,m_tObjectAction);
	}

	IC		void			SetAction(const CActionCondition &tActionCondition)
	{
		SetAction			(tActionCondition,m_tActionCondition);
	}

	IC		bool			CheckIfActionCompleted(const CAbstractAction &tAbstractAction) const
	{
		return				(tAbstractAction.m_bCompleted);
	}

	IC		bool			CheckIfMovementCompleted() const
	{
		return				(CheckIfActionCompleted(m_tMovementAction));
	}

	IC		bool			CheckIfWatchCompleted() const
	{
		return				(CheckIfActionCompleted(m_tWatchAction));
	}

	IC		bool			CheckIfAnimationCompleted() const
	{
		return				(CheckIfActionCompleted(m_tAnimationAction));
	}

	IC		bool			CheckIfSoundCompleted() const
	{
		return				(CheckIfActionCompleted(m_tSoundAction));
	}

	IC		bool			CheckIfObjectCompleted() const
	{
		return				(CheckIfActionCompleted(m_tObjectAction));
	}

	IC		bool			CheckIfTimeOver() const
	{
		return((m_tActionCondition.m_tLifeTime >= 0) && ((m_tActionCondition.m_tStartTime + m_tActionCondition.m_tLifeTime) < Level().GetGameTime()));
	}

	IC		bool			CheckIfActionCompleted() const
	{
		if ((CActionCondition::MOVEMENT_FLAG	& m_tActionCondition.m_dwFlags)	&& CheckIfMovementCompleted	())
			return			(true);
		if ((CActionCondition::WATCH_FLAG		& m_tActionCondition.m_dwFlags)	&& CheckIfWatchCompleted	())
			return			(true);
		if ((CActionCondition::ANIMATION_FLAG	& m_tActionCondition.m_dwFlags)	&& CheckIfAnimationCompleted())
			return			(true);
		if ((CActionCondition::SOUND_FLAG		& m_tActionCondition.m_dwFlags)	&& CheckIfSoundCompleted	())
			return			(true);
		if ((CActionCondition::OBJECT_FLAG		& m_tActionCondition.m_dwFlags)	&& CheckIfObjectCompleted	())
			return			(true);
		if ((CActionCondition::TIME_FLAG		& m_tActionCondition.m_dwFlags)	&& CheckIfTimeOver			())
			return			(true);
		if (!m_tActionCondition.m_dwFlags && (m_tActionCondition.m_tLifeTime < 0) 
			&& CheckIfMovementCompleted()
			&& CheckIfWatchCompleted()
			&& CheckIfAnimationCompleted()
			&& CheckIfSoundCompleted()
			&& CheckIfObjectCompleted()
			)
			return			(true);
		else
			return			(false);
	}
};