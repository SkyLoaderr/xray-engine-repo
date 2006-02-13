//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "main.h"
#include "image.h"
#include "PropertiesList.h"               
#include "LogForm.h"
#include "StatisticGrid.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "ElFolderDlg"
#pragma link "MXCtrls"
#pragma link "multi_edit"
#pragma link "mxPlacemnt"
#pragma link "ElScrollBar"
#pragma link "ElXPThemedControl"
#pragma link "ElTree"
#pragma link "ElTreeGrids"
#pragma link "ElBiProgr"
#pragma link "CGAUGES"
#pragma resource "*.dfm"
TfrmMain *frmMain;
//---------------------------------------------------------------------------
struct SMemCat{
	enum{
    	flVisible	= (1<<0)
    };
	shared_str	name;			// key
	Flags32		flags;
    u32			count;
    u32			min_sz;
    u32			max_sz;
    u32			real_sz;
    u32			pure_sz;
    			SMemCat	(LPCSTR nm,u32 r_sz,u32 p_sz){name=nm;flags.assign(flVisible);count=1;min_sz=r_sz;max_sz=r_sz;real_sz=r_sz;pure_sz=p_sz;}
};
DEFINE_VECTOR	(SMemCat*,MemCatVec,MemCatVecIt);
MemCatVec		m_cat_data;
bool 			sort_cat_name_pred	(const SMemCat* a, const SMemCat* b){return strcmp(a->name.c_str(),b->name.c_str())<0;}
bool 			find_cat_name_pred	(const SMemCat* a, LPCSTR b) 		{return strcmp(a->name.c_str(),b)<0;}
SMemCat* 		create_cat			(LPCSTR key, u32 r_sz, u32 p_sz)
{
	MemCatVecIt c_it = std::lower_bound(m_cat_data.begin(),m_cat_data.end(),key,find_cat_name_pred);
    if (c_it!=m_cat_data.end()){
     	if (0==strcmp((*c_it)->name.c_str(),key)){
            (*c_it)->count++;
            (*c_it)->min_sz = _min((*c_it)->min_sz,r_sz);
            (*c_it)->max_sz = _max((*c_it)->max_sz,r_sz);
            (*c_it)->real_sz+= r_sz;
            (*c_it)->pure_sz+= p_sz;
            return *c_it;
        }else{
		    m_cat_data.insert	(c_it,xr_new<SMemCat>(key,r_sz,p_sz));
			return 				m_cat_data.back();
        }
    }else{
        m_cat_data.push_back	(xr_new<SMemCat>(key,r_sz,p_sz));
        return 					m_cat_data.back();
    }
}
struct SMemItem{
	SMemCat*	cat;
    u32 		ptr;
    u32 		sz;
    u32 		r_sz;
    s8 			pool_id;
};
DEFINE_VECTOR	(SMemItem,MemItemVec,MemItemVecIt);
MemItemVec 		m_items_data;
U32Vec			m_items_ptr;
U32Vec			m_items_cat;
bool sort_ptr_pred	(const u32& a, const u32& b) 			{return m_items_data[a].ptr<m_items_data[b].ptr;}
bool find_ptr_pred	(const u32& a, u32 val) 				{return m_items_data[a].ptr<val;}
bool sort_cat_pred	(const u32& a, const u32& b) 			{return m_items_data[a].cat<m_items_data[b].cat;}
bool find_cat_pred	(const u32& a, const SMemCat* val) 	 	{return m_items_data[a].cat<val;}

__inline const SMemItem& 	get_item(u32 idx)	{return m_items_data[idx];}

u32 			m_pools_count	= 0;
u32 			m_pools_ebase	= 0;
struct SPoolItem{
    u32 		base_size;
    u32 		block_count;
    u32 		usage;
    SPoolItem():usage(0),block_count(0),base_size(0){}
};
DEFINE_VECTOR(SPoolItem,PoolItemVec,PoolItemVecIt);
PoolItemVec		m_pool_data;
bool sort_base_pred	(const SPoolItem& a, const SPoolItem& b){return a.base_size<b.base_size;}
bool find_base_pred	(const SPoolItem& a, u32 val) 			{return a.base_size<val;}

U32Vec			m_memory;
U32Vec 			m_cell_frame;
U32Vec 			m_full_frame;
u32 			m_begin_ptr		= (u32)-1;
u32 			m_end_ptr		= 0;
u32				m_memory_allocated	= 0;
u32 			m_cell_size		= 1024;
const u32		m_cell_px		= 4;
u32				m_draw_offs		= 0;
u32				m_min_block		= 0xFFFFFFFF;
u32				m_max_block		= 0;
u32				m_curr_cell		= 0;
TProperties*	m_Props			= 0;
TProperties*	m_CategoryProps	= 0;


__inline bool 				items_valid()		{return !m_items_data.empty();}
__inline bool 				pool_valid()		{return !m_pool_data.empty();}
__inline bool 				stat_valid()		{return items_valid()||pool_valid();}

// color defs
u32		        COLOR_DEF_UNUSED= 0x00D1D0D0;
u32		        COLOR_DEF_EMPTY = 0x00F1F0F0;
u32		        COLOR_DEF_FULL 	= 0x00008000; 	// 100%
u32		        COLOR_DEF_100 	= 0x0080C000; 	// (75%-100%)
u32		        COLOR_DEF_75 	= 0x00D08000;	// (50%-75%]
u32		        COLOR_DEF_50 	= 0x000080D0;	// (25%-50%]
u32		        COLOR_DEF_25 	= 0x00FF0000;	// (0%-25%]

