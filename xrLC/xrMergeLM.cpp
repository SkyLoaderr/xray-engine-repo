#include "stdafx.h"
#include "build.h"

struct _point {
	int x,y;
	IC void set(int _x, int _y)
	{	x=_x; y=_y;	}
	IC void set(_point &P)
	{	set(P.x,P.y); }
};
const int USED_P1	= (1<<0);
const int USED_P2	= (1<<1);
struct _rect {
	_point	a,b;		// min,max
	int		iArea;
	int		mask;		// which control points we already used
	
	IC void	set	(_rect &R)
	{
		a.set	(R.a);
		b.set	(R.b);
		iArea	= R.iArea;
		mask	= R.mask;
	};
	IC void init(int ax, int ay, int bx, int by)
	{
		a.set(ax,ay);
		b.set(bx,by);
	}
	IC void	calc_area()
	{
		iArea = SizeX()*SizeY();
	};
	IC bool	PointInside(_point &P)
	{
		return (P.x>=a.x && P.x<=b.x && P.y>=a.y && P.y<=b.y);
	};
	IC bool	Intersect(_rect &R)
	{
		if (R.b.x < a.x) return false;
		if (R.b.y < a.y) return false;
		if (R.a.x > b.x) return false;
		if (R.a.y > b.y) return false;
		return true;
	};
	IC void	GetAB(_point &A, _point &B)
	{
		A.x = b.x; A.y = a.y;
		B.x = a.x; B.y = b.y;
	};
	IC void	Invalidate()
	{
		a.set(SHRT_MAX,SHRT_MAX);
		b.set(SHRT_MIN,SHRT_MIN);
	}
	IC void	Merge(_rect &R) 
	{
		if (R.a.x<a.x) a.x=R.a.x;
		if (R.a.y<a.y) a.y=R.a.y;
		if (R.b.x>b.x) b.x=R.b.x;
		if (R.b.y>b.y) b.y=R.b.y;
	}
	IC int	SizeX()
	{	return b.x-a.x+1; }
	IC int	SizeY()
	{	return b.y-a.y+1; }
};

IC bool cmp_defl(CDeflector *p1, CDeflector *p2)
{
	Fvector &C = Deflector->Center;
	WORD M1 = p1->GetBaseMaterial();
	WORD M2 = p2->GetBaseMaterial();
	if (M1<M2) return true;
	if (M1>M2) return false;
	return C.distance_to_sqr(p1->Center) < C.distance_to_sqr(p1->Center);
}

typedef vector<_rect>	vecR;
typedef vecR::iterator	vecRIT;

typedef vector<_point>	vecP;
typedef vecP::iterator	vecPIT;

typedef vector<int>		vecI;
typedef vecI::iterator	vecIIT;

// Data in
static	vecR	rects;
static	vecR	selected;

// Data in process
static	vecR	collected;
static	vecI	perturb;

// Result
static	vecR	best;
static	vecI	best_seq;
_rect	brect  = { {0,0}, {0,0}, INT_MAX};
_rect	currect= { {0,0}, {0,0}, 0 };

// Sorting by areas
IC bool cmp_rect(int r1, int r2)
{
	return selected[r1].iArea > selected[r2].iArea;	// Need decreasing order
}

static	BYTE	surface[512*512];
static	int		current_rect	= 0;
const	DWORD	alpha_ref		= 254-BORDER;

// Initialization
void InitSurface()
{
	FillMemory(surface,512*512,0);
}

