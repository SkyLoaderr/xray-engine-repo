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
	union {
		CObject						*m_tpObjectToGo;
		string32					m_caPatrolPathToGo;
		Fvector						m_tDestinationPosition;
	};
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

							CMovementAction		(StalkerSpace::EBodyState tBodyState, StalkerSpace::EMovementType tMovementType, StalkerSpace::EPathType tPathType, CGameObject *tpObjectToGo)
	{
		m_tBodyState		= tBodyState;
		m_tMovementType		= tMovementType;
		m_tPathType			= tPathType;
		m_tpObjectToGo		= tpObjectToGo;
		m_tGoalType			= eGoalTypeObject;
	}

							CMovementAction		(StalkerSpace::EBodyState tBodyState, StalkerSpace::EMovementType tMovementType, StalkerSpace::EPathType tPathType, LPCSTR caPatrolPathToGo)
	{
		m_tBodyState		= tBodyState;
		m_tMovementType		= tMovementType;
		m_tPathType			= tPathType;
		strcpy				(m_caPatrolPathToGo,m_caPatrolPathToGo);
		m_tGoalType			= eGoalTypePatrolPath;
	}

							CMovementAction		(StalkerSpace::EBodyState tBodyState, StalkerSpace::EMovementType tMovementType, StalkerSpace::EPathType tPathType, Fvector tPosition)
	{
		m_tBodyState		= tBodyState;
		m_tMovementType		= tMovementType;
		m_tPathType			= tPathType;
		m_tDestinationPosition = tPosition;
		m_tGoalType			= eGoalTypePosition;
	}

			void			SetBodyState		(const StalkerSpace::EBodyState tBodyState)
	{
		m_tBodyState		= tBodyState;
	}

			void			SetMovementType		(const StalkerSpace::EMovementType tMovementType)
	{
		m_tMovementType		= tMovementType;
	}

			void			SetPathType			(const StalkerSpace::EPathType tPathType)
	{
		m_tPathType			= tPathType;
	}

			void			SetObjectToGo		(CGameObject *tpObjectToGo)
	{
		m_tpObjectToGo		= tpObjectToGo;
		m_tGoalType			= eGoalTypeObject;
	}

			void			SetPatrolPath		(LPCSTR caPatrolPath)
	{
		strcpy				(m_caPatrolPathToGo,caPatrolPath);
		m_tGoalType			= eGoalTypePatrolPath;
	}

			void			SetPosition			(const Fvector &tPosition)
	{
		m_tDestinationPosition = tPosition;
		m_tGoalType			= eGoalTypePosition;
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
	union {
		CObject						*m_tpObjectToWatch;
		StalkerSpace::ELookType		m_tWatchType;
		Fvector						m_tDirectionToWatch;
	};
	EGoalType						m_tGoalType;

							CWatchAction		()
	{
		m_tpObjectToWatch	= 0;
		m_tWatchType		= eLookTypeDirection;
		m_tDirectionToWatch.set(0,0,0);
		m_tGoalType			= eGoalTypeWatchType;
	}

							CWatchAction		(CObject *tpObjectToWatch)
	{
		SetWatchObject		(tpObjectToWatch);;
	}

							CWatchAction		(StalkerSpace::ELookType tWatchType)
	{
		SetWatchType		(tWatchType);
	}

							CWatchAction		(Fvector tDirection)
	{
		SetWatchDirection	(tDirection);
	}

			void			SetWatchObject		(CObject *tpObjectToWatch)
	{
		m_tpObjectToWatch	= tpObjectToWatch;
		m_tGoalType			= eGoalTypeObject;
	}

			void			SetWatchType		(StalkerSpace::ELookType tWatchType)
	{
		m_tWatchType		= tWatchType;
		m_tGoalType			= eGoalTypeWatchType;
	}

			void			SetWatchDirection	(Fvector tDirection)
	{
		m_tDirectionToWatch = tDirection;
		m_tGoalType			= eGoalTypeDirection;
	}
};

