#include "stdafx.h"
#pragma hdrstop

#include "GameFont.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define MAX_CHARS	1024
CGameFont::CGameFont(LPCSTR section, u32 flags)
{
	Initialize	(pSettings->r_string(section,"shader"),pSettings->r_string(section,"texture"),flags);
	if (pSettings->line_exist(section,"size"))
		SetSize(pSettings->r_float(section,"size"));
	if (pSettings->line_exist(section,"interval"))
		SetInterval(pSettings->r_fvector2(section,"interval"));
}
CGameFont::CGameFont(LPCSTR shader, LPCSTR texture, u32 flags)
{
	Initialize	(shader,texture,flags);
}
void CGameFont::Initialize		(LPCSTR cShader, LPCSTR cTexture, u32 flags)
{
	eCurrentAlignment			= alLeft;
	uFlags						= flags;
	vInterval.set				(1.f,1.f);

	for (int i=0; i<256; i++)	CharMap[i] = i;
	strings.reserve				(128);

	// check ini exist
	string256 fn,buf;
	strcpy		(buf,cTexture); if (strext(buf)) *strext(buf)=0;
	R_ASSERT2	(FS.exist(fn,"$game_textures$",buf,".ini"),fn);
	CInifile* ini				= CInifile::Create(fn);
	if (ini->section_exist("symbol_coords")){
		for (int i=0; i<256; i++){
			sprintf					(buf,"%03d",i);
			Fvector v				= ini->r_fvector3("symbol_coords",buf);
			TCMap[i].set			(v.x,v.y,v[2]-v[0]);
		}
		fHeight						= ini->r_float("symbol_coords","height");
	}else{
		if (ini->section_exist("char widths")){
			fHeight					= ini->r_float("char widths","height");
			const int cpl			= 16;
			for (int i=0; i<256; i++){
				sprintf				(buf,"%d",i);
				float w				= ini->r_float("char widths",buf);
				TCMap[i].set		((i%cpl)*fHeight,(i/cpl)*fHeight,w);
			}
		}else{
			R_ASSERT(ini->section_exist("font_size"));
			fHeight					= ini->r_float("font_size","height");
			float width				= ini->r_float("font_size","width");
			const int cpl			= ini->r_s32	("font_size","cpl");
			for (int i=0; i<256; i++)
				TCMap[i].set			((i%cpl)*width,(i/cpl)*fHeight,width);
		}
	}
	if (!(uFlags&fsDeviceIndependent))
		SetSize					(fHeight);
	CInifile::Destroy			(ini);

	// Shading
	pShader.create				(cShader,cTexture);
	pGeom.create				(FVF::F_TL, RCache.Vertex.Buffer(), RCache.QuadIB);
}

CGameFont::~CGameFont()
{
	// Shading
	pShader.destroy		();
	pGeom.destroy		();
}

