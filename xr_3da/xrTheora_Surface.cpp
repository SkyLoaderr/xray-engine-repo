#include "stdafx.h"
#include "xrtheora_surface.h"
#include "xrtheora_stream.h"

CTheoraSurface::CTheoraSurface()
{
	ready				= FALSE;
	// streams
	m_rgb				= 0;
	m_alpha				= 0;
	// timing
	tm_play				= 0;
	tm_total			= 0;
	// sdl
#ifdef SDL_OUTPUT
	sdl_screen			= 0;
	sdl_yuv_overlay		= 0;
#endif
	// controls
	playing				= FALSE;
	looped				= FALSE;
}

CTheoraSurface::~CTheoraSurface()
{
	xr_delete			(m_rgb);
	xr_delete			(m_alpha);
#ifdef SDL_OUTPUT
	SDL_Quit			();
#endif
}

void CTheoraSurface::Reset()
{
	if (m_rgb)			m_rgb->Reset	();
	if (m_alpha)		m_alpha->Reset	();
	tm_play				= 0;
}

BOOL CTheoraSurface::Valid()
{
	return				ready;
}
/*
void CTheoraSurface::write_tga(u32 width, u32 height, u32 frame, u32 pixelformat)
{
	VERIFY		(m_rgb);
	yuv_buffer*	yuv_rgb		= m_rgb->current_yuv_buffer();
	yuv_buffer*	yuv_alpha	= m_alpha?m_alpha->current_yuv_buffer():0;

	u32* data		= xr_alloc<u32>(width*height*4);

	int uv_w, uv_h;
	switch (pixelformat){
	case OC_PF_444:	uv_w=1; uv_h=1; break;
	case OC_PF_422:	uv_w=2; uv_h=1; break;
	case OC_PF_420:	uv_w=2; uv_h=2; break;
	default: NODEFAULT;
	}
	float K = 0.256788f + 0.504129f + 0.097906f;

	// rgb
	if (yuv_rgb){
		yuv_buffer&	yuv	= *yuv_rgb;
		u32 pos = 0;
		for (u32 h=0; h<height; ++h){
			u8* Y		= yuv.y+yuv.y_stride*h;
			u8* U		= yuv.u+yuv.uv_stride*(h/uv_h);
			u8* V		= yuv.v+yuv.uv_stride*(h/uv_h);
			for (u32 w=0; w<width; ++w){
				u8 y	= Y[w];
				u8 u	= U[w/uv_w];
				u8 v	= V[w/uv_w];

				int C	= y - 16;
				int D	= u - 128;
				int E	= v - 128;

				int R	= clampr(( 298 * C           + 409 * E + 128) >> 8,0,255);
				int G	= clampr(( 298 * C - 100 * D - 208 * E + 128) >> 8,0,255);
				int B	= clampr(( 298 * C + 516 * D           + 128) >> 8,0,255);
				data[pos++] = color_rgba(R,G,B,0);
			}
		}
	}

	// alpha
	if (yuv_alpha){
		yuv_buffer&	yuv	= *yuv_alpha;
		u32 pos = 0;
		for (u32 h=0; h<height; ++h){
			u8* Y		= yuv.y+yuv.y_stride*h;
			u8* U		= yuv.u+yuv.uv_stride*(h/uv_h);
			u8* V		= yuv.v+yuv.uv_stride*(h/uv_h);
			for (u32 w=0; w<width; ++w){
				u8 y	= Y[w];
				u32& clr= data[pos++];
				clr		= subst_alpha(clr,iFloor(float((y-16))/K));
			}
		}
	}

	TGAdesc		tga;
	tga.data	= data;
	tga.format	= IMG_32B;
	tga.height	= height;
	tga.width	= width;
	tga.scanlenght=width*4;

	string_path p;
	sprintf_s	(p,sizeof(p),"u:\\data\\test_#%04d.tga",frame);

	vfs::writer w = vfs::wopen(p);
	tga.maketga	(w);

	u_free(data);
}
*/
void CTheoraSurface::Play(BOOL _looped)		
{
	playing=TRUE;
	looped=_looped;
	tm_start = Device.dwTimeContinual;
}

