#ifndef SoundRender_CoreDH
#define SoundRender_CoreDH
#pragma once

#include "SoundRender_Core.h"

class CSoundRender_CoreD: public CSoundRender_Core
{
	typedef CSoundRender_Core inherited;
public:
	// DSound interface
	IDirectSound8*				pDevice;		// The device itself
	IDirectSoundBuffer*			pBuffer;		// The primary buffer (mixer destination)
	IDirectSound3DListener8*	pListener;
	LPKSPROPERTYSET				pExtensions;
	DSCAPS						dsCaps;
private:
	virtual void		update_listener			( const Fvector& P, const Fvector& D, const Fvector& N, float dt );
	virtual void		i_set_eax				(CSound_environment* E);
	virtual void		i_get_eax				(CSound_environment* E);
public:
                        CSoundRender_CoreD		();
    virtual				~CSoundRender_CoreD		();

	virtual void		_initialize				( u64 window );
	virtual void		_destroy				( );
    
#ifdef _EDITOR
	virtual void 		set_environment_size	(CSound_environment* src_env, CSound_environment** dst_env);
	virtual void 		set_environment			(u32 id, CSound_environment** dst_env);
#endif
};
extern CSoundRender_CoreD	SoundRenderD;
#endif
