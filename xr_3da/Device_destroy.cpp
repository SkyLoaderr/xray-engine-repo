#include "stdafx.h"

void CRenderDevice::_Destroy	(BOOL bKeepTextures)
{
	// before destroy
	bReady						= FALSE;
	Streams.OnDeviceDestroy		();
	Statistic.OnDeviceDestroy	();
	Primitive.OnDeviceDestroy	();
	seqDevDestroy.Process		(rp_DeviceDestroy);
	Streams.OnDeviceDestroy		();
	Shader.OnDeviceDestroy		(bKeepTextures);

	Engine.mem_Compact			();
}

void CRenderDevice::Destroy	(void) {
	if (!bReady) return;

	Log("Destroying Direct3D...");

	ShowCursor					(TRUE);
	HW.Validate					();

	_Destroy					(FALSE);

	// real destroy
	HW.DestroyDevice			();
}

void CRenderDevice::Reset		(LPCSTR shName, BOOL bKeepTextures)
{
	u32 tm_start		= TimerAsync();
	Engine.mem_Compact	();
	_Destroy			(bKeepTextures);
	_Create				(shName);
	Engine.mem_Compact	();
	u32 tm_end			= TimerAsync();
	Msg					("*** RESET [%d ms]",tm_end-tm_start);
}
