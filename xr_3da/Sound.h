#pragma once

// refs
class	ENGINE_API					CObject;
class	ENGINE_API					CSound_source;
class	ENGINE_API					CSound_interface;
class	ENGINE_API					CSound_stream_interface;

// definition (Sound Structure + control)
struct	ENGINE_API	sound
{
	CSound_source*					handle;
	CSound_interface*				feedback;
	int								g_type;
	CObject*						g_object;
	sound()							{ handle = 0; feedback=0; g_type=0; g_object=0; }

	IC void	create					( BOOL _3D,	LPCSTR name,	int		type=0);
	IC void	clone					( const sound& from, bool leave_type=true);
	IC void	destroy					( );
	IC void	play					( CObject* O,						BOOL bLoop=false);
	IC void	play_unlimited			( CObject* O,						BOOL bLoop=false);
	IC void	play_at_pos				( CObject* O,	const Fvector &pos,	BOOL bLoop=false);
	IC void	play_at_pos_unlimited	( CObject* O,	const Fvector &pos,	BOOL bLoop=false);

	IC void	set_position			( const Fvector &pos);
	IC void	set_frequency			( float freq);
	IC void	set_range				( float min, float max );
	IC void	set_volume				( float vol );
};

// definition (Sound Interface)
class ENGINE_API	CSound_interface
{
public:
	virtual void					set_position			(const Fvector &pos)										= 0;
	virtual void					set_frequency			(float freq)												= 0;
	virtual void					set_range				(float min, float max)										= 0;
	virtual void					set_volume				(float vol)													= 0;
};

class ENGINE_API	CSound_stream_interface
{
public:
};

// definition (Sound Manager Interface)
class ENGINE_API	CSound_manager_interface				: public pureFrame
{
public:
	// General
	virtual void					_initialize				( )																						= 0;
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
	virtual CDB::MODEL* 			get_geometry			( )																						= 0;

	// Stream interface
	/*
	virtual CSound_stream_interface*CreateStream			( LPCSTR fName )																		= 0;
	virtual void					DeleteStream			( CSound_stream_interface* pSnd )														= 0;
	*/

	virtual void					OnFrame					( )																						= 0;
};
extern ENGINE_API CSound_manager_interface*		Sound;

// ********* Sound ********* (utils, accessors, helpers)
IC void	sound::create				( BOOL _3D,	LPCSTR name,	int		type)				{	::Sound->create					(*this,_3D,name,type);	}
IC void	sound::destroy				( )														{	::Sound->destroy				(*this);				}
IC void	sound::play					( CObject* O,						BOOL bLoop)			{	::Sound->play					(*this,O,bLoop);		}
IC void sound::play_unlimited		( CObject* O,						BOOL bLoop)			{	::Sound->play_unlimited			(*this,O,bLoop);		}
IC void	sound::play_at_pos			( CObject* O,	const Fvector &pos,	BOOL bLoop)			{	::Sound->play_at_pos			(*this,O,pos,bLoop);	}
IC void	sound::play_at_pos_unlimited( CObject* O,	const Fvector &pos,	BOOL bLoop)			{	::Sound->play_at_pos_unlimited	(*this,O,pos,bLoop);	}
IC void	sound::set_position			( const Fvector &pos)									{	if (feedback)	feedback->set_position(pos);			}
IC void	sound::set_frequency		( float freq)											{	if (feedback)	feedback->set_frequency(freq);			}
IC void	sound::set_range			( float min, float max )								{	if (feedback)	feedback->set_range(min,max);			}
IC void	sound::set_volume			( float vol )											{	if (feedback)	feedback->set_volume(vol);				}
IC void	sound::clone				( const sound& from, bool leave_type)		
{
	feedback	= 0;
	g_object	= 0;
	handle		= from.handle;
	if (!leave_type) 
		g_type	= from.g_type;
}
