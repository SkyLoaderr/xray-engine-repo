#ifndef SoundRender_CoreAH
#define SoundRender_CoreAH
#pragma once

#include "SoundRender_Core.h"            

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>

#ifdef DEBUG
#	define A_CHK(expr)		{ alGetError(); 		expr; ALenum error=alGetError(); 			VERIFY2(error==AL_NO_ERROR, (LPCSTR)alGetString(error)); }
#	define AC_CHK(expr)		{ alcGetError(pDevice); expr; ALCenum error=alcGetError(pDevice); 	VERIFY2(error==ALC_NO_ERROR,(LPCSTR)alcGetString(pDevice,error)); }
#else
#	define A_CHK(expr)		{ expr; }
#	define AC_CHK(expr)		{ expr; }
#endif

// temorary patch
typedef ALenum    __cdecl EAXGet(const struct _GUID *propertySetID,ALuint property,ALuint source,ALvoid *value,ALuint size);
typedef ALenum    __cdecl EAXSet(const struct _GUID *propertySetID,ALuint property,ALuint source,ALvoid *value,ALuint size);

class CSoundRender_CoreA: public CSoundRender_Core
{
	typedef CSoundRender_Core inherited;
	EAXSet*					eaxSet;					// EAXSet function, retrieved if EAX Extension is supported
	EAXGet*					eaxGet;					// EAXGet function, retrieved if EAX Extension is supported
	ALCdevice* 				pDevice;
    ALCcontext*				pContext;

	struct SListener{
		Fvector				position;
		Fvector				orientation[2];
	};
	SListener				Listener;

    BOOL 					EAXQuerySupport			(BOOL bDeferred, const GUID* guid, u32 prop, void* val, u32 sz);
	BOOL 					EAXTestSupport			(BOOL bDeferred);
protected:
	virtual void			i_eax_set				(const GUID* guid, u32 prop, void* val, u32 sz);
	virtual void			i_eax_get				(const GUID* guid, u32 prop, void* val, u32 sz);
	virtual void			update_listener			( const Fvector& P, const Fvector& D, const Fvector& N, float dt );
public:	
						    CSoundRender_CoreA		();
    virtual					~CSoundRender_CoreA		();

	virtual void			_initialize				( u64 window );
	virtual void			_clear					( );
    
	virtual void			set_volume				( float f		);

	virtual const Fvector&	listener_position		( ){return Listener.position;}
};
extern CSoundRender_CoreA* SoundRenderA;
#endif
