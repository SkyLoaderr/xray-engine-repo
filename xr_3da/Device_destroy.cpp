#include "stdafx.h"

#include "ResourceManager.h"

void CRenderDevice::_Destroy	(BOOL bKeepTextures)
{
	// before destroy
	bReady						= FALSE;
	Statistic.OnDeviceDestroy	();
	seqDevDestroy.Process		(rp_DeviceDestroy);
	Resources->OnDeviceDestroy	(bKeepTextures);
	RCache.OnDeviceDestroy		();

	Memory.mem_compact			();
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
	Memory.mem_compact	();
	_Destroy			(bKeepTextures);
	_Create				(shName);
	Memory.mem_compact	();
	u32 tm_end			= TimerAsync();
	Msg					("*** RESET [%d ms]",tm_end-tm_start);
}
