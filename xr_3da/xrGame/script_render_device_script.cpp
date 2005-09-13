////////////////////////////////////////////////////////////////////////////
//	Module 		: script_render_device_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script render device script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_render_device.h"
#include "script_space.h"

using namespace luabind;

bool is_device_paused(CRenderDevice* d)
{
	return !!Device.Pause();
}

void set_device_paused(CRenderDevice* d, bool b)
{
	Device.Pause(b);
}
extern ENGINE_API BOOL g_appLoaded;
bool is_app_ready()
{
	return !!g_appLoaded;
}
void CScriptRenderDevice::script_register(lua_State *L)
{
	module(L)
	[
		class_<CRenderDevice>("render_device")
			.def_readonly("width",					&CRenderDevice::dwWidth)
			.def_readonly("height",					&CRenderDevice::dwHeight)
			.def_readonly("time_delta",				&CRenderDevice::dwTimeDelta)
			.def_readonly("f_time_delta",			&CRenderDevice::fTimeDelta)
			.def_readonly("cam_pos",				&CRenderDevice::vCameraPosition)
			.def_readonly("cam_dir",				&CRenderDevice::vCameraDirection)
			.def_readonly("cam_top",				&CRenderDevice::vCameraTop)
			.def_readonly("cam_right",				&CRenderDevice::vCameraRight)
//			.def_readonly("view",					&CRenderDevice::mView)
//			.def_readonly("projection",				&CRenderDevice::mProject)
//			.def_readonly("full_transform",			&CRenderDevice::mFullTransform)
			.def_readonly("fov",					&CRenderDevice::fFOV)
			.def_readonly("aspect_ratio",			&CRenderDevice::fASPECT)
			.def("time_global",						&CRenderDevice::TimerAsync)
			.def_readonly("precache_frame",			&CRenderDevice::dwPrecacheFrame)
			.def("is_paused",						&is_device_paused)
			.def("pause",							&set_device_paused),
			def("app_ready",						&is_app_ready)
	];
}
