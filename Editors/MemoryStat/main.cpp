//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "main.h"
#include "image.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "ElFolderDlg"
#pragma link "MXCtrls"
#pragma link "multi_edit"
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
boolVec			m_full_memory;
U32Vec 			m_cell_memory;
u32 			m_begin_ptr		= (u32)-1;
u32 			m_end_ptr		= 0;
u32 			m_cell_size		= 1024;
const u32		m_cell_px		= 4;
u32				m_draw_offs		= 0;
u32				m_min_block		= 0xFFFFFFFF;
u32				m_max_block		= 0;
u32				m_curr_cell		= 0;

// color defs
static u32		COLOR_DEF_UNUSED= 0x00D0D0D0;
static u32		COLOR_DEF_EMPTY = 0x00F0F0F0;
static u32		COLOR_DEF_FULL 	= 0x00007F00;

bool sort_ptr_pred(const SMemItem& a, const SMemItem& b) 	{return a.ptr<b.ptr;}
bool find_ptr_pred(const SMemItem& a, u32 val) 				{return a.ptr<val;}

void DrawBitmap(TMxPanel* panel, const Irect& r, u32* data, u32 w, u32 h)
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
/* 
    frmMain->paMem->Canvas->Pen->Color 	= 0x00808080;
    frmMain->paMem->Canvas->Brush->Style = bsSolid;
    for (u32 k=0; k<frmMain->paMem->Height/m_cell_px; k++){
    	MoveToEx(hdc,0,k*m_cell_px,0);
    	LineTo(hdc,frmMain->paMem->Width,k*m_cell_px);
    }
*/
}

void TfrmMain::ResizeImage()
{
/*
    m_memory.resize	(m_cell_size+(m_end_ptr-m_begin_ptr)/m_cell_size);
    std::fill		(m_memory.begin(),m_memory.end(),0x00000000);
    for (MemItemVecIt it=m_items.begin(); it!=m_items.end(); it++){
        u32 begin	= iFloor((it->ptr-m_begin_ptr)/m_cell_size);
        int sz		= iFloor(float(it->r_sz)/float(m_cell_size)+1.f);
        if (sz==1){
            m_memory[begin]		+=it->r_sz;
        }else{
            u32 cell_begin		= begin*m_cell_size;
            m_memory[begin]		+=m_cell_size-(it->ptr-m_begin_ptr-cell_begin);

            u32 cell_last		= iFloor((begin+sz)*m_cell_size);
            u32 local_end		= (it->ptr-m_begin_ptr+it->r_sz);
            m_memory[begin+sz-1]+= m_cell_size-(cell_last-local_end);

            begin				++; 
            sz					-=2;
            if (sz>0) std::fill	(m_memory.begin()+begin,m_memory.begin()+begin+sz,m_cell_size);
        }
    }     
    u32 m_sz 					= m_memory.size();
    for (u32 k=0;k<m_sz;k++){ 
        u32 color				= COLOR_DEF_EMPTY;
        if (m_memory[k]!=0){
            if (m_memory[k]==m_cell_size)			color = COLOR_DEF_FULL;
            else if (m_memory[k]>(m_cell_size/2))	color = 0x00C0C000;
            else 			 						color = 0x00FF0000;
        }
    	m_memory[k]		 		= color;
	}
*/    
}              

