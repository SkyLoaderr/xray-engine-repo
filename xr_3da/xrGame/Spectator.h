#ifndef __SPECTATOR_H__
#define __SPECTATOR_H__

#pragma once

#include "entity.h"
#include "actor_flags.h"
#include "../feel_touch.h"

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

	//------------------------------
	void					cam_Set					(EActorCameras style);
	void					cam_Update				(CActor* A=0);
public:
							CSpectator				( );
	virtual					~CSpectator				( );

	virtual void			IR_OnMouseMove			(int x, int y);
	virtual void			IR_OnKeyboardPress		(int dik);
	virtual void			IR_OnKeyboardRelease	(int dik);
	virtual void			IR_OnKeyboardHold		(int dik);
	//virtual	void			Hit						(float P, Fvector &dir,	CObject* who, s16 element,Fvector p_in_object_space, float impulse){};
	virtual void			shedule_Update			( u32 T ); 
	virtual void			UpdateCL				( );
};

#endif // __SPECTATOR_H__
