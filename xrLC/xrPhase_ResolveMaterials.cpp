#include "stdafx.h"
#include "build.h"

vec2Face		g_XSplit;

extern void		Detach		(vecFace* S);

struct _counter
{
	WORD	dwMaterial;
	DWORD	dwCount;
};

void	CBuild::xrPhase_ResolveMaterials()
{
	// Count number of materials
	Status		("Calculating materials/subdivs...");
	vector<_counter>	counts;
	{
		counts.reserve		(256);
		for (vecFaceIt F_it=g_faces.begin(); F_it!=g_faces.end(); F_it++)
		{
			Face*	F			= *F_it;
			BOOL	bCreate		= TRUE;
			for (DWORD I=0; I<counts.size(); I++)
			{
				if (F->dwMaterial == counts[I].dwMaterial)
				{
					counts[I].dwCount	+= 1;
					bCreate				= FALSE;
					break;
				}
			}
			if (bCreate)	{
				_counter	C;
				C.dwMaterial	= F->dwMaterial;
				C.dwCount		= 1;
				counts.push_back(C);
			}
			Progress(float(F_it-g_faces.begin())/float(g_faces.size()));
		}
	}
	
	Status				("Perfroming subdivisions...");
	{
		g_XSplit.reserve(64*1024);
		g_XSplit.resize	(counts.size());
		for (DWORD I=0; I<counts.size(); I++) 
		{
			g_XSplit[I] = xr_new<vecFace> ();
			g_XSplit[I]->reserve	(counts[I].dwCount);
		}
		
		for (vecFaceIt F_it=g_faces.begin(); F_it!=g_faces.end(); F_it++)
		{
			Face*	F							= *F_it;
			if (!F->Shader().flags.bRendering)	continue;

			for (DWORD I=0; I<counts.size(); I++)
			{
				if (F->dwMaterial == counts[I].dwMaterial)
				{
					g_XSplit[I]->push_back	(F);
				}
			}
			Progress(float(F_it-g_faces.begin())/float(g_faces.size()));
		}
	}

	Status				("Removing empty subdivs...");
	{
		for (int SP = 0; SP<int(g_XSplit.size()); SP++) 
			if (g_XSplit[SP]->empty())	xr_delete(g_XSplit[SP]);
		g_XSplit.erase(remove(g_XSplit.begin(),g_XSplit.end(),(vecFace*) NULL),g_XSplit.end());
	}
	
	Status		("Detaching subdivs...");
	{
		for (DWORD it=0; it<g_XSplit.size(); it++)
		{
			Detach(g_XSplit[it]);
		}
	}
	Msg("%d subdivisions.",g_XSplit.size());
}
