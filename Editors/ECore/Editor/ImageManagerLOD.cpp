#include "stdafx.h"
#pragma hdrstop

#include "ImageManager.h"

#include "d3dutils.h"
void CImageManager::CreateLODTexture(const Fbox& bb, U32Vec& tgt_data, u32 tgt_w, u32 tgt_h, int samples)
{
	U32Vec pixels;

    Fvector C;
    Fvector S;
    bb.getradius				(S);
    float R 					= _max(S.x,S.z);
    bb.getcenter				(C);

    Fmatrix save_projection		= Device.mProjection;
    Fvector save_pos 			= Device.m_Camera.GetPosition();
    Fvector save_hpb 			= Device.m_Camera.GetHPB();
    float save_far		 		= Device.m_Camera._Zfar();
	ECameraStyle save_style 	= Device.m_Camera.GetStyle();

    float D		= 500.f;
    u32 pitch 					= tgt_w*samples;

    tgt_data.resize				(pitch*tgt_h);
	Device.m_Camera.SetStyle	(csPlaneMove);
    Device.m_Camera.SetDepth	(D*2,true);

    // save render params
    Flags32 old_flag			= psDeviceFlags;
	u32 old_dwFillMode			= Device.dwFillMode;
    u32 old_dwShadeMode			= Device.dwShadeMode;
    // set render params

    u32 cc						= 	EPrefs.scene_clear_color;
    EPrefs.scene_clear_color 	= 	0x0000000;
    psDeviceFlags.zero			();
	psDeviceFlags.set			(rsFilterLinear,TRUE);
	Device.dwFillMode			= D3DFILL_SOLID;
    Device.dwShadeMode			= D3DSHADE_GOURAUD;

    SetCamera(0,C,S.y,R,D);

    for (int frame=0; frame<samples; frame++){
    	float angle 			= frame*(PI_MUL_2/samples);
	    SetCamera				(angle,C,S.y,R,D);
	    Device.MakeScreenshot	(pixels,tgt_w,tgt_h);
        // copy LOD to final
		for (u32 y=0; y<tgt_h; y++)
    		CopyMemory			(tgt_data.begin()+y*pitch+frame*tgt_w,pixels.begin()+y*tgt_w,tgt_w*sizeof(u32));
    }

    ApplyBorders				(tgt_data,pitch,tgt_h);

    // flip data
	for (u32 y=0; y<tgt_h/2; y++){
		u32 y2 = tgt_h-y-1;
		for (u32 x=0; x<pitch; x++){
        	std::swap	(tgt_data[y*pitch+x],tgt_data[y2*pitch+x]);	
		}
	}
        
    // restore render params
	Device.dwFillMode			= old_dwFillMode;
    Device.dwShadeMode			= old_dwShadeMode;
    psDeviceFlags 				= old_flag;
    EPrefs.scene_clear_color 	= cc;

	Device.m_Camera.SetStyle	(save_style);
    RCache.set_xform_project	(save_projection);
    Device.m_Camera.Set			(save_hpb,save_pos);
    Device.m_Camera.Set			(save_hpb,save_pos);
    Device.m_Camera.SetDepth	(save_far,false);
}

void CImageManager::CreateLODTexture(const Fbox& bbox, LPCSTR tex_name, u32 tgt_w, u32 tgt_h, int samples, int age)
{
    U32Vec 						new_pixels;

	CreateLODTexture			(bbox,new_pixels,tgt_w,tgt_h,samples);

    xr_string out_name;
    FS.update_path				(out_name,_textures_,tex_name);

    CImage* I 					= xr_new<CImage>();
    I->Create					(tgt_w*samples,tgt_h,new_pixels.begin());
    I->Vflip					();
    I->SaveTGA					(out_name.c_str());
    xr_delete					(I);
    FS.set_file_age				(out_name.c_str(), age);

    SynchronizeTexture			(tex_name,age);
}