// Rendering of rect
void _rect_register(_rect &R, CDeflector* D, BOOL bRotate)
{
	R.mask				= 0;
	collected.push_back(R);
	
	LPDWORD lm	= D->lm.pSurface;
	DWORD	s_x	= D->lm.dwWidth+2*BORDER;
	DWORD	s_y = D->lm.dwHeight+2*BORDER;
	
	if (!bRotate) {
		// Normal (and fastest way)
		for (DWORD y=0; y<s_y; y++)
		{
			BYTE*	P = surface+(y+R.a.y)*512+R.a.x;	// destination scan-line
			DWORD*	S = lm + y*s_x;
			for (DWORD x=0; x<s_x; x++) 
			{
				DWORD C = *S++;
				DWORD A = RGBA_GETALPHA(C);
				*P ++	= (A>=alpha_ref)?255:0;
			}
		}
	} else {
		// Rotated :(
		for (DWORD y=0; y<s_x; y++)
		{
			BYTE*	P = surface+(y+R.a.y)*512+R.a.x;	// destination scan-line
			for (DWORD x=0; x<s_y; x++)
			{
				DWORD C = lm[x*s_x+y];
				DWORD A = RGBA_GETALPHA(C);
				*P ++	= (A>=alpha_ref)?255:0;
			}
		}
	}
}

// Test of per-pixel intersection (surface test)
bool Place_Perpixel(_rect& R, CDeflector* D, BOOL bRotate)
{
	LPDWORD lm			= D->lm.pSurface;
	DWORD	s_x			= D->lm.dwWidth+2*BORDER;
	DWORD	s_y			= D->lm.dwHeight+2*BORDER;
	CDeflector::span* sp= D->lm_spans;
	
	if (!bRotate) {
		// Normal (and fastest way)
		for (DWORD y=0; y<s_y; y++)
		{
			BYTE*	P = surface+(y+R.a.y)*512+R.a.x+sp[y].first;	// destination scan-line
			BYTE*	E = P + (sp[y].second-sp[y].first) + 1;
			DWORD*	S = lm + y*s_x + sp[y].first;
			for (; P!=E; P++,S++) {
				if ((*P)&&(RGBA_GETALPHA(*S)>=alpha_ref))	return false;	// overlap detected
			}
		}
	} else {
		// Rotated :(
		for (DWORD y=0; y<s_x; y++)
		{
			BYTE*	P = surface+(y+R.a.y)*512+R.a.x;
			for (DWORD x=0; x<s_y; x++,P++)
			{
				DWORD C = lm[x*s_x+y];
				if ((*P)&&(RGBA_GETALPHA(C)>=alpha_ref))	return false;	// overlap detected
			}
		}
	}
	
	// It's OK to place it
	return true;
}

// Check for intersection
/*
bool Place(_rect &R, vecRIT CIT)
{
	if (R.b.x >= 512) return false;
	if (R.b.y >= 512) return false;
	
	// Test next - major speedup
	if (CIT!=collected.end()) {
		if (R.Intersect(*(CIT+1))) return false;
	}

	// Degrade to surface testing
	return Place_Perpixel(R);
};
*/

BOOL _rect_place(_rect &r, CDeflector* D)
{
	// Normal
	{
		_rect R;
		DWORD x_max = 512-r.b.x; 
		DWORD y_max = 512-r.b.y; 
		for (DWORD _Y=0; _Y<y_max; _Y++)
		{
			for (DWORD _X=0; _X<x_max; _X++)
			{
				if (surface[_Y*512+_X]) continue;
				R.init(_X,_Y,_X+r.b.x,_Y+r.b.y);
				if (Place_Perpixel(R,D,FALSE)) {
					_rect_register(R,D,FALSE);
					return TRUE;
				}
			}
		}
	}

	// Rotated
	{
		_rect R;
		DWORD x_max = 512-r.b.y; 
		DWORD y_max = 512-r.b.x; 
		for (DWORD _Y=0; _Y<y_max; _Y++)
		{
			for (DWORD _X=0; _X<x_max; _X++)
			{
				if (surface[_Y*512+_X]) continue;

				R.init(_X,_Y,_X+r.b.y,_Y+r.b.x);
				if (Place_Perpixel(R,D,TRUE)) {
					_rect_register(R,D,TRUE);
					return TRUE;
				}
			}
		}
	}

	return FALSE;
};

