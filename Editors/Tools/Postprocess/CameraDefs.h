#pragma once
#ifndef _CAMERA_MANAGER_INCLUDED_
#define _CAMERA_MANAGER_INCLUDED_
// t_defs
/*
enum ECameraMove	{
	cmNone		,
	cmZoomIn	,
	cmZoomOut	,
	cmLeft		,
	cmRight		,
	cmUp		,
	cmDown		,
	cmMove
};
*/
enum ECameraStyle       {
	csCamDebug,
	csFirstEye,
	csLookAt,
    csMax,
	csFixed,
	cs_forcedword = u32(-1)
};

enum EEffectorType		{
	cefDemo,
	cefFall,
	cefNoise,
	cefShot,
	cefRecoil,
	cefBobbing,
	cefHit
};

enum EEffectorPPType	{
	cefppDemo,
	cefppHit,
	cefppUser
};

// refs
class ENGINE_API CCameraBase;
class ENGINE_API CEffector;
class ENGINE_API CEffectorPP;
#endif /*_CAMERA_MANAGER_INCLUDED_*/