void TfrmMain::DrawImage()
{
    u32 d_w				= paMem->Width/m_cell_px;
    u32 d_h				= paMem->Height/m_cell_px;
//	if (!m_memory.empty())
    {
        if (m_full_memory.size()!=(d_w*d_h)) m_full_memory.resize(d_w*d_h);
            u32 r_begin_ptr	= m_draw_offs+m_begin_ptr;
            u32 r_end_ptr	= m_draw_offs+m_begin_ptr+d_w*d_h;
            u32 r_size		= r_end_ptr-r_begin_ptr;
	            
            std::fill		(m_full_memory.begin(),m_full_memory.begin()+d_size,COLOR_DEF_EMPTY);
            std::fill		(m_full_memory.begin()+d_size,m_full_memory.begin()+d_w*d_h,COLOR_DEF_UNUSED);
            MemItemVecIt it	= std::lower_bound(m_items.begin(),m_items.end(),r_begin_ptr,find_ptr_pred);
            if (it!=m_items.end()){
                if ((it->ptr>r_begin_ptr) && (it!=m_items.begin())) it--;
                while((it!=m_items.end())&&(it->ptr<(r_begin_ptr+m_cell_size))){
                    if (it->ptr+it->r_sz>r_begin_ptr){
                        int	i_begin	= (_max(r_begin_ptr,it->ptr)-r_begin_ptr);
                        int	i_end	= (_min(r_end_ptr,it->ptr+it->r_sz)-r_begin_ptr);
                        std::fill	(m_cell_memory.begin()+i_begin/4,m_cell_memory.begin()+i_end/4,COLOR_DEF_FULL);
                    }
                    it++;
                }
            }
    
            DrawBitmap			(paDetMem,Irect().set(0,0,paDetMem->Width,paDetMem->Height),
                                m_cell_memory.begin(), d_w,d_h);
        }
/*
        if (m_frame.dwWidth!=d_w || m_frame.dwHeight!=d_h )
        	m_frame.Create	(d_w,d_h);
        u32 req_frame_sz	= d_w*d_h;
        u32 real_frame_sz	= _min(req_frame_sz,m_memory.size()-m_draw_offs);
		CopyMemory			(m_frame.pData,m_memory.begin()+m_draw_offs,real_frame_sz*4);
        if (req_frame_sz!=real_frame_sz)
		    std::fill		(m_frame.pData+real_frame_sz,m_frame.pData+d_w*d_h,COLOR_DEF_EMPTY);
    
        DrawBitmap			(paMem,Irect().set(0,0,paMem->Width,paMem->Height),
                            m_frame.pData, d_w,d_h);
*/
        
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

        lbBeginPtr->Caption	    = AnsiString().sprintf("0x%08X",m_begin_ptr);
        lbEndPtr->Caption	    = AnsiString().sprintf("0x%08X",m_end_ptr);
        lbMinBlock->Caption	    = AnsiString().sprintf("%d b",m_min_block);
        lbMaxBlock->Caption	    = AnsiString().sprintf("%3.3f Kb",float(m_max_block)/1024.f);
        lbMemSize->Caption	    = AnsiString().sprintf("%3.3f Kb",float(m_end_ptr-m_begin_ptr)/1024.f);
        lbCurrentCell->Caption	= AnsiString().sprintf("[0x%08X - 0x%08X]",m_curr_cell*m_cell_size+m_begin_ptr,(m_curr_cell+1)*m_cell_size+m_begin_ptr-1);
        //
        lbDetDesc->Clear		();
        u32 real_pos			= m_curr_cell*m_cell_size+m_begin_ptr;
        MemItemVecIt it			= std::lower_bound(m_items.begin(),m_items.end(),real_pos,find_ptr_pred);
        if (it!=m_items.end()){
            if ((it->ptr>real_pos) && (it!=m_items.begin())) it--;
            while((it!=m_items.end())&&(it->ptr<(real_pos+m_cell_size))){
            	if (it->ptr+it->r_sz>real_pos){
	                lbDetDesc->Items->Add	(it->name.c_str());
                }
                it++;
            }
        }
        DrawDetImage			();
    }
    
    // grid
    paMem->Canvas->Pen->Color 	= 0x00808080;
    paMem->Canvas->Brush->Style = bsSolid;
    for (u32 k=0; k<paMem->Height/m_cell_px; k++){
        paMem->Canvas->MoveTo(0,k*m_cell_px);
        paMem->Canvas->LineTo(paMem->Width,k*m_cell_px);
    }

    // current cell
    if (!m_memory.empty() && ((m_curr_cell>=m_draw_offs)||(m_curr_cell<m_draw_offs+d_w*d_h))){
        paMem->Canvas->Pen->Color 	= 0x00000000;
        paMem->Canvas->Brush->Style = bsSolid;
        u32 c			= m_curr_cell-m_draw_offs;
        u32 x			= (c%d_w)*m_cell_px;
        u32 y			= (c/d_w)*m_cell_px;
        paMem->Canvas->MoveTo(x,y);
        paMem->Canvas->LineTo(x+m_cell_px,y);
        paMem->Canvas->LineTo(x+m_cell_px,y+m_cell_px);
        paMem->Canvas->LineTo(x,y+m_cell_px);
        paMem->Canvas->LineTo(x,y);
    }
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

        m_full_memory.resize		(m_end_ptr-m_begin_ptr,false);
        
        for (MemItemVecIt it=m_items.begin(); it!=m_items.end(); it++){
            u32 begin	= (it->ptr-m_begin_ptr)/4;
            int sz		= it->r_sz/4;
            std::fill	(m_full_memory.begin()+begin,m_full_memory.begin()+begin+sz,true);
        }
        
        DrawImage		();
    }
}
//---------------------------------------------------------------------------

