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
#pragma resource "*.dfm"
TfrmMain *frmMain;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
struct SMemItem{
	shared_str	name;
    u32 		ptr;
    u32 		sz;
    u32 		r_sz;
    s8 			pool_id;
};
DEFINE_VECTOR(SMemItem,MemItemVec,MemItemVecIt);
MemItemVec 		m_items_data;
U32Vec			m_items_ptr;
U32Vec			m_items_name;
//bool sort_ptr_pred(const SMemItem& a, const SMemItem& b) 	{return a.ptr<b.ptr;}
//bool find_ptr_pred(const SMemItem& a, u32 val) 		   	{return a.ptr<val;}
bool sort_ptr_pred	(const u32& a, const u32& b) 			{return m_items_data[a].ptr<m_items_data[b].ptr;}
bool find_ptr_pred	(const u32& a, u32 val) 				{return m_items_data[a].ptr<val;}
bool sort_name_pred	(const u32& a, const u32& b) 			{return strcmp(m_items_data[a].name.c_str(),m_items_data[b].name.c_str())<0;}
bool find_name_pred	(const u32& a, const char* val)			{return strcmp(m_items_data[a].name.c_str(),val)<0;}

__inline const SMemItem& 	get_item(u32 idx)	{return m_items_data[idx];}
__inline bool 				items_valid()		{return !m_items_data.empty();}


U32Vec			m_memory;
U32Vec 			m_cell_frame;
U32Vec 			m_full_frame;
u32 			m_begin_ptr		= (u32)-1;
u32 			m_end_ptr		= 0;
u32 			m_cell_size		= 1024;
const u32		m_cell_px		= 4;
u32				m_draw_offs		= 0;
u32				m_min_block		= 0xFFFFFFFF;
u32				m_max_block		= 0;
u32				m_curr_cell		= 0;
TProperties*	m_Props			= 0;


// color defs
u32		        COLOR_DEF_UNUSED= 0x00D1D0D0;
u32		        COLOR_DEF_EMPTY = 0x00F1F0F0;
u32		        COLOR_DEF_FULL 	= 0x00008000; 	// 100%
u32		        COLOR_DEF_100 	= 0x0080C000; 	// (75%-100%)
u32		        COLOR_DEF_75 	= 0x00D08000;	// (50%-75%]
u32		        COLOR_DEF_50 	= 0x000080D0;	// (25%-50%]
u32		        COLOR_DEF_25 	= 0x00FF0000;	// (0%-25%]

xr_token						cell_token	[ ]={
	{ "8 bytes",		  		8		  	},
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
        	u32 i_begin_ptr	= it->ptr-m_begin_ptr;
        	u32 i_end_ptr	= i_begin_ptr+it->r_sz;
            u32 c_begin		= i_begin_ptr/m_cell_size;
            u32 c_end		= i_end_ptr/m_cell_size;
            for (u32 c=c_begin; c<=c_end; c++)
	            m_memory[c]	+= cell_intersect(c*m_cell_size,(c+1)*m_cell_size,i_begin_ptr,i_end_ptr);
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
        IReader* F		= FS.r_open	(fn); VERIFY(F);                            
    	m_items_data.clear	();
        m_items_data.reserve(250000);

		m_min_block		= 0xFFFFFFFF;
		m_max_block		= 0;
		m_begin_ptr		= 0xFFFFFFFF;
		m_end_ptr		= 0;
        m_curr_cell		= 0;

        LPCSTR src	    = (LPCSTR)F->pointer();
        LPSTR data	    = (LPSTR)F->pointer();
        string1024	    name;
        int 			chunk=-1;
        do{
            LPSTR e		= data;
            for (;e[0]!='\n';e++){}
            e++;
            if (is_chunk(data,chunk)){
            }else if (chunk==0){
            }else if (chunk==1){
                SMemItem	item;
                sscanf		(data,"0x%08X[%2d]: %d %[^\n]",&item.ptr,&item.pool_id,&item.sz,name);
                if (item.sz > 16*65)	item.r_sz	= (((34 + item.sz) - 1)/16 + 1)*16;
                else					item.r_sz	= (item.sz/16 + 1)*16;
                item.name	= _Trim(name);
                if ((item.ptr+item.r_sz)>m_end_ptr) m_end_ptr 	= item.ptr+item.r_sz;
                if (item.ptr<m_begin_ptr) 			m_begin_ptr = item.ptr;
                if (item.r_sz<m_min_block)			m_min_block	= item.r_sz;
                if (item.r_sz>m_max_block)			m_max_block	= item.r_sz;
                m_items_data.push_back(item);
            }
            data		= e;
            if ((e-src)>=F->length()) break;
        }while(true);
        FS.r_close		(F);

        if (m_items_data.size()){
            m_items_ptr.resize	(m_items_data.size());	
            m_items_name.resize	(m_items_data.size());
            for (u32 k=0; k<m_items_data.size(); ++k){m_items_ptr[k]=k;m_items_name[k]=k;}
        	
            std::sort		(m_items_ptr.begin(),m_items_ptr.end(),sort_ptr_pred);
            std::sort		(m_items_name.begin(),m_items_name.end(),sort_name_pred);

            ResizeBuffer	();
            RedrawBuffer	();
        }
    }
}
//---------------------------------------------------------------------------

