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
	cs_forcedword = DWORD(-1)
};

// refs
class ENGINE_API CCameraBase;
class ENGINE_API CEffector;

