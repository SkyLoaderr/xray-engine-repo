////////////////////////////////////////////////////////////////////////////
//	Module 		: ai_script_actions.h
//	Created 	: 30.09.2003
//  Modified 	: 30.09.2003
//	Author		: Dmitriy Iassenev
//	Description : Script actions
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ai_monster_space.h"

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

class CPatrolPathParams {
public:
	enum EPatrolPathStart {
		ePatrolPathFirst = u32(0),
		ePatrolPathLast,
		ePatrolPathNearest,
		ePatrolPathDummy = u32(-1),
	};
	
	enum EPatrolPathStop {
		ePatrolPathStop = u32(0),
		ePatrolPathContinue,
		ePatrolPathInvalid = u32(-1),
	};

	string32						m_caPatrolPathToGo;
	EPatrolPathStart				m_tPatrolPathStart;
	EPatrolPathStop					m_tPatrolPathStop;
	bool							m_bRandom;

							CPatrolPathParams	(LPCSTR caPatrolPathToGo, const EPatrolPathStart tPatrolPathStart = ePatrolPathNearest, const EPatrolPathStop tPatrolPathStop = ePatrolPathContinue, bool bRandom = true)
	{
		strcpy				(m_caPatrolPathToGo,caPatrolPathToGo);
		m_tPatrolPathStart	= tPatrolPathStart;
		m_tPatrolPathStop	= tPatrolPathStop;
		m_bRandom			= bRandom;
	}

	virtual					~CPatrolPathParams	()
	{
	}
};

class CLuaGameObject;

class CMovementAction : public CAbstractAction {
public:
	enum EGoalType {
		eGoalTypeObject = u32(0),
		eGoalTypePatrolPath,
		eGoalTypePathPosition,
		eGoalTypeNoPathPosition,
		eGoalTypeInput,
		eGoalTypeDummy = u32(-1),
	};

	enum EInputKeys {
		eInputKeyNone		= u32(1) << 0,
		eInputKeyForward	= u32(1) << 1,
		eInputKeyBack		= u32(1) << 2,
		eInputKeyLeft		= u32(1) << 3,
		eInputKeyRight		= u32(1) << 4,
		eInputKeyShiftUp	= u32(1) << 5,
		eInputKeyShiftDown	= u32(1) << 6,
		eInputKeyBreaks		= u32(1) << 7,
		eInputKeyEngineOn	= u32(1) << 8,
		eInputKeyEngineOff	= u32(1) << 9,
		eInputKeyDummy		= u32(1) << 10,
	};

	MonsterSpace::EBodyState		m_tBodyState;
	MonsterSpace::EMovementType		m_tMovementType;
	MonsterSpace::EPathType			m_tPathType;
	CObject							*m_tpObjectToGo;
	string32						m_caPatrolPathToGo;
	CPatrolPathParams::EPatrolPathStart	m_tPatrolPathStart;
	CPatrolPathParams::EPatrolPathStop	m_tPatrolPathStop;
	Fvector							m_tDestinationPosition;
	u32								m_tNodeID;
	EGoalType						m_tGoalType;
	float							m_fSpeed;
	bool							m_bRandom;
	EInputKeys						m_tInputKeys;

							CMovementAction		()
	{
		SetInputKeys		(eInputKeyNone);
		SetBodyState		(MonsterSpace::eBodyStateStand);
		SetMovementType		(MonsterSpace::eMovementTypeStand);
		SetPathType			(MonsterSpace::ePathTypeStraight);
		SetPatrolPath		("");
		SetPatrolStart		(CPatrolPathParams::ePatrolPathNearest);
		SetPatrolStop		(CPatrolPathParams::ePatrolPathContinue);
		SetPatrolRandom		(true);
		SetSpeed			(0);
		SetObjectToGo		(0);
		SetPosition			(Fvector().set(0,0,0));
		m_tGoalType			= eGoalTypeDummy;
	}

							CMovementAction		(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, MonsterSpace::EPathType tPathType, CLuaGameObject *tpObjectToGo, float fSpeed = 0.f)
	{
		SetBodyState		(tBodyState);
		SetMovementType		(tMovementType);
		SetPathType			(tPathType);
		SetObjectToGo		(tpObjectToGo);
		SetSpeed			(fSpeed);
	}

							CMovementAction		(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, MonsterSpace::EPathType tPathType, const CPatrolPathParams &tPatrolPathParams, float fSpeed = 0.f)
	{
		SetBodyState		(tBodyState);
		SetMovementType		(tMovementType);
		SetPathType			(tPathType);
		SetPatrolPath		(tPatrolPathParams.m_caPatrolPathToGo);
		SetPatrolStart		(tPatrolPathParams.m_tPatrolPathStart);
		SetPatrolStop		(tPatrolPathParams.m_tPatrolPathStop);
		SetPatrolRandom		(tPatrolPathParams.m_bRandom);
		SetSpeed			(fSpeed);
	}

