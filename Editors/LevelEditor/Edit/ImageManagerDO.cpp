#include "stdafx.h"
#pragma hdrstop

#include "ImageManager.h"
#include "ETextureParams.h"
#include "ImageThumbnail.h"
#include "xrImage_Resampler.h"
#include "freeimage.h"
#include "Image.h"
#include "ui_main.h"
#include "EditObject.h"
/*
void CDeflector::RemapUV        (xr_vector<UVtri>& dest, u32 base_u, u32 base_v, u32 size_u, u32 size_v, u32 lm_u, u32 lm_v, BOOL bRotate)
{
    dest.clear      ();
    dest.reserve(UVpolys.size());
        
    // UV rect (actual)
    Fvector2                a_min,a_max,a_size;
    GetRect         (a_min,a_max);
    a_size.sub      (a_max,a_min);
        
    // UV rect (dedicated)
    Fvector2                d_min,d_max,d_size;
    d_min.x         = (float(base_u)+.5f)/float(lm_u);
    d_min.y         = (float(base_v)+.5f)/float(lm_v);
    d_max.x         = (float(base_u+size_u)-.5f)/float(lm_u);
    d_max.y         = (float(base_v+size_v)-.5f)/float(lm_v);
    if (d_min.x>=d_max.x)   { d_min.x=d_max.x=(d_min.x+d_max.x)/2; d_min.x-=EPS_S; d_max.x+=EPS_S; }
    if (d_min.y>=d_max.y)   { d_min.y=d_max.y=(d_min.y+d_max.y)/2; d_min.y-=EPS_S; d_max.y+=EPS_S; }
    d_size.sub      (d_max,d_min);
        
    // Remapping
    Fvector2                tc;
    UVtri           tnew;
    if (bRotate)    {
            for (UVIt it = UVpolys.begin(); it!=UVpolys.end(); it++)
            {
                    UVtri&  T       = *it;
                    tnew.owner      = T.owner;
                    for (int i=0; i<3; i++) 
                    {
                            tc.x = ((T.uv[i].y-a_min.y)/a_size.y)*d_size.x + d_min.x;
                            tc.y = ((T.uv[i].x-a_min.x)/a_size.x)*d_size.y + d_min.y;
                            tnew.uv[i].set(tc);
                    }
                    dest.push_back  (tnew);
            }
    } else {
            for (UVIt it = UVpolys.begin(); it!=UVpolys.end(); it++)
            {
                    UVtri&  T       = *it;
                    tnew.owner      = T.owner;
                    for (int i=0; i<3; i++) 
                    {
                            tc.x = ((T.uv[i].x-a_min.x)/a_size.x)*d_size.x + d_min.x;
                            tc.y = ((T.uv[i].y-a_min.y)/a_size.y)*d_size.y + d_min.y;
                            tnew.uv[i].set(tc);
                    }
                    dest.push_back  (tnew);
            }
    }
}
*/

IC void blit(u32* dest, u32 ds_x, u32 ds_y, u32* src, u32 ss_x, u32 ss_y, u32 px, u32 py)
{
    R_ASSERT(ds_x>=(ss_x+px));
    R_ASSERT(ds_y>=(ss_y+py));
    for (u32 y=0; y<ss_y; y++)
        for (u32 x=0; x<ss_x; x++){
            u32 dx = px+x;
            u32 dy = py+y;
            dest[dy*ds_x+dx] = src[y*ss_x+x];
        }
}

IC void blit_r(u32* dest, u32 ds_x, u32 ds_y, u32* src, u32 ss_x, u32 ss_y, u32 px, u32 py)
{
    R_ASSERT(ds_x>=(ss_y+px));
    R_ASSERT(ds_y>=(ss_x+py));
    for (u32 y=0; y<ss_y; y++)
        for (u32 x=0; x<ss_x; x++){
            u32 dx = px+y;
            u32 dy = py+x;
            dest[dy*ds_x+dx] = src[y*ss_x+x];
        }
}
// Rendering of rect
IC void _rect_register(U8Vec& mask, int dest_width, int dest_height, Irect& R)
{
    u32     s_x                     = R.width()+1;
    u32     s_y                     = R.height()+1;
        
    // Normal (and fastest way)
    for (u32 y=0; y<s_y; y++){
        u8* P 	= mask.begin()+(y+R.lt.y)*dest_width+R.lt.x;  // destination scan-line
        for (u32 x=0; x<s_x; x++,P++) *P = 0xff;
    }
}

// Test of per-pixel intersection (surface test)
IC bool _rect_test(U8Vec& mask, int dest_width, int dest_height, Irect& R)
{
    u32     s_x                     = R.width()+1;
    u32     s_y                     = R.height()+1;

    if (R.rb.x>=dest_width)			return false;
    if (R.rb.y>=dest_height)		return false;
    // Normal (and fastest way)
    for (u32 y=0; y<s_y; y++){
        u8* P 	= mask.begin()+(y+R.lt.y)*dest_width+R.lt.x;  // destination scan-line
        for (u32 x=0; x<s_x; x++,P++) if (*P) return false;
    }
    // It's OK to place it
    return true;
}

