//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "main.h"
#include "image.h"
#include "PropertiesList.h"               
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "ElFolderDlg"
#pragma link "MXCtrls"
#pragma link "multi_edit"
#pragma link "mxPlacemnt"
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
};
DEFINE_VECTOR(SMemItem,MemItemVec,MemItemVecIt);
MemItemVec 		m_items;
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
u32		        COLOR_DEF_UNUSED= 0x00D0D0D0;
u32		        COLOR_DEF_EMPTY = 0x00F0F0F0;
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

bool sort_ptr_pred(const SMemItem& a, const SMemItem& b) 	{return a.ptr<b.ptr;}
bool find_ptr_pred(const SMemItem& a, u32 val) 				{return a.ptr<val;}

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
	if (!m_items.empty()){
        m_memory.resize	(m_cell_size+(m_end_ptr-m_begin_ptr)/m_cell_size);
        std::fill		(m_memory.begin(),m_memory.end(),0x00000000);
        for (MemItemVecIt it=m_items.begin(); it!=m_items.end(); it++){
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
}              

void TfrmMain::RealRedrawBuffer()
{
    u32 d_w				= paMem->Width/m_cell_px;
    u32 d_h				= paMem->Height/m_cell_px;
    if (!m_items.empty()){
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
            sbMem->Max			= (m_memory.size()-d_w*d_h+2*d_w)/d_w;
            sbMem->LargeChange	= (d_w*d_h)/d_w;
            sbMem->SmallChange	= d_w/d_w;
        	sbMem->Enabled		= true;
        }else{
        	sbMem->Position		= 0;
        	sbMem->Enabled		= false;
        }

        RedrawDetailed			();
    }
    
    // grid
    paMem->Canvas->Pen->Color 	= 0x00808080;
    for (u32 k=0; k<paMem->Height/m_cell_px; k++){
        paMem->Canvas->MoveTo(0,k*m_cell_px);
        paMem->Canvas->LineTo(paMem->Width,k*m_cell_px);
    }

    // current cell
    if (!m_memory.empty() && ((m_curr_cell>=m_draw_offs)||(m_curr_cell<m_draw_offs+d_w*d_h))){
        paMem->Canvas->Pen->Color 	= 0x00000000;
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
}

void __fastcall TfrmMain::OpenClick(TObject *Sender)
{
//	if (od->Execute())
    {
    	LPCSTR fn 	= "x:\\$memory_dump$.txt";//od->FileName.c_str();
        IReader* F	= FS.r_open	(fn); VERIFY(F);                            
    	m_items.clear	();
        m_items.reserve	(250000);

		m_min_block		= 0xFFFFFFFF;
		m_max_block		= 0;
		m_begin_ptr		= 0xFFFFFFFF;
		m_end_ptr		= 0;
        m_curr_cell		= 0;

        LPCSTR src	= (LPCSTR)F->pointer();
        LPSTR data	= (LPSTR)F->pointer();
        string1024	name;
        do{
			LPSTR e		= data;
            for (;e[0]!='\n';e++){}
            e++;
            SMemItem	item;
			sscanf		(data,"0x%08X: %d %[^\n]",&item.ptr,&item.sz,name);
			if (item.sz > 16*65)	item.r_sz	= (((34 + item.sz) - 1)/16 + 1)*16;
		    else					item.r_sz	= (item.sz/16 + 1)*16;
            item.name	= _Trim(name);
            if ((item.ptr+item.r_sz)>m_end_ptr) m_end_ptr 	= item.ptr+item.r_sz;
            if (item.ptr<m_begin_ptr) 			m_begin_ptr = item.ptr;
            if (item.r_sz<m_min_block)			m_min_block	= item.r_sz;
            if (item.r_sz>m_max_block)			m_max_block	= item.r_sz;
			m_items.push_back(item);
            data		= e;
            if ((e-src)>=F->length()) break;
        }while(true);
        FS.r_close		(F);

		std::sort		(m_items.begin(),m_items.end(),sort_ptr_pred);

        ResizeBuffer	();
        RedrawBuffer	();
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
	Core._initialize("MemoryStat",0,true);
    m_Props 				= TProperties::CreateForm("",paInfo,alClient,0,0,0,TProperties::plFolderStore|TProperties::plFullExpand);
    Application->OnIdle 	= IdleHandler;
    m_Flags.zero();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
	TProperties::DestroyForm(m_Props);
	Core._destroy();
}
//---------------------------------------------------------------------------

void TfrmMain::OnFrame()
{
	if (m_Flags.is(flUpdateProps))		{ RealUpdateProperties(); 	m_Flags.set(flUpdateProps,FALSE);	}
	if (m_Flags.is(flResizeBuffer))		{ RealResizeBuffer(); 		m_Flags.set(flResizeBuffer,FALSE);	}
	if (m_Flags.is(flRedrawBuffer))		{ RealRedrawBuffer(); 		m_Flags.set(flRedrawBuffer,FALSE);	}
	if (m_Flags.is(flRedrawDetailed))	{ RealRedrawDetailed(); 	m_Flags.set(flRedrawDetailed,FALSE);}
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::paMemPaint(TObject *Sender)
{
	RedrawBuffer();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::paMemMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
	if (!m_items.empty()){
        u32 local_pos						= m_draw_offs+(Y/m_cell_px)*(paMem->Width/m_cell_px)+(X/m_cell_px);
        u32 real_pos						= m_begin_ptr+iFloor(float(local_pos)*m_cell_size+0.5f);
        sbStatus->Panels->Items[0]->Text 	= AnsiString().sprintf("Address: 0x%08X",real_pos);
        MemItemVecIt it						= std::lower_bound(m_items.begin(),m_items.end(),real_pos,find_ptr_pred);
        SStringVec names;
        if (it!=m_items.end()){
            if ((it->ptr>real_pos) && (it!=m_items.begin())) it--;
            while((it!=m_items.end())&&(it->ptr<(real_pos+m_cell_size))){
            	if (it->ptr+it->r_sz>real_pos)
                	names.push_back			(it->name.c_str());
                it++;
            }
        }
        sbStatus->Panels->Items[1]->Text 	= AnsiString().sprintf("Content: %s",_ListToSequence(names).c_str());
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::sbMemChange(TObject *Sender)
{
    u32 d_w			= paMem->Width/m_cell_px;
    m_draw_offs		= d_w*(d_w*sbMem->Position/d_w); // align line
	sbMem->Position	= m_draw_offs/d_w;
    RedrawBuffer	();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::paMemBaseResize(TObject *Sender)
{
	paMem->Left 	= 0;	
    paMem->Top 		= 0;	
	paMem->Width 	= iFloor(float(paMemBase->Width)/m_cell_px+1.f)*m_cell_px;	
    paMem->Height	= iFloor(float(paMemBase->Height)/m_cell_px+1.f)*m_cell_px;
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
	if (!m_items.empty()){
    	if (m_cell_frame.size()!=(d_w*d_h)) m_cell_frame.resize(d_w*d_h);

    	u32 r_begin_ptr	= m_curr_cell*m_cell_size+m_begin_ptr;
    	u32 r_end_ptr	= (m_curr_cell+1)*m_cell_size+m_begin_ptr;
    	u32 r_size		= r_end_ptr-r_begin_ptr;
    	u32 d_size		= r_size/4;

        std::fill		(m_cell_frame.begin(),m_cell_frame.begin()+d_size,COLOR_DEF_EMPTY);
        std::fill		(m_cell_frame.begin()+d_size,m_cell_frame.begin()+d_w*d_h,COLOR_DEF_UNUSED);
        MemItemVecIt it	= std::lower_bound(m_items.begin(),m_items.end(),r_begin_ptr,find_ptr_pred);
        if (it!=m_items.end()){
            if ((it->ptr>r_begin_ptr) && (it!=m_items.begin())) it--;
            while((it!=m_items.end())&&(it->ptr<r_end_ptr)){
            	if (it->ptr+it->r_sz>r_begin_ptr){
                	int	i_begin	= (_max(r_begin_ptr,it->ptr)-r_begin_ptr);
                	int	i_end	= (_min(r_end_ptr,it->ptr+it->r_sz)-r_begin_ptr);
					std::fill	(m_cell_frame.begin()+i_begin/4,m_cell_frame.begin()+i_end/4,COLOR_DEF_FULL);
                }
                it++;
            }
        }
    
        DrawBitmap			(paDetMem,Irect().set(0,0,paDetMem->Width,paDetMem->Height),
                            m_cell_frame.begin(), d_w,d_h);
    }
    
    // grid
    paDetMem->Canvas->Pen->Color 	= 0x00808080;
    paDetMem->Canvas->Brush->Style = bsSolid;
    for (u32 k=0; k<paDetMem->Height/m_cell_px; k++){
        paDetMem->Canvas->MoveTo(0,k*m_cell_px);
        paDetMem->Canvas->LineTo(paDetMem->Width,k*m_cell_px);
    }
}

void __fastcall TfrmMain::paDetMemPaint(TObject *Sender)
{
	RedrawDetailed();
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
/*	
    u32 d_w			= paDetMem->Width/m_cell_px;
	switch (Key){
    case VK_LEFT: 	m_curr_cell++; 		Key=0; break;
    case VK_RIGHT: 	m_curr_cell--; 		Key=0; break;
    case VK_DOWN: 	m_curr_cell+=d_w;	Key=0; break;
    case VK_UP: 	m_curr_cell+=d_w;	Key=0; break;
    }
    clamp			(m_curr_cell,u32(0),m_memory.size());
    DrawImage		();
*/
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
}
//---------------------------------------------------------------------------

void __stdcall TfrmMain::OnCellChanged	(PropValue* v)
{
    sbMem->Position	= 0;
	OnResizeBuffer	(v);
	OnRedrawBuffer	(v);
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
    if (m_Props && !m_items.empty()){
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
        V=PHelper().CreateColor	(items, "Cell Legend\\Unused", 		&COLOR_DEF_UNUSED);	V->OnChangeEvent.bind	(this,&TfrmMain::OnResizeBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\Empty", 		&COLOR_DEF_EMPTY);  V->OnChangeEvent.bind	(this,&TfrmMain::OnResizeBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\Full", 		&COLOR_DEF_FULL);   V->OnChangeEvent.bind	(this,&TfrmMain::OnResizeBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\(75-100]%", 	&COLOR_DEF_100);    V->OnChangeEvent.bind	(this,&TfrmMain::OnResizeBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\(50-75]%", 	&COLOR_DEF_75);     V->OnChangeEvent.bind	(this,&TfrmMain::OnResizeBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\(25-50]%", 	&COLOR_DEF_50);     V->OnChangeEvent.bind	(this,&TfrmMain::OnResizeBuffer);
        V=PHelper().CreateColor	(items, "Cell Legend\\(0-25]%", 	&COLOR_DEF_25);     V->OnChangeEvent.bind	(this,&TfrmMain::OnResizeBuffer);

        // UI
        PHelper().CreateCaption	(items, "Current Cell\\ID",		 	shared_str().sprintf("%d",m_curr_cell));
        PHelper().CreateCaption	(items, "Current Cell\\Address", 	shared_str().sprintf("[0x%08X - 0x%08X)",m_curr_cell*m_cell_size+m_begin_ptr,(m_curr_cell+1)*m_cell_size+m_begin_ptr));

        // Detailed info
        u32 r_begin_ptr	= m_curr_cell*m_cell_size+m_begin_ptr;
        u32 r_end_ptr	= (m_curr_cell+1)*m_cell_size+m_begin_ptr;
        MemItemVecIt it	= std::lower_bound(m_items.begin(),m_items.end(),r_begin_ptr,find_ptr_pred);
        if (it!=m_items.end()){
            if ((it->ptr>r_begin_ptr) && (it!=m_items.begin())) it--;
            while((it!=m_items.end())&&(it->ptr<r_end_ptr)){
                if (it->ptr+it->r_sz>r_begin_ptr)
                    PHelper().CreateCaption	(items, AnsiString().sprintf("Current Cell\\Data\\0x%08X",it->ptr).c_str(), AnsiString().sprintf("%d b: %s",it->r_sz,it->name.c_str()).c_str());
                it++;
            }
        }
        // 
        m_Props->AssignItems	(items);
    }
}
//---------------------------------------------------------------------------