							CMovementAction		(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, MonsterSpace::EPathType tPathType, const Fvector &tPosition, float fSpeed = 0.f)
	{
		SetBodyState		(tBodyState);
		SetMovementType		(tMovementType);
		SetPathType			(tPathType);
		SetPosition			(tPosition);
		SetSpeed			(fSpeed);
	}

							CMovementAction		(const Fvector &tPosition, float fSpeed)
	{
		SetBodyState		(MonsterSpace::eBodyStateStand);
		SetMovementType		(MonsterSpace::eMovementTypeStand);
		SetPathType			(MonsterSpace::ePathTypeStraight);
		SetPosition			(tPosition);
		SetSpeed			(fSpeed);
		m_tGoalType			= eGoalTypeNoPathPosition;
	}

							CMovementAction		(const EInputKeys tInputKeys, float fSpeed = 0.f)
	{
		SetInputKeys		(tInputKeys);
		SetSpeed			(fSpeed);
	}

			void			SetBodyState		(const MonsterSpace::EBodyState tBodyState)
	{
		m_tBodyState		= tBodyState;
		m_bCompleted		= false;
	}

			void			SetMovementType		(const MonsterSpace::EMovementType tMovementType)
	{
		m_tMovementType		= tMovementType;
		m_bCompleted		= false;
	}

