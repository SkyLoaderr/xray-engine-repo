#include "stdafx.h"
#include "build.h"
#include "lightmap.h"
#include "xrPhase_MergeLM_Rect.h"

// Surface access
extern void _InitSurface	();
extern BOOL _rect_place		(_rect &r, CDeflector::Layer* D);

b_light*	compare_layer	= 0;

IC int	compare_defl		(CDeflector* D1, CDeflector* D2)
{
	// First  - by material
	WORD M1		= D1->GetBaseMaterial();
	WORD M2		= D2->GetBaseMaterial();
	if (M1<M2)	return 1;
	if (M1>M2)	return 0;
	
	// Second - by number of layers
	DWORD L1	= D1->layers.size();
	DWORD L2	= D2->layers.size();
	if (L1<L2)	return 1;
	if (L1>L2)	return 0;
	
	// Third  - lexicographical compare
	for (DWORD I=0; I<L1; I++)
	{
		CDeflector::Layer&	lay1	= D1->layers[I];
		CDeflector::Layer&	lay2	= D2->layers[I];
		if (lay1.base<lay2.base)	return 1;
		if (lay1.base>lay2.base)	return 0;
	}
	return 2;
}

// by material / state changes
bool	compare1_defl		(CDeflector* D1, CDeflector* D2)
{
	switch (compare_defl(D1,D2))	
	{
	case 0:		return false;
	case 1:		return true;
	case 2:		return false;
	default:	return false;
	}
}

// as 1, but also by geometric locality
bool	compare2_defl		(CDeflector* D1, CDeflector* D2)
{
	switch (compare_defl(D1,D2))	
	{
	case 0:		return false;
	case 1:		return true;
	case 2:		
		{
			float	dist1	= Deflector->Sphere.P.distance_to(D1->Sphere.P) - D1->Sphere.R;
			float	dist2	= Deflector->Sphere.P.distance_to(D2->Sphere.P) - D2->Sphere.R;
			return	dist1 < dist2;
		}
	default:	return false;
	}
}

// by area of layer - reverse
bool	compare3_defl		(CDeflector* D1, CDeflector* D2)
{
	CDeflector::Layer*	L1 = D1->GetLayer(compare_layer);
	CDeflector::Layer*	L2 = D2->GetLayer(compare_layer);
	return L1->Area() > L2->Area();
}
class	pred_remove { public: IC bool	operator() (CDeflector* D) { { if (0==D) return TRUE;}; if (D->bMerged) {D->bMerged=FALSE; return TRUE; } else return FALSE;  }; };

// O(n*n) sorting
void	dumb_sort	(vecDefl& L)
{
	for (int n1=0; n1<int(L.size()); n1++)
	{
		Progress(float(n1)/float(L.size()));
		for (int n2=1; n2<int(L.size()); n2++)
			if (compare2_defl(L[n2],L[n2-1]))	swap(L[n2],L[n2-1]);
	}
}

void CBuild::xrPhase_MergeLM()
{
	vecDefl			Layer;

	for (DWORD light_layer=0; light_layer<pBuild->lights.size(); light_layer++)
	{
		Status		("-= LM-Layer #%d =-",light_layer);
		
		// **** Select all deflectors, which contain this light-layer
		Layer.clear	();
		b_light*	L_base	= pBuild->lights[light_layer].original;
		for (int it=0; it<(int)g_deflectors.size(); it++)
		{
			if (g_deflectors[it]->bMerged)				continue;
			if (0==g_deflectors[it]->GetLayer(L_base))	continue;	
			Layer.push_back	(g_deflectors[it]);
		}
		if (Layer.empty())	continue;
		
		// Merge this layer
		while (Layer.size()) 
		{
			string256	phase_name;
			sprintf		(phase_name,"Building lightmap %d...",g_lightmaps.size());
			Phase		(phase_name);
			
			// Sort layer by similarity (state changes)
			Status		("Selection 1...");
			Msg			("LS: %d",	Layer.size());
			std::sort	(Layer.begin(),Layer.end(),compare1_defl);

			// Sort layer (by material and distance from "base" deflector)
			Status		("Selection 2...");
			Msg			("LS: %d",	Layer.size());
			Deflector	= Layer[0];
			R_ASSERT	(Deflector);
			if (Layer.size()>2)	dumb_sort(Layer);

			// Select first deflectors which can fit
			Status		("Selection 3...");
			int maxarea		= lmap_size*lmap_size*5;	// Max up to 5 lm selected
			int curarea		= 0;
			int merge_count	= 0;
			for (it=0; it<(int)Layer.size(); it++)
			{
				int		defl_area	= Layer[it]->GetLayer(L_base)->Area();
				if (curarea + defl_area > maxarea) break;
				curarea		+=	defl_area;
				merge_count ++;
			}

			// Sort part of layer by size decreasing
			Status		("Selection 4...");
			compare_layer	= L_base;
			std::sort	(Layer.begin(),Layer.begin()+merge_count,compare3_defl);

			// Startup
			Status		("Selection 5...");
			_InitSurface			();
			CLightmap*	lmap		= new CLightmap;
			g_lightmaps.push_back	(lmap);

			// Process 
			for (it=0; it<merge_count; it++) 
			{
				if (0==(it%16))		Status	("Process [%d/%d]...",it,merge_count);
				CDeflector::Layer&	L = *(Layer[it]->GetLayer(L_base));
				_rect		rT,rS; 
				rS.a.set	(0,0);
				rS.b.set	(L.lm.dwWidth+2*BORDER-1, L.lm.dwHeight+2*BORDER-1);
				rS.iArea	= L.Area();
				rT			= rS;
				if (_rect_place(rT,&L)) 
				{
					BOOL		bRotated;
					if (rT.SizeX() == rS.SizeX()) {
						R_ASSERT(rT.SizeY() == rS.SizeY());
						bRotated = FALSE;
					} else {
						R_ASSERT(rT.SizeX() == rS.SizeY());
						R_ASSERT(rT.SizeY() == rS.SizeX());
						bRotated = TRUE;
					}
					lmap->Capture		(Layer[it],rT.a.x,rT.a.y,rT.SizeX(),rT.SizeY(),bRotated,L_base);
					Layer[it]->bMerged	= TRUE;
				}
				Progress(sqrtf(float(it)/float(merge_count)));
			}
			Progress	(1.f);
			
			// Remove merged lightmaps
			Status		("Cleanup...");
			vecDeflIt last	= remove_if	(Layer.begin(),Layer.end(),pred_remove());
			Layer.erase		(last,Layer.end());

			// Save
			Status		("Saving...");
			lmap->Save	();
		}
	}
	Msg	("%d lightmaps builded",g_lightmaps.size());

	// Cleanup deflectors
	Progress	(1.f);
	Status		("Destroying deflectors...");
	for (DWORD it=0; it<g_deflectors.size(); it++)
		_DELETE(g_deflectors[it]);
	g_deflectors.clear	();
}
