#pragma once

// refs
class	ENGINE_API					CSound;
class	ENGINE_API					CObject;
class	ENGINE_API					CSound_interface;
class	ENGINE_API					CSound_stream_interface;

const	u32 soundEventPulse			= 500;		// ms
const	u32 soundUndefinedHandle	= u32(-1);

// definition (Sound Structure)
struct	ENGINE_API	sound
{
	u32								handle;
	CSound_interface*				feedback;
	int								g_type;
	CObject*						g_object;
	sound()							{ handle = soundUndefinedHandle; feedback=0; g_type=0; g_object=0; }
};

// definition (Sound Interface)
class ENGINE_API	CSound_interface
{
public:
	virtual void					Play					(sound* P, BOOL bLoop=false, int lcnt=0)					= 0;
	virtual void					Rewind					()															= 0;
	virtual void					Stop					(void)														= 0;
	virtual void					SetPosition				(const Fvector &pos)										= 0;
	virtual void					SetFrequency			(u32 freq)													= 0;
	virtual void					SetFrequencyScale		(float scale)												= 0;
	virtual void					SetMinMax				(float min, float max)										= 0;
	virtual void					SetVolume				(float vol)													= 0;
};

class ENGINE_API	CSound_stream_interface
{
public:
};

// definition (Sound Manager Interface)
class ENGINE_API	CSound_manager_interface	
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
	virtual CSound_stream_interface*CreateStream			( LPCSTR fName )																		= 0;
	virtual void					DeleteStream			( CSound_stream_interface* pSnd )														= 0;

	virtual void					OnFrame					( )																						= 0;
};

extern ENGINE_API CSound_manager_interface*		Sound;
