#pragma once

class ENGINE_API		CRenderTarget
{
private:
	BOOL				bAvailable;
	IDirect3DTexture8*	pSurface;
	IDirect3DSurface8*	pRT;
	IDirect3DSurface8*	pBaseRT;
	IDirect3DSurface8*	pBaseZB;
	
	CTexture*			pTexture;
	Shader*				pShaderSet;
	Shader*				pShaderGray;
	Shader*				pShaderBlend;
	CVertexStream*		pStream;
	
	BOOL				Create				();
	
	float				param_blur;
	float				param_gray;
public:
	CRenderTarget		();

	void				OnDeviceCreate		();
	void				OnDeviceDestroy		();

	BOOL				NeedPostProcess		()	{ return (param_blur>0.01f)||(param_gray>0.01f); }
	BOOL				Available			()	{ return bAvailable; }

	void				Begin				();
	void				End					();

	void				set_blur			(float f)		{ param_blur=f; }
	void				set_gray			(float f)		{ param_gray=f; }
};