BOOL CTheoraSurface::Update(u32 dt)
{
	VERIFY				(Valid());
	BOOL redraw			= FALSE;


	if (playing){
		tm_play			= Device.dwTimeContinual-tm_start;
		if (tm_play>=tm_total){ 
			if (looped){	
				tm_start = tm_start+tm_total;
				Reset	();
			}else{	
				Stop	();
				return	FALSE;
			}
		}
		if (m_rgb)		redraw|=m_rgb->Decode	(tm_play);
		if (m_alpha)	redraw|=m_alpha->Decode	(tm_play);
	}
	return redraw;
} 

BOOL CTheoraSurface::Load(const char* fname)
{
	VERIFY				(FALSE==ready);
	m_rgb				= xr_new<CTheoraStream>();
	BOOL res			= m_rgb->Load(fname);
	if (res){
		string_path		alpha,ext;
		strcpy			(alpha,fname);
		pstr pext		= strext(alpha);
		if (pext){	
			strcpy		(ext,pext);
			*pext		= 0;
		}
		strconcat		(alpha,alpha,"#alpha",ext);
		if (FS.exist(alpha)){
			m_alpha		= xr_new<CTheoraStream>	();
			if (!m_alpha->Load(alpha))	res = FALSE;
		}
	}
	if (res){
#ifdef DEBUG
		if (m_alpha){
			VERIFY		(m_rgb->tm_total==m_alpha->tm_total);
			VERIFY		(m_rgb->t_info.frame_width==m_alpha->t_info.frame_width);
			VERIFY		(m_rgb->t_info.frame_height==m_alpha->t_info.frame_height);
			VERIFY		(m_rgb->t_info.pixelformat==m_alpha->t_info.pixelformat);
		}
#endif
		VERIFY3			(btwIsPow2(m_rgb->t_info.frame_width)&&btwIsPow2(m_rgb->t_info.frame_height),"Invalid size.",fname);
		tm_total		= m_rgb->tm_total;
		VERIFY			(0!=tm_total);
		// reset playback
		Reset			();
		// open SDL video
#ifdef SDL_OUTPUT
		open_sdl_video	();
#endif
		ready			= TRUE;
	}else{
		xr_delete		(m_rgb);
		xr_delete		(m_alpha);
	}
	return				res;
}

u32	CTheoraSurface::Width()
{
	return				m_rgb->t_info.frame_width;
}

u32	CTheoraSurface::Height()
{
	return				m_rgb->t_info.frame_height;
}

void CTheoraSurface::DecompressFrame(u32* data)
{
	VERIFY		(m_rgb);
	yuv_buffer*	yuv_rgb		= m_rgb->CurrentFrame();
	yuv_buffer*	yuv_alpha	= m_alpha?m_alpha->CurrentFrame():0;

	u32 width		= m_rgb->t_info.frame_width;
	u32 height		= m_rgb->t_info.frame_height;
	u32 pixelformat	= m_rgb->t_info.pixelformat;
/*
	Memory.mem_fill32(data,0x00FF0000,width*height);
/*/
	int uv_w=1, uv_h=1;
	switch (pixelformat){
	case OC_PF_444:	uv_w=1; uv_h=1; break;
	case OC_PF_422:	uv_w=2; uv_h=1; break;
	case OC_PF_420:	uv_w=2; uv_h=2; break;
	default: NODEFAULT;
	}
	static const float K = 0.256788f + 0.504129f + 0.097906f;

	// rgb
	if (yuv_rgb){
		yuv_buffer&	yuv	= *yuv_rgb;
		u32 pos = 0;
		for (u32 h=0; h<height; ++h){
			u8* Y		= yuv.y+yuv.y_stride*h;
			u8* U		= yuv.u+yuv.uv_stride*(h/uv_h);
			u8* V		= yuv.v+yuv.uv_stride*(h/uv_h);
			for (u32 w=0; w<width; ++w){
				u8 y	= Y[w];
				u8 u	= U[w/uv_w];
				u8 v	= V[w/uv_w];

				int C	= y - 16;
				int D	= u - 128;
				int E	= v - 128;

				int R	= clampr(( 298 * C           + 409 * E + 128) >> 8,0,255);
				int G	= clampr(( 298 * C - 100 * D - 208 * E + 128) >> 8,0,255);
				int B	= clampr(( 298 * C + 516 * D           + 128) >> 8,0,255);
				data[pos++] = color_rgba(R,G,B,255);
			}
		}
	}

	// alpha
	if (yuv_alpha){
		yuv_buffer&	yuv	= *yuv_alpha;
		u32 pos = 0;
		for (u32 h=0; h<height; ++h){
			u8* Y		= yuv.y+yuv.y_stride*h;
//			u8* U		= yuv.u+yuv.uv_stride*(h/uv_h);
//			u8* V		= yuv.v+yuv.uv_stride*(h/uv_h);
			for (u32 w=0; w<width; ++w){
				u8 y	= Y[w];
				u32& clr= data[pos++];
				clr		= subst_alpha(clr,iFloor(float((y-16))/K));
			}
		}
	}
//*/
}

