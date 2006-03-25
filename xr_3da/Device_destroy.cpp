#include "stdafx.h"

#include "ResourceManager.h"
#include "render.h"

void CRenderDevice::_Destroy	(BOOL bKeepTextures)
{
	m_WireShader.destroy		();
	m_SelectionShader.destroy	();

	// before destroy
	bReady						= FALSE;
	Statistic->OnDeviceDestroy	();
	::Render->destroy			();
	Resources->OnDeviceDestroy	(bKeepTextures);
	RCache.OnDeviceDestroy		();

	Memory.mem_compact			();
}

void CRenderDevice::Destroy	(void) {
	if (!bReady) return;

	Log("Destroying Direct3D...");

	ShowCursor	(TRUE);
	HW.Validate					();

	_Destroy					(FALSE);

	xr_delete					(Resources);

	// real destroy
	HW.DestroyDevice			();

	seqRender.R.clear			();
	seqAppActivate.R.clear		();
	seqAppDeactivate.R.clear	();
	seqAppStart.R.clear			();
	seqAppEnd.R.clear			();
	seqFrame. R.clear			();
	seqFrameMT.R.clear			();
	seqDeviceReset.R.clear		();
	seqParallel.clear			();

	xr_delete					(Statistic);
}

void CRenderDevice::Reset		()
{
	ShowCursor				(TRUE);
	u32 tm_start			= TimerAsync();
	if (g_pGamePersistent)	g_pGamePersistent->Environment.OnDeviceDestroy();
	Resources->reset_begin	();
	Memory.mem_compact		();
	HW.Reset				(m_hWnd);
	dwWidth					= HW.DevPP.BackBufferWidth;
	dwHeight				= HW.DevPP.BackBufferHeight;
	fWidth_2				= float(dwWidth/2);
	fHeight_2				= float(dwHeight/2);
	Resources->reset_end	();
	if (g_pGamePersistent)	g_pGamePersistent->Environment.OnDeviceCreate();
	_SetupStates			();
	PreCache				(DEVICE_RESET_PRECACHE_FRAME_COUNT);
	u32 tm_end				= TimerAsync();
	Msg						("*** RESET [%d ms]",tm_end-tm_start);
	if (!strstr(Core.Params, "-dedicated")) 
		ShowCursor	(FALSE);
	seqDeviceReset.Process(rp_DeviceReset);
}
