#ifndef __CAMERALOOK_H__
#define __CAMERALOOK_H__

#include "CameraBase.h"

class ENGINE_API CInifile;

class ENGINE_API CCameraLook	: public CCameraBase
{
	Fvector2		lim_zoom;
	float			dist, prev_d;
public:
					CCameraLook		( CObject* p, CInifile* ini, LPSTR section, BOOL rlink );
	virtual			~CCameraLook	( );

	virtual void	Move			( int cmd, float val=0 );

	virtual	void	OnActivate		( CCameraBase* old_cam );
	virtual void	Update			( Fvector& point, Fvector& noise_dangle );

	virtual float	GetWorldYaw		( )	{ return -yaw;	};
	virtual float	GetWorldPitch	( )	{ return pitch; };
};

#endif // __CAMERALOOK_H__