IC u32 GetPowerOf2Plus1	(u32 v)
{
    u32 cnt=0;
    while (v) {v>>=1; cnt++; };
    return cnt;
}

void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
	Core._initialize("MemoryStat",0,true);
    rgCellSize->ItemIndex	= GetPowerOf2Plus1(m_cell_size)-4;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::FormDestroy(TObject *Sender)
{
	Core._destroy();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::paMemPaint(TObject *Sender)
{
	DrawImage();
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
    DrawImage		();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::rgCellSizeClick(TObject *Sender)
{
    u32 n_cell_size	= pow(2,rgCellSize->ItemIndex+3);
    sbMem->Position	= iFloor(float(sbMem->Position)/(float(n_cell_size)/float(m_cell_size)));
	m_cell_size		= n_cell_size;
    
    ResizeImage		();
    DrawImage		();
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
    DrawImage		();
}
//---------------------------------------------------------------------------


void TfrmMain::DrawDetImage()
{
    u32 d_w				= paDetMem->Width/m_cell_px;
    u32 d_h				= paDetMem->Height/m_cell_px;
	if (!m_memory.empty()){
    	if (m_cell_memory.size()!=(d_w*d_h)) m_cell_memory.resize(d_w*d_h);

    	u32 r_begin_ptr	= m_curr_cell*m_cell_size+m_begin_ptr;
    	u32 r_end_ptr	= (m_curr_cell+1)*m_cell_size+m_begin_ptr;
    	u32 r_size		= r_end_ptr-r_begin_ptr;
    	u32 d_size		= r_size/4;
	            
        std::fill		(m_cell_memory.begin(),m_cell_memory.begin()+d_size,COLOR_DEF_EMPTY);
        std::fill		(m_cell_memory.begin()+d_size,m_cell_memory.begin()+d_w*d_h,COLOR_DEF_UNUSED);
        MemItemVecIt it	= std::lower_bound(m_items.begin(),m_items.end(),r_begin_ptr,find_ptr_pred);
        if (it!=m_items.end()){
            if ((it->ptr>r_begin_ptr) && (it!=m_items.begin())) it--;
            while((it!=m_items.end())&&(it->ptr<(r_begin_ptr+m_cell_size))){
            	if (it->ptr+it->r_sz>r_begin_ptr){
                	int	i_begin	= (_max(r_begin_ptr,it->ptr)-r_begin_ptr);
                	int	i_end	= (_min(r_end_ptr,it->ptr+it->r_sz)-r_begin_ptr);
					std::fill	(m_cell_memory.begin()+i_begin/4,m_cell_memory.begin()+i_end/4,COLOR_DEF_FULL);
                }
                it++;
            }
        }
    
        DrawBitmap			(paDetMem,Irect().set(0,0,paDetMem->Width,paDetMem->Height),
                            m_cell_memory.begin(), d_w,d_h);
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
	DrawDetImage();
}
//---------------------------------------------------------------------------

