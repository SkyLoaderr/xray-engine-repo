#ifndef SoundH
#define SoundH
#pragma once

#ifdef XRSOUND_EXPORTS
	#define XRSOUND_API __declspec(dllexport)
#else
	#define XRSOUND_API __declspec(dllimport)
#endif

#ifdef __BORLANDC__
	#define XRSOUND_EDITOR_API XRSOUND_API

	// editor only refs
	class XRSOUND_EDITOR_API SoundEnvironment_LIB;
#else
	#define XRSOUND_EDITOR_API
#endif

#define SNDENV_FILENAME				"sEnvironment.xr"
#define OGG_COMMENT_VERSION 		0x0003 

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
XRSOUND_API extern float			psSoundVEffects			;
XRSOUND_API extern float			psSoundVFactor			;
XRSOUND_API extern float			psSoundVMusic			;
XRSOUND_API extern float			psSoundRolloff			;
XRSOUND_API extern float			psSoundOcclusionScale	;
XRSOUND_API extern Flags32			psSoundFlags			;
XRSOUND_API extern int				psSoundTargets			;
XRSOUND_API extern int				psSoundCacheSizeMB		;

// Flags
enum {
	ssHardware			= (1ul<<1ul),	//!< Use hardware mixing only
    ssEAX				= (1ul<<2ul),	//!< Use eax
	ss_forcedword		= u32(-1)
};
enum {
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
enum {
	sg_Undefined		= 0,
	sg_SourceType		= u32(-1),
	sg_forcedword		= u32(-1),
};
enum {
	sm_Looped			= (1ul<<0ul),	//!< Looped
	sm_2D				= (1ul<<1ul),	//!< 2D mode
	sm_forcedword		= u32(-1),
};
enum {
	st_Effect			= 0,
	st_Music			= 1,
	st_forcedword		= u32(-1),
};

class CSound_UserDataVisitor;

class CSound_UserData	: public xr_resource		{
public:
	virtual							~CSound_UserData()							{}
	virtual void					accept			(CSound_UserDataVisitor*)	=0;
	virtual void					invalidate		()							=0;
};
typedef resptr_core<CSound_UserData,resptr_base<CSound_UserData> >	CSound_UserDataPtr;

class ref_sound_data	: public xr_resource		{
public:
	shared_str						nm;
	CSound_source*					handle;			//!< Pointer to wave-source interface
	CSound_interface*				feedback;		//!< Pointer to emitter, automaticaly clears on emitter-stop
	int								g_type;			//!< Sound type, usually for AI
	CObject*						g_object;		//!< Game object that emitts ref_sound
	CSound_UserDataPtr				g_userdata;
public:
									ref_sound_data	();
									ref_sound_data	(LPCSTR fName, int type);
	virtual							~ref_sound_data	();
};
typedef resptr_core<ref_sound_data,resptr_base<ref_sound_data> >	ref_sound_data_ptr;
/*! \class ref_sound
\brief Sound source + control

The main class respresenting source/emitter interface
This class infact just hides internals and redirect calls to 
specific sub-systems
*/
struct	ref_sound
{
	ref_sound_data_ptr		_p;
public:
    //! A constructor
    /*!
		\sa ~ref_sound()
	*/
							ref_sound				(){ }
							~ref_sound				(){ }

	IC CSound_source*		_handle					(){return _p?_p->handle:0;}
	IC CSound_interface*	_feedback				(){return _p?_p->feedback:0;}
	IC CObject*				_g_object				(){VERIFY(_p); return _p->g_object;}
	IC int					_g_type					(){VERIFY(_p); return _p->g_type;}
	IC CSound_UserDataPtr	_g_userdata				(){VERIFY(_p); return _p->g_userdata;}

	//! Loader/initializer
	/*!
		\sa clone()
		\sa destroy()
		\param name Name of wave-file
		\param type Sound type, usually for \a AI
	*/
	IC void					create					( LPCSTR name,	int		type);

	//! Clones ref_sound from another
	/*!
		\sa create()
		\sa destroy()
		\param from Source to clone.
		\param leave_type Controls whenewer to leave game/AI type as is
	*/
	IC void					clone					( const ref_sound& from,	int		type);

	//! Destroys and unload wave
	/*!
		\sa create()
		\sa clone()
	*/
	IC void					destroy					( );