#pragma optimize( "g", off )
//agpwy
void CBuild::MergeLM()
{
	vecDefl		deflNew;
	vecDefl		SEL;

	Status("Processing...");
	DWORD dwOldCount = g_deflectors.size();
	while (g_deflectors.size()) {
		Deflector = g_deflectors[0];
		if (g_deflectors.size()>1) {
			std::sort(g_deflectors.begin()+1,g_deflectors.end(),cmp_defl);
			int maxarea = 512*512*4;	// Max up to 4 lm selected
			int curarea = 0;
			for (int i=1; i<(int)g_deflectors.size(); i++)
			{
				if (curarea+g_deflectors[i]->iArea > maxarea) break;
				curarea += g_deflectors[i]->iArea;
				SEL.push_back(g_deflectors[i]);
			}
			if (SEL.empty()) 
			{
				// No deflectors found to merge
				// Simply transfer base deflector to new list
				deflNew.push_back(Deflector);
				g_deflectors.erase(g_deflectors.begin());
			} else {
				// Transfer rects
				SEL.push_back(Deflector);
				for (int K=0; K<(int)SEL.size(); K++)
				{
					_rect	T; 
					T.a.set	(0,0);
					T.b.set	(SEL[K]->lm.dwWidth+2*BORDER-1, SEL[K]->lm.dwHeight+2*BORDER-1);
					T.iArea = SEL[K]->iArea;
					selected.push_back(T);
					perturb.push_back(K);
				}

				// Sort by size decreasing and startup
				std::sort			(perturb.begin(),perturb.end(),cmp_rect);
				InitSurface			();
				int id				= perturb[0];
				_rect &First		= selected[id];
				_rect_register		(First,SEL[id],FALSE);
				best.push_back		(First);
				best_seq.push_back	(id);
				brect.set			(First);

				// Process 
				collected.reserve	(SEL.size());
				for (int R=1; R<(int)selected.size(); R++) 
				{
					int ID = perturb[R];
					if (_rect_place(selected[ID],SEL[ID])) 
					{
						brect.Merge			(collected.back());
						best.push_back		(collected.back());
						best_seq.push_back	(ID);
					}
					Progress(float(R)/float(selected.size()));
				}
				R_ASSERT	(brect.a.x==0 && brect.a.y==0);
				 
				//  Analyze resuls
				Msg("%3d / %3d - [%d,%d]",best.size(),selected.size(),brect.SizeX(),brect.SizeY());
				CDeflector*	pDEFL = new CDeflector;
				pDEFL->lm.bHasAlpha = FALSE;
				pDEFL->lm.dwWidth   = 512;
				pDEFL->lm.dwHeight  = 512;
				for (K = 0; K<(int)best.size(); K++) 
				{
					int			iRealIndex	= best_seq	[K];
					_rect&		Place		= best		[K];
					_point&		Offset		= Place.a;
					BOOL		bRotated;
					b_texture&	T			= SEL[iRealIndex]->lm;
					int			T_W			= (int)T.dwWidth	+ 2*BORDER;
					int			T_H			= (int)T.dwHeight	+ 2*BORDER;
					if (Place.SizeX() == T_W) {
						R_ASSERT(Place.SizeY() == T_H);
						bRotated = FALSE;
					} else {
						R_ASSERT(Place.SizeX() == T_H);
						R_ASSERT(Place.SizeY() == T_W);
						bRotated = TRUE;
					}

					// Merge
					pDEFL->Capture		(SEL[iRealIndex],Offset.x,Offset.y,Place.SizeX(),Place.SizeY(),bRotated);

					// Destroy old deflector
					vecDeflIt		OLD = find(g_deflectors.begin(),g_deflectors.end(),SEL[iRealIndex]);
					VERIFY			(OLD!=g_deflectors.end());
					g_deflectors.erase(OLD);
					delete			SEL[iRealIndex];
				}
				pDEFL->Save			();
				deflNew.push_back	(pDEFL);
				
				// Cleanup
				SEL.clear			();
				collected.clear		();
				selected.clear		();
				perturb.clear		();
				best.clear			();
				best_seq.clear		();
				brect.iArea			= INT_MAX;
			}
		} else {
			deflNew.push_back(Deflector);
			g_deflectors.clear();
		}
		Progress(1.f-float(g_deflectors.size())/float(dwOldCount));
	}
	R_ASSERT(g_deflectors.empty());
	g_deflectors = deflNew;
	Msg("%d lightmaps builded",g_deflectors.size());
}
