#include "stdafx.h"
#include "..\xr_effgamma.h"
#include "..\tga.h"
#include "..\xrImage_Resampler.h"

#define	GAMESAVE_SIZE	128

IC u32 convert				(float c)	{
	u32 C=iFloor(c);
	if (C>255) C=255;
	return C;
}
IC void MouseRayFromPoint	( Fvector& direction, int x, int y, Fmatrix& m_CamMat )
{
	int halfwidth		= Device.dwWidth/2;
	int halfheight		= Device.dwHeight/2;

	Ivector2 point2;
	point2.set			(x-halfwidth, halfheight-y);

	float size_y		= VIEWPORT_NEAR * tanf( deg2rad(60.f) * 0.5f );
	float size_x		= size_y / (Device.fHeight_2/Device.fWidth_2);

	float r_pt			= float(point2.x) * size_x / (float) halfwidth;
	float u_pt			= float(point2.y) * size_y / (float) halfheight;

	direction.mul		( m_CamMat.k, VIEWPORT_NEAR );
	direction.mad		( direction, m_CamMat.j, u_pt );
	direction.mad		( direction, m_CamMat.i, r_pt );
	direction.normalize	();
}

void CRender::Screenshot		(IRender_interface::ScreenshotMode mode, LPCSTR name)
{
	if (!Device.bReady)			return;
	if ((psDeviceFlags.test(rsFullscreen)) == 0) {
		Log("! Can't capture screen while in windowed mode...");
		return;
	}

	// Create temp-surface
	IDirect3DSurface9*	pFB;
	D3DLOCKED_RECT		D;
	R_CHK(HW.pDevice->CreateOffscreenPlainSurface(Device.dwWidth,Device.dwHeight,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&pFB,NULL));
	R_CHK(HW.pDevice->GetFrontBufferData(0,pFB));
	R_CHK(pFB->LockRect(&D,0,D3DLOCK_NOSYSLOCK));

	// Image processing (gamma-correct)
	u32* pPixel		= (u32*)D.pBits;
	u32* pEnd		= pPixel+(Device.dwWidth*Device.dwHeight);
	D3DGAMMARAMP	G;
	Device.Gamma.GenLUT	(G);
	for (int i=0; i<256; i++) {
		G.red	[i]	/= 256;
		G.green	[i]	/= 256;
		G.blue	[i]	/= 256;
	}
	for (;pPixel!=pEnd; pPixel++)	{
		u32 p = *pPixel;
		*pPixel = color_xrgb	(
			G.red	[color_get_R(p)],
			G.green	[color_get_G(p)],
			G.blue	[color_get_B(p)]
			);
	}
	R_CHK			(pFB->UnlockRect());

	// Save
	switch (mode)	{
		case IRender_interface::SM_FOR_GAMESAVE:
			{
				// texture
				IDirect3DTexture9*	texture	= 0;
				CHK_DX				(D3DXCreateTexture(HW.pDevice,GAMESAVE_SIZE,GAMESAVE_SIZE,1,0,D3DFMT_DXT1,D3DPOOL_SCRATCH,&texture));
				VERIFY				(texture);

				// resize&convert to surface
				IDirect3DSurface9*	surface = 0;
				CHK_DX				(texture->GetSurfaceLevel(0,&surface));
				VERIFY				(surface);
				CHK_DX				(D3DXLoadSurfaceFromSurface(surface,0,0,pFB,0,0,D3DX_DEFAULT,0));
				_RELEASE			(surface);

				// save (logical & physical)
				ID3DXBuffer*		saved	= 0;
				CHK_DX				(D3DXSaveTextureToFileInMemory (&saved,D3DXIFF_DDS,texture,0));
				IWriter*			fs		= FS.w_open	(name); 
				if (fs)				{
					fs->w				(saved->GetBufferPointer(),saved->GetBufferSize());
					FS.w_close			(fs);
				}
				_RELEASE			(saved);

				// cleanup
				_RELEASE			(texture);
			}
			break;
		case IRender_interface::SM_NORMAL:
			{
				string64			t_stemp;
				string_path			buf;
				sprintf				(buf,"ss_%s_%s_(%s).jpg",Core.UserName,timestamp(t_stemp),g_pGameLevel->name().c_str());
				ID3DXBuffer*		saved	= 0;
				CHK_DX				(D3DXSaveSurfaceToFileInMemory (&saved,D3DXIFF_JPG,pFB,0,0));
				IWriter*		fs	= FS.w_open	("$screenshots$",buf); R_ASSERT(fs);
				fs->w				(saved->GetBufferPointer(),saved->GetBufferSize());
				FS.w_close			(fs);
				_RELEASE			(saved);
				if (strstr(Core.Params,"-ss_tga"))	{ // hq
					sprintf				(buf,"ssq_%s_%s_(%s).tga",Core.UserName,timestamp(t_stemp),g_pGameLevel->name().c_str());
					ID3DXBuffer*		saved	= 0;
					CHK_DX				(D3DXSaveSurfaceToFileInMemory (&saved,D3DXIFF_TGA,pFB,0,0));
					IWriter*		fs	= FS.w_open	("$screenshots$",buf); R_ASSERT(fs);
					fs->w				(saved->GetBufferPointer(),saved->GetBufferSize());
					FS.w_close			(fs);
					_RELEASE			(saved);
				}
			}
			break;
		case IRender_interface::SM_FOR_LEVELMAP:
		case IRender_interface::SM_FOR_CUBEMAP:
			{
				string64			t_stemp;
				string_path			buf;
				VERIFY				(name);
				strconcat			(buf,"ss_",Core.UserName,"_",timestamp(t_stemp),"_#",name);
				strcat				(buf,".tga");
				IWriter*		fs	= FS.w_open	("$screenshots$",buf); R_ASSERT(fs);
				TGAdesc				p;
				p.format			= IMG_24B;

				// save
				u32* data			= (u32*)xr_malloc(Device.dwHeight*Device.dwHeight*4);
				imf_Process			(data,Device.dwHeight,Device.dwHeight,(u32*)D.pBits,Device.dwWidth,Device.dwHeight,imf_lanczos3);
				p.scanlenght		= Device.dwHeight*4;
				p.width				= Device.dwHeight;
				p.height			= Device.dwHeight;
				p.data				= data;
				p.maketga			(*fs);
				xr_free				(data);

				FS.w_close			(fs);
			}
			break;
	}

	_RELEASE		(pFB);
}