	//@{
	//! Starts playing this source
	/*!
		\sa stop()
	*/
	IC void					play					( CObject* O /*!< Object */,											u32 flags=0 /*!< Looping */, float delay=0.f /*!< Delay */);
	IC void					play_at_pos				( CObject* O /*!< Object */,	const Fvector &pos /*!< 3D position */,	u32 flags=0 /*!< Looping */, float delay=0.f /*!< Delay */);
	IC void					play_no_feedback		( CObject* O /*!< Object */,											u32 flags=0 /*!< Looping */, float delay=0.f /*!< Delay */, Fvector* pos=0, float* vol=0, float* freq=0, Fvector2* range=0);
	//@}

	//! Stops playing this source
	/*!
		\sa play(), etc
	*/
	IC void					stop 					( );
	IC void					stop_deffered			( );
	IC void					set_position			( const Fvector &pos);
	IC void					set_frequency			( float freq);
	IC void					set_range				( float min, float max );
	IC void					set_volume				( float vol );
	IC void					set_priority			( float vol );

	IC const CSound_params*	get_params				( );
    IC void					set_params				( CSound_params* p );
};

/// definition (Sound Source)
class XRSOUND_API	CSound_source
{
public:
	virtual	u32				length_ms				( )		= 0;
	virtual u32				game_type				( )		= 0;
	virtual LPCSTR			file_name				( )		= 0;
};

/// definition (Sound Source)
class XRSOUND_API	CSound_environment
{
public:
};

/// definition (Sound Params)
class XRSOUND_API	CSound_params
{
public:
	Fvector			position;
	float			base_volume;
	float			volume;
	float			freq;
	float			min_distance;
	float			max_distance;
	float			max_ai_distance;
};

/// definition (Sound Interface)
class XRSOUND_API	CSound_interface
{
public:
	virtual BOOL					is_2D					()															= 0;
	virtual void					switch_to_2D			()															= 0;
	virtual void					switch_to_3D			()															= 0;
	virtual void					set_position			(const Fvector &pos)										= 0;
	virtual void					set_frequency			(float freq)												= 0;
	virtual void					set_range				(float min, float max)										= 0;
	virtual void					set_volume				(float vol)													= 0;
	virtual void					set_priority			(float vol)													= 0;
	virtual void					stop					(BOOL bDeffered)											= 0;
	virtual	const CSound_params*	get_params				( )															= 0;
};

/// definition (Sound Stream Interface)
class XRSOUND_API	CSound_stream_interface
{
public:
};

/// definition (Sound Stream Interface)
class XRSOUND_API	CSound_stats
{
public:
	u32				_rendered;
	u32				_simulated;
	u32				_cache_hits;
	u32				_cache_misses;
	u32				_events;
};

/// definition (Sound Callback)
typedef		void __stdcall sound_event						(ref_sound_data_ptr S, float range);

/// definition (Sound Manager Interface)
class XRSOUND_API	CSound_manager_interface
{
	virtual void	  				_initialize				( u64 window )																			= 0;
	virtual void					_clear					( )																						= 0;

protected:
	friend class 					ref_sound_data;
	virtual void					_create_data			( ref_sound_data& S, LPCSTR fName, int	type=sg_SourceType)					= 0;
	virtual void					_destroy_data			( ref_sound_data& S)																	= 0;
public:
	virtual							~CSound_manager_interface(){}
	//@{
	/// General
	static void						_create					( u64 window );
	static void						_destroy				( );

	virtual void					_restart				( )																						= 0;
    virtual BOOL					i_locked 				( )																						= 0;
	//@}

	//@{
	/// Sound interface
	virtual void					create					( ref_sound& S, LPCSTR fName,				int		type=sg_SourceType)					= 0;
	virtual void					clone					( ref_sound& S, const ref_sound& from,		int		type=sg_SourceType)					= 0;
	virtual void					destroy					( ref_sound& S)																			= 0;
	virtual void					stop_emitters			( )																						= 0;	

	virtual void					play					( ref_sound& S, CObject* O,						u32 flags=0, float delay=0.f)			= 0;
	virtual void					play_at_pos				( ref_sound& S, CObject* O,	const Fvector &pos,	u32 flags=0, float delay=0.f)			= 0;
	virtual void					play_no_feedback		( ref_sound& S, CObject* O,						u32 flags=0, float delay=0.f, Fvector* pos=0, float* vol=0, float* freq=0, Fvector2* range=0)= 0;

	virtual void					set_volume				( float f=1.f )																			= 0;
	virtual void					set_geometry_env		( IReader* I )																			= 0;
	virtual void					set_geometry_som		( IReader* I )																			= 0;
	virtual void					set_geometry_occ		( CDB::MODEL* M )																		= 0;
	virtual void					set_handler				( sound_event* E )																		= 0;
	//@}

