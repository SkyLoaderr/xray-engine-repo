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
	CVertexStream*		pStream;
	
	BOOL				Create				();
	
	float				param_blur;
	float				param_gray;
public:
	CRenderTarget		();

	void				OnDeviceCreate		();
	void				OnDeviceDestroy		();

	BOOL				NeedPostProcess		()	{ return (param_blur>EPS)||(param_gray>EPS); }

	BOOL				Available			()	{ return bAvailable; }
	void				Begin				();
	void				End					();
};
