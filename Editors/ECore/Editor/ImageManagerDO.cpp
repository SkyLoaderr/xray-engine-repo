#include "stdafx.h"
#pragma hdrstop

#include "ImageManager.h"

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

    // Normal
    if ((r.rb.x<dest_width)&&(r.rb.y<dest_height)){
        u32 x_max = dest_width -r.rb.x; 
        u32 y_max = dest_height-r.rb.y; 
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

    // Rotated
    if ((r.rb.y<dest_width)&&(r.rb.x<dest_height)){
        u32 x_max = dest_width -r.rb.y; 
        u32 y_max = dest_height-r.rb.x; 
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
IC bool operator == (const SrcItem& a, const AnsiString& nm){return a.tname==nm;}
DEFINE_VECTOR(SrcItem,SrcItemVec,SrcItemIt);
extern bool Surface_Load(LPCSTR full_name, U32Vec& data, u32& w, u32& h, u32& a);

bool item_area_sort_pred(const SrcItem& item0, const SrcItem& item1){return ((item0.Area()>item1.Area())||(item0.LongestEdge()>item1.LongestEdge()));}

int CImageManager::CreateMergedTexture(const RStringVec& _names, LPCSTR dest_name, STextureParams::ETFormat fmt, int dest_width, int dest_height, Fvector2Vec& dest_offset, Fvector2Vec& dest_scale, boolVec& dest_rotate, U32Vec& dest_remap)
{
	if (_names.empty()) return -1;

    dest_offset.clear	();
    dest_scale.clear	();
    dest_rotate.clear	();

    RStringVec src_names= _names;
    dest_remap.resize	(src_names.size());
    std::sort			(src_names.begin(),src_names.end());
    src_names.erase		(std::unique(src_names.begin(),src_names.end()),src_names.end());
    
	U32Vec 	dest_pixels	(dest_width*dest_height,0); 
	U8Vec 	dest_mask	(dest_width*dest_height,0); 

    int max_area		= dest_width*dest_height;
    int calc_area		= 0;

    SrcItemVec src_items(src_names.size());
    SrcItemIt s_it		= src_items.begin();
    for (RStringVecIt n_it=src_names.begin(); n_it!=src_names.end(); n_it++,s_it++){
        std::string		t_name;
        FS.update_path	(t_name,_textures_,**n_it);
        if (!Surface_Load(EFS.ChangeFileExt(t_name,".tga").c_str(),s_it->data,s_it->w,s_it->h,s_it->a)){
            ELog.DlgMsg	(mtError,"Can't load texture '%s'. Check file existence.",**n_it);
            return -1;
        }
        calc_area		+= (s_it->w*s_it->h);
        if (calc_area>max_area) 
        	return 0;
        s_it->tname		= **n_it;
    }

    std::sort			(src_items.begin(),src_items.end(),item_area_sort_pred);

    for (u32 k=0; k<_names.size(); k++){
    	AnsiString nm	= *_names[k];
        s_it			= std::find(src_items.begin(),src_items.end(),nm); VERIFY(s_it!=src_items.end());
    	dest_remap[k]	= s_it-src_items.begin();
    }
    
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
    AnsiString fn		= ChangeFileExt	(dest_name,".dds");
    MakeGameTexture		(fn.c_str(),dest_pixels.begin(),dest_width,dest_height,fmt,STextureParams::ttImage,STextureParams::flDitherColor|STextureParams::flGenerateMipMaps);

    return 1;
}

int	CImageManager::CreateMergedTexture	(const RStringVec& src_names, LPCSTR dest_name, STextureParams::ETFormat fmt, int dest_width_min, int dest_width_max, int dest_height_min, int dest_height_max, Fvector2Vec& dest_offset, Fvector2Vec& dest_scale, boolVec& dest_rotate, U32Vec& remap)
{
	BOOL WH	= FALSE;
    int res	= 0;
    int w	= dest_width_min;
	int h	= dest_height_min;
    do{
		res	= ImageLib.CreateMergedTexture(src_names,dest_name,fmt,w,h,dest_offset,dest_scale,dest_rotate,remap);
        if (WH) w *= 2;
        else	h *= 2;
        WH	= !WH;
    }while((0==res)&&(w<=dest_width_max)&&(h<=dest_height_max));
    return res;
}

void CImageManager::MergedTextureRemapUV(float& dest_u, float& dest_v, float src_u, float src_v, const Fvector2& offs, const Fvector2& scale, bool bRotate)
{
    if (bRotate){
        dest_u	= scale.x*src_v+offs.x;
        dest_v	= scale.y*src_u+offs.y;
    }else{
        dest_u	= scale.x*src_u+offs.x;
        dest_v	= scale.y*src_v+offs.y;
    }
}
 