	virtual void					update					( const Fvector& P, const Fvector& D, const Fvector& N, float dt )						= 0;
	virtual void					statistic				( CSound_stats&  dest )																	= 0;

	virtual float					get_occlusion_to		( const Fvector& hear_pt, const Fvector& snd_pt, float dispersion=0.2f)					= 0;

	virtual void					object_relcase			( CObject* obj )																		= 0;
	virtual const Fvector&			listener_position		()																						= 0;
#ifdef __BORLANDC__
	virtual SoundEnvironment_LIB*	get_env_library			()																						= 0;
	virtual void					refresh_env_library		()																						= 0;
	virtual void					set_user_env			(CSound_environment* E)																	= 0;
	virtual void					refresh_sources			()																						= 0;
    virtual void					set_environment			(u32 id, CSound_environment** dst_env)													= 0;
    virtual void					set_environment_size	(CSound_environment* src_env, CSound_environment** dst_env)								= 0;
#endif
};
extern XRSOUND_API CSound_manager_interface*		Sound;

/// ********* Sound ********* (utils, accessors, helpers)
IC ref_sound_data::ref_sound_data				()														{	handle=0;feedback=0;g_type=0;g_object=0;															}
IC ref_sound_data::ref_sound_data				( LPCSTR fName, 	int 	type)				{	::Sound->_create_data			(*this,fName, type);											}
IC ref_sound_data::~ref_sound_data				()														{	::Sound->_destroy_data			(*this);															}

IC void	ref_sound::create						( LPCSTR name,	int		type)				{	VERIFY(!::Sound->i_locked()); 	::Sound->create		(*this,name,type);							}
IC void	ref_sound::clone						( const ref_sound& from,	int		type)				{	VERIFY(!::Sound->i_locked()); 	::Sound->clone		(*this,from,type);								}
IC void	ref_sound::destroy						( )														{	VERIFY(!::Sound->i_locked()); 	::Sound->destroy	(*this);										}
IC void	ref_sound::play							( CObject* O,						u32 flags, float d)	{	VERIFY(!::Sound->i_locked()); 	::Sound->play		(*this,O,flags,d);								}
IC void	ref_sound::play_at_pos					( CObject* O, const Fvector &pos,	u32 flags, float d)	{	VERIFY(!::Sound->i_locked()); 	::Sound->play_at_pos(*this,O,pos,flags,d);							}
IC void	ref_sound::play_no_feedback				( CObject* O, u32 flags, float d, Fvector* pos, float* vol, float* freq, Fvector2* range){	VERIFY(!::Sound->i_locked()); ::Sound->play_no_feedback(*this,O,flags,d,pos,vol,freq,range);	}
IC void	ref_sound::set_position					( const Fvector &pos)									{	VERIFY(!::Sound->i_locked()); 	VERIFY(_feedback());_feedback()->set_position(pos);								}
IC void	ref_sound::set_frequency				( float freq)											{	VERIFY(!::Sound->i_locked()); 	if (_feedback())	_feedback()->set_frequency(freq);							}
IC void	ref_sound::set_range					( float min, float max )								{	VERIFY(!::Sound->i_locked()); 	if (_feedback())	_feedback()->set_range(min,max);							}
IC void	ref_sound::set_volume					( float vol )											{	VERIFY(!::Sound->i_locked()); 	if (_feedback())	_feedback()->set_volume(vol);								}
IC void	ref_sound::set_priority					( float p )												{	VERIFY(!::Sound->i_locked()); 	if (_feedback())	_feedback()->set_priority(p);								}
IC void	ref_sound::stop							( )														{	VERIFY(!::Sound->i_locked()); 	if (_feedback())	_feedback()->stop(FALSE);									}
IC void	ref_sound::stop_deffered				( )														{	VERIFY(!::Sound->i_locked()); 	if (_feedback())	_feedback()->stop(TRUE);									}
IC const CSound_params*	ref_sound::get_params	( )														{	VERIFY(!::Sound->i_locked()); 	if (_feedback())	return _feedback()->get_params(); else return NULL;			}
IC void	ref_sound::set_params					( CSound_params* p )									
{	
	VERIFY(!::Sound->i_locked()); 	
	if (_feedback()){
    	_feedback()->set_position	(p->position);
    	_feedback()->set_frequency	(p->freq);
        _feedback()->set_range   	(p->min_distance,p->max_distance);
        _feedback()->set_volume   	(p->volume);
    }
}
#endif