#include "stdafx.h"

void	CBuild::Flex2LOD()
{
	int complete = g_tree.size();
	for (int it=0; it<int(lods.size()); it++)
	{
		Progress(float(it)/float(lods.size()));

		// Search all 'OGFs' with such LOD-id
		OGF_LOD* pNode					= new OGF_LOD(1,pBuild->materials[lods[it].dwMaterial].sector);
		for (int o=0; o<complete; o++)
		{
			OGF*	P		= (OGF*)g_tree[o];
			int		lod_id	= pBuild->materials[P->material].lod_id;
			if (it!=lod_id)	continue;
			pNode->AddChield(o);
		}

		// Register node
		R_ASSERT						(pNode->chields.size());
		pNode->CalcBounds				();
		g_tree.push_back				(pNode);
	}
}
