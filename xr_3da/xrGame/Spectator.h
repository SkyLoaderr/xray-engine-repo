#ifndef __SPECTATOR_H__
#define __SPECTATOR_H__

#pragma once

#include "entity.h"
#include "actor_flags.h"
#include "..\feel_touch.h"

// refs
class ENGINE_API CCameraBase;
class ENGINE_API CBoneInstance;
class ENGINE_API CMotionDef;
class ENGINE_API CKinematics;
class ENGINE_API CBlend;
class CWeaponList;
class CEffectorBobbing;
class CTargetCS;

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
	void					cam_Update				(CCameraBase* C=0);
public:
							CSpectator				( );
	virtual					~CSpectator				( );

	virtual void			OnMouseMove				(int x, int y);
	virtual void			OnKeyboardPress			(int dik);
	virtual void			OnKeyboardRelease		(int dik);
	virtual void			OnKeyboardHold			(int dik);

	virtual void			Update					( u32 T ); 
	virtual void			UpdateCL				( );
};

#endif // __SPECTATOR_H__
