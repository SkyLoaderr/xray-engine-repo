#include "stdafx.h"
#include "build.h"

vec2Face g_XMerge;

IC bool defl_compare(Face* F1, Face* F2)
{ return F1->pDeflector < F2->pDeflector; }

void CBuild::MergeGeometry()
{
	Status("Processing...");
	g_XSplit.clear();

	for (int i=0; i<(int)g_XMerge.size(); i++)
	{
		if (g_XMerge[i].empty()) continue;

		vecFace vfBase;
		g_XSplit.push_back(vfBase);

		std::sort(g_XMerge[i].begin(),g_XMerge[i].end(),defl_compare);
		void* pCURD = g_XMerge[i][0]->pDeflector;
		for (int j=0; j<(int)g_XMerge[i].size(); j++)
		{
			Face*	F = g_XMerge[i][j];
			if (F->pDeflector == pCURD) {
				g_XSplit.back().push_back(F);
			} else {
				pCURD = F->pDeflector;
				vecFace vF;
				vF.push_back(F);
				g_XSplit.push_back(vF);
			}
		}

		Progress(float(i)/float(g_XMerge.size()));
	}
	Msg("%d subdivisions",g_XSplit.size());

	// Cleanup
	g_XMerge.clear();
}
