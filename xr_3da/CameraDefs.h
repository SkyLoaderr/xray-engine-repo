#pragma once

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
	cefBobbing
};

// refs
class ENGINE_API CCameraBase;
class ENGINE_API CEffector;