#ifdef SDL_OUTPUT
void CTheoraSurface::open_sdl_video()
{
	VERIFY(m_rgb);
	theora_info& t_info		= m_rgb->t_info;

	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		msg					("Unable to init SDL: %s", SDL_GetError());
		return;
	}

	sdl_screen				= SDL_SetVideoMode(t_info.frame_width, t_info.frame_height, 0, SDL_SWSURFACE);
	if ( sdl_screen == NULL ) {
		msg					("Unable to set %dx%d video: %s", t_info.frame_width,t_info.frame_height,SDL_GetError());
		return;
	}

	sdl_yuv_overlay			= SDL_CreateYUVOverlay(t_info.frame_width, t_info.frame_height, SDL_YV12_OVERLAY, sdl_screen);
	if ( sdl_yuv_overlay == NULL ) {
		msg					("SDL: Couldn't create SDL_yuv_overlay: %s", SDL_GetError());
		return;
	}
	sdl_rect.x				= 0;
	sdl_rect.y				= 0;
	sdl_rect.w				= t_info.frame_width;
	sdl_rect.h				= t_info.frame_height;

	SDL_DisplayYUVOverlay	(sdl_yuv_overlay, &sdl_rect);
}

void CTheoraSurface::write_sdl_video()
{
	VERIFY(m_rgb);
	theora_info& t_info		= m_rgb->t_info;
	yuv_buffer&	t_yuv_buffer= *m_rgb->current_yuv_buffer();
	int i;
	int crop_offset;
	// Lock SDL_yuv_overlay 
	if ( SDL_MUSTLOCK(sdl_screen) )  
		if ( SDL_LockSurface(sdl_screen) < 0 ) return;
	if (SDL_LockYUVOverlay(sdl_yuv_overlay) < 0) return;
	// let's draw the data (*yuv[3]) on a SDL screen (*screen) 
	// deal with border stride 
	// reverse u and v for SDL 
	// and crop input properly, respecting the encoded frame rect 
	crop_offset=t_info.offset_x+t_yuv_buffer.y_stride*t_info.offset_y;
	for(i=0;i<sdl_yuv_overlay->h;i++)
		mem_copy(sdl_yuv_overlay->pixels[0]+sdl_yuv_overlay->pitches[0]*i, t_yuv_buffer.y+crop_offset+t_yuv_buffer.y_stride*i, sdl_yuv_overlay->w);
	crop_offset=(t_info.offset_x/2)+(t_yuv_buffer.uv_stride)*(t_info.offset_y/2);
	for(i=0;i<sdl_yuv_overlay->h/2;i++){
		mem_copy(sdl_yuv_overlay->pixels[1]+sdl_yuv_overlay->pitches[1]*i, t_yuv_buffer.v+crop_offset+t_yuv_buffer.uv_stride*i, sdl_yuv_overlay->w/2);
		mem_copy(sdl_yuv_overlay->pixels[2]+sdl_yuv_overlay->pitches[2]*i, t_yuv_buffer.u+crop_offset+t_yuv_buffer.uv_stride*i, sdl_yuv_overlay->w/2);
	}
	// Unlock SDL_yuv_overlay 
	if ( SDL_MUSTLOCK(sdl_screen) ) SDL_UnlockSurface(sdl_screen);
	SDL_UnlockYUVOverlay(sdl_yuv_overlay);
	// Show, baby, show!
	SDL_DisplayYUVOverlay(sdl_yuv_overlay, &sdl_rect);
}
#endif
