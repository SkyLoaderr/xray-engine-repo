#include "stdafx.h"

#include "ResourceManager.h"
#include "render.h"

void CRenderDevice::_Destroy	(BOOL bKeepTextures)
{
	// before destroy
	bReady						= FALSE;
	Statistic.OnDeviceDestroy	();
	::Render->destroy			();
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

	xr_delete					(Resources);

	// real destroy
	HW.DestroyDevice			();
}

void CRenderDevice::Reset		()
{
	ShowCursor				(TRUE);
	u32 tm_start			= TimerAsync();
	Resources->reset_begin	();
	Memory.mem_compact		();
	HW.Reset				();
	dwWidth					= HW.DevPP.BackBufferWidth;
	dwHeight				= HW.DevPP.BackBufferHeight;
	fWidth_2				= float(dwWidth/2);
	fHeight_2				= float(dwHeight/2);
	Resources->reset_end	();
	_SetupStates			();
	PreCache				(10);
	u32 tm_end				= TimerAsync();
	Msg						("*** RESET [%d ms]",tm_end-tm_start);
	ShowCursor				(FALSE);
}