			void			SetPathType			(const MonsterSpace::EPathType tPathType)
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
		m_tGoalType			= eGoalTypePathPosition;
		m_bCompleted		= false;
	}

			void			SetSpeed			(float fSpeed)
	{
		m_fSpeed			= fSpeed;
		m_bCompleted		= false;
	}

			void			SetPatrolStart		(CPatrolPathParams::EPatrolPathStart tPatrolPathStart)
	{
		m_tPatrolPathStart	= tPatrolPathStart;
		m_bCompleted		= false;
	}

			void			SetPatrolStop		(CPatrolPathParams::EPatrolPathStop tPatrolPathStop)
	{
		m_tPatrolPathStop	= tPatrolPathStop;
		m_bCompleted		= false;
	}

			void			SetPatrolRandom		(bool bRandom)
	{
		m_bRandom			= bRandom;
		m_bCompleted		= false;
	}

			void			SetInputKeys		(const EInputKeys tInputKeys)
	{
		m_tInputKeys		= tInputKeys;
		m_tGoalType			= eGoalTypeInput;
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
	MonsterSpace::ELookType			m_tWatchType;
	EGoalType						m_tGoalType;
	Fvector							m_tWatchVector;

							CWatchAction		()
	{
		m_tpObjectToWatch	= 0;
		m_tWatchType		= MonsterSpace::eLookTypePathDirection;
		m_tWatchVector.set	(0,0,0);
		m_tGoalType			= eGoalTypeWatchType;
	}

							CWatchAction		(MonsterSpace::ELookType tWatchType)
	{
		SetWatchType		(tWatchType);
		m_tGoalType			= eGoalTypeWatchType;
	}

							CWatchAction		(MonsterSpace::ELookType tWatchType, const Fvector &tDirection)
	{
		SetWatchDirection	(tDirection);
		SetWatchType		(tWatchType);
	}

							CWatchAction		(MonsterSpace::ELookType tWatchType, CLuaGameObject *tpObjectToWatch)
	{
		SetWatchType		(tWatchType);
		SetWatchObject		(tpObjectToWatch);
	}

			void			SetWatchObject		(CLuaGameObject *tpObjectToWatch);

			void			SetWatchType		(MonsterSpace::ELookType tWatchType)
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
	MonsterSpace::EMentalState		m_tMentalState;
	EGoalType						m_tGoalType;

							CAnimationAction	()
	{
		strcpy				(m_caAnimationToPlay,"");
		m_tMentalState		= MonsterSpace::eMentalStateDanger;
		m_tGoalType			= eGoalTypeMental;
		m_bCompleted		= false;
	}

							CAnimationAction	(LPCSTR caAnimationToPlay)
	{
		SetAnimation		(caAnimationToPlay);
	}

							CAnimationAction	(MonsterSpace::EMentalState tMentalState)
	{
		SetMentalState		(tMentalState);
	}

			void			SetAnimation		(LPCSTR caAnimationToPlay)
	{
		strcpy				(m_caAnimationToPlay,caAnimationToPlay);
		m_tMentalState		= MonsterSpace::eMentalStateDanger;
		m_tGoalType			= eGoalTypeAnimation;
		m_bCompleted		= false;
	}

			void			SetMentalState		(MonsterSpace::EMentalState tMentalState)
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
	string32						m_caBoneName;
	EGoalType						m_tGoalType;
	ref_sound						*m_tpSound;
	bool							m_bLooped;
	bool							m_bStartedToPlay;
	Fvector							m_tSoundPosition;
	Fvector							m_tSoundAngles;

							CSoundAction		()
	{
		strcpy				(m_caSoundToPlay,"");
		strcpy				(m_caBoneName,"");
		m_tGoalType			= eGoalTypeDummy;
		m_bCompleted		= false;
		m_bStartedToPlay	= false;
		m_bLooped			= false;
		m_tpSound			= 0;
		m_tSoundPosition.set(0,0,0);
		m_tSoundAngles.set	(0,0,0);
	}

							CSoundAction		(LPCSTR caSoundToPlay, LPCSTR caBoneName, const Fvector &tPositionOffset = Fvector().set(0,0,0), const Fvector &tAngleOffset = Fvector().set(0,0,0), bool bLooped = false)
	{
		m_bLooped			= bLooped;
		SetBone				(caBoneName);
		SetPosition			(tPositionOffset);
		SetAngles			(tAngleOffset);
		SetSound			(caSoundToPlay);
	}

							CSoundAction		(LPCSTR caSoundToPlay, const Fvector &tPosition, const Fvector &tAngleOffset = Fvector().set(0,0,0), bool bLooped = false)
	{
		m_bLooped			= bLooped;
		SetSound			(caSoundToPlay);
		SetPosition			(tPosition);
		SetAngles			(tAngleOffset);
	}

	virtual					~CSoundAction		()
	{
		if (m_tpSound) {
			m_tpSound->destroy();
			m_tpSound		= 0;
		}
	}

			void			SetSound			(LPCSTR caSoundToPlay)
	{
		strcpy				(m_caSoundToPlay,caSoundToPlay);
		m_tGoalType			= eGoalTypeSoundAttached;
		m_tpSound			= 0;
		m_bStartedToPlay	= false;
		string256			l_caFileName;
		if (FS.exist(l_caFileName,"$game_sounds$",m_caSoundToPlay,".wav")) {
			::Sound->create	(*(m_tpSound = xr_new<ref_sound>()),TRUE,m_caSoundToPlay);
			m_bStartedToPlay= false;
			m_bCompleted	= false;
		}
		else {
			Log				("* [LUA] File not found \"%s\"!",l_caFileName);
			m_tpSound		= 0;
			m_bStartedToPlay= true;
			m_bCompleted	= true;
		}
	}
			void			SetPosition			(const Fvector &tPosition)
	{
		m_tSoundPosition	= tPosition;
		m_tGoalType			= eGoalTypeSoundPosition;
		m_bStartedToPlay	= false;
	}

			void			SetBone				(LPCSTR caBoneName)
	{
		strcpy				(m_caBoneName,caBoneName);
		m_bStartedToPlay	= false;
	}
	
			void			SetAngles			(const Fvector &tAngles)
	{
		m_tSoundAngles		= tAngles;
		m_bStartedToPlay	= false;
	}
};

class CParticleParams {
public:
	Fvector							m_tParticlePosition;
	Fvector							m_tParticleAngles;
	Fvector							m_tParticleVelocity;

							CParticleParams(const Fvector &tPositionOffset = Fvector().set(0,0,0), const Fvector &tAnglesOffset = Fvector().set(0,0,0), const Fvector &tVelocity = Fvector().set(0,0,0))
	{
		m_tParticlePosition	= tPositionOffset;
		m_tParticleAngles	= tAnglesOffset;
		m_tParticleVelocity	= tVelocity;
	}

	virtual					~CParticleParams()
	{
	}
};

class CParticleAction : public CAbstractAction {
public:
	enum EGoalType {
		eGoalTypeParticleAttached = u32(0),
		eGoalTypeParticlePosition,
		eGoalTypeDummy = u32(-1),
	};
	string32						m_caParticleToRun;
	string32						m_caBoneName;
	EGoalType						m_tGoalType;
	CParticlesObject				*m_tpParticleSystem;
	bool							m_bStartedToPlay;
	Fvector							m_tParticlePosition;
	Fvector							m_tParticleAngles;
	Fvector							m_tParticleVelocity;