xr_token						cell_token	[ ]={
//	{ "8 bytes",		  		8		  	},
	{ "16 bytes",		  	  	16		  	},
	{ "32 bytes",		  	  	32		  	},
	{ "64 bytes",		  	  	64		  	},
	{ "128 bytes",		  	   	128		  	},
	{ "256 bytes",		  	   	256		  	},
	{ "512 bytes",		  	   	512		  	},
	{ "1024 bytes",		  	   	1024	  	},
	{ "2048 bytes",		  	   	2048	  	},
	{ "4096 bytes",		    	4096	  	},
	{ "8192 bytes",		    	8192	  	},
	{ "16384 bytes",	  	   	16384	  	},
	{ 0,					   	0   	 	}
};

void DrawBitmap(TMxPanel* panel, const Irect& r, u32* data, int w, int h)
{
	HDC hdc		= panel->Canvas->Handle;
    
    BITMAPINFO  bmi;
    bmi.bmiHeader.biSize 			= sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth 			= w;
    bmi.bmiHeader.biHeight 			= h;
    bmi.bmiHeader.biPlanes 			= 1;
    bmi.bmiHeader.biBitCount 		= 32;
    bmi.bmiHeader.biCompression 	= BI_RGB;
    bmi.bmiHeader.biSizeImage 		= 0;
    bmi.bmiHeader.biXPelsPerMeter 	= 1;
    bmi.bmiHeader.biYPelsPerMeter 	= 1;
    bmi.bmiHeader.biClrUsed 		= 0;
    bmi.bmiHeader.biClrImportant 	= 0;

//	SetMapMode		(hdc,	MM_ANISOTROPIC	);
//	SetStretchBltMode(hdc,	HALFTONE		);
    int err 		= StretchDIBits	(hdc, 	r.x1, 	r.y1, 	r.x2-r.x1, 	r.y2-r.y1,
    							 	0,		h+1,	w,		-h,			data, &bmi,
                    				DIB_RGB_COLORS, SRCCOPY);
    if (err==GDI_ERROR){
    	Log("!StretchDIBits - Draw failed.");
    }
}

u32 cell_intersect(u32 c_min, u32 c_max, u32 i_min, u32 i_max)
{
	u32 x = _max(c_min,i_min);
	u32 y = _min(c_max,i_max);
	return (y>x)?y-x:0;
}

void TfrmMain::RealResizeBuffer()
{
	if (items_valid()){
        m_memory.resize	(1+(m_end_ptr-m_begin_ptr)/m_cell_size);
        std::fill		(m_memory.begin(),m_memory.end(),0x00000000);
        for (MemItemVecIt it=m_items_data.begin(); it!=m_items_data.end(); it++){
        	if (it->cat->flags.is(SMemCat::flVisible)){
                u32 i_begin_ptr	= it->ptr-m_begin_ptr;
                u32 i_end_ptr	= i_begin_ptr+it->r_sz;
                u32 c_begin		= i_begin_ptr/m_cell_size;
                u32 c_end		= i_end_ptr/m_cell_size;
                for (u32 c=c_begin; c<=c_end; c++)
                    m_memory[c]	+= cell_intersect(c*m_cell_size,(c+1)*m_cell_size,i_begin_ptr,i_end_ptr);
            }
        }     
        u32 m_sz 					= m_memory.size();
        for (u32 k=0;k<m_sz;k++){ 
            u32& color				= m_memory[k];
            float w					= float(color)/float(m_cell_size);
            if (color==0)				color	= COLOR_DEF_EMPTY;
            else if (color==m_cell_size)color	= COLOR_DEF_FULL;
            else if (w>0.75f)			color	= COLOR_DEF_100;
            else if (w>0.5f)			color	= COLOR_DEF_75;
            else if (w>0.25f)			color	= COLOR_DEF_50;
            else 						color	= COLOR_DEF_25;
        }
    }
	m_Flags.set(flResizeBuffer,FALSE);    
}              

