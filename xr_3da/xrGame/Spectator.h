#ifndef __SPECTATOR_H__
#define __SPECTATOR_H__

#pragma once

#include "entity.h"
#include "actor_flags.h"
#include "..\feel_touch.h"

// refs
class CActor;

class CSpectator: public CGameObject
{
private:
	typedef CGameObject		inherited;
protected:
	enum EActorCameras {
		eacFreeFly		= 0,
		eacFirstEye,
		eacLookAt,
		eacFreeLook,
		eacMaxCam
	};
private:
	// Cameras
	CCameraBase*			cameras[eacMaxCam];
	EActorCameras			cam_active;

	int						look_idx;

	CActor*					last_actor;
	//------------------------------
	void					cam_Set					(EActorCameras style);
	void					cam_Update				(CActor* A=0);
public:
							CSpectator				( );
	virtual					~CSpectator				( );

	virtual void			OnMouseMove				(int x, int y);
	virtual void			OnKeyboardPress			(int dik);
	virtual void			OnKeyboardRelease		(int dik);
	virtual void			OnKeyboardHold			(int dik);

	virtual void			Update					( u32 T ); 
	virtual void			UpdateCL				( );

	virtual void			OnHUDDraw				(CCustomHUD* hud);
};

#endif // __SPECTATOR_H__