							CParticleAction		()
	{
		strcpy				(m_caParticleToRun,"");
		strcpy				(m_caBoneName,"");
		m_tGoalType			= eGoalTypeDummy;
		m_bCompleted		= false;
		m_bStartedToPlay	= false;
		m_tpParticleSystem	= 0;
		m_tParticlePosition.set	(0,0,0);
		m_tParticleAngles.set	(0,0,0);
		m_tParticleVelocity.set	(0,0,0);
	}

#pragma todo("Dima to Dima : Add velocity for particle systems here")
							CParticleAction		(LPCSTR caPartcileToRun, LPCSTR caBoneName, const CParticleParams &tParticleParams = CParticleParams(), bool bAutoRemove = false)
	{
		SetBone				(caBoneName);
		SetPosition			(tParticleParams.m_tParticlePosition);
		SetAngles			(tParticleParams.m_tParticleAngles);
		SetVelocity			(tParticleParams.m_tParticleVelocity);
		SetParticle			(caPartcileToRun,bAutoRemove);
	}

							CParticleAction		(LPCSTR caPartcileToRun, const CParticleParams &tParticleParams = CParticleParams(), bool bAutoRemove = false)
	{
		SetParticle			(caPartcileToRun,bAutoRemove);
		SetPosition			(tParticleParams.m_tParticlePosition);
		SetAngles			(tParticleParams.m_tParticleAngles);
		SetVelocity			(tParticleParams.m_tParticleVelocity);
	}

	virtual					~CParticleAction	();

			void			SetParticle			(LPCSTR caParticleToRun, bool bAutoRemove);
			
			void			SetPosition			(const Fvector &tPosition)
	{
		m_tParticlePosition	= tPosition;
		m_tGoalType			= eGoalTypeParticlePosition;
		m_bStartedToPlay	= false;
		m_bCompleted		= false;
	}

			void			SetBone				(LPCSTR caBoneName)
	{
		strcpy				(m_caBoneName,caBoneName);
		m_bStartedToPlay	= false;
		m_bCompleted		= false;
	}

			void			SetAngles			(const Fvector &tAngleOffset)
	{
		m_tParticleAngles	= tAngleOffset;
		m_bStartedToPlay	= false;
		m_bCompleted		= false;
	}

			void			SetVelocity			(const Fvector &tVelocity)
	{
		m_tParticleVelocity	= tVelocity;
		m_bStartedToPlay	= false;
		m_bCompleted		= false;
	}
};

class CObjectAction : public CAbstractAction {
public:
	CObject							*m_tpObject;
	MonsterSpace::EObjectAction		m_tGoalType;
	u32								m_dwQueueSize;

							CObjectAction		()
	{
		m_tpObject			= 0;
		m_tGoalType			= MonsterSpace::eObjectActionIdle;
		m_bCompleted		= false;
	}

							CObjectAction		(CLuaGameObject *tpLuaGameObject, MonsterSpace::EObjectAction tObjectActionType, u32 dwQueueSize = 1)
	{
		SetObject			(tpLuaGameObject);
		SetObjectAction		(tObjectActionType);
		SetQueueSize		(dwQueueSize);
	}

			void			SetObject			(CLuaGameObject *tpLuaGameObject);

			void			SetObjectAction		(MonsterSpace::EObjectAction tObjectActionType)
	{
		m_tGoalType			= tObjectActionType;
		m_bCompleted		= false;
	}

			void			SetQueueSize		(u32 dwQueueSize)
	{
		m_dwQueueSize		= dwQueueSize;
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
		PARTICLE_FLAG	= u32(1 << 4),
		OBJECT_FLAG		= u32(1 << 5),
		TIME_FLAG		= u32(1 << 6),
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
	CParticleAction					m_tParticleAction;
	CObjectAction					m_tObjectAction;
	CActionCondition				m_tActionCondition;
	bool							m_bFirstTime;

							CEntityAction		()
	{
		m_bFirstTime				= true;
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

	IC		void			SetAction(const CParticleAction &tParticleAction)
	{
		SetAction			(tParticleAction,m_tParticleAction);
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

	IC		bool			CheckIfParticleCompleted() const
	{
		return				(CheckIfActionCompleted(m_tParticleAction));
	}

	IC		bool			CheckIfObjectCompleted() const
	{
		return				(CheckIfActionCompleted(m_tObjectAction));
	}

	IC		bool			CheckIfTimeOver()
	{
		if (m_bFirstTime) {
			m_tActionCondition.m_tStartTime	= Level().GetGameTime();
			m_bFirstTime	= false;
		}
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
		if ((CActionCondition::PARTICLE_FLAG	& m_tActionCondition.m_dwFlags)	&& CheckIfParticleCompleted	())
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
			&& CheckIfParticleCompleted()
			&& CheckIfObjectCompleted()
			)
			return			(true);
		else
			return			(false);
	}
};