void TfrmMain::RealRedrawBuffer()
{
    u32 d_w				= paMem->Width/m_cell_px;
    u32 d_h				= paMem->Height/m_cell_px;
    if (items_valid()){
        u32 req_frame_sz		= d_w*d_h;
        u32 real_frame_sz		= _min(req_frame_sz,m_memory.size()-m_draw_offs);
        if (m_full_frame.size()!= req_frame_sz)
        	m_full_frame.resize	(req_frame_sz);
		CopyMemory				(m_full_frame.begin(),m_memory.begin()+m_draw_offs,real_frame_sz*4);
        if (req_frame_sz!=real_frame_sz)
		    std::fill			(m_full_frame.begin()+real_frame_sz,m_full_frame.begin()+d_w*d_h,COLOR_DEF_EMPTY);
    
        DrawBitmap				(paMem,Irect().set(0,0,paMem->Width,paMem->Height),
                           		m_full_frame.begin(), d_w,d_h);
	    // UI update
        if (m_memory.size()>d_w*d_h){
            sbMem->Max			= 2+m_memory.size()/d_w-d_h;
			sbMem->LargeChange	= d_h-1;
			sbMem->SmallChange	= 1;
        	sbMem->Enabled		= true;
        }else{
        	sbMem->Position		= 0;
        	sbMem->Enabled		= false;
        }

        RedrawDetailed			();
    }
    
    // grid
    paMem->Canvas->Pen->Color 	= TColor(0x00808080);
    for (u32 k=0; k<paMem->Height/m_cell_px; k++){
        paMem->Canvas->MoveTo(0,k*m_cell_px);
        paMem->Canvas->LineTo(paMem->Width,k*m_cell_px);
    }

    // current cell
    if (!m_memory.empty() && ((m_curr_cell>=m_draw_offs)||(m_curr_cell<m_draw_offs+d_w*d_h))){
        paMem->Canvas->Pen->Color 	= TColor(0x00000000);
        u32 c			= m_curr_cell-m_draw_offs;
        u32 x			= (c%d_w)*m_cell_px;
        u32 y			= (c/d_w)*m_cell_px;
        paMem->Canvas->MoveTo(x,y);
        paMem->Canvas->LineTo(x+m_cell_px,y);
        paMem->Canvas->LineTo(x+m_cell_px,y+m_cell_px);
        paMem->Canvas->LineTo(x,y+m_cell_px);
        paMem->Canvas->LineTo(x,y);
    }

	UpdateProperties	();
    m_Flags.set			(flRedrawBuffer,FALSE);	
}

bool is_chunk		(LPCSTR str, int& num)
{
	return			(1==sscanf(str,"$BEGIN CHUNK #%d\n",&num));
}

void __fastcall TfrmMain::OpenClick(TObject *Sender)
{
	if (od->Execute()){
    	LPCSTR fn 		= od->FileName.c_str();
        frmMain->Caption= AnsiString().sprintf("Memory Dump - [%s]",fn);
        IReader* F		= FS.r_open	(fn); VERIFY(F);                            
        for (MemCatVecIt c_it=m_cat_data.begin(); c_it!=m_cat_data.end(); c_it++)
        	xr_delete	(*c_it);
		m_cat_data.clear();            
    	m_items_data.clear	();
        m_items_data.reserve(250000);
        m_pool_data.clear	();
        m_pool_data.reserve	(mem_pools_count);

		m_min_block		= 0xFFFFFFFF;
		m_max_block		= 0;
		m_begin_ptr		= 0xFFFFFFFF;
		m_end_ptr		= 0;
        m_curr_cell		= 0;
        m_memory_allocated	= 0;

        LPCSTR src	    = (LPCSTR)F->pointer();
        LPSTR data	    = (LPSTR)F->pointer();
        string1024	    name;
        int 			chunk=-1;
        cgProgress->Show		();
		cgProgress->MaxValue 	= F->length();
        do{
            LPSTR e		= data;
            for (;e[0]!='\n';e++){}
            e++;
            if (is_chunk(data,chunk)){
            }else if (chunk==0){
				if (2==sscanf(data,"POOL: %d %dKb\n",&m_pools_count,&m_pools_ebase))
	                m_pool_data.resize(m_pools_count);
            }else if (chunk==1){
                SPoolItem	item;
                u32 		idx;
                if (3==sscanf(data,"%2d: %d %db\n",&idx,&item.block_count,&item.base_size))
	                m_pool_data[idx] = item;
            }else if (chunk==2){
                SMemItem	item;
                if (4==sscanf(data,"0x%08X[%2d]: %d %[^\n]",&item.ptr,&item.pool_id,&item.sz,name)){
                    if (item.sz > 16*65)	item.r_sz	= (((34 + item.sz) - 1)/16 + 1)*16;
                    else					item.r_sz	= (item.sz/16 + 1)*16;
                    item.cat	= create_cat(_Trim(name),item.r_sz,item.sz);
                    if ((item.ptr+item.r_sz)>m_end_ptr) m_end_ptr 	= item.ptr+item.r_sz;
                    if (item.ptr<m_begin_ptr) 			m_begin_ptr = item.ptr;
                    if (item.r_sz<m_min_block)			m_min_block	= item.r_sz;
                    if (item.r_sz>m_max_block)			m_max_block	= item.r_sz;
					m_memory_allocated					+= item.r_sz;
                    m_items_data.push_back(item);
                }
            }
            data		= e;
            if ((e-src)>=F->length()) break;
            if ((e-src)%1000 == 0) cgProgress->Progress = e-src;
        }while(true);
        FS.r_close		(F);

        if (!m_pool_data.empty()){
            for (MemItemVecIt it=m_items_data.begin(); it!=m_items_data.end(); it++)
            	if (it->pool_id>-1) m_pool_data[it->pool_id].usage += it->r_sz;
        }
        if (!m_cat_data.empty()){
        	std::sort(m_cat_data.begin(),m_cat_data.end(),sort_cat_name_pred);
        }
        if (m_items_data.size()){
            m_items_ptr.resize	(m_items_data.size());	
            m_items_cat.resize	(m_items_data.size());	
            for (u32 k=0; k<m_items_data.size(); ++k){m_items_ptr[k]=k;m_items_cat[k]=k;}
            std::sort		(m_items_ptr.begin(),m_items_ptr.end(),sort_ptr_pred);
            std::sort		(m_items_cat.begin(),m_items_cat.end(),sort_cat_pred);
            ResizeBuffer	();
            RedrawBuffer	();
        }
        if (!items_valid()){
			m_min_block		= 0;
			m_max_block		= 0;
			m_begin_ptr		= 0;
			m_end_ptr		= 0;
        }

        UpdateProperties		();
        UpdateCatProperties		();
        cgProgress->Progress 	= 0;
        cgProgress->Hide		();
    }
}
//---------------------------------------------------------------------------

