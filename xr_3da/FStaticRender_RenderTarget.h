#pragma once

class ENGINE_API		CRenderTarget
{
private:
	BOOL				bAvailable;
	IDirect3DTexture8*	pSurface;

	BOOL				Create				();
public:
	void				OnDeviceCreate		();
	void				OnDeviceDestroy		();

	void				Begin				();
	void				End					(float blur);
};
