#ifndef SoundRender_CoreAH
#define SoundRender_CoreAH
#pragma once

#include "SoundRender_Core.h"

class CSoundRender_CoreA: public CSoundRender_Core
{
	typedef CSoundRender_Core inherited;
    EAXSet					eaxSet;					// EAXSet function, retrieved if EAX Extension is supported
    EAXGet					eaxGet;					// EAXGet function, retrieved if EAX Extension is supported

	struct SListener{
		Fvector				position;
		Fvector				orientation[2];
	};
	SListener				Listener;
protected:
	virtual void			i_eax_set				(const GUID* guid, u32 prop, void* val, u32 sz);
	virtual void			i_eax_get				(const GUID* guid, u32 prop, void* val, u32 sz);
	virtual void			update_listener			( const Fvector& P, const Fvector& D, const Fvector& N, float dt );
public:	
						    CSoundRender_CoreA		();
    virtual					~CSoundRender_CoreA		();

	virtual void			_initialize				( u64 window );
	virtual void			_destroy				( );
    
	virtual const Fvector&	listener_position		( ){return Listener.position;}
};
#endif
