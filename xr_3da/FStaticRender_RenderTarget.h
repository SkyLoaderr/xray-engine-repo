#pragma once

class ENGINE_API		CRenderTarget
{
private:
	BOOL				bAvailable;
	IDirect3DTexture8*	pSurface;
	IDirect3DSurface8*	pRT;
	IDirect3DSurface8*	pBaseRT;
	IDirect3DSurface8*	pBaseZB;
	
	BOOL				Create				();
public:
	CRenderTarget		();

	void				OnDeviceCreate		();
	void				OnDeviceDestroy		();

	void				Begin				();
	void				End					(float blur);
};