void CGameFont::OnRender()
{
	if (pShader)		RCache.set_Shader	(pShader);

	if (!(uFlags&fsValid))
	{
		CTexture* T		= RCache.get_ActiveTexture(0);
		vTS.set			((int)T->get_Width(),(int)T->get_Height());
		vHalfPixel.set	(0.5f/float(vTS.x),0.5f/float(vTS.y));
		for (int i=0; i<256; i++){
			TCMap[i].x	/= float(vTS.x);
			TCMap[i].y	/= float(vTS.y);
			TCMap[i].z	/= float(vTS.x);
		}
		fTCHeight		= fHeight/float(vTS.y);
		uFlags			|= fsValid;
	}

	float				w_2		= float	(Device.dwWidth)	/ 2;
	float				h_2		= float	(Device.dwHeight)	/ 2;

	for (u32 i=0; i<strings.size(); )
	{
		// calculate first-fit
		int		count	=	1;
		int		length	=	xr_strlen(strings[i].string);
		while	((i+count)<strings.size()) {
			int	L	=	xr_strlen(strings[i+count].string);
			if ((L+length)<MAX_CHARS)
			{
				count	++;
				length	+=	L;
			}
			else		break;
		}

		// lock AGP memory
		u32	vOffset;
		FVF::TL* v		= (FVF::TL*)RCache.Vertex.Lock	(length*4,pGeom.stride(),vOffset);
		FVF::TL* start	= v;

		// fill vertices
		u32 last		= i+count;
		for (; i<last; i++) {
			String		&PS	= strings[i];
			int			len	= xr_strlen(PS.string);
			if (len) {
				float	X	= float			(iFloor((uFlags&fsDeviceIndependent)?(PS.x+1)*w_2:PS.x));
				float	Y	= float			(iFloor((uFlags&fsDeviceIndependent)?(PS.y+1)*h_2:PS.y));
				float	S	= ConvertSize	(PS.size);
				float	Y2	= Y+S;
				S			= (S*vTS.x)/fHeight;

				switch(PS.align)
				{
				case alCenter:	X-=SizeOf(*PS.string,PS.size)*.5f;	break;
				case alRight:	X-=SizeOf(*PS.string,PS.size);		break;
				}

				u32	clr,clr2;
				clr2 = clr	= PS.c;
				if (uFlags&fsGradient){
					u32	_R	= color_get_R	(clr)/2;
					u32	_G	= color_get_G	(clr)/2;
					u32	_B	= color_get_B	(clr)/2;
					u32	_A	= color_get_A	(clr);
					clr2	= color_rgba	(_R,_G,_B,_A);
				}

				float	tu,tv;
				for (int j=0; j<len; j++)
				{
					int c		= CharMap	[(u8)PS.string[j]];
					Fvector& l	= TCMap		[(u8)PS.string[j]];
					float scw	= S*l.z;
					if ((c>=0)&&!fis_zero(l.z))
					{
						tu		= l.x+vHalfPixel.x;
						tv		= l.y+vHalfPixel.y;
						v->set	(X,		Y2,	clr2,tu,		tv+fTCHeight);	v++;
						v->set	(X,		Y,	clr, tu,		tv);			v++;
						v->set	(X+scw,	Y2,	clr2,tu+l.z,	tv+fTCHeight);	v++;
						v->set	(X+scw,	Y,	clr, tu+l.z,	tv);			v++;
					}
					X+=scw*vInterval.x;
				}
			}
		}

		// Unlock and draw
		u32 vCount = (u32)(v-start);
		RCache.Vertex.Unlock		(vCount,pGeom.stride());
		if (vCount)
		{
			RCache.set_Geometry		(pGeom);
			RCache.Render			(D3DPT_TRIANGLELIST,vOffset,0,vCount,0,vCount/2);
		}
	}
	strings.clear();
}

void CGameFont::Add(float _x, float _y, LPCSTR s, u32 _c, float _size)
{
	VERIFY(xr_strlen(s)<127);
	String rs;
	rs.x=_x;
	rs.y=_y;
	rs.c=_c;
	rs.size=_size;
	strings.push_back(rs);
	strcpy((char *) &(strings[strings.size()-1].string),s);
}

static string2048 s_tmp;
void __cdecl CGameFont::Out(float _x, float _y, LPCSTR fmt,...)
{
	String rs;
	rs.x=_x;
	rs.y=_y;
	rs.c=dwCurrentColor;
	rs.size=fCurrentSize;
	rs.align=eCurrentAlignment;

	va_list p;
	va_start(p,fmt);

	int vs_sz = vsprintf(s_tmp,fmt,p); VERIFY(vs_sz<sizeof(s_tmp));
	rs.string = s_tmp;
	va_end(p);

	strings.push_back(rs);
}

void __cdecl CGameFont::OutNext(LPCSTR fmt,...)
{
	String rs;
	rs.x=fCurrentX;
	rs.y=fCurrentY;
	rs.c=dwCurrentColor;
	rs.size=fCurrentSize;
	rs.align=eCurrentAlignment;

	va_list p;
	va_start(p,fmt);
	int vs_sz = vsprintf(s_tmp,fmt,p); VERIFY(vs_sz<sizeof(s_tmp));
	rs.string = s_tmp;
	va_end(p);

	strings.push_back(rs);
	OutSkip(1);
}

void __cdecl CGameFont::OutPrev(LPCSTR fmt,...)
{
	String rs;
	rs.x=fCurrentX;
	rs.y=fCurrentY;
	rs.c=dwCurrentColor;
	rs.size=fCurrentSize;
	rs.align=eCurrentAlignment;

	va_list p;
	va_start(p,fmt);
	int vs_sz = vsprintf(s_tmp,fmt,p); VERIFY(vs_sz<sizeof(s_tmp));
	rs.string = s_tmp;
	va_end(p);

	strings.push_back(rs);
	OutSkip(-1);
}

void CGameFont::OutSkip(float val)
{
	fCurrentY += val*CurrentHeight();
}

float CGameFont::SizeOf(LPCSTR s,float size)
{
	if (uFlags&fsValid){
		int		len			= xr_strlen(s);
		float	X			= 0;
		if (len) for (int j=0; j<len; j++) X+=TCMap[s[j]].z;
		return				X*ConvertSize(size)/fHeight*vInterval.x*vTS.x;
	}else{
		return 0;
	}
}

