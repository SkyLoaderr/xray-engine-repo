#pragma once

// refs
class	ENGINE_API					CSound;
class	ENGINE_API					CObject;

const	u32 soundEventPulse			= 500;		// ms
const	u32 soundUndefinedHandle	= u32(-1);

// definition (Sound)
struct	ENGINE_API	sound
{
	int								handle;
	CSound*							feedback;
	int								g_type;
	CObject*						g_object;
	sound()							{ handle = soundUndefinedHandle; feedback=0; g_type=0; g_object=0; }
};

// definition (Sound Interface)
class ENGINE_API	CSound_interface	
	: public pureFrame
{
public:
	// General
	virtual void					Initialize				( )																						= 0;
	virtual void					Destroy					( )																						= 0;
	virtual void					Restart					( )																						= 0;
	virtual BOOL					isPresent				( )																						= 0;

	// Sound interface
	virtual void					Create					( sound& S, BOOL _3D,	LPCSTR fName,	BOOL bCtrlFreq=FALSE,	int		type=0)			= 0;
	virtual void					Play					( sound& S, CObject* O,								BOOL bLoop=false,	int	iLoopCnt=0)	= 0;
	virtual void					Play_Unlimited			( sound& S, CObject* O,								BOOL bLoop=false,	int	iLoopCnt=0)	= 0;
	virtual void					PlayAtPos				( sound& S, CObject* O,		const Fvector &pos,		BOOL bLoop=false,	int iLoopCnt=0)	= 0;
	virtual void					PlayAtPos_Unlimited		( sound& S, CObject* O,		const Fvector &pos,		BOOL bLoop=false,	int iLoopCnt=0)	= 0;
	virtual void					Delete					( sound& S)																				= 0;
	virtual void					SetGeometry				( CDB::MODEL* M )																		= 0;
	virtual CDB::MODEL* 			GetGeometry				( )																						= 0;
	virtual BOOL					IsOccluded				( Fvector& P, float R, Fvector* occ )													= 0;

	// Stream interface
	virtual CSoundStream*			CreateStream			( LPCSTR fName )																		= 0;
	virtual void					DeleteStream			( CSoundStream* pSnd )																	= 0;

	virtual void					OnFrame					( )																						= 0;
};

extern ENGINE_API CSound_interface*		Sound;