IC u32 GetPowerOf2Plus1	(u32 v)
{
    u32 cnt=0;
    while (v) {v>>=1; cnt++; };
    return cnt;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::IdleHandler(TObject *Sender, bool &Done)
{
    Done = false;
    OnFrame();
}
//---------------------------------------------------------------------------

__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

struct aaa{
	AnsiString name;
    int cnt;
    aaa(LPCSTR nm){name=nm;cnt=0;}
};
DEFINE_VECTOR(aaa,aaa_vec,aaa_vec_it);
aaa_vec aaas;

void append_aaa(LPCSTR nm, int sign)
{
	for (aaa_vec_it it=aaas.begin(); it!=aaas.end(); it++)
    	if (it->name==nm){
        	it->cnt+=sign;
            return;
        }
    aaas.push_back(aaa(nm));
    aaas.back().cnt+=sign;
}
/*
void ref_snd_counter		()
{
	aaas.clear();
    IReader* F	  	= FS.r_open	("x:\\a.txt"); VERIFY(F);                            
    LPCSTR src	    = (LPCSTR)F->pointer();
    LPSTR data	    = (LPSTR)F->pointer();
    string1024	    name;
    do{
        LPSTR e		= data;
        for (;e[0]!='\n';e++){}
        e++;
        char		c_sign;
        if (2==sscanf(data,"%c rsd %[^\n]",&c_sign,&name))
        	append_aaa(_Trim(name),c_sign=='+'?1:-1);
        data		= e;
        if ((e-src)>=F->length()) break;
    }while(true);
    FS.r_close		(F);

	for (aaa_vec_it it=aaas.begin(); it!=aaas.end(); it++){
    	if (it->cnt!=0)
        {
        	Msg		("%d: %s",it->cnt,it->name.c_str());
        }
    }
}
*/
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
    m_Props 				= TProperties::CreateForm("Description",paInfo,alClient,0,0,0,TProperties::plFolderStore|TProperties::plFullExpand);
	m_CategoryProps    		= TProperties::CreateForm("Category",0,alNone,0,0,0,TProperties::plFolderStore|TProperties::plFullExpand);
    Application->OnIdle 	= IdleHandler;
    m_Flags.zero			();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
	TProperties::DestroyForm(m_Props);
	TProperties::DestroyForm(m_CategoryProps);
}
//---------------------------------------------------------------------------