// Check for intersection
IC bool _rect_place(U8Vec& mask, int dest_width, int dest_height, Irect& r, BOOL& bRotated)
{
    Irect R;

    u32 x_max = dest_width -r.rb.x; 
    u32 y_max = dest_height-r.rb.y; 

    // Rotated
    {
	    bRotated = FALSE;
        for (u32 _Y=0; _Y<y_max; _Y++){
            for (u32 _X=0; _X<x_max; _X++){
                if (mask[_Y*dest_width+_X]) continue;
                R.set(_X,_Y,_X+r.rb.x,_Y+r.rb.y);
                if (_rect_test(mask,dest_width,dest_height,R)){
                    _rect_register  (mask,dest_width,dest_height,R);
                    r.set			(R);
                    return true;
                }
            }
        }
    }

    // Normal
    {
	    bRotated = TRUE;
        for (u32 _Y=0; _Y<y_max; _Y++){
            for (u32 _X=0; _X<x_max; _X++){
                if (mask[_Y*dest_width+_X]) continue;
                R.set(_X,_Y,_X+r.rb.y,_Y+r.rb.x);
                if (_rect_test(mask,dest_width,dest_height,R)) {
                    _rect_register  (mask,dest_width,dest_height,R);
                    r.set			(R);
                    return true;
                }
            }
        }
    }
        
    return false;
};

struct SrcItem{
	AnsiString  tname;
    U32Vec 		data;
    u32 		w,h,a;
    int 		LongestEdge()const 	{return (w>h)?w:h;}
    int			Area()const			{return w*h;}
};
DEFINE_VECTOR(SrcItem,SrcItemVec,SrcItemIt);
extern bool Surface_Load(LPCSTR full_name, U32Vec& data, u32& w, u32& h, u32& a);

bool item_sort_pred(const SrcItem& item0, const SrcItem& item1){return ((item0.Area()>item1.Area())||(item0.LongestEdge()>item1.LongestEdge()));}

int CImageManager::CreateMergedTexture(AStringVec& src_names, LPCSTR dest_name, int dest_width, int dest_height, Fvector2Vec& dest_offset, Fvector2Vec& dest_scale, boolVec& dest_rotate)
{
	if (src_names.empty()) return -1;
	U32Vec 	dest_pixels	(dest_width*dest_height,0); 
	U8Vec 	dest_mask	(dest_width*dest_height,0); 

    int max_area		= dest_width*dest_height;
    int calc_area		= 0;

    SrcItemVec src_items(src_names.size());
    SrcItemIt s_it		= src_items.begin();
    for (AStringIt n_it=src_names.begin(); n_it!=src_names.end(); n_it++,s_it++){
        AnsiString 		t_name;
        FS.update_path	(t_name,_textures_,n_it->c_str());
        if (!Surface_Load((t_name+".tga").c_str(),s_it->data,s_it->w,s_it->h,s_it->a)&&
        	!Surface_Load((t_name+".bmp").c_str(),s_it->data,s_it->w,s_it->h,s_it->a)){
            ELog.DlgMsg	(mtError,"Can't load texture '%s'. Check file existence.",n_it->c_str());
            return -1;
        }
        calc_area		+= (s_it->w*s_it->h);
        if (calc_area>max_area) 
        	return 0;
        s_it->tname		= *n_it;
    }

    std::sort			(src_items.begin(),src_items.end(),item_sort_pred);
    for (s_it = src_items.begin(); s_it!=src_items.end(); s_it++){
		Irect R;		R.set(0,0, s_it->w-1,s_it->h-1);
        BOOL bRotated;
        if (!_rect_place(dest_mask,dest_width,dest_height,R,bRotated)) return 0;
		Fvector2 offs,scale;
        offs.x			= float(R.lt.x)/float(dest_width);
        offs.y			= float(R.lt.y)/float(dest_height);
        scale.x			= float(R.width()+1)/float(dest_width);
        scale.y			= float(R.height()+1)/float(dest_height);
        dest_offset.push_back	(offs);
        dest_scale.push_back	(scale);
        dest_rotate.push_back	(bRotated);
        // Perform BLIT
        if (!bRotated) 	blit	(dest_pixels.begin(),dest_width,dest_height,s_it->data.begin(),s_it->w,s_it->h,R.lt.x,R.lt.y);
        else            blit_r  (dest_pixels.begin(),dest_width,dest_height,s_it->data.begin(),s_it->w,s_it->h,R.lt.x,R.lt.y);
    }

    // all right. make texture.
    src_names.clear		();
    for (s_it = src_items.begin(); s_it!=src_items.end(); s_it++)
    	src_names.push_back		(s_it->tname);
    
    MakeGameTexture		(dest_name,dest_pixels.begin(),dest_width,dest_height,STextureParams::tfADXT1,true);

    return 1;
}

 