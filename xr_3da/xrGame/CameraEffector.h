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
	BOOL					bAffected;
	float					fLifeTime;
public:
	CCameraEffector							(ECameraEffectorType type, float tm, BOOL affected) {eType = type; fLifeTime=tm; bAffected=affected;};
	virtual				~CCameraEffector	() {};
	IC ECameraEffectorType	GetType			() {return eType;}
	IC BOOL					Affected		() {return bAffected;}
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