IC u32 GetPowerOf2Plus1	(u32 v)
{
    u32 cnt=0;
    while (v) {v>>=1; cnt++; };
    return cnt;
}

void __fastcall TfrmMain::IdleHandler(TObject *Sender, bool &Done)
{
    Done = false;
    OnFrame();
}

void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
    m_Props 				= TProperties::CreateForm("",paInfo,alClient,0,0,0,TProperties::plFolderStore|TProperties::plFullExpand);
    Application->OnIdle 	= IdleHandler;
    m_Flags.zero			();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
	TProperties::DestroyForm(m_Props);
}
//---------------------------------------------------------------------------

void TfrmMain::OnFrame()
{
	if (m_Flags.is(flUpdateProps))		{ RealUpdateProperties(); 	}
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
        if (it!=m_items_ptr.end()){
            if ((get_item(*it).ptr>real_pos) && (it!=m_items_ptr.begin())) it--;
            while((it!=m_items_ptr.end())&&(get_item(*it).ptr<(real_pos+m_cell_size))){
            	if (get_item(*it).ptr+get_item(*it).r_sz>real_pos)
                	names.push_back			(get_item(*it).name.c_str());
                it++;
            }
        }
        address.sprintf("Address: 0x%08X",real_pos); 
        content.sprintf("Content: %s",_ListToSequence(names).c_str());
    }
    sbStatus->Panels->Items[0]->Text 		= address;
    sbStatus->Panels->Items[1]->Text 		= content;
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
            if (it!=m_items_ptr.end()){
                if ((get_item(*it).ptr>real_pos) && (it!=m_items_ptr.begin())) it--;
                while((it!=m_items_ptr.end())&&(get_item(*it).ptr<(real_pos+4))){
                    if (get_item(*it).ptr+get_item(*it).r_sz>real_pos)
                        names.push_back	  	(get_item(*it).name.c_str());
                    it++;
                }
            }
			address.sprintf("Address: 0x%08X",real_pos); 
			content.sprintf("Content: %s",_ListToSequence(names).c_str());
        }
    }
    sbStatus->Panels->Items[0]->Text 	= address;
    sbStatus->Panels->Items[1]->Text 	= content;
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
	paMem->Left 	= 0;	
    paMem->Top 		= 0;	
	paMem->Width 	= paMemBase->Width+(m_cell_px-paMemBase->Width%m_cell_px);
    paMem->Height	= paMemBase->Height+(m_cell_px-paMemBase->Height%m_cell_px);
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
        if (it!=m_items_ptr.end()){
            if ((get_item(*it).ptr>r_begin_ptr) && (it!=m_items_ptr.begin())) it--;
            while((it!=m_items_ptr.end())&&(get_item(*it).ptr<r_end_ptr)){
            	if (get_item(*it).ptr+get_item(*it).r_sz>r_begin_ptr){
                	int	i_begin	= (_max(r_begin_ptr,get_item(*it).ptr)-r_begin_ptr);
                	int	i_end	= (_min(r_end_ptr,get_item(*it).ptr+get_item(*it).r_sz)-r_begin_ptr);
					std::fill	(m_cell_frame.begin()+i_begin/4,m_cell_frame.begin()+i_end/4,COLOR_DEF_FULL);
                }
                it++;
            }
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
	m_Props->RestoreParams(fsStorage);
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
	m_Props->SaveParams(fsStorage);
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

void __stdcall TfrmMain::OnCellChanged	(PropValue* v)
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

void __stdcall TfrmMain::OnRedrawBuffer	(PropValue* v)
{
    RedrawBuffer	();
}

void __stdcall TfrmMain::OnResizeBuffer	(PropValue* v)
{
    ResizeBuffer	();
}

void TfrmMain::RealUpdateProperties()
{
    if (m_Props && items_valid()){
        PropItemVec 	items;

        PropValue* V	= 0;
        
        // Base info
        PHelper().CreateCaption	(items, "Common\\Begin ptr", 		shared_str().sprintf("0x%08X",m_begin_ptr));
        PHelper().CreateCaption	(items, "Common\\End ptr", 			shared_str().sprintf("0x%08X",m_end_ptr));
        PHelper().CreateCaption	(items, "Common\\Memory used", 		shared_str().sprintf("%3.3f Kb",float(m_end_ptr-m_begin_ptr)/1024.f));
        PHelper().CreateCaption	(items, "Common\\Min block size", 	shared_str().sprintf("%d b",m_min_block));
        PHelper().CreateCaption	(items, "Common\\Max block size", 	shared_str().sprintf("%3.3f Kb",float(m_max_block)/1024.f));
        V=PHelper().CreateToken32(items, "Common\\Cell size",		&m_cell_size, cell_token);
        V->OnChangeEvent.bind	(this,&TfrmMain::OnCellChanged);

        // Legend
        V=PHelper().CreateColor	(items, "Cell Legend\\Unused", 		&COLOR_DEF_UNUSED);	V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\Empty", 		&COLOR_DEF_EMPTY);  V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\Full", 		&COLOR_DEF_FULL);   V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\% (75-100]", 	&COLOR_DEF_100);    V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\% (50-75]", 	&COLOR_DEF_75);     V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\% (25-50]", 	&COLOR_DEF_50);     V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\% (0-25]", 	&COLOR_DEF_25);     V->OnChangeEvent.bind	(this,&TfrmMain::OnRefreshBuffer);

        // UI                                                                               
        PHelper().CreateCaption	(items, "Current Cell\\ID",		 	shared_str().sprintf("%d",m_curr_cell));
        PHelper().CreateCaption	(items, "Current Cell\\Begin ptr", 	shared_str().sprintf("0x%08X",m_curr_cell*m_cell_size+m_begin_ptr));
        PHelper().CreateCaption	(items, "Current Cell\\End ptr", 	shared_str().sprintf("0x%08X",(m_curr_cell+1)*m_cell_size+m_begin_ptr-1));

        // Detailed info
        u32 r_begin_ptr	= m_curr_cell*m_cell_size+m_begin_ptr;
        u32 r_end_ptr	= (m_curr_cell+1)*m_cell_size+m_begin_ptr;
        U32It it		= std::lower_bound(m_items_ptr.begin(),m_items_ptr.end(),r_begin_ptr,find_ptr_pred);
        if (it!=m_items_ptr.end()){
            if ((get_item(*it).ptr>r_begin_ptr) && (it!=m_items_ptr.begin())) it--;
            while((it!=m_items_ptr.end())&&(get_item(*it).ptr<r_end_ptr)){
                if (get_item(*it).ptr+get_item(*it).r_sz>r_begin_ptr)
                    PHelper().CreateCaption	(items, AnsiString().sprintf("Current Cell\\Data\\0x%08X",get_item(*it).ptr).c_str(), AnsiString().sprintf("%d b: %s",get_item(*it).r_sz,get_item(*it).name.c_str()).c_str());
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
	if (items_valid()){
        TfrmStatistic::SHVec 	headers;
        headers.push_back		(TfrmStatistic::SStatHeader("Count",			false));
        headers.push_back		(TfrmStatistic::SStatHeader("Pure Size",		false));
        headers.push_back		(TfrmStatistic::SStatHeader("Allocated Size",	false));
        headers.push_back		(TfrmStatistic::SStatHeader("Description",		false));
    	frmStatistic->Prepare	("Statistic By Type",headers);
        
        u32 	diff_cnt= 1;
        LPCSTR 	nm	= get_item(m_items_name[0]).name.c_str();
        for (U32It itt=m_items_name.begin(); itt!=m_items_name.end(); itt++){
            if (0!=strcmp(nm,get_item(*itt).name.c_str())){ 
                diff_cnt++; nm=get_item(*itt).name.c_str(); 
            }
        }
        nm				= get_item(m_items_name[0]).name.c_str();
        u32	cnt			= 0;
        u32	mem_pure	= 0;
        u32	mem_alloc	= 0;
        for (itt=m_items_name.begin(); itt!=m_items_name.end(); itt++){
            const SMemItem& inf	= get_item(*itt);
            if (0!=strcmp(nm,inf.name.c_str())){ 
            	AStringVec 	columns;
                columns.push_back(AnsiString().sprintf("%d",cnt));
                columns.push_back(AnsiString().sprintf("%d",mem_pure));
                columns.push_back(AnsiString().sprintf("%d",mem_alloc));
                columns.push_back(nm);
            	frmStatistic->AppendItem(columns);
                cnt			= 0;
        		mem_pure	= 0;
        		mem_alloc	= 0;
                nm			= inf.name.c_str(); 
            }
        	cnt			++;
            mem_pure	+= inf.sz;
            mem_alloc	+= inf.r_sz;
        }
        AStringVec 	columns;
        columns.push_back(AnsiString().sprintf("%d",cnt));
        columns.push_back(AnsiString().sprintf("%d",mem_pure));
        columns.push_back(AnsiString().sprintf("%d",mem_alloc));
        columns.push_back(nm);
        frmStatistic->AppendItem	(columns);
        frmStatistic->SortByColumn	(2,false);
    	frmStatistic->Show			();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Bypool1Click(TObject *Sender)
{
	if (items_valid()){
        TfrmStatistic::SHVec 	headers;
        headers.push_back		(TfrmStatistic::SStatHeader("Index",			false));
        headers.push_back		(TfrmStatistic::SStatHeader("Base Size",		false));
        headers.push_back		(TfrmStatistic::SStatHeader("Block Count",		false));
        headers.push_back		(TfrmStatistic::SStatHeader("Description",		false));
    	frmStatistic->Prepare	("Statistic By Pool",headers);
        
        u32 	diff_cnt= 1;
        LPCSTR 	nm	= get_item(m_items_name[0]).name.c_str();
        for (U32It itt=m_items_name.begin(); itt!=m_items_name.end(); itt++){
            if (0!=strcmp(nm,get_item(*itt).name.c_str())){ 
                diff_cnt++; nm=get_item(*itt).name.c_str(); 
            }
        }
        nm				= get_item(m_items_name[0]).name.c_str();
        u32	cnt			= 0;
        u32	mem_pure	= 0;
        u32	mem_alloc	= 0;
        for (itt=m_items_name.begin(); itt!=m_items_name.end(); itt++){
            const SMemItem& inf	= get_item(*itt);
            if (0!=strcmp(nm,inf.name.c_str())){ 
            	AStringVec 	columns;
                columns.push_back(AnsiString().sprintf("%d",cnt));
                columns.push_back(AnsiString().sprintf("%d",mem_pure));
                columns.push_back(AnsiString().sprintf("%d",mem_alloc));
                columns.push_back(nm);
            	frmStatistic->AppendItem(columns);
                cnt			= 0;
        		mem_pure	= 0;
        		mem_alloc	= 0;
                nm			= inf.name.c_str(); 
            }
        	cnt			++;
            mem_pure	+= inf.sz;
            mem_alloc	+= inf.r_sz;
        }
        AStringVec 	columns;
        columns.push_back(AnsiString().sprintf("%d",cnt));
        columns.push_back(AnsiString().sprintf("%d",mem_pure));
        columns.push_back(AnsiString().sprintf("%d",mem_alloc));
        columns.push_back(nm);
        frmStatistic->AppendItem	(columns);
        frmStatistic->SortByColumn	(1,true);
    	frmStatistic->Show			();
	}
}
//---------------------------------------------------------------------------