void TfrmMain::OnFrame()
{
	if (m_Flags.is(flUpdateProps))		{ RealUpdateProperties(); 	}
    if (m_Flags.is(flUpdateCatProps))	{ RealUpdateCatProperties();}
	if (m_Flags.is(flResizeBuffer))		{ RealResizeBuffer(); 		}
	if (m_Flags.is(flRedrawBuffer))		{ RealRedrawBuffer(); 		}
	if (m_Flags.is(flRedrawDetailed))	{ RealRedrawDetailed(); 	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::paMemPaint(TObject *Sender)
{
	RedrawBuffer();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::paMemMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
	AnsiString address						= "Address:";
	AnsiString content						= "Content:";
	if (items_valid()){
        u32 local_pos						= m_draw_offs+(Y/m_cell_px)*(paMem->Width/m_cell_px)+(X/m_cell_px);
        u32 real_pos						= m_begin_ptr+iFloor(float(local_pos)*m_cell_size+0.5f);
        U32It it							= std::lower_bound(m_items_ptr.begin(),m_items_ptr.end(),real_pos,find_ptr_pred);
        SStringVec names;
        if ((it==m_items_ptr.end()) && (it!=m_items_ptr.begin())) it--;
        if ((get_item(*it).ptr>real_pos) && (it!=m_items_ptr.begin())) it--;
        while((it!=m_items_ptr.end())&&(get_item(*it).ptr<(real_pos+m_cell_size))){
        	if (get_item(*it).cat->flags.is(SMemCat::flVisible))
                if (get_item(*it).ptr+get_item(*it).r_sz>real_pos)
                    names.push_back			(get_item(*it).cat->name.c_str());
            it++;
        }
        address.sprintf("Address: 0x%08X",real_pos); 
        content.sprintf("Content: %s",_ListToSequence(names).c_str());
    }
    paStatus0->Caption 		= address;
    paStatus1->Caption 		= content;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::paDetMemMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	AnsiString address						= "Address:";
	AnsiString content						= "Content:";
	if (items_valid()){
        u32 local_pos						= 4*((Y/m_cell_px)*(paDetMem->Width/m_cell_px)+(X/m_cell_px));
        if (local_pos<m_cell_size){
            u32 real_pos					= m_begin_ptr+m_curr_cell*m_cell_size+local_pos;
            U32It it						= std::lower_bound(m_items_ptr.begin(),m_items_ptr.end(),real_pos,find_ptr_pred);
            SStringVec names;                                                      
	        if ((it==m_items_ptr.end()) && (it!=m_items_ptr.begin())) it--;
            if ((get_item(*it).ptr>real_pos) && (it!=m_items_ptr.begin())) it--;
            while((it!=m_items_ptr.end())&&(get_item(*it).ptr<(real_pos+4))){
	        	if (get_item(*it).cat->flags.is(SMemCat::flVisible))
    	            if (get_item(*it).ptr+get_item(*it).r_sz>real_pos)
        	            names.push_back	  	(get_item(*it).cat->name.c_str());
                it++;
            }
			address.sprintf("Address: 0x%08X",real_pos); 
			content.sprintf("Content: %s",_ListToSequence(names).c_str());
        }
    }
    paStatus0->Caption 		= address;
    paStatus1->Caption 		= content;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbMemChange(TObject *Sender)
{
    u32 d_w			= paMem->Width/m_cell_px;
    m_draw_offs		= d_w*sbMem->Position;
    RedrawBuffer	(true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::paMemBaseResize(TObject *Sender)
{
	u32 dx			= (paMemBase->Width%m_cell_px);
	u32 dy			= (paMemBase->Height%m_cell_px);
	paMem->Left 	= dx/2;	
    paMem->Top 		= dy/2;	
	paMem->Width 	= paMemBase->Width-dx;
    paMem->Height	= paMemBase->Height-dy;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::paMemMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    m_curr_cell		= m_draw_offs+(Y/m_cell_px)*(paMem->Width/m_cell_px)+(X/m_cell_px);
    RedrawBuffer	();
}
//---------------------------------------------------------------------------


void TfrmMain::RealRedrawDetailed()
{
    u32 d_w				= paDetMem->Width/m_cell_px;
    u32 d_h				= paDetMem->Height/m_cell_px;
	if (items_valid()){
    	if (m_cell_frame.size()!=(d_w*d_h)) m_cell_frame.resize(d_w*d_h);

    	u32 r_begin_ptr	= m_curr_cell*m_cell_size+m_begin_ptr;
    	u32 r_end_ptr	= (m_curr_cell+1)*m_cell_size+m_begin_ptr;
    	u32 r_size		= r_end_ptr-r_begin_ptr;
    	u32 d_size		= r_size/4;

        std::fill		(m_cell_frame.begin(),m_cell_frame.begin()+d_size,COLOR_DEF_EMPTY);
        std::fill		(m_cell_frame.begin()+d_size,m_cell_frame.begin()+d_w*d_h,COLOR_DEF_UNUSED);
        U32It it		= std::lower_bound(m_items_ptr.begin(),m_items_ptr.end(),r_begin_ptr,find_ptr_pred);
        if ((it==m_items_ptr.end()) && (it!=m_items_ptr.begin())) it--;
        if ((get_item(*it).ptr>r_begin_ptr) && (it!=m_items_ptr.begin())) it--;
        while((it!=m_items_ptr.end())&&(get_item(*it).ptr<r_end_ptr)){
        	if (get_item(*it).cat->flags.is(SMemCat::flVisible))
                if (get_item(*it).ptr+get_item(*it).r_sz>r_begin_ptr){
                    int	i_begin	= (_max(r_begin_ptr,get_item(*it).ptr)-r_begin_ptr);
                    int	i_end	= (_min(r_end_ptr,get_item(*it).ptr+get_item(*it).r_sz)-r_begin_ptr);
                    std::fill	(m_cell_frame.begin()+i_begin/4,m_cell_frame.begin()+i_end/4,COLOR_DEF_FULL);
                }
            it++;
        }
    
        DrawBitmap			(paDetMem,Irect().set(0,0,paDetMem->Width,paDetMem->Height),
                            m_cell_frame.begin(), d_w,d_h);
    }
    
    // grid
    paDetMem->Canvas->Pen->Color 	= TColor(0x00808080);
    paDetMem->Canvas->Brush->Style = bsSolid;
    for (u32 k=0; k<paDetMem->Height/m_cell_px; k++){
        paDetMem->Canvas->MoveTo(0,k*m_cell_px);
        paDetMem->Canvas->LineTo(paDetMem->Width,k*m_cell_px);
    }
    m_Flags.set(flRedrawDetailed,FALSE);
}

void __fastcall TfrmMain::paDetMemPaint(TObject *Sender)
{
	RedrawDetailed();
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	bool need_redraw= false;
	int cc			= m_curr_cell;
    u32 d_w			= paMem->Width/m_cell_px;
    u32 d_h			= paMem->Height/m_cell_px;
	switch (Key){
    case VK_LEFT: 	cc--; 		    	need_redraw=true; break;
    case VK_RIGHT: 	cc++; 		    	need_redraw=true; break;
    case VK_DOWN: 	cc+=d_w;	    	need_redraw=true; break;
    case VK_UP: 	cc-=d_w;	    	need_redraw=true; break;

    case VK_NEXT: 	cc+=d_w*d_h;		need_redraw=true; break;
    case VK_PRIOR: 	cc-=d_w*d_h; 	    need_redraw=true; break;
    case VK_END: 	cc=m_memory.size(); need_redraw=true; break;
    case VK_HOME: 	cc=0;       		need_redraw=true; break;
    }
    if (need_redraw){
	    m_curr_cell		= clampr(cc,0,(int)m_memory.size()-1);
        if (m_curr_cell<m_draw_offs)				m_draw_offs = m_curr_cell;
        else if (m_curr_cell>=m_draw_offs+d_w*d_h)	m_draw_offs = m_curr_cell;
        m_draw_offs		= d_w*(m_draw_offs/d_w);
        sbMem->Position	= m_draw_offs/d_w;
    	Key				= 0; 
    	RedrawBuffer	();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::fsStorageRestorePlacement(TObject *Sender)
{
	m_Props->RestoreParams			(fsStorage);
    m_CategoryProps->RestoreParams	(fsStorage);
    COLOR_DEF_UNUSED	= fsStorage->ReadInteger("COLOR_DEF_UNUSED",COLOR_DEF_UNUSED);
    COLOR_DEF_EMPTY		= fsStorage->ReadInteger("COLOR_DEF_EMPTY",	COLOR_DEF_EMPTY	);
    COLOR_DEF_FULL		= fsStorage->ReadInteger("COLOR_DEF_FULL",	COLOR_DEF_FULL	);
    COLOR_DEF_100		= fsStorage->ReadInteger("COLOR_DEF_100",	COLOR_DEF_100	);
    COLOR_DEF_75		= fsStorage->ReadInteger("COLOR_DEF_75",	COLOR_DEF_75	);
    COLOR_DEF_50		= fsStorage->ReadInteger("COLOR_DEF_50",	COLOR_DEF_50	);
    COLOR_DEF_25		= fsStorage->ReadInteger("COLOR_DEF_25",	COLOR_DEF_25	);
    m_cell_size			= fsStorage->ReadInteger("cell_size",		m_cell_size		);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::fsStorageSavePlacement(TObject *Sender)
{
	m_Props->SaveParams			(fsStorage);
    m_CategoryProps->SaveParams	(fsStorage);
    fsStorage->WriteInteger("COLOR_DEF_UNUSED",	COLOR_DEF_UNUSED);
    fsStorage->WriteInteger("COLOR_DEF_EMPTY",	COLOR_DEF_EMPTY	);
    fsStorage->WriteInteger("COLOR_DEF_FULL",	COLOR_DEF_FULL	);
    fsStorage->WriteInteger("COLOR_DEF_100",	COLOR_DEF_100	);
    fsStorage->WriteInteger("COLOR_DEF_75",		COLOR_DEF_75	);
    fsStorage->WriteInteger("COLOR_DEF_50",		COLOR_DEF_50	);
    fsStorage->WriteInteger("COLOR_DEF_25",		COLOR_DEF_25	);
    fsStorage->WriteInteger("cell_size",		m_cell_size		);
}
//---------------------------------------------------------------------------

void __stdcall TfrmMain::OnCellSizeChanged	(PropValue* v)
{
    sbMem->Position	= 0;
    ResizeBuffer	();
    RedrawBuffer	();
}

void __stdcall TfrmMain::OnRefreshBuffer(PropValue* v)
{
    ResizeBuffer	();
    RedrawBuffer	();
}

void __stdcall TfrmMain::OnCategoryCommandClick(ButtonValue* V, bool& bModif, bool& bSafe)
{
    for (MemCatVecIt c_it=m_cat_data.begin(); c_it!=m_cat_data.end(); c_it++){
        switch(V->btn_num){
        case 0: (*c_it)->flags.set(SMemCat::flVisible,TRUE);	break;
        case 1: (*c_it)->flags.set(SMemCat::flVisible,FALSE);	break;
        case 2: (*c_it)->flags.invert(SMemCat::flVisible);		break;
        }
    }
    ResizeBuffer	();
    RedrawBuffer	();
    UpdateCatProperties(); 
	bModif			= false;
}

void __stdcall TfrmMain::OnCommonCommandClick(ButtonValue* V, bool& bModif, bool& bSafe)
{
    switch(V->btn_num){
    case 0: m_CategoryProps->ShowProperties();	break;
    case 1: m_CategoryProps->HideProperties();	break;
    }
	bModif			= false;
}

void TfrmMain::RealUpdateCatProperties()
{
    PropItemVec 	items;
    items.clear		();
    ButtonValue* B 	= PHelper().CreateButton	(items, "Category\\Command", "Show All, Hide All, Invert",0);
    B->OnBtnClickEvent.bind		(this,&TfrmMain::OnCategoryCommandClick);
    PHelper().CreateCaption		(items, "Category\\Count",	shared_str().sprintf("%d",	m_cat_data.size()));
    cgProgress->Show			();
    cgProgress->MaxValue 		= m_cat_data.size();
    for (MemCatVecIt c_it=m_cat_data.begin(); c_it!=m_cat_data.end(); c_it++){
        AnsiString pref			= PrepareKey("Category\\Items",(*c_it)->name.c_str()).c_str();
        PropValue* V			= PHelper().CreateFlag32(items, pref.c_str(), 			&(*c_it)->flags, SMemCat::flVisible);
        V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);
        PHelper().CreateCaption	(items, PrepareKey(pref.c_str(),"Count").c_str(),		shared_str().sprintf("%d",		(*c_it)->count));
        PHelper().CreateCaption	(items, PrepareKey(pref.c_str(),"Min/Max, b").c_str(),	shared_str().sprintf("%d/%d",	(*c_it)->min_sz,(*c_it)->max_sz));
        PHelper().CreateCaption	(items, PrepareKey(pref.c_str(),"Total, b").c_str(),	shared_str().sprintf("%d",		(*c_it)->real_sz));
/*
        U32It it						= std::lower_bound(m_items_cat.begin(),m_items_cat.end(),*c_it,find_cat_pred);
        if ((it==m_items_cat.end()) && (it!=m_items_cat.begin())) it--;
        while((it!=m_items_cat.end())&&(get_item(*it).cat==*c_it)){
            PHelper().CreateCaption	(items, PrepareKey(pref.c_str(),AnsiString().sprintf("0x%08X",get_item(*it).ptr).c_str()).c_str(),"");
            it++;
        }
*/        
        if ((c_it-m_cat_data.begin())%100 == 0) cgProgress->Progress = c_it-m_cat_data.begin();
    }
    cgProgress->Hide			();
    m_CategoryProps->AssignItems	(items);
	m_Flags.set		(flUpdateCatProps,FALSE);	
}

void TfrmMain::RealUpdateProperties()
{
    if (m_Props && stat_valid()){
        PropItemVec 	items;

        PropValue* V	= 0;
        ButtonValue* B	= 0;
        
        // Base info
        PHelper().CreateCaption	(items, "Common\\Begin ptr", 		shared_str().sprintf("0x%08X",m_begin_ptr));
        PHelper().CreateCaption	(items, "Common\\End ptr", 			shared_str().sprintf("0x%08X",m_end_ptr));
        PHelper().CreateCaption	(items, "Common\\Address used", 	shared_str().sprintf("%3.3f Kb",float(m_end_ptr-m_begin_ptr)/1024.f));
        PHelper().CreateCaption	(items, "Common\\Memory allocated",	shared_str().sprintf("%3.3f Kb",float(m_memory_allocated)/1024.f));
        PHelper().CreateCaption	(items, "Common\\Min block size", 	shared_str().sprintf("%d b",m_min_block));
        PHelper().CreateCaption	(items, "Common\\Max block size", 	shared_str().sprintf("%3.3f Kb",float(m_max_block)/1024.f));
        V=PHelper().CreateToken32(items, "Common\\Cell size",		&m_cell_size, cell_token);
        V->OnChangeEvent.bind	(this,&TfrmMain::OnCellSizeChanged);
        B = PHelper().CreateButton	(items, "Common\\Category", 	"Show, Hide",0);
        B->OnBtnClickEvent.bind	(this,&TfrmMain::OnCommonCommandClick);
        

        // Legend
        V=PHelper().CreateColor	(items, "Cell Legend\\Unused", 		&COLOR_DEF_UNUSED);	V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\Empty", 		&COLOR_DEF_EMPTY);  V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\Full", 		&COLOR_DEF_FULL);   V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\% (75-100]", 	&COLOR_DEF_100);    V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\% (50-75]", 	&COLOR_DEF_75);     V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\% (25-50]", 	&COLOR_DEF_50);     V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\% (0-25]", 	&COLOR_DEF_25);     V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);

        if (items_valid()){
            // UI                                                                               
            PHelper().CreateCaption	(items, "Current Cell\\ID",		 	shared_str().sprintf("%d",m_curr_cell));
            PHelper().CreateCaption	(items, "Current Cell\\Begin ptr", 	shared_str().sprintf("0x%08X",m_curr_cell*m_cell_size+m_begin_ptr));
            PHelper().CreateCaption	(items, "Current Cell\\End ptr", 	shared_str().sprintf("0x%08X",(m_curr_cell+1)*m_cell_size+m_begin_ptr-1));

            // Detailed info
            u32 r_begin_ptr	= m_curr_cell*m_cell_size+m_begin_ptr;
            u32 r_end_ptr	= (m_curr_cell+1)*m_cell_size+m_begin_ptr;
            U32It it		= std::lower_bound(m_items_ptr.begin(),m_items_ptr.end(),r_begin_ptr,find_ptr_pred);
            if ((it==m_items_ptr.end()) && (it!=m_items_ptr.begin())) it--;
            if ((get_item(*it).ptr>r_begin_ptr) && (it!=m_items_ptr.begin())) it--;
            while((it!=m_items_ptr.end())&&(get_item(*it).ptr<r_end_ptr)){
                if (get_item(*it).cat->flags.is(SMemCat::flVisible))
                    if (get_item(*it).ptr+get_item(*it).r_sz>r_begin_ptr)
                        PHelper().CreateCaption	(items, AnsiString().sprintf("Current Cell\\Data\\0x%08X",get_item(*it).ptr).c_str(), AnsiString().sprintf("%d b: %s",get_item(*it).r_sz,get_item(*it).cat->name.c_str()).c_str());
                it++;
            }
        }
        // 
        m_Props->AssignItems	(items);
    }
	m_Flags.set(flUpdateProps,FALSE);	
}
//---------------------------------------------------------------------------

void TfrmMain::ShowLog		(bool val)
{
	if (val)				TfrmLog::ShowLog();
    else					TfrmLog::HideLog();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::ShowLog1Click(TObject *Sender)
{
    ShowLog					(true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Bytype1Click(TObject *Sender)
{
	if (stat_valid()){
        TfrmStatistic::SHVec 	headers;
        headers.push_back		(TfrmStatistic::SStatHeader("Count",			TfrmStatistic::SStatHeader::sctInteger));
        headers.push_back		(TfrmStatistic::SStatHeader("Pure Size, b",		TfrmStatistic::SStatHeader::sctInteger));
        headers.push_back		(TfrmStatistic::SStatHeader("Allocated Size, b",TfrmStatistic::SStatHeader::sctInteger));
        headers.push_back		(TfrmStatistic::SStatHeader("Min Size, b",		TfrmStatistic::SStatHeader::sctInteger));
        headers.push_back		(TfrmStatistic::SStatHeader("Max Size, b",		TfrmStatistic::SStatHeader::sctInteger));
        headers.push_back		(TfrmStatistic::SStatHeader("Description",		TfrmStatistic::SStatHeader::sctText));
    	frmStatistic->Prepare	("Statistic By Type",headers);
        
        for (MemCatVecIt c_it=m_cat_data.begin(); c_it!=m_cat_data.end(); c_it++){
            AStringVec 			columns;
            columns.push_back   (AnsiString().sprintf("%d",(*c_it)->count));
            columns.push_back   (AnsiString().sprintf("%d",(*c_it)->pure_sz));
            columns.push_back   (AnsiString().sprintf("%d",(*c_it)->real_sz));
            columns.push_back   (AnsiString().sprintf("%d",(*c_it)->min_sz));
            columns.push_back   (AnsiString().sprintf("%d",(*c_it)->max_sz));
            columns.push_back   ((*c_it)->name.c_str());
            frmStatistic->AppendItem(columns);
        }
        frmStatistic->SortByColumn	(2,false);
    	frmStatistic->Show			();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Bypool1Click(TObject *Sender)
{
	if (pool_valid()){
        TfrmStatistic::SHVec 	headers;
        headers.push_back		(TfrmStatistic::SStatHeader("Index",			TfrmStatistic::SStatHeader::sctInteger));
        headers.push_back		(TfrmStatistic::SStatHeader("Base Size, b",		TfrmStatistic::SStatHeader::sctInteger));
        headers.push_back		(TfrmStatistic::SStatHeader("Block Count",		TfrmStatistic::SStatHeader::sctInteger));
        headers.push_back		(TfrmStatistic::SStatHeader("Pool Size, Kb",	TfrmStatistic::SStatHeader::sctFloat));
        headers.push_back		(TfrmStatistic::SStatHeader("Using Size, Kb",	TfrmStatistic::SStatHeader::sctFloat));
        headers.push_back		(TfrmStatistic::SStatHeader("Usage, %",			TfrmStatistic::SStatHeader::sctFloat));
    	frmStatistic->Prepare	("Statistic By Pool",headers);
        
        for (PoolItemVecIt it=m_pool_data.begin(); it!=m_pool_data.end(); it++){
            AStringVec 	columns;
            columns.push_back(AnsiString().sprintf("%d",it-m_pool_data.begin()));
            columns.push_back(AnsiString().sprintf("%d",it->base_size));
            columns.push_back(AnsiString().sprintf("%d",it->block_count));
            columns.push_back(AnsiString().sprintf("%3.2f",float(it->block_count*m_pools_ebase)));
            columns.push_back(AnsiString().sprintf("%3.2f",float(it->usage)/1024.f));
            columns.push_back(AnsiString().sprintf("%3.2f",100.f*(float(it->usage)/1024.f)/(it->block_count*m_pools_ebase)));
            frmStatistic->AppendItem(columns);
        }
        frmStatistic->SortByColumn	(1,true);
    	frmStatistic->Show			();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::AllItems1Click(TObject *Sender)
{
	if (items_valid()){
        TfrmStatistic::SHVec 	headers;
        headers.push_back		(TfrmStatistic::SStatHeader("Address",			TfrmStatistic::SStatHeader::sctInteger));
        headers.push_back		(TfrmStatistic::SStatHeader("Pool ID",			TfrmStatistic::SStatHeader::sctInteger));
        headers.push_back		(TfrmStatistic::SStatHeader("Pure Size, b",		TfrmStatistic::SStatHeader::sctInteger));
        headers.push_back		(TfrmStatistic::SStatHeader("Allocated Size, b",TfrmStatistic::SStatHeader::sctInteger));
        headers.push_back		(TfrmStatistic::SStatHeader("Description",		TfrmStatistic::SStatHeader::sctText));
    	frmStatistic->Prepare	("View All Items",headers);
        
        cgProgress->Show		();
		cgProgress->MaxValue	= m_items_data.size();
        AStringVec 				columns;
        frmStatistic->LockUpdating();
        for (MemItemVecIt it=m_items_data.begin(); it!=m_items_data.end(); it++){
            if ((it-m_items_data.begin())%1000 == 0) cgProgress->Progress = it-m_items_data.begin();
            columns.clear_not_free();
            columns.push_back	(AnsiString().sprintf("0x%08X",it->ptr));
            columns.push_back	(AnsiString().sprintf("%d",it->pool_id));
            columns.push_back	(AnsiString().sprintf("%d",it->sz));
            columns.push_back	(AnsiString().sprintf("%d",it->r_sz));
            columns.push_back	(AnsiString().sprintf("%s",it->cat->name.c_str()));
			frmStatistic->AppendItem(columns);
        }
        frmStatistic->SortByColumn	(1,true);
        frmStatistic->UnlockUpdating();
    	frmStatistic->Show			();
        cgProgress->Progress 	= 0;
        cgProgress->Hide		();
	}
}
//---------------------------------------------------------------------------

