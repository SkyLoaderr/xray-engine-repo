#ifndef SoundH
#define SoundH
#pragma once

#ifdef XRSOUND_EXPORTS
#define XRSOUND_API __declspec(dllexport)
#else
#define XRSOUND_API __declspec(dllimport)
#endif

// refs
class	CObject;
class	XRSOUND_API					CSound_params;
class	XRSOUND_API					CSound_source;
class	XRSOUND_API					CSound_interface;
class	XRSOUND_API					CSound_stream_interface;
class	XRSOUND_API					CSound_environment;

//
XRSOUND_API extern u32				psSoundFreq				;
XRSOUND_API extern u32				psSoundModel			;
XRSOUND_API extern float			psSoundVMaster			;
XRSOUND_API extern float			psSoundVEffects			;
XRSOUND_API extern float			psSoundVMusic			;
XRSOUND_API extern float			psSoundRolloff			;
XRSOUND_API extern float			psSoundDoppler			;
XRSOUND_API extern float			psSoundOcclusionScale	;
XRSOUND_API extern float			psSoundCull				;
XRSOUND_API extern Flags32			psSoundFlags			;
XRSOUND_API extern int				psSoundRelaxTime		;

// Flags
enum {
	ssWaveTrace			= (1ul<<0ul),
	ssEAX				= (1ul<<1ul),
	ssSoftware			= (1ul<<2ul),
	ss_forcedword		= u32(-1)
};
enum {
	sf_11K,
	sf_22K,
	sf_44K,
	sf_forcedword = u32(-1)
};
enum {
	sq_DEFAULT,
	sq_NOVIRT,
	sq_LIGHT,
	sq_HIGH,
	sq_forcedword = u32(-1)
};

// definition (Sound Structure + control)
struct	/*XRSOUND_API*/	sound
{
	CSound_source*					handle;
	CSound_interface*				feedback;
	int								g_type;
	CObject*						g_object;
	sound()							{ handle = 0; feedback=0; g_type=0; g_object=0; }

	IC void					create					( BOOL _3D,	LPCSTR name,	int		type=0);
	IC void					clone					( const sound& from, bool leave_type=true);
	IC void					destroy					( );
	IC void					play					( CObject* O,						BOOL bLoop=false);
	IC void					play_unlimited			( CObject* O,						BOOL bLoop=false);
	IC void					play_at_pos				( CObject* O,	const Fvector &pos,	BOOL bLoop=false);
	IC void					play_at_pos_unlimited	( CObject* O,	const Fvector &pos,	BOOL bLoop=false);
	IC void					stop 					( );

	IC void					set_position			( const Fvector &pos);
	IC void					set_frequency			( float freq);
	IC void					set_range				( float min, float max );
	IC void					set_volume				( float vol );

	IC const CSound_params*	get_params				( );
};

// definition (Sound Source)
class XRSOUND_API	CSound_source
{
public:
};

// definition (Sound Params)
class XRSOUND_API	CSound_params
{
public:
	Fvector			position;
	float			volume;
	float			freq;
	float			min_distance;
	float			max_distance;
};

// definition (Sound Interface)
class XRSOUND_API	CSound_interface
{
public:
	virtual void					set_position			(const Fvector &pos)										= 0;
	virtual void					set_frequency			(float freq)												= 0;
	virtual void					set_range				(float min, float max)										= 0;
	virtual void					set_volume				(float vol)													= 0;
	virtual void					stop					( )															= 0;
	virtual	const CSound_params*	get_params				( )															= 0;
};

class XRSOUND_API	CSound_stream_interface
{
public:
};

// definition (Sound Manager Interface)
typedef		void __stdcall sound_event						(sound* S, float range);


class XRSOUND_API	CSound_manager_interface
{
public:
	// General
	virtual void					_initialize				( u32 window )																			= 0;
	virtual void					_destroy				( )																						= 0;
	virtual void					_restart				( )																						= 0;

	// Sound interface
	virtual void					create					( sound& S, BOOL _3D,	LPCSTR fName,	int		type=0)									= 0;
	virtual void					destroy					( sound& S)																				= 0;
	virtual void					play					( sound& S, CObject* O,								BOOL bLoop=false)					= 0;
	virtual void					play_unlimited			( sound& S, CObject* O,								BOOL bLoop=false)					= 0;
	virtual void					play_at_pos				( sound& S, CObject* O,		const Fvector &pos,		BOOL bLoop=false)					= 0;
	virtual void					play_at_pos_unlimited	( sound& S, CObject* O,		const Fvector &pos,		BOOL bLoop=false)					= 0;
	virtual void					set_geometry			( CDB::MODEL* M )																		= 0;
	virtual void					set_handler				( sound_event* E )																		= 0;
	virtual void					set_environment_mode	( BOOL bUseSpecified, CSound_environment* E)											= 0;

	virtual void					update					( const Fvector& P, const Fvector& D, const Fvector& N, float dt )						= 0;
};
extern XRSOUND_API CSound_manager_interface*		Sound;

// ********* Sound ********* (utils, accessors, helpers)
IC void	sound::create						( BOOL _3D,	LPCSTR name,	int		type)				{	::Sound->create					(*this,_3D,name,type);				}
IC void	sound::destroy						( )														{	::Sound->destroy				(*this);							}
IC void	sound::play							( CObject* O,						BOOL bLoop)			{	::Sound->play					(*this,O,bLoop);					}
IC void sound::play_unlimited				( CObject* O,						BOOL bLoop)			{	::Sound->play_unlimited			(*this,O,bLoop);					}
IC void	sound::play_at_pos					( CObject* O,	const Fvector &pos,	BOOL bLoop)			{	::Sound->play_at_pos			(*this,O,pos,bLoop);				}
IC void	sound::play_at_pos_unlimited		( CObject* O,	const Fvector &pos,	BOOL bLoop)			{	::Sound->play_at_pos_unlimited	(*this,O,pos,bLoop);				}
IC void	sound::set_position					( const Fvector &pos)									{	if (feedback)	feedback->set_position(pos);						}
IC void	sound::set_frequency				( float freq)											{	if (feedback)	feedback->set_frequency(freq);						}
IC void	sound::set_range					( float min, float max )								{	if (feedback)	feedback->set_range(min,max);						}
IC void	sound::set_volume					( float vol )											{	if (feedback)	feedback->set_volume(vol);							}
IC void	sound::stop							( )														{	if (feedback)	feedback->stop();									}
IC const CSound_params*	sound::get_params	( )														{	if (feedback)	return feedback->get_params(); else return NULL;	}
IC void	sound::clone						( const sound& from, bool leave_type)		
{
	feedback	= 0;
	g_object	= 0;
	handle		= from.handle;
	if (!leave_type) 
		g_type	= from.g_type;
}
#endif