#ifndef __CAMERA_FE_H__
#define __CAMERA_FE_H__

#include "CameraBase.h"

class ENGINE_API CCameraFirstEye : public CCameraBase
{
public:
					CCameraFirstEye	( CObject* p, CInifile* ini, LPSTR section, BOOL rlink );
	virtual			~CCameraFirstEye( );

	virtual void	Move			( int cmd, float val=0 );

	virtual	void	OnActivate		( CCameraBase* old_cam );
	virtual void	Update			( Fvector& point, Fvector& noise_angle );

	virtual float	GetWorldYaw		( )	{ return -yaw;	};
	virtual float	GetWorldPitch	( )	{ return pitch; };
};

#endif // __CAMERALOOK_H__