class CAnimationAction : public CAbstractAction {
public:
	enum EGoalType {
		eGoalTypeAnimation = u32(0),
		eGoalTypeMental,
		eGoalTypeDummy = u32(-1),
	};
	union {
		string32					m_caAnimationToPlay;
		StalkerSpace::EMentalState	m_tMentalState;
	};
	EGoalType						m_tGoalType;

							CAnimationAction	()
	{
		strcpy				(m_caAnimationToPlay,"");
		m_tMentalState		= eMentalStateFree;
		m_tGoalType			= eGoalTypeMental;
	}

							CAnimationAction	(LPCSTR caAnimationToPlay)
	{
		SetAnimation		(caAnimationToPlay);
		strcpy				(m_caAnimationToPlay,caAnimationToPlay);
		m_tGoalType			= eGoalTypeAnimation;
	}

							CAnimationAction	(StalkerSpace::EMentalState tMentalState)
	{
		SetMentalState		(tMentalState);
		m_tMentalState		= tMentalState;
		m_tGoalType			= eGoalTypeMental;
	}

			void			SetAnimation		(LPCSTR caAnimationToPlay)
	{
		strcpy				(m_caAnimationToPlay,caAnimationToPlay);
		m_tGoalType			= eGoalTypeAnimation;
	}

			void			SetMentalState		(StalkerSpace::EMentalState tMentalState)
	{
		m_tMentalState		= tMentalState;
		m_tGoalType			= eGoalTypeMental;
	}
};

class CSoundAction : public CAbstractAction {
public:
	enum EGoalType {
		eGoalTypeSound = u32(0),
		eGoalTypeDummy = u32(-1),
	};
	string32						m_caSoundToPlay;
	EGoalType						m_tGoalType;

							CSoundAction		()
	{
		strcpy				(m_caSoundToPlay,"");
		m_tGoalType			= eGoalTypeSound;
	}

							CSoundAction		(LPCSTR caSoundToPlay)
	{
		SetSound			(caSoundToPlay);
	}

			void			SetSound			(LPCSTR caSoundToPlay)
	{
		strcpy				(m_caSoundToPlay,caSoundToPlay);
		m_tGoalType			= eGoalTypeSound;
	}
};

class CLuaGameObject;

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
	EObjectActionType				m_tGoalType;

							CObjectAction		()
	{
		m_tpObject			= 0;
		m_tGoalType			= eObjectActionTypeDummy;
	}

							CObjectAction		(CLuaGameObject *tpLuaGameObject, EObjectActionType tObjectActionType)
	{
		SetObject			(tpLuaGameObject);
		SetObjectAction		(tObjectActionType);
	}

			void			SetObject			(CLuaGameObject *tpLuaGameObject)
	{
		m_tpObject			= (CObject*)tpLuaGameObject;
	}

			void			SetObjectAction		(EObjectActionType tObjectActionType)
	{
		m_tGoalType			= tObjectActionType;
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

	IC		bool			CheckIfMovementCompleted()
	{
		return				(CheckIfActionCompleted(m_tMovementAction));
	}

	IC		bool			CheckIfWatchCompleted()
	{
		return				(CheckIfActionCompleted(m_tWatchAction));
	}

	IC		bool			CheckIfAnimationCompleted()
	{
		return				(CheckIfActionCompleted(m_tAnimationAction));
	}

	IC		bool			CheckIfSoundCompleted()
	{
		return				(CheckIfActionCompleted(m_tSoundAction));
	}

	IC		bool			CheckIfObjectCompleted()
	{
		return				(CheckIfActionCompleted(m_tObjectAction));
	}

	IC		bool			CheckIfTimeOver()
	{
		return((m_tActionCondition.m_tLifeTime >= 0) && ((m_tActionCondition.m_tStartTime + m_tActionCondition.m_tLifeTime) < Level().GetGameTime()));
	}

	IC		bool			CheckIfActionCompleted()
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