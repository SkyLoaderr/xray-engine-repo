#include "stdafx.h"
#include "build.h"

vec2Face		g_XSplit;

IC		BOOL	FaceEqual(Face* F1, Face* F2)
{
	if (F1->dwMaterial != F2->dwMaterial)	return FALSE;
	return TRUE;
}

void	CBuild::xrPhase_ResolveMaterials()
{
	g_XSplit.reserve	(4096);
	for (vecFaceIt F_it=g_faces.begin(); F_it!=g_faces.end(); F_it++)
	{
		Face*	F			= *F_it;
		BOOL	bCreate		= TRUE;
		for (DWORD I=0; I<g_XSplit.size(); I++)
		{
			vecFace&	split	= g_XSplit[I];
			Face*		Fbase	= split.front();
			if (FaceEqual(F,Fbase))	{
				split.push_back	(F);
				bCreate			= FALSE;
				break;
			}
		}
		if (bCreate)	{
			g_XSplit.push_back(vecFace());
			g_XSplit.back().push_back(F);
		}

		Progress(float(F_it-g_faces.begin())/float(g_faces.size()));
	}
	for (int it=0; it<g_XSplit.size(); it++)
	{
		Detach(&g_XSplit[it]);
	}
	Msg("%d subdivisions.",g_XSplit.size());
}
