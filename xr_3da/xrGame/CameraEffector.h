// CameraEffector.h: interface for the CCameraEffector class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../CameraDefs.h"
#include "../cameramanager.h"
#include "script_export_space.h"

enum ECameraEffectorType
{
	eCEFall			=0,
	eCENoise		=1,
	eCEShot			=2,
	eCEZoom			=3,
	eCERecoil		=4,
	eCEBobbing		=5,
	eCEHit			=6,  //hit from monster
	eExplode		=7,
	eCEAlcohol		=20,
	eCEFireHit		= 21,
	eCEUser			=100,
	eCEMax			=u32(-1)
};

class CCameraShotEffector;
class CEffectorZoomInertion;

class CCameraEffector
{
protected:
	ECameraEffectorType		eType;
	float					fLifeTime;
public:
	CCameraEffector							(ECameraEffectorType type, float tm) {eType = type; fLifeTime=tm;};
	virtual				~CCameraEffector	() {};
	IC ECameraEffectorType	GetType			() {return eType;}
	IC float				LifeTime		() {return fLifeTime;}

	virtual CCameraShotEffector		*cast_effector_shot				()	{return 0;}
	virtual CEffectorZoomInertion	*cast_effector_zoom_inertion	()	{return 0;}

	virtual	BOOL		Process			(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect)= 0;
public:
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CCameraEffector)
#undef script_type_list
#define script_type_list save_type_list(CCameraEffector)
